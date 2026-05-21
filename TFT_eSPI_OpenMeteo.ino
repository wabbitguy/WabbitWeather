//  Example from OpenWeather library: https://github.com/Bodmer/OpenWeather
//  Adapted by Bodmer to use the TFT_eSPI library:  https://github.com/Bodmer/TFT_eSPI

//  This sketch is compatible with the ESP8266

//                           >>>  IMPORTANT  <<<
//         Modify setup in All_Settings.h tab to configure your location etc

//                >>>  EVEN MORE IMPORTANT TO PREVENT CRASHES <<<
//>>>>>>  For ESP8266 set SPIFFS to at least 2Mbytes before uploading files  <<<<<<

//  ESP8266/ESP32 pin connections to the TFT are defined in the TFT_eSPI library.

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

// Modifications to use Open Meteo by WabbitGuy (Mel)


#include <FS.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define AA_FONT_SMALL "fonts/NotoSansBold15"  // 15 point sans serif bold
#define AA_FONT_LARGE "fonts/NotoSansBold36"  // 36 point sans serif bold
//
//
/***************************************************************************************
**                          Load the libraries and settings
***************************************************************************************/
#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h>  // https://github.com/Bodmer/TFT_eSPI

// Additional functions
#include "GfxUi.h"  // Attached to this sketch
//#include "SPIFFS_Support.h" // Attached to this sketch

#include <WiFi.h>
#include <WiFiManager.h>

// check All_Settings.h for adapting to your needs
#include "All_Settings.h"

#include <JSON_Decoder.h>  // https://github.com/Bodmer/JSON_Decoder

#include "OpenMeteo.h"  // Latest here: https://github.com/Bodmer/OpenWeather
#include "Nominatim.h"  // for returning city from long.lat

#include "NTP_Time.h"   // Attached to this sketch, see that tab for library needs
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

GfxUi ui = GfxUi(&tft);  // Jpeg and bmpDraw functions TODO: pull outside of a class

long lastDownloadUpdate = millis();
boolean runOnce = false;
boolean runBdcOnce = false;  // only need a location once..

#define TFT_BL 21         // CYD backlight pin
#define TFT_BL_FREQ 5000  // PWM frequency Hz
#define TFT_BL_RES 8      // 8-bit = 0-255

#define BL_DAY 255  // full brightness during daylight
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
void apModeCallback(WiFiManager *myWiFiManager);
void handleTempRangeColour(uint16_t theVal);
void handlePrecipRange(uint16_t theVal);
void findTheLocation();  // find the location based on long/lat
void updateBacklight();  // handles the display dimming at night

/***************************************************************************************
**                          Setup
***************************************************************************************/
void setup() {
  Serial.begin(115200);

  tft.begin();
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

  tft.drawString("Original by: blog.squix.org", 120, 260);
  tft.drawString("Adapted by: Bodmer", 120, 280);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("Open-Meteo by: Wabbitguy", 120, 300);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  delay(3000);

  tft.fillRect(0, 208, 240, 320 - 206, TFT_BLACK);

  tft.drawString("Connecting to WiFi", 120, 240);
  tft.setTextPadding(240);  // Pad next drawString() text to full width to over-write old text
  //
  //WiFiManager
  WiFiManager wm;
  wm.setAPCallback(apModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240);        // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 220);  // Clear line above using set padding width
  tft.drawString("Connected to WiFi", 120, 240);
  delay(1000);
  //
  // Fetch the time
  //  udp.begin(localPort);
  // Seed Random With vVlues Unique To This Device
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  uint32_t seed1 =
    (macAddr[5] << 24) | (macAddr[4] << 16) | (macAddr[3] << 8) | macAddr[2];
  randomSeed(analogRead(A0));
  // randomSeed(WiFi.localIP());// just the IP address and name?
  String ipaddress = WiFi.localIP().toString();
  //  ipaddress.toCharArray(theIPAddress, ipaddress.length() + 1); // stored so we can get the IP
  localPort = random(1024, 65535);
  udp.begin(localPort);
  //
  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240);        // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 220);  // Clear line above using set padding width
  tft.drawString("Fetching weather data...", 120, 240);
  delay(2000);
  //
  WiFi.hostname(HOSTNAME);  //
  syncTime(tzIndex);        // now we go look for a time server
  //
  tft.unloadFont();
  //
  // this section uses the long and lat to return the city and region for display
  // findTheLocation();// go find the location we're interested in

  ow.partialDataSet(false);  // Collect a full set of the data available (false)

  // LittleFS.remove(CONFIG_FILE);  // just for testing
  loadConfig();
  startWebConfig();
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
      syncTime(tzIndex);
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
}
//
/***************************************************************************************
**                          Fetch the weather data  and update screen
***************************************************************************************/
// Update the Internet based information and update screen
void updateData() {
  // booted = true;  // Test only
  // booted = false; // Test only
  uint16_t smurf;
  bool parsed;
  //
  if (runBdcOnce == false) tft.fillScreen(TFT_BLACK);
  //
  tft.loadFont(AA_FONT_SMALL, LittleFS);

  if (booted) drawProgress(20, "Updating time...");
  else fillSegment(22, 22, 0, (int)(20 * 3.6), 16, TFT_NAVY);

  if (booted) drawProgress(50, "Updating conditions...");
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
                      //
  parsed = ow.getForecast(current, hourly, daily, latitude, longitude, units);

  // try again in 30 seconds if it fails
  if (!parsed) {
    Serial.println("Fetch failed, retrying in 60 seconds...");
    delay(60000);
    parsed = ow.getForecast(current, hourly, daily, latitude, longitude, units);
  }

  printWeather();  // For debug, turn on output with #define SERIAL_MESSAGES

  if (booted) {
    drawProgress(100, "Done...");
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

    // Font ASCII code 0xB0 is a degree symbol, but o used instead in small font
    tft.setTextPadding(tft.textWidth(" -88"));  // Max width of values

    //    String weatherText = "";
    //    weatherText = (int16_t) current->temp;  // Make it integer temperature
    //    tft.drawString(weatherText, 215, 95); //  + "°" symbol is big... use o in small font
    //
    String weatherText = "";       // this draws the temperature number
    smurf = current->temp + .5;    // Make it integer temperature
    weatherText = String(smurf);   // Make it round temperature
    handleTempRangeColour(smurf);  // go set the colour of the display to match the range
    if (runBdcOnce == false) {     // if we didn't get the info the first time, try again
      findTheLocation();           // based on long and lat, otherwise "unknown"
    }
    tft.drawString(weatherText, 215, 100);  //  + "°" symbol is big... use o in small font
    //
  } else {
    Serial.println("Failed to get weather");
  }

  // Delete to free up space
  //  delete current;
  //  delete hourly;
  //  delete daily;

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
  uint32_t currentSecond;
  tft.loadFont(AA_FONT_LARGE, LittleFS);

  // Convert UTC to local time, returns zone code in tz1_Code, e.g "GMT"
  time_t local_time = (*tz).toLocal(now(), &tcr);

  String timeNow = "";

  if (show24Hour == true) {  // we want to show 24 hour time?
    if (hour(local_time) < 10) timeNow += "0";
  } else {  // we want to show normal time 12 hour format
    if (hour(local_time) > 12) {
      timeNow += hour(local_time) - 12;  // make it a normal time
    } else {
      timeNow += hour(local_time);                // else just use it (dont make it digital)
      if (hour(local_time) == 0) timeNow = "12";  // at midnight we make it 12:xx
    }
  }
  //  timeNow += hour(local_time);
  timeNow += ":";
  if (minute(local_time) < 10) timeNow += "0";
  timeNow += minute(local_time);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 44:44 "));  // String width + margin
  tft.drawString(timeNow, 120, 53);
  //
  drawSeparator(51);

  tft.setTextPadding(0);

  tft.unloadFont();
}

/***************************************************************************************
**                          Draw the current weather
***************************************************************************************/
void drawCurrentWeather() {
  time_t local_time = (*tz).toLocal(now(), &tcr);
  String date = "Updated: " + strDate(local_time);

  int x, y, uvMax, temp, theWidth;  // used for drawing the UVindex
  uint16_t theColour;
  String weatherText = "None";

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Updated: Mmm 44 44:44 "));  // String width + margin
  tft.drawString(date, 120, 16);

  String weatherIcon = "";

  String currentSummary = current->main;
  currentSummary.toLowerCase();

  weatherIcon = getMeteoconIcon(current->id, true);

  //uint32_t dt = millis();
  ui.drawBmp("/icon/" + weatherIcon + ".bmp", 0, 62);
  //Serial.print("Icon draw time = "); Serial.println(millis()-dt);

  // Weather Text
  weatherText = current->description;

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);

  int splitPoint = 0;
  int xpos = 235;
  splitPoint = splitIndex(weatherText);

  tft.setTextPadding(xpos - 100);  // xpos - icon width
  if (splitPoint) tft.drawString(weatherText.substring(0, splitPoint), xpos, 86);
  else tft.drawString(" ", xpos, 74);
  tft.drawString(weatherText.substring(splitPoint), xpos, 100);  // clouds, rain, etc

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(0);
  if (units == "metric") tft.drawString("oC", 237, 105);
  else tft.drawString("oF", 237, 105);

  //Temperature large digits added in updateData() to save swapping font here

  tft.setTextColor(labelColour, TFT_BLACK);
  weatherText = (uint16_t)current->wind_speed;

  if (units == "metric") weatherText += " m/s";
  else weatherText += " mph";

  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(tft.textWidth("888 m/s"));  // Max string length?
  tft.drawString(weatherText, 124, 136);

  if (showBarometric) {
    if (units == "imperial") {
      weatherText = current->pressure;
      weatherText += " in";
    } else {
      weatherText = (uint16_t)current->pressure;
      weatherText += " hPa";
    }

    tft.setTextDatum(TR_DATUM);
    tft.setTextPadding(tft.textWidth(" 8888hPa"));  // Max string length?
    tft.drawString(weatherText, 230, 136);
  }

  int windAngle = (current->wind_deg + 22.5) / 45;
  if (windAngle > 7) windAngle = 0;
  String wind[] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
  ui.drawBmp("/wind/" + wind[windAngle] + ".bmp", 101, 86);
  //
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(theCityLocation));  // simple city name
  tft.drawString(theCityLocation, 120, 70);
  // Serial.println(theCityLocation);
  //
  if (showUVindex) {
    theColour = TFT_GREEN;
    uvMax = (current->uvi + .5);  // the number of squares to draw (0 to ##)
    //  Serial.print("uvMAX : ") ; Serial.println(uvMax);
    // Serial.print("Current : ") ; Serial.println(current->uvi);
    //  uvMax = 9;// we only draw 0 to 9 which translates to 0 to 11...
    if (uvMax > 9) uvMax = 9;  // maximum boxes to draw (10)
    for (temp = 0; temp <= 9; temp++) {
      x = 178 + (temp * 6);  // this is where to start the X point for the UV
      y = 40 - (temp * 2);   // where the Y point start is
      if (temp <= uvMax) {   // we change colours to the max of the UVindex
        switch (temp) {
          case 0:
            theColour = TFT_GREEN;
            break;
          case 1:
            theColour = TFT_GREEN;
            break;
          case 2:
            theColour = TFT_GREEN;
            break;
          case 3:
            theColour = TFT_YELLOW;
            break;
          case 4:
            theColour = TFT_YELLOW;
            break;
          case 5:
            theColour = TFT_YELLOW;
            break;
          case 6:
            theColour = TFT_ORANGE;
            break;
          case 7:
            theColour = TFT_ORANGE;
            break;
          case 8:
            theColour = TFT_RED;
            break;
          case 9:
            theColour = TFT_RED;
            break;
          case 10:
            theColour = TFT_RED;
            break;
          default:
            theColour = TFT_DARKGREY;
            break;
        }
      } else {
        theColour = TFT_DARKGREY;
      }
      tft.fillRect(x, y, 4, 8 + (temp * 2), theColour);  // width, height
    }
  }
  //
  drawSeparator(153);

  tft.setTextDatum(TL_DATUM);  // Reset datum to normal
  tft.setTextPadding(0);       // Reset padding width to none
}

/***************************************************************************************
**                          Draw the 4 forecast columns
***************************************************************************************/
// draws the three forecast columns
void drawForecast() {
  int8_t dayIndex = 1;

  drawForecastDetail(8, 171, dayIndex++);
  drawForecastDetail(66, 171, dayIndex++);   // was 95
  drawForecastDetail(124, 171, dayIndex++);  // was 180
  drawForecastDetail(182, 171, dayIndex);    // was 180
  drawSeparator(171 + 69);
}

/***************************************************************************************
**                          Draw 1 forecast column at x, y
***************************************************************************************/
// helper for the forecast columns
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) {
  uint16_t precipProbability;

  if (dayIndex >= MAX_DAYS) return;

  // Serial.print("dayIndex: "); Serial.print(dayIndex);
  // Serial.print(" pop: "); Serial.print(daily->pop[dayIndex] * 100);
  // Serial.print(" dt: "); Serial.println(strDate(daily->dt[dayIndex]));
  // Serial.print("dt raw: "); Serial.println(daily->dt[1]);

  //String day = shortDOW[weekday((*tz).toLocal(daily->dt[dayIndex], &tcr))];
  String day = shortDOW[weekday(daily->dt[dayIndex])];
  day.toUpperCase();

  tft.setTextDatum(BC_DATUM);

  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("WWW"));
  tft.drawString(day, x + 25, y);  // display the day of the week

  tft.setTextPadding(tft.textWidth("-88"));
  String highTemp = String(daily->temp_max[dayIndex], 0);
  String lowTemp = String(daily->temp_min[dayIndex], 0);
  handleTempRangeColour(daily->temp_min[dayIndex]);  // set the low colour first
  tft.drawString(lowTemp, x + 12, y + 14);           //
  handleTempRangeColour(daily->temp_max[dayIndex]);  // set high colour next
  tft.drawString(highTemp, x + 37, y + 14);          //

  String weatherIcon = getMeteoconIcon(daily->id[dayIndex], false);

  ui.drawBmp("/icon50/" + weatherIcon + ".bmp", x, y + 10);  // was 18
  //
  // added percentage of precip

  if (showPrecipProbability) {
    precipProbability = daily->pop[dayIndex] * 100;  // API returns a floating point number
    String myPOP = String(precipProbability);
    myPOP += "%";  // add a percent sign
    tft.setTextPadding(tft.textWidth(" 88% "));
    handlePrecipRange(precipProbability);   // set the colour according to the range
    tft.drawString(myPOP, x + 25, y + 72);  // draw the POP
  }
  //
  tft.setTextPadding(0);  // Reset padding width to none
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
  tft.drawString("Pop%:", theX - 12, theY);  // line labels
  tft.drawString("Temp:", theX - 12, theY + theOffsetY);
  tft.drawString(" Dew:", theX - 12, theY + (theOffsetY * 2));
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  tft.drawString("6Hr Forecast", 120, theY - 20);
  tft.setTextDatum(BL_DATUM);
  //
  drawSeparator(260);  // top of the info

  for (temp = 0; temp < 6; temp++) {
    //
    precipProbability = hourly->pop[temp] * 100;
    if (precipProbability >= 100) {
      precipProbability = 99;
    }
    //
    handlePrecipRange(precipProbability);  // set the colour according to the range
    if (precipProbability < 10) {          // we need to pad single digit numbers
      result = "  ";
      result += String(precipProbability);
    } else {
      result = String(precipProbability);  // else use two digits
    }
    //
    tft.drawString(result, theX + (temp * 26), theY);  // show the POP
    //
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // smurf = hourly->temp[temp] + .5;  // we need this to round up the float
    smurf = hourly->temp[temp] + .5;  // round her up
    if (smurf >= 100) {               // look for 3 digit numbers
      smurf = 99;
    }
    handleTempRangeColour(smurf);  // go set the colour of the display to match the range
    //
    if (smurf < 10) {  // we need to pad single digit numbers
      result = "  ";
      result += String(smurf);
    } else {
      result = String(smurf);  // else use two digits
    }
    tft.drawString(result, theX + (temp * 26), theY + theOffsetY);  // show the result temperature
    //
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // smurf = hourly->dew_point[temp] + .5;  // we need this to round up the float
    smurf = hourly->dew_point[temp] + .5;  // round her up
    if (smurf >= 100) {                    // look for 3 digit numbers
      smurf = 99;
      tft.setTextColor(TFT_RED, TFT_BLACK);
    }
    if (smurf < 10) {  // we need to pad single digit numbers
      result = "  ";
      result += String(smurf);
    } else {
      result = String(smurf);  // else use two digits
    }
    tft.drawString(result, theX + (temp * 26), theY + (theOffsetY * 2));  // show the result dewpoint
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

  time_t local_time = (*tz).toLocal(current->dt, &tcr);
  uint16_t y = year(local_time);
  uint8_t m = month(local_time);
  uint8_t d = day(local_time);
  uint8_t h = hour(local_time);
  int ip;
  uint8_t icon = moon_phase(y, m, d, h, &ip);

  tft.drawString(moonPhase[ip], 120, 319);
  ui.drawBmp("/moon/moonphase_L" + String(icon) + ".bmp", 120 - 30, 318 - 16 - 60);
  //tft.fillCircle(120, 272, 30, TFT_WHITE);  // same centre and radius as the 60x60 BMP (testing)

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.setTextPadding(0);  // Reset padding width to none
  tft.drawString(sunStr, 44, 270);

  // Add Rise:/Set: labels
  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(astrologyColour, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("Set:"));
  tft.drawString("Rise:", 43, 290);
  tft.drawString("Set:", 43, 305);

  // tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("8:88 "));  // no leading space — max realistic width

  String rising = strTime(current->sunrise) + " ";
  int dt = rightOffset(rising, ":");  // Draw relative to colon to them aligned
  tft.drawString(rising, 55 + dt, 290);
  //
  sunrise = daySeconds(current->sunrise);  // trigger point for clock colour
  //
  String setting = strTime(current->sunset) + " ";
  dt = rightOffset(setting, ":");
  tft.drawString(setting, 55 + dt, 305);
  //
  sunset = daySeconds(current->sunset);  // trigger point for clock colour
  //
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

  tft.setTextPadding(0);  // Reset padding width to none
}
//
//
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
**                          Get the icon file name from the WMO weather code
**                          https://open-meteo.com/en/docs#weathervariables
***************************************************************************************/
/***************************************************************************************
**                Get the icon file name from the WMO weather code
**                Matches icon filenames in /icon/ and /icon50/ folders
***************************************************************************************/
const char *getMeteoconIcon(uint16_t id, bool today) {
  bool isNight = today && (current->dt < current->sunrise || current->dt > current->sunset);

  // Clear
  if (id == 0) return isNight ? "clear_night" : "sunny";
  if (id == 1) return isNight ? "mostly_clear_night" : "mostly_sunny";

  // Partly cloudy
  if (id == 2) return isNight ? "partly_cloudy_night" : "partly_cloudy";

  // Overcast
  if (id == 3) return "cloudy";

  // Fog
  if (id == 45 || id == 48) return "haze_fog_dust_smoke";

  // Drizzle light/moderate
  if (id == 51 || id == 53) return "drizzle";
  // Drizzle dense
  if (id == 55) return "drizzle";
  // Freezing drizzle
  if (id == 56 || id == 57) return "wintry_mix_rain_snow";

  // Rain slight
  if (id == 61) return "showers_rain";
  // Rain moderate
  if (id == 63) return "showers_rain";
  // Rain heavy
  if (id == 65) return "heavy_rain";
  // Freezing rain
  if (id == 66 || id == 67) return "sleet_hail";

  // Snow slight
  if (id == 71) return "flurries";
  // Snow moderate
  if (id == 73) return "heavy_snow";
  // Snow heavy
  if (id == 75) return "heavy_snow";
  // Snow grains
  if (id == 77) return "flurries";

  // Rain showers slight
  if (id == 80) return isNight ? "scattered_showers_night" : "scattered_showers_day";
  // Rain showers moderate
  if (id == 81) return isNight ? "scattered_showers_night" : "scattered_showers_day";
  // Rain showers heavy/violent
  if (id == 82) return "heavy_rain";

  // Snow showers
  if (id == 85) return "snow_showers_snow";
  if (id == 86) return "blizzard";

  // Thunderstorm
  if (id == 95) return isNight ? "isolated_scattered_tstorms_night" : "isolated_scattered_tstorms_day";
  // Thunderstorm with hail
  if (id == 96 || id == 99) return "strong_tstorms";

  return "unknown";
} /***************************************************************************************
**                          Draw screen section separator line
***************************************************************************************/
// if you don't want separators, comment out the tft-line
void drawSeparator(uint16_t y) {
  tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
}

/***************************************************************************************
**                          Determine place to split a line line
***************************************************************************************/
// determine the "space" split point in a long string
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
// Calculate coord delta from end of text String to start of sub String contained within that text
// Can be used to vertically right align text so for example a colon ":" in the time value is always
// plotted at same point on the screen irrespective of different proportional character widths,
// could also be used to align decimal points for neat formatting
int rightOffset(String text, String sub) {
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(index));
}

/***************************************************************************************
**                          Left side offset to a character
***************************************************************************************/
// Calculate coord delta from start of text String to start of sub String contained within that text
// Can be used to vertically left align text so for example a colon ":" in the time value is always
// plotted at same point on the screen irrespective of different proportional character widths,
// could also be used to align decimal points for neat formatting
int leftOffset(String text, String sub) {
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(0, index));
}

/***************************************************************************************
**                          Draw circle segment
***************************************************************************************/
// Draw a segment of a circle, centred on x,y with defined start_angle and subtended sub_angle
// Angles are defined in a clockwise direction with 0 at top
// Segment has radius r and it is plotted in defined colour
// Can be used for pie charts etc, in this sketch it is used for wind direction
#define DEG2RAD 0.0174532925  // Degrees to Radians conversion factor
#define INC 2                 // Minimum segment subtended angle and plotting angle increment (in degrees)
void fillSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int colour) {
  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x1 = sx * r + x;
  uint16_t y1 = sy * r + y;

  // Draw colour blocks every INC degrees
  for (int i = start_angle; i < start_angle + sub_angle; i += INC) {

    // Calculate pair of coordinates for segment end
    int x2 = cos((i + 1 - 90) * DEG2RAD) * r + x;
    int y2 = sin((i + 1 - 90) * DEG2RAD) * r + y;

    tft.fillTriangle(x1, y1, x2, y2, x, y, colour);

    // Copy segment end to segment start for next segment
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
  Serial.println(current->humidity);  // percentage
  Serial.print("pressure           : ");
  Serial.println(current->pressure);  // pressure sea level hPa
  Serial.print("wind_speed         : ");
  Serial.println(current->wind_speed);
  Serial.print("wind_deg           : ");
  Serial.println(current->wind_deg);
  Serial.print("clouds             : ");
  Serial.println(current->clouds);  // current cloud %
  Serial.print("UVIndex            : ");
  Serial.println(current->uvi);  // currect UV index %...
  Serial.print("id                 : ");
  Serial.println(current->id);
  Serial.println();

  Serial.println("###############  Daily weather  ###############\n");
  Serial.println();

  for (int i = 0; i < 5; i++) {
    Serial.print("dt (time)          : ");
    Serial.println(strDate(daily->dt[i]));
    Serial.print("id                 : ");
    Serial.println(daily->id[i]);
    Serial.print("temp_Max          : ");
    Serial.println(daily->temp_max[i]);
    Serial.print("temp_min           : ");
    Serial.println(daily->temp_min[i]);
    Serial.print("Pressure           : ");
    Serial.println(daily->pressure[i]);
    Serial.print("Clouds             : ");
    Serial.println(daily->clouds[i]);  // clouds are daily total
    Serial.print("UV Index           : ");
    Serial.println(daily->uvi[i]);  // this is taken at midnight so no UVI
    Serial.print("POP                : ");
    Serial.println(daily->pop[i]);  // this is correct for a future forecast
    Serial.println();
  }
  //
  Serial.println("###############  Hourly weather  ###############\n");
  Serial.println();

  for (int i = 0; i < 5; i++) {
    Serial.print("dt (time)          : ");
    Serial.println(strDate(hourly->dt[i]));
    Serial.print("temp               : ");
    Serial.println(hourly->temp[i]);
    //Serial.print("temp_min           : "); Serial.println(daily->temp_min[i]);
    //Serial.print("Pressure           : "); Serial.println(daily->pressure[i]);
    Serial.print("Dew Point          : ");
    Serial.println(hourly->dew_point[i]);  // clouds are daily total
    //Serial.print("UV Index           : "); Serial.println(daily->uvi[i]);// this is taken at midnight so no UVI
    Serial.print("POP                : ");
    Serial.println(hourly->pop[i]);  // this is correct for a future forecast
    Serial.println();
  }
#endif
}
/***************************************************************************************
**             Convert Unix time to a "local time" time string "12:34"
***************************************************************************************/
String strTime(time_t unixTime) {
  // Expects a time already converted to local — no tz conversion here
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
//
/***************************************************************************************
   Convert the time to the seconds of the day
***************************************************************************************/
uint32_t daySeconds(time_t unixTime) {
  return (hour(unixTime) * 3600) + (minute(unixTime) * 60) + (second(unixTime));
}
//
/***************************************************************************************
**  Convert Unix time to a local date + time string "Oct 16 17:18", ends with newline
***************************************************************************************/
String strDate(time_t unixTime) {
  // Open-Meteo returns local time already — no tz conversion needed
  String localDate = "";
  localDate += monthShortStr(month(unixTime));
  localDate += " ";
  localDate += day(unixTime);
  localDate += " " + strTime(unixTime);
  return localDate;
}
//
void drawWiFiQuality() {
  const byte numBars = 5;        // set the number of total bars to display
  const byte barWidth = 3;       // set bar width, height in pixels
  const byte barHeight = 20;     // should be multiple of numBars, or to indicate zero value
  const byte barSpace = 1;       // set number of pixels between bars
  const byte barXPosBase = 215;  // set the baseline X-pos for drawing the bars
  const byte barYPosBase = 20;   // set the baseline Y-pos for drawing the bars
  const uint16_t barColor = TFT_YELLOW;
  const uint16_t barBackColor = TFT_DARKGREY;

  int8_t quality = getWifiQuality();

  // tft.setFont(&Droid_Sans_10);
  //  ui.setTextAlignment(RIGHT);
  // tft.setTextColor(ILI9341_YELLOW);
  //  ui.drawString(220, 10,  String(quality) + "%");

  for (int8_t i = 0; i < numBars; i++) {  // current bar loop
    byte barSpacer = i * barSpace;
    byte tempBarHeight = (barHeight / numBars) * (i + 1);
    for (int8_t j = 0; j < tempBarHeight; j++) {  // draw bar height loop
      for (byte ii = 0; ii < barWidth; ii++) {    // draw bar width loop
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
// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}
//
//To Display <Setup> if not connected to AP
void apModeCallback(WiFiManager *myWiFiManager) {
  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240);        // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 220);  // Clear line above using set padding width
  tft.drawString("Setup 192.168.4.1", 120, 240);
  delay(2000);
}
//uses the long and lat variables to find the city location name
void findTheLocation() {
  String temp;
  // Serial.println("BCD IN");
  //bool BDCparsed;
  if (runBdcOnce == false) {
    BDCcurrent = new BDC_current;  // create the instance
    bool BDCparsed = bcd.getBDCLocation(BDCcurrent, latitude, longitude, language);
    if (BDCparsed) {
      theCityLocation = BDCcurrent->city;                                    // + ", " + temp;// Vancouver, BC for example
      delete BDCcurrent;                                                     // and then free up the memory
      runBdcOnce = true;                                                     // we only need this info once
      if (theCityLocation.length() > 26) theCityLocation = "Local Weather";  // when we get back a long name
    }
  }
  // Serial.println("BCD OUT");
}

void updateBacklight() {
  if (autoDimDusk) {
    time_t local_time = (*tz).toLocal(now(), &tcr);
    uint32_t currentSecond = (hour(local_time) * 3600) + (minute(local_time) * 60) + second(local_time);
    if (currentSecond >= sunrise && currentSecond < sunset) {
      ledcWrite(TFT_BL, BL_DAY);
    } else {
      ledcWrite(TFT_BL, blDusk);
    }
  } else {
    ledcWrite(TFT_BL, BL_DAY);
  }
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

//  Minor changes to text placement and auto-blanking out old text with background colour padding
//  Moon phase text added (not provided by OpenWeather)
//  Forecast text lines are automatically split onto two lines at a central space (some are long!)
//  Time is printed with colons aligned to tidy display
//  Min and max forecast temperatures spaced out
//  New smart splash startup screen and updated progress messages
//  Display does not need to be blanked between updates
//  Icons nudged about slightly to add wind direction + speed
//  Barometric pressure added

//  Adapted to use the OpenWeather library: https://github.com/Bodmer/OpenWeather
//  Moon phase/rise/set (not provided by OpenWeather) replace with  and cloud cover humidity
//  Created and added new 100x100 and 50x50 pixel weather icons, these are in the
//  sketch data folder, press Ctrl+K to view
//  Add moon icons, eliminate all downloads of icons (may lose server!)
//  Adapted to use anti-aliased fonts, tweaked coords
//  Added forecast for 4th day
//  Added cloud cover and humidity in lieu of Moon rise/set
//  Adapted to be compatible with ESP32

/*  Changes made by WabbitGuy:

Adapted to use Open-Meteo for weather information (updates every 15 minutes; no subscription required)
Added Nominatim to use for location of your weather station (no subscription required)
Added options to display what you want, like 6hr forecast for POP%, Temp, Dew Point
Added thresholds for temperature ranges with selectable colours
Added thresholds for POP% ranges with selectable colours
Added UVindex (current)
Added display dimming for Sunrise/Sunset
Full HTML browser support for user adjustable settings (including long/lat)
*/