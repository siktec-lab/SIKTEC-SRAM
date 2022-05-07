/******************************************************************************/
// Created by: SIKTEC.
// Release Version : 1.0.1
// Creation Date: 2022-03-31
// Copyright 2022, SIKTEC.
/******************************************************************************/
/** @file SIKTEC_SRAM.cpp */

#include "SIKTEC_SRAM.h"

namespace SIKtec {

/** 
 * @brief  Class constructor when using software SPI
 * @param mosi  master out slave in pin
 * @param miso  master in slave out pin
 * @param sck   serial clock pin
 * @param cs    chip select pin
 * @param freq  The SPI clock frequency to use, defaults to 4MHz e.g 4000000
*/
SIKTEC_SRAM::SIKTEC_SRAM(int8_t mosi, int8_t miso, int8_t sck, int8_t cs, uint32_t freq) {
    this->_cs   = cs;
    this->_destruct_spi = true;
    this->_spi = new SIKTEC_SPI(
        cs, sck, miso, mosi, 
        freq
    );
}

/**
 * @brief  Class constructor when using hardware SPI
 * 
 * @param cs    chip select pin
 * @param spi   the SPI bus to use
 * @param freq  The SPI clock frequency to use, defaults to 4MHz
*/
SIKTEC_SRAM::SIKTEC_SRAM(int8_t cs, SPIClass *spi, uint32_t freq) {
    this->_cs   = cs;
    this->_destruct_spi = true;
    this->_spi = new SIKTEC_SPI(
        cs,
        freq,
        SPI_BITORDER_MSBFIRST,
        SPI_MODE0,
        spi
    );
}

/**
 * @brief class destructor
*/
SIKTEC_SRAM::~SIKTEC_SRAM() {
    if (this->_destruct_spi) 
        delete this->_spi;
}

/** 
 * @brief begin communication with the SRAM chip
 *      
 * @returns void
*/
void SIKTEC_SRAM::begin() {
    if (!this->_spi->begun) {
        this->_spi->begin();
    }
}

/** 
 * @brief read SRAM status register:
 * 
 * @returns uint8_t the register bit set
*/
uint8_t SIKTEC_SRAM::read_status() {

    // write command and address
    uint8_t instruction = SIK_SRAM_RDSR;
    uint8_t status_register;
    (void)this->_spi->write_then_read(&instruction, 1, &status_register, 1);
    return status_register;
}

/** 
 * @brief  write data to the specific address
 * 
 * @param addr the address to write to
 * @param buf the data buffer to write
 * @param num the number of bytes to write
 * @param reg pass SIK_SRAM_WRSR if you are writing the status
 *            register, SIK_SRAM_WRITE if you are writing data. Defaults to SIK_SRAM_WRITE.
 * 
 * @returns void
*/
void SIKTEC_SRAM::write(uint16_t addr, uint8_t *buf, uint16_t num, uint8_t reg) {
    // write command and address
    uint8_t cmdbuf[] = {
        reg,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF)
    };
    (void)this->_spi->write(buf, num, cmdbuf, 3);
}

/** 
 * @brief  read data at the specific address
 * 
 * @param addr the address to read from
 * @param buf the data buffer to read into
 * @param num the number of bytes to read
 * @param reg pass SIK_SRAM_RDSR if you are reading the status
 *            register, SIK_SRAM_READ if you are reading data. Defaults to SIK_SRAM_READ.
 * 
 * @returns void
*/
void SIKTEC_SRAM::read(uint16_t addr, uint8_t *buf, uint16_t num, uint8_t reg) {

    // write command and address
    uint8_t cmdbuf[] = {
        reg,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF)
    };
    (void)this->_spi->write_then_read(cmdbuf, 3, buf, num);

}

/** 
 * @brief read 1 byte of data at the specified address
 * 
 * @param addr the address to read data from
 * @param reg SIK_SRAM_READ if reading data, SIK_SRAM_RDSR if reading a status register.
 * 
 * @returns uint8_t - the read data byte.
*/
uint8_t SIKTEC_SRAM::read8(uint16_t addr, uint8_t reg) {
    uint8_t c;
    this->read(addr, &c, 1, reg);
    return c;
}

/** 
 * @brief read 2 bytes of data at the specified address
 * 
 * @param addr the address to read from
 * 
 * @returns uint16_t - the read data bytes as a 16 bit unsigned integer.
*/
uint16_t SIKTEC_SRAM::read16(uint16_t addr) {
    uint8_t b[2];
    this->read(addr, b, 2);
    return ((uint16_t)b[0] << 8) | b[1];
}

/** 
 * @brief write 1 byte of data to the specified address.
 * 
 * @param addr the address to write to
 * @param val the value to write
 * @param reg SIK_SRAM_WRITE if writing data, SIK_SRAM_WRSR if writing a status register.
 * 
 * @returns void
*/
void SIKTEC_SRAM::write8(uint16_t addr, uint8_t val, uint8_t reg) {
    this->write(addr, &val, 1, reg);
}

/** 
 * @brief write 2 bytes of data to the specified address.
 * 
 * @param addr the address to write to
 * @param val the value to write
 * 
 * @returns void
*/
void SIKTEC_SRAM::write16(uint16_t addr, uint16_t val) {
    uint8_t b[2];
    b[0] = (val >> 8);
    b[1] = (val);
    this->write(addr, b, 2);
}

/** 
 * @brief erase a block of data.
 * 
 * @param addr the address to start the erase at
 * @param length the number of bytes to erase 
 * @param val the value to set the data to.
 * 
 * @returns void
*/
void SIKTEC_SRAM::erase(uint16_t addr, size_t length, uint8_t val) {

    // write command and address
    uint8_t cmdbuf[] = {
        SIK_SRAM_WRITE,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF)
    };
    (void)this->_spi->repeated(val, length, cmdbuf, 3);
}

/** 
 * @brief set specific mode to use the SRAM chip 
 * 
 * @param mode_instruction the mode to use as defined is enum SRAM_MODE
 * 
 * @returns bool - true on success, false otherwise
*/
bool SIKTEC_SRAM::set_mode(SRAM_MODE mode_instruction) {
    // write command and address
    uint8_t cmdbuf[] = {
        SIK_SRAM_WRSR,
        (uint8_t)mode_instruction
    };
    (void)this->_spi->write(cmdbuf, 2);
    delay(10); //Give it time to adjust
    return this->is_mode(mode_instruction);
}

/** 
 * @brief check the sram mode -> reading the STATUS register
 * 
 * @param mode_instruction the mode to use as defined is enum SRAM_MODE
 * 
 * @returns bool
*/
bool SIKTEC_SRAM::is_mode(SRAM_MODE mode_instruction) {
    uint8_t reg = this->read_status();
    this->mode = reg >> 6; // save this as the 7,6 bits that defines the required mode.
    if (this->mode == (mode_instruction >> 6)) {
        return true;
    }
    return false;
}

/** 
 * @brief prints the STATUS register - for debuging
 * 
 * @param serialport the serial interface to use
 * 
 * @returns void
*/
void SIKTEC_SRAM::print_status(Stream *serialport) {
    uint8_t reg = this->read_status();
    serialport->print("SRAM STATUS REGISTER - ");
    serialport->print(reg);
    serialport->print(" [");
    serialport->print(reg, BIN);
    serialport->println("]");

}

/** 
 * @brief memory dump for debuging
 * 
 * @param from          from address
 * @param length        how many to dump
 * @param address       print HEX address?
 * @param hex           print HEX value?
 * @param character     print ASCII value?
 * @param binary        print Binary value?
 * @param serialport    the serial interface to use
 * 
 * @returns void
*/
void SIKTEC_SRAM::mem_dump(uint16_t from, uint16_t length, bool address, bool decimal, bool hex, bool binary, Stream *serialport) {
    uint8_t value;
    for (; length; length--) {
        value = this->read8(from++);
        if (address) {
            serialport->print("SRAM [0x");
            serialport->print(from - 1, HEX);
            serialport->print("] => ");
        } else {
            serialport->print("SRAM => ");
        }
        if (decimal) {
            serialport->print(value, DEC);
        }
        if (hex) {
            if (decimal) serialport->print(" , 0x"); 
            serialport->print(value, HEX);
        }
        if (binary) {
            if (hex || (!hex && decimal)) serialport->print(" , "); 
            serialport->print(value, BIN);
        }
        serialport->print("\r\n");
    }
}

/**
 * @brief disables SPI auto cs toggle
 * 
 * @returns void
 */
void SIKTEC_SRAM::disableCsToggle() {
    this->_spi->disableCsToggle();
}

/**
 * @brief enables SPI auto cs toggle
 * 
 * @returns void
 */
void SIKTEC_SRAM::enableCsToggle() {
    this->_spi->enableCsToggle();
}

/**
 * @brief manually pull cs HIGH
 * 
 * @returns void
 */
void SIKTEC_SRAM::csHigh() {
    digitalWrite(this->_cs, HIGH);
}

/**
 * @brief manually pull cs LOW
 * 
 * @returns void
 */
void SIKTEC_SRAM::csLow() {
    digitalWrite(this->_cs, LOW);
}

}