#include "Free_Fonts.h" // Include the header file attached to this sketch

#include "SPI.h"
#include "TFT_eSPI.h"

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

void tftSetup(void) {
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);            // Clear screen
}

void tftShowData(String timestamp, float temp, float hum) {
    char tempStr[8];
    char humStr[8];
    dtostrf(hum, 4, 2, humStr);
    dtostrf(temp, 4, 2, tempStr);

    // Set text colour to orange with black background
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(FF17);                 // Select the font
    tft.drawString("Timestamp",         10, 10, GFXFF);
    tft.drawString(timestamp.c_str(),   10, 30, GFXFF);
    tft.drawString("Temperature",       10, 60, GFXFF);
    tft.drawString(tempStr,             160, 60, GFXFF);
    tft.drawString("Humidity",          10, 90, GFXFF);
    tft.drawString(humStr,              160, 90, GFXFF);
} 

void tftShowInfo(char *info) {
    Serial.print("tftShowInfo get info char string: ");
    Serial.println(info);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(FF17);                 // Select the font
    tft.drawString("                                                  ", 10, 150, GFXFF);
    // Set text colour to orange with black background
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(FF17);                 // Select the font
    tft.drawString(info, 10, 150, GFXFF);
}