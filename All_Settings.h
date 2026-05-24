//  Use the OpenWeather library: https://github.com/Bodmer/OpenWeather

//  The weather icons and fonts are in the sketch data folder, press Ctrl+K
//  to view.


//            >>>       IMPORTANT TO PREVENT CRASHES      <<<
//>>>>>>  Set SPIFFS to at least 1.5Mbytes before uploading files  <<<<<<


//                >>>           DON'T FORGET THIS             <<<
//  Upload the fonts and icons to SPIFFS using the "Tools"  "ESP32 Sketch Data Upload"
//  or "ESP8266 Sketch Data Upload" menu option in the IDE.
//  To add this option follow instructions here for the ESP8266:
//  https://github.com/esp8266/arduino-esp8266fs-plugin
//  To add this option follow instructions here for the ESP32:
//  https://github.com/me-no-dev/arduino-esp32fs-plugin

//  Close the IDE and open again to see the new menu option.

/* ----- Time Zone settings for your location are in NPT_Time.h --------*/

//////////////////////////////
// Setttings defined below

#pragma once

/***************************************************************************************
**                          Set the default time zone and the alternate one
***************************************************************************************/
uint8_t tzIndex = 0;  // indexes to the timezones

// Update every 15 minutes, up to 1000 request per day are free (viz average of ~40 per hour)
const int UPDATE_INTERVAL_SECS = 15 * 60UL;  // 15 minutes

// Pins for the TFT interface are defined in the User_Config.h file inside the TFT_eSPI library

// // For language codes for BDC response
// const String language = "en";  // Default language = en = English

// Short day of week abbreviations used in 4 day forecast (change to your language)
// const String shortDOW[8] = { "???", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

// // Change the labels to your language here:
// const char sunStr[] = "Sun";
// const char cloudStr[] = "Cloud";
// const char humidityStr[] = "Humidity";
// const String moonPhase[8] = { "New", "Waxing", "1st qtr", "Waxing", "Full", "Waning", "Last qtr", "Waning" };

uint32_t sunrise, sunset;  // where the trigger points are for autodim

#define HOSTNAME "WabbitWeather"
#define DEFAULT_CAPTIVE_SSID "WabbitWeather"

/***************************************************************************************
**                          USER settings for defaults
***************************************************************************************/

// For units use "metric" or "imperial"
String units = "imperial";

// Set both longitude and latitude to at least 4 decimal places
String latitude = "49.2747";     // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude = "-123.1038";  // 180.000 to -180.000 negative for West

//
bool show24Hour = false;            // display a 24 or 12 hour clock
bool autoDimDusk = true;            // daytime is orange clock numbers, nighttime is dark grey
bool showBarometric = false;        // show the barometric pressure
bool showPrecipProbability = true;  // show precip probab for future forecast
bool showUVindex = true;            // show the UV index under the WIFI strength
bool show6hrForecast = true;        // show 6hr forecast on bottom of the display...
//************
// Temp range colour display control, in metric values for high and low are 29C, 4C
// ***********
uint8_t highTempVal = 85;            // This temp and higher will be hightempcolour, lower will be midcolour
uint8_t lowTempVal = 40;             // temps lower than this will be different colour
uint32_t highTempColour = TFT_RED;   // colour used if temp is above highTempVal
uint32_t midTempColour = TFT_GREEN;  // colour used if temp is less then highTempVal but greater than lowtempVal
uint32_t lowTempColour = TFT_BLUE;   // colour used if less than lowTempValue
//
uint8_t highPrecipProb = 75;         // higher than this, for sure
uint8_t lowPrecipProb = 30;          // less than this it's anyones guess
uint32_t highPOPColour = TFT_RED;    // colour used if POP higher then highPrecipProb
uint32_t midPOPColour = TFT_YELLOW;  // colour used if POP less then highPrecipProb
uint32_t lowPOPColour = TFT_GREEN;   // colour used if POP less then lowPrecipProb
//
uint32_t labelColour = TFT_WHITE;       // for the top/mid labels
uint32_t astrologyColour = TFT_YELLOW;  // for the astrology labels

String theCityLocation = "Unknown";  // where the long and lat point to

uint8_t blDusk = 30;  // backlight brightness at night (0-255)

// End of user settings
//////////////////////////////
