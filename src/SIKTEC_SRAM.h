/******************************************************************************/
// Created by: SIKTEC.
// Release Version : 1.0.1
// Creation Date: 2022-03-31
// Copyright 2022, SIKTEC.
/******************************************************************************/
/*****************************      NOTES       *******************************
    -> Class to interface and interact with Microchip SRAM memory 
       chips
    -> Tested with 23K256-I/SN - should work with others too.
    -> datasheet of test chip: 
       http://ww1.microchip.com/downloads/en/devicedoc/22100f.pdf
*******************************************************************************/
/*****************************      Changelog       ****************************
1.0.1:
    -> initial release.
*******************************************************************************/

#pragma once 
/** @file SIKTEC_SRAM.h */

//------------------------------------------------------------------------//
// INCLUDES:
//------------------------------------------------------------------------//

#include <Arduino.h>
#include <SIKTEC_SPI.h>

//------------------------------------------------------------------------//
// DEFAULT ADDRESSES AND CONSTANTS:
//------------------------------------------------------------------------//

#define SIK_SRAM_READ  0x03     ///< read command
#define SIK_SRAM_WRITE 0x02     ///< write command
#define SIK_SRAM_RDSR  0x05     ///< read status register command
#define SIK_SRAM_WRSR  0x01     ///< write status register command


/** @brief SRAM modes  */
enum SRAM_MODE : uint8_t {
    SRAM_SEQ_MODE  = (1 << 6), // => 0100 0000
    SRAM_PAGE_MODE = (1 << 7), // => 0100 0000
    SRAM_BYTE_MODE = 0,        // => 0000 0000
};

//------------------------------------------------------------------------//
// SIKTEC_SRAM
//------------------------------------------------------------------------//

/** 
 * @brief  Class for interfacing with Microchip SPI SRAM chips used across SIKTEC
*/
class SIKTEC_SRAM {

    private:
    
        uint8_t mode;
        int8_t _cs, _mosi, _miso, _sck;
        bool _destruct_spi;
        SIKTEC_SPI *_spi = NULL;
    
    public:

        SIKTEC_SRAM(int8_t mosi, int8_t miso, int8_t sck, int8_t cs, uint32_t freq = 4000000);
        SIKTEC_SRAM(int8_t cs, SPIClass *spi = &SPI, uint32_t freq = 4000000);

        ~SIKTEC_SRAM();

        void begin();
        uint8_t read_status();

        void write(uint16_t addr, uint8_t *buf, uint16_t num, uint8_t reg = SIK_SRAM_WRITE);
        void read(uint16_t addr, uint8_t *buf, uint16_t num, uint8_t reg = SIK_SRAM_READ);
        void erase(uint16_t addr, size_t length, uint8_t val = 0x00);

        uint8_t read8(uint16_t addr, uint8_t reg = SIK_SRAM_READ);
        uint16_t read16(uint16_t addr);

        void write8(uint16_t addr, uint8_t val, uint8_t reg = SIK_SRAM_WRITE);
        void write16(uint16_t addr, uint16_t val);

        bool set_mode(SRAM_MODE mode_instruction);
        bool is_mode(SRAM_MODE mode_instruction);

        void print_status(Stream *serialport = &Serial);
        void mem_dump(uint16_t from, uint16_t length, bool address = true, bool decimal = true, bool character = true, bool binary = true, Stream *serialport = &Serial);

        void disableCsToggle();
        void enableCsToggle();
        void csHigh();
        void csLow();
};
