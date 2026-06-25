//  Adapted by Bodmer to use the TFT_eSPI library:  https://github.com/Bodmer/TFT_eSPI

//  Original by Daniel Eichhorn, see license at end of file.

//#define SERIAL_MESSAGES // For serial output weather reports
//#define SCREEN_SERVER   // For dumping screen shots from TFT
//#define RANDOM_LOCATION // Test only, selects random weather location every refresh
//#define SHOW_JSON

// This sketch uses font files created from the Noto family of fonts as bitmaps
// generated from these fonts may be freely distributed:
// https://www.google.com/get/noto/

// A processing sketch to create new fonts can be found in the Tools folder of TFT_eSPI
// https://github.com/Bodmer/TFT_eSPI/tree/master/Tools/Create_Smooth_Font/Create_font
// New fonts can be generated to include language specific characters. The Noto family
// of fonts has an extensive character set coverage.

// Json streaming parser (do not use IDE library manager version) to use is here:
// https://github.com/Bodmer/JSON_Decoder

// Modifications to use Open Meteo by WabbitGuy (Mel) more at end of this file

#include <FS.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define AA_FONT_SMALL "fonts/NotoSansBold15"  // 15 point sans serif bold
#define AA_FONT_LARGE "fonts/NotoSansBold36"  // 36 point sans serif bold
//
#define WABBIT_VERSION "Version: 1.5.0"
//
/***************************************************************************************
**                          Load the libraries and settings
***************************************************************************************/
#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h>             // https://github.com/Bodmer/TFT_eSPI
#include <XPT2046_Touchscreen.h>  // touch screen function

// Additional functions
#include "GfxUi.h"  // Attached to this sketch
//#include "SPIFFS_Support.h" // Attached to this sketch

#include <WiFi.h>
#include <WiFiManager.h>

// check All_Settings.h for adapting to your needs
#include "All_Settings.h"
#include "Language.h"  // language you want to use
#include "Translation.h"

#include <JSON_Decoder.h>  // https://github.com/Bodmer/JSON_Decoder

#include "OpenMeteo.h"  // Latest here: https://github.com/Bodmer/OpenWeather
#include "Nominatim.h"  // for returning city from long.lat

#include "NTP_Time.h"   // NTP sync + toLocal() helper + fetchTZOffset()
#include "MoonPhase.h"  // phase of the moon

#include "WebConfig.h"
volatile bool configUpdated = false;

/***************************************************************************************
**                          Define the globals and class instances
***************************************************************************************/

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

OW_Weather ow;  // Weather forecast library instance

OW_current *current;  // Pointers to structs that temporarily holds weather data
OW_hourly *hourly;    // Not used
OW_daily *daily;

BDC_location bcd;

BDC_current *BDCcurrent;  // struct that holds the location data

boolean booted = true;

bool forecastPage = false;       // false = days 1-4 (default), true = days 5-8
uint32_t forecastPageTimer = 0;  // millis() timestamp when forecastPage was set true

bool altLocation = false;  // false = primary location, true = alternate location

GfxUi ui = GfxUi(&tft);  // Jpeg and bmpDraw functions TODO: pull outside of a class

long lastDownloadUpdate = millis();
boolean runOnce = false;
boolean runBdcOnce = false;  // only need a location once..

#define TFT_BL 21         // CYD backlight pin
#define TFT_BL_FREQ 5000  // PWM frequency Hz
#define TFT_BL_RES 8      // 8-bit = 0-255

#define BL_DAY 255  // full brightness during daylight

#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_W 240
#define SCREEN_H 320
/***************************************************************************************
**                          Declare prototypes
***************************************************************************************/
void updateData();
void drawProgress(uint8_t percentage, String text);
void drawTime();
void drawCurrentWeather();
void drawForecast();
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
const char *getMeteoconIcon(uint16_t id, bool today);
void drawAstronomy();
void drawSeparator(uint16_t y);
void fillSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int colour);
String strDate(time_t unixTime);
String strTime(time_t unixTime);
void printWeather(void);
int leftOffset(String text, String sub);
int rightOffset(String text, String sub);
int splitIndex(String text);
void drawWiFiQuality();
uint32_t daySeconds(time_t unixTime);
void handleAstronomyFrame();
void handleHourlyFrame();
void configModeCallback(WiFiManager *myWiFiManager);
void handleTempRangeColour(uint16_t theVal);
void handlePrecipRange(uint16_t theVal);
void findTheLocation();       // find the location based on long/lat
void updateBacklight();       // handles the display dimming at night
void showSplashScreen();      // shows the credits and version
void checkTimezoneOffsets();  // re-fetch TZ offsets from timeapi.io at 2am

/***************************************************************************************
**                          Setup
***************************************************************************************/
void setup() {
  Serial.begin(115200);

  tft.init();
  tft.fillScreen(TFT_BLACK);

  ledcAttach(TFT_BL, TFT_BL_FREQ, TFT_BL_RES);
  ledcWrite(TFT_BL, BL_DAY);

  if (!LittleFS.begin()) {
    Serial.println("Flash FS initialisation failed!");
    while (1) yield();  // Stay here twiddling thumbs waiting
  }
  Serial.println("\nFlash FS available!");

#ifdef SERIAL_MESSAGES
  //listFiles();
#endif
  //
  // Enable if you want to erase SPIFFS, this takes some time!
  // then disable and reload sketch to avoid reformatting on every boot!
#ifdef FORMAT_LittleFS
  tft.setTextDatum(BC_DATUM);  // Bottom Centre datum
  tft.drawString("Formatting LittleFS, so wait!", 120, 195);
  SPIFFS.format();
#endif

  // Draw splash screen
  if (LittleFS.exists("/splash/OpenMeteo.jpg") == true) ui.drawJpeg("/splash/OpenMeteo.jpg", 0, 80);  // 240 x 124

  delay(1000);

  // Clear bottom section of screen
  tft.fillRect(0, 208, 240, 320 - 206, TFT_BLACK);

  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextDatum(BC_DATUM);  // Bottom Centre datum
  tft.setTextColor(TFT_GOLD, TFT_BLACK);

  tft.drawString(creditOriginal, 120, 260);
  tft.drawString(creditBodmer, 120, 280);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(creditWabbit, 120, 300);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  delay(3000);

  tft.fillRect(0, 208, 240, 320 - 206, TFT_BLACK);
  //
  WiFiManager wm;
  wm.setAPCallback(configModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240);        // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 220);  // Clear line above using set padding width
  tft.drawString(bootConnectedWifi, 120, 240);
  delay(1000);
  //
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  uint32_t seed1 =
    (macAddr[5] << 24) | (macAddr[4] << 16) | (macAddr[3] << 8) | macAddr[2];
  randomSeed(analogRead(A0));
  String ipaddress = WiFi.localIP().toString();
  localPort = random(1024, 65535);
  udp.begin(localPort);
  //
  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240);        // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 220);  // Clear line above using set padding width
  tft.drawString(bootFetchingWeather, 120, 240);
  delay(2000);
  //
  syncTime();  // now we go look for a time server
  //
  tft.unloadFont();

  ow.partialDataSet(false);  // Collect a full set of the data available (false)

  // LittleFS.remove(CONFIG_FILE);  // just for testing
  loadConfig();
  startWebConfig();

  // Init touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);
}

/***************************************************************************************
**                          Loop
***************************************************************************************/
void loop() {

  // Check if we should update weather information
  if (booted || (millis() - lastDownloadUpdate > 1000UL * UPDATE_INTERVAL_SECS)) {
    updateData();
    lastDownloadUpdate = millis();
  }

  // If minute has changed then request new time from NTP server
  if (booted || minute() != lastMinute) {
    // Update displayed time first as we may have to wait for a response
    drawTime();         // show the time
    updateBacklight();  // change the backlight if needed
    lastMinute = minute();
    drawWiFiQuality();  // update signal strength once a minute

    // Request and synchronise the local clock
    if (booted || lastHour != hour()) {
      syncTime();
      // At 2am re-fetch timezone offsets from timeapi.io to catch any DST changes
      if (hour(toLocal(now())) == 2) {
        checkTimezoneOffsets();
      }
      lastHour = hour();
    }

#ifdef SCREEN_SERVER
    screenServer();
#endif
  }
  //
  if (runOnce == true && second() != lastSecond) {
    switch (second()) {
      case 5:
        handleAstronomyFrame();  // go draw the astronomy portion of the screen
        break;
      case 20:
        if (show6hrForecast) handleHourlyFrame();  // show 6hr forcast
        break;
      case 35:
        handleAstronomyFrame();  // go draw the astronomy portion of the screen
        break;
      case 50:
        if (show6hrForecast) handleHourlyFrame();
        break;
    }
    lastSecond = second();  // save for next pass
  }
  //
  booted = false;

  handleWebConfig();

  if (configUpdated) {
    configUpdated = false;
    runBdcOnce = false;
    updateData();
    drawTime();         // redraw clock immediately without waiting for next minute
    updateBacklight();  // change the backlight if needed
  }

  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    while (touchscreen.touched()) delay(10);

    if (p.y < 900) {
      // Clock area — show splash
      showSplashScreen();
    } else if (p.y < 1800) {
      // Icon/weather area — toggle location
      altLocation = !altLocation;
      runBdcOnce = false;
      Serial.println(altLocation ? "Switched to TZ2" : "Switched to TZ1");
      updateData();
      drawTime();         // show the time
      updateBacklight();  // change the backlight if needed
      drawWiFiQuality();  // update signal strength once a minute
    } else {
      // Forecast area — toggle forecast page
      tft.loadFont(AA_FONT_SMALL, LittleFS);
      forecastPage = !forecastPage;
      if (forecastPage) forecastPageTimer = millis();
      drawForecast();
      tft.unloadFont();
    }
  }
}

//
/***************************************************************************************
**                          Fetch the weather data  and update screen
***************************************************************************************/
// Update the Internet based information and update screen
void updateData() {
  uint16_t smurf;
  bool parsed;
  //
  if (runBdcOnce == false) tft.fillScreen(TFT_BLACK);
  //
  tft.loadFont(AA_FONT_SMALL, LittleFS);

  if (booted) drawProgress(20, bootUpdatingTime);
  else fillSegment(22, 22, 0, (int)(20 * 3.6), 16, TFT_NAVY);

  if (booted) drawProgress(50, bootUpdatingConditions);
  else fillSegment(22, 22, 0, (int)(50 * 3.6), 16, TFT_NAVY);

  // Create the structures that hold the retrieved weather
  if (runOnce == false) {
    current = new OW_current;
    daily = new OW_daily;
    hourly = new OW_hourly;
    runOnce = true;  // set flag for only doing this once
  }

#ifdef RANDOM_LOCATION  // Randomly choose a place on Earth to test icons etc
  String latitude = "";
  latitude = (random(180) - 90);
  String longitude = "";
  longitude = (random(360) - 180);
  Serial.print("Lat = ");
  Serial.print(latitude);
  Serial.print(", Lon = ");
  Serial.println(longitude);
#endif

  findTheLocation();  // go find the location we're interested in

  // Use active location coordinates for weather fetch
  String activeLat = altLocation ? latitude2 : latitude;
  String activeLon = altLocation ? longitude2 : longitude;

  parsed = ow.getForecast(current, hourly, daily, activeLat, activeLon, units);

  // try again in 60 seconds if it fails
  if (!parsed) {
    Serial.println("Fetch failed, retrying in 60 seconds...");
    delay(60000);
    parsed = ow.getForecast(current, hourly, daily, activeLat, activeLon, units);
  }

  printWeather();  // For debug, turn on output with #define SERIAL_MESSAGES

  if (booted) {
    drawProgress(100, bootDone);
    delay(2000);
    tft.fillScreen(TFT_BLACK);
  } else {
    fillSegment(22, 22, 0, 360, 16, TFT_NAVY);
    fillSegment(22, 22, 0, 360, 22, TFT_BLACK);
  }

  if (parsed) {
    drawCurrentWeather();
    drawForecast();
    drawAstronomy();

    tft.unloadFont();

    // Update the temperature here so we don't need to keep
    // loading and unloading font which takes time
    tft.loadFont(AA_FONT_LARGE, LittleFS);
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    tft.setTextPadding(tft.textWidth(" -88"));  // Max width of values

    String weatherText = "";
    smurf = current->temp + .5;  // Make it integer temperature
    weatherText = String(smurf);
    handleTempRangeColour(smurf);
    if (runBdcOnce == false) {
      findTheLocation();
    }
    tft.drawString(weatherText, 215, 100);
    //
  } else {
    Serial.println("Failed to get weather");
  }

  tft.unloadFont();
}

/***************************************************************************************
**                          Update progress bar
***************************************************************************************/
void drawProgress(uint8_t percentage, String text) {
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(240);
  tft.drawString(text, 120, 260);

  ui.drawProgressBar(10, 269, 240 - 20, 15, percentage, TFT_WHITE, TFT_BLUE);

  tft.setTextPadding(0);
}

/***************************************************************************************
**                          Draw the clock digits
***************************************************************************************/
void drawTime() {
  tft.loadFont(AA_FONT_LARGE, LittleFS);

  // Apply timezone offset to get local time
  time_t local_time = toLocal(now());

  String timeNow = "";

  if (show24Hour == true) {
    if (hour(local_time) < 10) timeNow += "0";
    timeNow += hour(local_time);
  } else {
    if (hour(local_time) > 12) {
      timeNow += hour(local_time) - 12;
    } else {
      timeNow += hour(local_time);
      if (hour(local_time) == 0) timeNow = "12";
    }
  }
  timeNow += ":";
  if (minute(local_time) < 10) timeNow += "0";
  timeNow += minute(local_time);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 44:44 "));  // String width + margin
  tft.drawString(timeNow, 120, 53);

  tft.setTextPadding(0);

  tft.unloadFont();
}

/***************************************************************************************
**                          Draw the current weather
***************************************************************************************/
void drawCurrentWeather() {
  time_t local_time = toLocal(now());
  String date = String(updatedStr) + strDate(local_time);

  int x, y, uvMax, temp, theWidth;
  uint16_t theColour;
  String weatherText = "None";

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Updated: Mmm 44 44:44 "));
  tft.drawString(date, 120, 16);

  String weatherIcon = "";

  String currentSummary = wmoMain(current->id);

  weatherIcon = getMeteoconIcon(current->id, true);

  ui.drawBmp("/icon/" + weatherIcon + ".bmp", 0, 62);

  // Weather Text
  weatherText = wmoDescription(current->id);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);

  int splitPoint = 0;
  int xpos = 235;
  splitPoint = splitIndex(weatherText);

  tft.setTextPadding(xpos - 100);
  if (splitPoint) tft.drawString(weatherText.substring(0, splitPoint), xpos, 86);
  else tft.drawString(" ", xpos, 74);
  tft.drawString(weatherText.substring(splitPoint), xpos, 100);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(0);
  if (units == "metric") tft.drawString(tempMetric, 237, 105);
  else tft.drawString(tempImperial, 237, 105);

  tft.setTextColor(labelColour, TFT_BLACK);
  weatherText = (uint16_t)current->wind_speed;

  if (units == "metric") weatherText += windMetric;
  else weatherText += windImperial;

  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(tft.textWidth("888 m/s"));
  tft.drawString(weatherText, 124, 136);

  if (showBarometric) {
    if (units == "imperial") {
      weatherText = current->pressure;
      weatherText += pressImperial;
    } else {
      weatherText = (uint16_t)current->pressure;
      weatherText += pressMetric;
    }

    tft.setTextDatum(TR_DATUM);
    tft.setTextPadding(tft.textWidth(" 8888hPa"));
    tft.drawString(weatherText, 230, 136);
  }

  int windAngle = (current->wind_deg + 22.5) / 45;
  if (windAngle > 7) windAngle = 0;
  String wind[] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
  ui.drawBmp("/wind/" + wind[windAngle] + ".bmp", 101, 86);
  //
  // Show city name for whichever location is active
  String cityName = altLocation ? theCityLocation2 : theCityLocation;
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(cityName));
  tft.drawString(cityName, 120, 70);
  //
  if (showUVindex) {
    theColour = TFT_GREEN;
    uvMax = (current->uvi + .5);
    if (uvMax > 9) uvMax = 9;
    for (temp = 0; temp <= 9; temp++) {
      x = 178 + (temp * 6);
      y = 40 - (temp * 2);
      if (temp <= uvMax) {
        switch (temp) {
          case 0: theColour = TFT_GREEN; break;
          case 1: theColour = TFT_GREEN; break;
          case 2: theColour = TFT_GREEN; break;
          case 3: theColour = TFT_YELLOW; break;
          case 4: theColour = TFT_YELLOW; break;
          case 5: theColour = TFT_YELLOW; break;
          case 6: theColour = TFT_ORANGE; break;
          case 7: theColour = TFT_ORANGE; break;
          case 8: theColour = TFT_RED; break;
          case 9: theColour = TFT_RED; break;
          case 10: theColour = TFT_RED; break;
          default: theColour = TFT_DARKGREY; break;
        }
      } else {
        theColour = TFT_DARKGREY;
      }
      tft.fillRect(x, y, 4, 8 + (temp * 2), theColour);
    }
  }

  tft.setTextDatum(TL_DATUM);
  tft.setTextPadding(0);
}

/***************************************************************************************
**                          Draw the 4 forecast columns
***************************************************************************************/
// draws the four forecast columns for either days 1-4 or days 5-8
void drawForecast() {
  // forecastPage false = days 1-4 (index 1..4), true = days 5-8 (index 5..7)
  // Index 0 = today, indices 1-7 = next 7 days. Index 8 does not exist.
  int8_t baseDay = forecastStartToday ? 0 : 1;
  int8_t dayIndex = forecastPage ? (baseDay + 4) : baseDay;

  // Clear the forecast area before redrawing so stale icons/text don't bleed through
  tft.fillRect(0, 156, SCREEN_W, 84, TFT_BLACK);

  drawForecastDetail(8, 171, dayIndex++);
  drawForecastDetail(66, 171, dayIndex++);
  drawForecastDetail(124, 171, dayIndex++);
  if (dayIndex < MAX_DAYS) drawForecastDetail(182, 171, dayIndex);
}

/***************************************************************************************
**                          Draw 1 forecast column at x, y
***************************************************************************************/
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) {
  uint16_t precipProbability;

  if (dayIndex >= MAX_DAYS) return;

  //String day = shortDOW[weekday(toLocal(daily->dt[dayIndex]))];
  String day = shortDOW[weekday(daily->dt[dayIndex])];
  day.toUpperCase();

  tft.setTextDatum(BC_DATUM);

  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("WWW"));
  tft.drawString(day, x + 25, y);

  tft.setTextPadding(tft.textWidth("-88"));
  String highTemp = String(daily->temp_max[dayIndex], 0);
  String lowTemp = String(daily->temp_min[dayIndex], 0);
  handleTempRangeColour(daily->temp_max[dayIndex]);
  tft.drawString(highTemp, x + 12, y + 14);
  handleTempRangeColour(daily->temp_min[dayIndex]);
  tft.drawString(lowTemp, x + 37, y + 14);

  String weatherIcon = getMeteoconIcon(daily->id[dayIndex], false);

  ui.drawBmp("/icon50/" + weatherIcon + ".bmp", x, y + 10);

  if (showPrecipProbability) {
    precipProbability = daily->pop[dayIndex] * 100;
    String myPOP = String(precipProbability);
    myPOP += "%";
    tft.setTextPadding(tft.textWidth(" 88% "));
    handlePrecipRange(precipProbability);
    tft.drawString(myPOP, x + 25, y + 72);
  }

  tft.setTextPadding(0);
}
//
/***************************************************************************************
**                     Handles the display in the lower frame of the display
**************************************************************************************/
void handleHourlyFrame() {
  uint16_t temp, theX, theY, theOffsetY;
  uint16_t smurf;
  uint16_t precipProbability;
  String result;
  theY = 280;
  theOffsetY = 18;
  temp = 0;
  theX = 70;
  tft.fillRect(0, 241, 240, 320 - 241, TFT_BLACK);
  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("Temp:"));
  tft.drawString(popLabelStr, theX - 12, theY);
  tft.drawString(tempLabelStr, theX - 12, theY + theOffsetY);
  tft.drawString(dewLabelStr, theX - 12, theY + (theOffsetY * 2));
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  tft.drawString(forecastTitleStr, 120, theY - 20);
  tft.setTextDatum(BL_DATUM);
  //
  drawSeparator(260);

  for (temp = 0; temp < 6; temp++) {
    precipProbability = hourly->pop[temp] * 100;
    if (precipProbability >= 100) precipProbability = 99;

    handlePrecipRange(precipProbability);
    if (precipProbability < 10) {
      result = "  ";
      result += String(precipProbability);
    } else {
      result = String(precipProbability);
    }

    tft.drawString(result, theX + (temp * 26), theY);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    smurf = hourly->temp[temp] + .5;
    if (smurf >= 100) smurf = 99;
    handleTempRangeColour(smurf);

    if (smurf < 10) {
      result = "  ";
      result += String(smurf);
    } else {
      result = String(smurf);
    }
    tft.drawString(result, theX + (temp * 26), theY + theOffsetY);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    smurf = hourly->dew_point[temp] + .5;
    if (smurf >= 100) {
      smurf = 99;
      tft.setTextColor(TFT_RED, TFT_BLACK);
    }
    if (smurf < 10) {
      result = "  ";
      result += String(smurf);
    } else {
      result = String(smurf);
    }
    tft.drawString(result, theX + (temp * 26), theY + (theOffsetY * 2));
  }
  tft.unloadFont();
}
//
void handleAstronomyFrame() {
  tft.fillRect(0, 241, 240, 320 - 241, TFT_BLACK);
  tft.loadFont(AA_FONT_SMALL, LittleFS);
  drawAstronomy();
  tft.unloadFont();
}

/***************************************************************************************
**                          Draw Sun rise/set, Moon, cloud cover and humidity
***************************************************************************************/
void drawAstronomy() {

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Last qtr "));

  time_t local_time = toLocal(current->dt);
  uint16_t y = year(local_time);
  uint8_t m = month(local_time);
  uint8_t d = day(local_time);
  uint8_t h = hour(local_time);
  int ip, xPos;
  uint8_t icon = moon_phase(y, m, d, h, &ip);

  tft.drawString(moonPhase[ip], 120, 319);
  // Use active location latitude for hemisphere detection
  String activeLat = altLocation ? latitude2 : latitude;
  bool southernHemisphere = activeLat.toFloat() < 0.0;
  String moonHemi = southernHemisphere ? "R" : "L";
  ui.drawBmp("/moon/moonphase_" + moonHemi + String(icon) + ".bmp", 120 - 30, 318 - 16 - 60);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.setTextPadding(0);
  tft.drawString(sunStr, 44, 270);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(riseStr));
  show24Hour ? xPos = 34 : xPos = 43;
  tft.drawString(riseStr, xPos, 290);
  tft.drawString(setStr, xPos, 305);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("8:88 "));

  String rising = strTime(current->sunrise) + " ";
  int dt = rightOffset(rising, ":");
  tft.drawString(rising, 55 + dt, 290);

  sunrise = daySeconds(current->sunrise);

  String setting = strTime(current->sunset) + " ";
  dt = rightOffset(setting, ":");
  tft.drawString(setting, 55 + dt, 305);

  sunset = daySeconds(current->sunset);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.drawString(cloudStr, 195, 260);

  String cloudCover = "";
  cloudCover += current->clouds;
  cloudCover += "%";

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 100%"));
  tft.drawString(cloudCover, 210, 277);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.drawString(humidityStr, 195, 300 - 2);

  String humidity = "";
  humidity += current->humidity;
  humidity += "%";

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("100%"));
  tft.drawString(humidity, 210, 315);

  tft.setTextPadding(0);
}

/***************************************************************************************
**                          Set the colour for the temp range
***************************************************************************************/
void handleTempRangeColour(uint16_t theVal) {
  if (theVal >= highTempVal) tft.setTextColor(highTempColour, TFT_BLACK);
  if (theVal < highTempVal) tft.setTextColor(midTempColour, TFT_BLACK);
  if (theVal < lowTempVal) tft.setTextColor(lowTempColour, TFT_BLACK);
}

/***************************************************************************************
**                          Set the colour for the POP range
***************************************************************************************/
void handlePrecipRange(uint16_t theVal) {
  if (theVal >= highPrecipProb) tft.setTextColor(highPOPColour, TFT_BLACK);
  if (theVal < highPrecipProb) tft.setTextColor(midPOPColour, TFT_BLACK);
  if (theVal < lowPrecipProb) tft.setTextColor(lowPOPColour, TFT_BLACK);
}

/***************************************************************************************
**                Get the icon file name from the WMO weather code
**                Matches icon filenames in /icon/ and /icon50/ folders
***************************************************************************************/
const char *getMeteoconIcon(uint16_t id, bool today) {
  bool isNight = today && (current->dt < current->sunrise || current->dt > current->sunset);

  if (id == 0) return isNight ? "clear_night" : "sunny";
  if (id == 1) return isNight ? "mostly_clear_night" : "mostly_sunny";
  if (id == 2) return isNight ? "partly_cloudy_night" : "partly_cloudy";
  if (id == 3) return "cloudy";
  if (id == 45 || id == 48) return "haze_fog_dust_smoke";
  if (id == 51 || id == 53) return "drizzle";
  if (id == 55) return "drizzle";
  if (id == 56 || id == 57) return "wintry_mix_rain_snow";
  if (id == 61) return "showers_rain";
  if (id == 63) return "showers_rain";
  if (id == 65) return "heavy_rain";
  if (id == 66 || id == 67) return "sleet_hail";
  if (id == 71) return "flurries";
  if (id == 73) return "heavy_snow";
  if (id == 75) return "heavy_snow";
  if (id == 77) return "flurries";
  if (id == 80) return isNight ? "scattered_showers_night" : "scattered_showers_day";
  if (id == 81) return isNight ? "scattered_showers_night" : "scattered_showers_day";
  if (id == 82) return "heavy_rain";
  if (id == 85) return "snow_showers_snow";
  if (id == 86) return "blizzard";
  if (id == 95) return isNight ? "isolated_scattered_tstorms_night" : "isolated_scattered_tstorms_day";
  if (id == 96 || id == 99) return "strong_tstorms";

  return "unknown";
}

/***************************************************************************************
**                          Draw screen section separator line
***************************************************************************************/
void drawSeparator(uint16_t y) {
  tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
}

/***************************************************************************************
**                          Determine place to split a line
***************************************************************************************/
int splitIndex(String text) {
  int index = 0;
  while ((text.indexOf(' ', index) >= 0) && (index <= text.length() / 2)) {
    index = text.indexOf(' ', index) + 1;
  }
  if (index) index--;
  return index;
}

/***************************************************************************************
**                          Right side offset to a character
***************************************************************************************/
int rightOffset(String text, String sub) {
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(index));
}

/***************************************************************************************
**                          Left side offset to a character
***************************************************************************************/
int leftOffset(String text, String sub) {
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(0, index));
}

/***************************************************************************************
**                          Draw circle segment
***************************************************************************************/
#define DEG2RAD 0.0174532925
#define INC 2
void fillSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int colour) {
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x1 = sx * r + x;
  uint16_t y1 = sy * r + y;

  for (int i = start_angle; i < start_angle + sub_angle; i += INC) {
    int x2 = cos((i + 1 - 90) * DEG2RAD) * r + x;
    int y2 = sin((i + 1 - 90) * DEG2RAD) * r + y;
    tft.fillTriangle(x1, y1, x2, y2, x, y, colour);
    x1 = x2;
    y1 = y2;
  }
}

/***************************************************************************************
**                          Print the weather info to the Serial Monitor
***************************************************************************************/
void printWeather(void) {
#ifdef SERIAL_MESSAGES
  Serial.println("Weather from OpenMeteo\n");

  Serial.println("############### Current weather ###############\n");
  Serial.print("dt (time)          : ");
  Serial.println(strDate(current->dt));
  Serial.print("sunrise            : ");
  Serial.println(strDate(current->sunrise));
  Serial.print("sunset             : ");
  Serial.println(strDate(current->sunset));
  Serial.print("main               : ");
  Serial.println(current->main);
  Serial.print("temp               : ");
  Serial.println(current->temp);
  Serial.print("humidity           : ");
  Serial.println(current->humidity);
  Serial.print("pressure           : ");
  Serial.println(current->pressure);
  Serial.print("wind_speed         : ");
  Serial.println(current->wind_speed);
  Serial.print("wind_deg           : ");
  Serial.println(current->wind_deg);
  Serial.print("clouds             : ");
  Serial.println(current->clouds);
  Serial.print("UVIndex            : ");
  Serial.println(current->uvi);
  Serial.print("id                 : ");
  Serial.println(current->id);
  Serial.println();

  Serial.println("###############  Daily weather  ###############\n");
  for (int i = 0; i < 5; i++) {
    Serial.print("dt (time)          : ");
    Serial.println(strDate(daily->dt[i]));
    Serial.print("id                 : ");
    Serial.println(daily->id[i]);
    Serial.print("temp_Max           : ");
    Serial.println(daily->temp_max[i]);
    Serial.print("temp_min           : ");
    Serial.println(daily->temp_min[i]);
    Serial.print("Pressure           : ");
    Serial.println(daily->pressure[i]);
    Serial.print("Clouds             : ");
    Serial.println(daily->clouds[i]);
    Serial.print("UV Index           : ");
    Serial.println(daily->uvi[i]);
    Serial.print("POP                : ");
    Serial.println(daily->pop[i]);
    Serial.println();
  }

  Serial.println("###############  Hourly weather  ###############\n");
  for (int i = 0; i < 5; i++) {
    Serial.print("dt (time)          : ");
    Serial.println(strDate(hourly->dt[i]));
    Serial.print("temp               : ");
    Serial.println(hourly->temp[i]);
    Serial.print("Dew Point          : ");
    Serial.println(hourly->dew_point[i]);
    Serial.print("POP                : ");
    Serial.println(hourly->pop[i]);
    Serial.println();
  }
#endif
}

/***************************************************************************************
**             Convert Unix time to a "local time" time string "12:34"
***************************************************************************************/
String strTime(time_t unixTime) {
  String localTime = "";

  if (show24Hour == true) {
    if (hour(unixTime) < 10) localTime += "0";
    localTime += hour(unixTime);
  } else {
    if (hour(unixTime) > 12) {
      localTime += hour(unixTime) - 12;
    } else {
      localTime += hour(unixTime);
      if (hour(unixTime) == 0) localTime = "12";
    }
  }
  localTime += ":";
  if (minute(unixTime) < 10) localTime += "0";
  localTime += minute(unixTime);

  return localTime;
}

/***************************************************************************************
**   Convert the time to the seconds of the day
***************************************************************************************/
uint32_t daySeconds(time_t unixTime) {
  return (hour(unixTime) * 3600) + (minute(unixTime) * 60) + (second(unixTime));
}

/***************************************************************************************
**  Convert Unix time to a local date + time string "Oct 16 17:18"
***************************************************************************************/
String strDate(time_t unixTime) {
  String localDate = "";
  localDate += shortMonth[month(unixTime) - 1];
  localDate += " ";
  localDate += day(unixTime);
  localDate += " " + strTime(unixTime);
  return localDate;
}

/***************************************************************************************
**                          Draw WiFi signal strength bars
***************************************************************************************/
void drawWiFiQuality() {
  const byte numBars = 5;
  const byte barWidth = 3;
  const byte barHeight = 20;
  const byte barSpace = 1;
  const byte barXPosBase = 215;
  const byte barYPosBase = 20;
  const uint16_t barColor = TFT_YELLOW;
  const uint16_t barBackColor = TFT_DARKGREY;

  int8_t quality = getWifiQuality();

  for (int8_t i = 0; i < numBars; i++) {
    byte barSpacer = i * barSpace;
    byte tempBarHeight = (barHeight / numBars) * (i + 1);
    for (int8_t j = 0; j < tempBarHeight; j++) {
      for (byte ii = 0; ii < barWidth; ii++) {
        byte nextBarThreshold = (i + 1) * (100 / numBars);
        byte currentBarThreshold = i * (100 / numBars);
        byte currentBarIncrements = (barHeight / numBars) * (i + 1);
        float rangePerBar = (100 / numBars);
        float currentBarStrength;
        if ((quality > currentBarThreshold) && (quality < nextBarThreshold)) {
          currentBarStrength = ((quality - currentBarThreshold) / rangePerBar) * currentBarIncrements;
        } else if (quality >= nextBarThreshold) {
          currentBarStrength = currentBarIncrements;
        } else {
          currentBarStrength = 0;
        }
        if (j < currentBarStrength) {
          tft.drawPixel((barXPosBase + barSpacer + ii) + (barWidth * i), barYPosBase - j, barColor);
        } else {
          tft.drawPixel((barXPosBase + barSpacer + ii) + (barWidth * i), barYPosBase - j, barBackColor);
        }
      }
    }
  }
}

int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) return 0;
  else if (dbm >= -50) return 100;
  else return 2 * (dbm + 100);
}

/***************************************************************************************
**                    Display Access Point setup screen
***************************************************************************************/
void configModeCallback(WiFiManager *myWiFiManager) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Access Point Active", SCREEN_W / 2, (SCREEN_H / 2) - 18, 2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(String(HOSTNAME), SCREEN_W / 2, SCREEN_H / 2, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.drawString("IP: 192.168.4.1", SCREEN_W / 2, (SCREEN_H / 2) + 18, 2);
  tft.unloadFont();
  delay(2000);
}

/***************************************************************************************
**                    Find city name for current active location
***************************************************************************************/
void findTheLocation() {
  if (runBdcOnce == false) {
    BDCcurrent = new BDC_current;
    // Look up whichever location is currently active
    String lat = altLocation ? latitude2 : latitude;
    String lon = altLocation ? longitude2 : longitude;
    bool BDCparsed = bcd.getBDCLocation(BDCcurrent, lat, lon, NOMINATIM_LANG);
    if (BDCparsed) {
      if (BDCcurrent->city.isEmpty() && !BDCcurrent->locality.isEmpty())
        BDCcurrent->city = BDCcurrent->locality;
      String cityName = BDCcurrent->city;
      delete BDCcurrent;
      if (cityName.length() > 26) cityName = "Local Weather";
      if (altLocation) theCityLocation2 = cityName;
      else theCityLocation = cityName;
      runBdcOnce = true;
    }
  }
}

/***************************************************************************************
**                     Re-fetch timezone offsets at 2am
**  Called once per hour when local hour == 2.
**  Queries timeapi.io for both locations and saves to config if changed.
***************************************************************************************/
void checkTimezoneOffsets() {
  extern int32_t tzOffset;
  extern int32_t tzOffset2;

  Serial.println("2am TZ check — fetching offsets...");

  int32_t newOffset = fetchTZOffset(latitude, longitude, tzOffset);
  int32_t newOffset2 = fetchTZOffset(latitude2, longitude2, tzOffset2);

  bool changed = false;
  if (newOffset != tzOffset) {
    tzOffset = newOffset;
    changed = true;
    Serial.printf("TZ1 offset updated: %d\n", tzOffset);
  }
  if (newOffset2 != tzOffset2) {
    tzOffset2 = newOffset2;
    changed = true;
    Serial.printf("TZ2 offset updated: %d\n", tzOffset2);
  }

  if (changed) {
    saveConfig();
    drawTime();
    updateBacklight();
    Serial.println("TZ offsets updated and saved.");
  } else {
    Serial.println("TZ offsets unchanged.");
  }
}

/***************************************************************************************
**                          Auto-dim backlight at dusk/dawn
***************************************************************************************/
void updateBacklight() {
  uint8_t targetBrightness;

  if (autoDimDusk) {
    time_t local_time = toLocal(now());
    uint32_t currentSecond = (hour(local_time) * 3600) + (minute(local_time) * 60) + second(local_time);
    targetBrightness = (currentSecond >= sunrise && currentSecond < sunset) ? BL_DAY : blDusk;
  } else {
    targetBrightness = BL_DAY;
  }

  if (targetBrightness != ledcRead(TFT_BL)) {
    ledcWrite(TFT_BL, targetBrightness);
  }
}

/***************************************************************************************
**                          Show splash screen with credits
***************************************************************************************/
void showSplashScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(false);
  if (LittleFS.exists("/splash/OpenMeteo.jpg") == true) ui.drawJpeg("/splash/OpenMeteo.jpg", 0, 40);
  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_GOLD, TFT_BLACK);

  tft.drawString(creditOriginal, 120, 220);
  tft.drawString(creditBodmer, 120, 240);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(creditWabbit, 120, 260);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString(WABBIT_VERSION, 120, 280);

  // Wait for touch or 15 second timeout then return to main display
  uint32_t start = millis();
  while (millis() - start < 15000) {
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      touchscreen.getPoint();
      while (touchscreen.touched()) delay(10);
      break;
    }
    delay(50);
  }
  tft.unloadFont();
  booted = true;
  tft.fillScreen(TFT_BLACK);
}

/**The MIT License (MIT)
  Copyright (c) 2015 by Daniel Eichhorn
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYBR_DATUM HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  See more at http://blog.squix.ch
*/

//  Changes made by Bodmer:
//  Minor changes to text placement and auto-blanking old text with background colour padding
//  Moon phase text added
//  Forecast text lines automatically split onto two lines at a central space
//  Time printed with colons aligned
//  Min and max forecast temperatures spaced out
//  New smart splash startup screen and updated progress messages
//  Display does not need to be blanked between updates
//  Icons nudged about slightly to add wind direction + speed
//  Barometric pressure added
//  Adapted to use the OpenWeather library
//  Moon phase/rise/set replaced with cloud cover and humidity
//  Created and added new 100x100 and 50x50 pixel weather icons
//  Add moon icons, eliminate all downloads of icons
//  Adapted to use anti-aliased fonts, tweaked coords
//  Added forecast for 4th day
//  Added cloud cover and humidity in lieu of Moon rise/set
//  Adapted to be compatible with ESP32

/*  Changes made by WabbitGuy V1.4:

Adapted to use Open-Meteo for weather information (updates every 15 minutes; no subscription required)
Added Nominatim to use for location of your weather station (no subscription required)
Added options to display what you want, like 6hr forecast for POP%, Temp, Dew Point
Added thresholds for temperature ranges with selectable colours
Added thresholds for POP% ranges with selectable colours
Added UVindex (current)
Added display dimming for Sunrise/Sunset
Full HTML browser support for user adjustable settings (including long/lat)
Added multilingual support for English, French, German, Turkish, Spanish, Dutch, Portuguese
Added flipped MoonPhase icons for south of equator
Added function to see 7 day forecast, tap the weekly row to see more, tap to get back or times out in 15 seconds
Added dual location support with automatic timezone offset via timeapi.io
Timezone offsets self-correct at 2am daily — no hardcoding required

New in v1.5.0:
Dual location support — tap the today big weather icon to toggle between TZ1 and TZ2
Automatic timezone detection via timeapi.io
Self-correcting timezone at 2am daily for both locations
Map search box — type "London UK" and the map jumps there
Correct local time for whichever location is active
Correct dusk dimming for whichever location is active
Forecast start today/tomorrow option for max 8 days (from current day)
Timezone library eliminated entirely
Language selection moved to All_Settings.h where it belongs
Fixed the daily forecast to read HIGH LOW to match weather outlets
*/
