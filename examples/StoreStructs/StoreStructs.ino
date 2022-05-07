/******************************************************************************/
// Created by: SIKTEC.
// Release Version : 1.0.1
// Creation Date: 2022-03-31
// Copyright 2022, SIKTEC.
/******************************************************************************/
/*****************************      DESCRIPTION       *************************
 * A simple example on how to declare the SRAM object, and directly writing 
 * and reading structs.
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

//Example struct we are going to serialize and write to SRAM:
typedef struct MyData {
    int count;
    uint8_t mode;
    char name[10];
    void print() {
        Serial.print("Name -> ");
        Serial.println(name);
        Serial.print("Mode -> ");
        Serial.println(mode);
        Serial.print("Count -> ");
        Serial.println(count);
    }
} Mydata_t;

//Helper function that prints the SRAM mode an
void print_sram_mode();

void setup() {
    
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("\nSRAM Struct Example:\n");

    //Start SRAM Communication
    sram.begin();

    //Put SRAM in sequential mode:
    if (!sram.set_mode(SRAM_MODE::SRAM_SEQ_MODE)) {
        Serial.print("SRAM MODE FAILED");
    } else {
        print_sram_mode();
    }

    //Test data - simple integer array:
    Mydata_t data = {100, 8, "SRAM"};
    
    Serial.println("\nTest data print .... ");
    data.print();

    Serial.println("\nWriting data to SRAM .... ");
    uint8_t writeBuffer[sizeof(data)];
    memcpy(writeBuffer, &data, sizeof(data));
    sram.write(0x0000, writeBuffer, sizeof(writeBuffer));

    //DUMP after write:
    Serial.println("SRAM DUMP written bytes:");
    sram.mem_dump(0x0000, sizeof(writeBuffer));

    //Reading back the stored values:
    Serial.println("\nReading back:");
    Mydata_t back;
    uint8_t readBuffer[sizeof(back)];
    sram.read(0x0000, readBuffer, sizeof(readBuffer));
    memcpy(&back, readBuffer, sizeof(readBuffer));
    back.print();
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