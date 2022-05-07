/******************************************************************************/
// Created by: SIKTEC.
// Release Version : 1.0.1
// Creation Date: 2022-03-31
// Copyright 2022, SIKTEC.
/******************************************************************************/
/*****************************      DESCRIPTION       *************************
 * A simple example on how to declare the SRAM object, write, read, erase
 * and dump mem for debugging.
/*****************************      NOTES       *******************************
    -> Tested with 23K256-I/SN - should work with others too.
    -> datasheet of test chip: 
       http://ww1.microchip.com/downloads/en/devicedoc/22100f.pdf
*******************************************************************************/
/*****************************      Changelog       ****************************
1.0.1:
    -> initial release.
*******************************************************************************/

#include <Arduino.h>
#include <SIKTEC_SRAM.h>

//Define SRAM CS Pin attached:
#define SRAM_CS 17

using namespace SIKtec;

//Declare SRAM object:
SIKTEC_SRAM sram(SRAM_CS);


//Helper function that prints the SRAM mode an
void print_sram_mode();

void setup() {
    
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\nSRAM Write/Read Example:\n");

    //Start SRAM Communication
    sram.begin();

    //Put SRAM in sequential mode:
    if (!sram.set_mode(SRAM_MODE::SRAM_SEQ_MODE)) {
        Serial.print("SRAM MODE FAILED");
    } else {
        print_sram_mode();
    }

    //Test data - simple integer array:
    uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};

    //Writing data to SRAM:
    Serial.println("\nWriting data to SRAM .... ");
    sram.write(0x0000, data, sizeof(data));

    //DUMP after write:
    Serial.println("SRAM DUMP written bytes:");
    sram.mem_dump(0x0000, sizeof(data));

    //Earsing some values:
    Serial.println("\nErasing data from SRAM [0x2 -> 0x4].... ");
    sram.erase(0x0002, 3);

    //DUMP after erase:
    Serial.println("SRAM DUMP after erase:");
    sram.mem_dump(0x0000, sizeof(data));

    //Reading back the stored values:
    Serial.println("\nReading back:");
    uint8_t back[sizeof(data)];
    sram.read(0x0000, back, sizeof(back));
    for (int i = 0; i < sizeof(back); ++i) {
        if (i) Serial.print(" | ");
        Serial.print(back[i], DEC);
    }
}

void loop() {
    delay(500);
}

/**
 * @brief will print the current SRAM mode and the status register
 * 
 * @return void
 */
void print_sram_mode() {
    Serial.print("SRAM IS IN MODE => ");
    if (sram.is_mode(SRAM_MODE::SRAM_SEQ_MODE)) {
        Serial.println("SEQ_MODE");
    } else if (sram.is_mode(SRAM_MODE::SRAM_BYTE_MODE)) {
        Serial.println("BYTE_MODE");
    } else if (sram.is_mode(SRAM_MODE::SRAM_PAGE_MODE)) {
        Serial.println("PAGE_MODE");
    } else {
        Serial.println("UNKNOWN_MODE");
    }
    sram.print_status();
}