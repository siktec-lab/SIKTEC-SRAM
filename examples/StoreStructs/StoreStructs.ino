/******************************************************************************/
// Created by: SIKTEC.
// Release Version : 1.0.1
// Creation Date: 2022-03-31
// Copyright 2022, SIKTEC.
/******************************************************************************/
/*****************************      DESCRIPTION       *************************
 * 
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

#define SRAM_CS     17

SIKTEC_SRAM sram(SRAM_CS);

void setup() {
    
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("Sram interfacing Example 1");

    //Start Sram communication
    sram.begin();

    //Put in seq mode:
    if (!sram.set_mode(SRAM_MODE::SRAM_SEQ_MODE)) {
        Serial.print("SRAM MODE FAILED");
    } else {
        print_sram_mode();
    }

    Serial.println("\nWriting data to SRAM .... ");
    uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint16_t length = 8;
    sram.write(0x0000, data, length);

    Serial.println("SRAM DUMP first 10 bytes:");
    sram.mem_dump(0x0000, 10);

    Serial.println("\nErasing data from SRAM [0x3 -> 0x5].... ");
    sram.erase(0x0002, 3);

    Serial.println("SRAM DUMP first 10 bytes:");
    sram.mem_dump(0x0000, 10);

}

void loop() {
    delay(500);
}

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