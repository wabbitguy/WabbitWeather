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

// Modifications to use Open Meteo by WabbitGuy (Mel)

#include <FS.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define AA_FONT_SMALL "fonts/NotoSansBold15"  // 15 point sans serif bold
#define AA_FONT_LARGE "fonts/NotoSansBold36"  // 36 point sans serif bold

#define WABBIT_VERSION "Version: 1.7.0"

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

#include "All_Settings.h"
#include "Translation.h"
#include "MeteoconIcons.h"  // Meteocons icon set, drawn from flash

#include <JSON_Decoder.h>  // https://github.com/Bodmer/JSON_Decoder

#include "OpenMeteo.h"  // Latest here: https://github.com/Bodmer/OpenWeather
#include "Nominatim.h"  // for returning city from long.lat

#include "NTP_Time.h"   // NTP sync + toLocal() helper + fetchTZOffset()
#include "MoonPhase.h"  // phase of the moon

#include "WebConfig.h"
volatile bool configUpdated = false;
uint8_t configChanges = 0;  // CFG_* bits from WebConfig.h — what the last Save changed

/***************************************************************************************
**                          Define the globals and class instances
***************************************************************************************/

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

OW_Weather ow;  // Weather forecast library instance

// Weather for the location on screen — these point into the wx* cache below
// (see selectLocationData()), so all the drawing code just uses them
OW_current *current;
OW_hourly *hourly;
OW_daily *daily;

// Per-location weather cache: [0] = location 1, [1] = location 2, [2] = scratch.
// Each download parses into the scratch set and is swapped into place only if
// it succeeds, so a failed or half-parsed download never wipes a location's
// last good data.
#define WX_SCRATCH 2
OW_current *wxCurrent[3];
OW_hourly *wxHourly[3];
OW_daily *wxDaily[3];

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

// When each location's cached weather was downloaded — 0 = nothing cached
static time_t lastGoodFetch[2] = { 0, 0 };
#define FAILURE_WARNING_SECS 3600UL  // 1 hour

// Cache slot for the location on screen
inline uint8_t activeSlot() {
  return altLocation ? 1 : 0;
}

#define LONG_PRESS_MS 2000  // hold the weather area this long to force a refresh

// Clock layout — hour, colon and minute are drawn as three independently
// anchored strings (see drawTime()) so the colon lives at a fixed pixel
// spot forever and blinkColon() never has to measure or move anything.
static const int16_t CLOCK_Y = 53;
static int16_t clockColonX = 120;  // fixed on-screen colon position
static bool blinkOn = true;
static uint32_t lastBlinkMs = 0;
#define COLON_BLINK_MS 1000  // once a second

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
void updateData(bool allowRetry = true);  // false = no blocking 60s retry (someone's at the screen)
void selectLocationData();  // point current/hourly/daily at the on-screen location's cache
void drawProgress(uint8_t percentage, String text);
void drawTime();
void drawCurrentWeather();
void drawForecast();
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
const char *getMeteoconIcon(uint16_t id, bool today);
const uint8_t *getMeteoconBitmap(uint16_t id, bool today, bool mini);
void drawAstronomy();
void drawSeparator(uint16_t y);
void drawProgressRing(uint8_t percent);  // smooth progress ring, top-left corner
void eraseProgressRing();
String strDate(time_t unixTime);
String strTime(time_t unixTime);
void printWeather(void);
int leftOffset(String text, String sub);
int rightOffset(String text, String sub);
int splitIndex(String text, int maxWidth = 0);
void drawWiFiQuality();
uint32_t daySeconds(time_t unixTime);
void handleAstronomyFrame();
void handleHourlyFrame();
void configModeCallback(WiFiManager *myWiFiManager);
void handleTempRangeColour(int16_t theVal);
void handlePrecipRange(uint16_t theVal);
int16_t roundTemp(float t);   // nearest whole degree, correct for negatives
void drawWeatherScreen();     // redraws everything from the weather already in memory
void applyConfigChanges(uint8_t changes);  // refresh only what a web Save actually changed
void redrawScreen(bool fetch);  // clear + redraw the main screen, downloading only if asked (or nothing in memory)
void convertWeatherUnits(bool toImperial);  // converts in-memory temps/wind after a units change
void findTheLocation();       // find the location based on long/lat
bool lookupCityName(bool alt);  // fetch + cache the city name for location 1 (false) or 2 (true)
void updateBacklight();       // handles the display dimming at night
void showSplashScreen();      // shows the credits and version
void checkTimezoneOffsets();  // re-fetch TZ offsets from timeapi.io at 2am
void drawColonGlyph(bool visible);  // draws/erases just the ":" at its fixed position
void blinkColon();            // toggles the clock's colon on/off, once a second
void drawUpdateFailedLine();  // shows "Failed: <time>" in place of "Updated:" when data is stale

/***************************************************************************************
**                          Setup
***************************************************************************************/
void setup() {
  Serial.begin(115200);

  tft.init();
  tft.invertDisplay(false);  // ST7789 panel needs inversion off; don't rely on whichever User_Setup.h is in the library
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
  wm.setHostname(HOSTNAME);  // name shown in the router's device list
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
  randomSeed(seed1);
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
  // Arm the NTP re-sync timer for 4hr + half a weather interval from now —
  // see NTP_Time.h for why that offset keeps it clear of the weather fetch
  // schedule permanently, not just by luck of boot timing.
  nextNTPSync = millis() + NTP_SYNC_OFFSET_MS + NTP_SYNC_INTERVAL_MS;
  //
  tft.unloadFont();

  ow.partialDataSet(false);  // Collect a full set of the data available (false)

  // LittleFS.remove(CONFIG_FILE);  // just for testing
  loadConfig();
  startWebConfig(HOSTNAME);

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

  // Re-sync with pool.ntp.org — its own millis() timer, deliberately not
  // tied to the wall clock or to location switches (touch handler doesn't
  // touch nextNTPSync). See NTP_Time.h for why the baked-in offset keeps
  // this permanently clear of the weather fetch above.
  if (millis() >= nextNTPSync) {
    syncTime();
    nextNTPSync += NTP_SYNC_INTERVAL_MS;
  }

  // If minute has changed then update the displayed clock
  if (booted || minute() != lastMinute) {
    // Update displayed time first as we may have to wait for a response
    drawTime();         // show the time
    updateBacklight();  // change the backlight if needed
    lastMinute = minute();
    drawWiFiQuality();  // update signal strength once a minute

    // At 2am re-fetch timezone offsets from timeapi.io to catch any DST changes —
    // this stays wall-clock based (DST happens at real calendar 2am), unlike NTP sync above.
    if (booted || lastHour != hour()) {
      if (hour(toLocal(now())) == 2) {
        checkTimezoneOffsets();
      }
      lastHour = hour();
    }

#ifdef SCREEN_SERVER
    screenServer();
#endif
  }
  // Blink the clock's colon (self-throttled to COLON_BLINK_MS internally)
  blinkColon();
  //
  if (runOnce == true && lastGoodFetch[activeSlot()] != 0 && second() != lastSecond) {
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
    uint8_t changes = configChanges;
    configChanges = 0;
    applyConfigChanges(changes);
  }

  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    bool weatherArea = (p.y >= 900 && p.y < 1800);
    bool longPress = false;

    // Wait for release, timing the hold. Released = no touch for 60ms, so a
    // momentary pressure dip during a long hold doesn't cut it short.
    uint32_t pressStart = millis();
    uint32_t lastTouched = pressStart;
    uint8_t ringShown = 0;
    while (millis() - lastTouched < 60) {
      if (touchscreen.touched()) lastTouched = millis();
      uint32_t held = millis() - pressStart;
      // Ring fills while the weather area is held — full = let go to refresh.
      // Waits 250ms so an ordinary tap doesn't flash it.
      if (weatherArea && !longPress && held >= 250) {
        uint8_t pct = min(held * 100 / LONG_PRESS_MS, (uint32_t)100);
        if (pct >= ringShown + 2 || pct == 100) {  // redraw every 2%, not every 10ms
          drawProgressRing(pct);
          ringShown = pct;
        }
        if (pct == 100) longPress = true;
      }
      delay(10);
    }

    if (p.y < 900) {
      // Clock area — show splash
      showSplashScreen();
    } else if (longPress) {
      // Long press on the weather area — fresh download for the location on screen
      Serial.println("Manual refresh");
      redrawScreen(true);
    } else if (weatherArea) {
      // Icon/weather area — toggle location
      altLocation = !altLocation;
      runBdcOnce = false;
      Serial.println(altLocation ? "Switched to TZ2" : "Switched to TZ1");
      // Cached weather younger than the refresh interval is shown as-is;
      // older (or none) is downloaded. The 30 min timer isn't touched either way.
      uint8_t slot = activeSlot();
      bool fresh = lastGoodFetch[slot] != 0 && (now() - lastGoodFetch[slot]) < UPDATE_INTERVAL_SECS;
      redrawScreen(!fresh);
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
void updateData(bool allowRetry) {
  bool parsed;
  uint8_t slot = activeSlot();
  //
  if (runBdcOnce == false) tft.fillScreen(TFT_BLACK);
  //
  tft.loadFont(AA_FONT_SMALL, LittleFS);

  if (booted) drawProgress(20, bootUpdatingTime);
  else drawProgressRing(20);

  if (booted) drawProgress(50, bootUpdatingConditions);
  else drawProgressRing(50);

  // Create the structures that hold the retrieved weather — one set per
  // location plus the scratch set downloads parse into
  if (runOnce == false) {
    for (uint8_t i = 0; i < 3; i++) {
      wxCurrent[i] = new OW_current;
      wxDaily[i] = new OW_daily;
      wxHourly[i] = new OW_hourly;
    }
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

  parsed = ow.getForecast(wxCurrent[WX_SCRATCH], wxHourly[WX_SCRATCH], wxDaily[WX_SCRATCH],
                          activeLat, activeLon, units);

  // Scheduled updates try again in 60 seconds if it fails. A touch or Save
  // doesn't — someone's standing at the screen, which would be frozen for that
  // minute — and shows this location's cached data instead.
  if (!parsed && allowRetry) {
    Serial.println("Fetch failed, retrying in 60 seconds...");
    delay(60000);
    parsed = ow.getForecast(wxCurrent[WX_SCRATCH], wxHourly[WX_SCRATCH], wxDaily[WX_SCRATCH],
                            activeLat, activeLon, units);
  }

  if (parsed) {
    // Fresh download goes into this location's slot; its old data becomes the next scratch
    std::swap(wxCurrent[slot], wxCurrent[WX_SCRATCH]);
    std::swap(wxHourly[slot], wxHourly[WX_SCRATCH]);
    std::swap(wxDaily[slot], wxDaily[WX_SCRATCH]);
    lastGoodFetch[slot] = now();
  }
  selectLocationData();

  if (parsed) printWeather();  // For debug, turn on output with #define SERIAL_MESSAGES

  if (booted) {
    drawProgress(100, bootDone);
    delay(2000);
    tft.fillScreen(TFT_BLACK);
  } else {
    drawProgressRing(100);
    eraseProgressRing();
  }

  // No second city lookup here if the one above failed — retrying within
  // a second could hit Nominatim while it's refusing us. The name stays
  // "Unknown" until the next scheduled update or touch switch tries again.

  if (!parsed) Serial.println("Failed to get weather");

  if (lastGoodFetch[slot] != 0) {
    // Fresh data, or this location's last good data if the download failed.
    // Short outages ride through silently — drawWeatherScreen() only adds the
    // red "Failed:" line once that data is over an hour old.
    drawWeatherScreen();
  } else {
    drawUpdateFailedLine();  // nothing cached for this location to fall back on
  }

  tft.unloadFont();
}

/***************************************************************************************
**          Point current/hourly/daily at the on-screen location's cache
***************************************************************************************/
void selectLocationData() {
  uint8_t slot = activeSlot();
  current = wxCurrent[slot];
  hourly = wxHourly[slot];
  daily = wxDaily[slot];
}

/***************************************************************************************
**                Draw the whole weather screen from the data in memory
**  Shared by updateData() (fresh download, or cache after a failed one) and
**  redrawScreen() (Save, splash, touch switch to a location with fresh cache).
**  Expects AA_FONT_SMALL loaded on entry; leaves no font loaded.
***************************************************************************************/
void drawWeatherScreen() {
  selectLocationData();  // altLocation may have just flipped
  drawCurrentWeather();
  drawForecast();
  drawAstronomy();

  // A redraw from old data still has to admit it's old
  if ((now() - lastGoodFetch[activeSlot()]) > FAILURE_WARNING_SECS) {
    drawUpdateFailedLine();
  }

  tft.unloadFont();

  // Update the temperature here so we don't need to keep
  // loading and unloading font which takes time
  tft.loadFont(AA_FONT_LARGE, LittleFS);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  // " -88" (64px) also covers 3 digits ("105" is 63px) — any wider and it
  // runs into the wind arrow, hence the -99 floor
  tft.setTextPadding(tft.textWidth(" -88"));

  int16_t currentTemp = constrain(roundTemp(current->temp), -99, 199);
  handleTempRangeColour(currentTemp);
  tft.drawString(String(currentTemp), 215, 100);

  tft.setTextPadding(0);
  tft.unloadFont();
}

/***************************************************************************************
**                Refresh only what a web Save actually changed
**  Coordinates are what matter — the TZ offset comes along with them. The
**  on-screen location moving needs a new name and new weather; the other one
**  only needs its name cached, its weather comes when a touch switches to it.
**  A units change converts both locations' cached weather; the next download
**  of each brings in the real values anyway.
***************************************************************************************/
void applyConfigChanges(uint8_t changes) {
  // Coordinates changed → that location's cached city name and weather are stale
  if (changes & CFG_LOC1_MOVED) {
    theCityLocation = "Unknown";
    lastGoodFetch[0] = 0;
  }
  if (changes & CFG_LOC2_MOVED) {
    theCityLocation2 = "Unknown";
    lastGoodFetch[1] = 0;
  }

  bool activeMoved = changes & (altLocation ? CFG_LOC2_MOVED : CFG_LOC1_MOVED);
  bool otherMoved  = changes & (altLocation ? CFG_LOC1_MOVED : CFG_LOC2_MOVED);

  // Off-screen location — cache its new name now. A failed lookup stays
  // "Unknown" and gets retried when a touch switches to it.
  if (otherMoved) lookupCityName(!altLocation);

  // Whatever's still cached (a moved location's was just dropped above)
  if (changes & CFG_UNITS_CHANGED) convertWeatherUnits(units == "imperial");

  // New place on screen — full fetch, which arrives in the new units
  if (activeMoved) runBdcOnce = false;
  redrawScreen(activeMoved);
}

/***************************************************************************************
**                Clear and redraw the whole main screen
**  fetch = false redraws the on-screen location from its cache (Save, leaving
**  the splash, touch switch to fresh cache); it only downloads if that
**  location has nothing cached. fetch = true always downloads first.
**  Never the blocking 60s retry — these are all someone at the screen.
***************************************************************************************/
void redrawScreen(bool fetch) {
  tft.fillScreen(TFT_BLACK);  // a Save may have switched sections off (UV, pressure...)
  if (fetch || lastGoodFetch[activeSlot()] == 0) {
    updateData(false);
  } else {
    findTheLocation();  // only goes to Nominatim if this location's name is still "Unknown"
    tft.loadFont(AA_FONT_SMALL, LittleFS);
    drawWeatherScreen();
  }

  drawTime();         // redraw clock immediately without waiting for next minute
  updateBacklight();  // change the backlight if needed
  drawWiFiQuality();  // the screen clear above wiped the signal bars
}

/***************************************************************************************
**                Convert the weather in memory after a units change
**  Only the fields Open-Meteo is asked for in the chosen units (temperature
**  and wind). Pressure always arrives in hPa, so it needs nothing here.
**  Converts every location that has weather cached.
***************************************************************************************/
static float convertTemp(float t, bool toImperial) {
  return toImperial ? (t * 9.0 / 5.0) + 32.0 : (t - 32.0) * 5.0 / 9.0;
}

void convertWeatherUnits(bool toImperial) {
  const float MPH_PER_MS = 2.23694;

  for (uint8_t slot = 0; slot < 2; slot++) {
    if (lastGoodFetch[slot] == 0) continue;  // nothing cached for this location
    OW_current *c = wxCurrent[slot];
    OW_hourly *h = wxHourly[slot];
    OW_daily *d = wxDaily[slot];

    c->temp       = convertTemp(c->temp, toImperial);
    c->wind_speed = toImperial ? c->wind_speed * MPH_PER_MS
                               : c->wind_speed / MPH_PER_MS;

    for (uint8_t i = 0; i < MAX_HOURS; i++) {
      h->temp[i]      = convertTemp(h->temp[i], toImperial);
      h->dew_point[i] = convertTemp(h->dew_point[i], toImperial);
    }

    for (uint8_t i = 0; i < MAX_DAYS; i++) {
      d->temp_max[i] = convertTemp(d->temp_max[i], toImperial);
      d->temp_min[i] = convertTemp(d->temp_min[i], toImperial);
    }
  }
}

/***************************************************************************************
**                Round a temperature to the nearest whole degree
**  lroundf() rounds negatives away from zero (-4.6 → -5) where the old
**  "+ .5" trick gave -4, and never produces the "-0" String(float, 0) can.
***************************************************************************************/
int16_t roundTemp(float t) {
  return (int16_t)lroundf(t);
}

/***************************************************************************************
**       Show "Failed: <time>" on the top line when data has gone stale
**  Only touches that one line — icons, forecast and astronomy are left
**  showing whatever they last successfully drew. Self-heals: the next
**  successful fetch draws over this same spot with the normal "Updated:"
**  text via drawCurrentWeather(), no cleanup needed here.
***************************************************************************************/
void drawUpdateFailedLine() {
  time_t local_time = toLocal(now());
  String msg = String(failedStr) + strDate(local_time);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Updated: Mmm 44 44:44 "));  // same width as the normal line
  tft.drawString(msg, 120, 16);
  tft.setTextPadding(0);
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

  String hourStr = "";
  if (show24Hour == true) {
    if (hour(local_time) < 10) hourStr += "0";
    hourStr += hour(local_time);
  } else {
    if (hour(local_time) > 12) {
      hourStr += hour(local_time) - 12;
    } else {
      hourStr += hour(local_time);
      if (hour(local_time) == 0) hourStr = "12";
    }
  }

  String minuteStr = "";
  if (minute(local_time) < 10) minuteStr += "0";
  minuteStr += minute(local_time);

  int colonHalfW = (tft.textWidth(":") + 1) / 2;

  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  // Hour — right-aligned, ends exactly at the colon's fixed left edge
  tft.setTextDatum(BR_DATUM);
  tft.setTextPadding(tft.textWidth("44"));  // covers 1 or 2 digit hours
  tft.drawString(hourStr, clockColonX - colonHalfW, CLOCK_Y);

  // Minute — left-aligned, starts exactly at the colon's fixed right edge
  tft.setTextDatum(BL_DATUM);
  tft.setTextPadding(tft.textWidth("44"));
  tft.drawString(minuteStr, clockColonX + colonHalfW, CLOCK_Y);

  tft.setTextPadding(0);

  blinkOn = true;  // a full redraw (minute change, touch, etc.) always leaves the colon "on"
  lastBlinkMs = millis();
  drawColonGlyph(true);

  // Where AM/PM goes: on the clock's baseline, just right of the minutes.
  // BL_DATUM put the bottom of the font box (baseline + descent) at CLOCK_Y,
  // and the small font's descent differs, so align on the baseline itself.
  int16_t clockBaseline = CLOCK_Y - tft.fontHeight() + tft.gFont.maxAscent;
  int16_t minutesRight = clockColonX + colonHalfW + tft.textWidth("44");

  tft.unloadFont();

  // AM/PM in the 12hr clock — otherwise there's no telling which it is at the
  // other location. Blanked in 24hr mode. HH:MM stays centred on the colon.
  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextDatum(L_BASELINE);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextPadding(max(tft.textWidth(amStr), tft.textWidth(pmStr)));  // AM fully covers PM and back
  if (show24Hour) tft.drawString(" ", minutesRight + 4, clockBaseline);
  else tft.drawString(hour(local_time) < 12 ? amStr : pmStr, minutesRight + 4, clockBaseline);
  tft.setTextPadding(0);
  tft.unloadFont();
}

/***************************************************************************************
**                          Blink the clock's colon
***************************************************************************************/
void blinkColon() {
  if (millis() - lastBlinkMs < COLON_BLINK_MS) return;
  lastBlinkMs = millis();
  blinkOn = !blinkOn;

  tft.loadFont(AA_FONT_LARGE, LittleFS);
  drawColonGlyph(blinkOn);
  tft.unloadFont();
}

// Shared by drawTime() (font already loaded) and blinkColon() (loads its
// own). Assumes AA_FONT_LARGE is currently loaded.
void drawColonGlyph(bool visible) {
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(visible ? TFT_GREEN : TFT_BLACK, TFT_BLACK);
  tft.setTextPadding(0);
  tft.drawString(":", clockColonX, CLOCK_Y);
}

/***************************************************************************************
**                          Draw the current weather
***************************************************************************************/
void drawCurrentWeather() {
  // Time of the download, not of the draw — a settings Save redraws without fetching
  time_t local_time = toLocal(lastGoodFetch[activeSlot()]);
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

  if (iconSet == 1) {
    drawMeteocon(tft, getMeteoconBitmap(current->id, true, false), 0, 62, 100, 100, meteoconPalette);
  } else {
    weatherIcon = getMeteoconIcon(current->id, true);
    ui.drawBmp("/icon/" + weatherIcon + ".bmp", 0, 62);
  }

  // Weather Text
  weatherText = wmoDescription(current->id);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(labelColour, TFT_BLACK);

  int splitPoint = 0;
  int xpos = 235;
  splitPoint = splitIndex(weatherText, xpos - 100);

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
    // Both are fetched every update, so switching between them is display-only
    float pressure = pressureSeaLevel ? current->pressure_msl : current->pressure;
    if (units == "imperial") {
      // Open-Meteo always sends hPa — there's no pressure unit parameter
      weatherText = String(pressure * 0.02953, 2);  // hPa → inHg, e.g. "29.92"
      weatherText += pressImperial;
    } else {
      weatherText = (uint16_t)pressure;
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
      // 3px bars, 1px apart (x 199-237) — compacted from 4px/2px at x 178
      // to leave room for AM/PM beside the clock's minutes
      x = 199 + (temp * 4);
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
      tft.fillRect(x, y, 3, 8 + (temp * 2), theColour);
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

  // High/low centred 28px apart, either side of the 50px icon's centre (x + 25),
  // so two 3-character values ("105" / "-30", 27px each) still don't touch
  tft.setTextPadding(tft.textWidth("-88"));
  int16_t highTemp = roundTemp(daily->temp_max[dayIndex]);
  int16_t lowTemp = roundTemp(daily->temp_min[dayIndex]);
  handleTempRangeColour(highTemp);
  tft.drawString(String(highTemp), x + 11, y + 14);
  handleTempRangeColour(lowTemp);
  tft.drawString(String(lowTemp), x + 39, y + 14);

  if (iconSet == 1) {
    drawMeteocon(tft, getMeteoconBitmap(daily->id[dayIndex], false, true), x, y + 10, 50, 50, meteoconPalette);
  } else {
    String weatherIcon = getMeteoconIcon(daily->id[dayIndex], false);
    ui.drawBmp("/icon50/" + weatherIcon + ".bmp", x, y + 10);
  }

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
  uint16_t temp, theY, theOffsetY;
  int16_t theTemp;
  uint16_t precipProbability;
  theY = 280;
  theOffsetY = 18;
  temp = 0;
  // Values are right-aligned on each column's right edge, so "5", "-12" and
  // "105" all line up. 31px pitch leaves ~5px between two 3-wide values
  // ("101" "100"), same breathing room as the daily high/low pair — 28px ran
  // them together. Last right edge lands at 237.
  const uint16_t colRight = 82;
  const uint16_t colPitch = 31;
  tft.fillRect(0, 241, 240, 320 - 241, TFT_BLACK);
  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextColor(labelColour, TFT_BLACK);
  // Labels left-aligned at the screen edge rather than right-aligned to a
  // shared edge: the widest label ("Neer%:", 55px) only sits on the POP row,
  // where values are 2-wide, so "Temp:" no longer has to leave room for it.
  // Leading spaces (" Dew:") were there to nudge right-aligned text — trimmed.
  tft.setTextDatum(BL_DATUM);
  tft.setTextPadding(0);
  const char *labels[3] = { popLabelStr, tempLabelStr, dewLabelStr };
  for (uint8_t row = 0; row < 3; row++) {
    String label = labels[row];
    label.trim();
    tft.drawString(label, 2, theY + (theOffsetY * row));
  }
  tft.setTextColor(labelColour, TFT_BLACK);
  tft.setTextDatum(BC_DATUM);
  tft.drawString(forecastTitleStr, 120, theY - 20);
  tft.setTextDatum(BR_DATUM);
  tft.setTextPadding(tft.textWidth("188"));  // one column's widest value, no wider
  //
  drawSeparator(260);

  for (temp = 0; temp < 6; temp++) {
    uint16_t x = colRight + (temp * colPitch);

    precipProbability = hourly->pop[temp] * 100;
    if (precipProbability >= 100) precipProbability = 99;

    handlePrecipRange(precipProbability);
    tft.drawString(String(precipProbability), x, theY);

    theTemp = constrain(roundTemp(hourly->temp[temp]), -99, 199);
    handleTempRangeColour(theTemp);
    tft.drawString(String(theTemp), x, theY + theOffsetY);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    theTemp = roundTemp(hourly->dew_point[temp]);
    if (theTemp > 199 || theTemp < -99) {
      theTemp = constrain(theTemp, -99, 199);
      tft.setTextColor(TFT_RED, TFT_BLACK);  // red = clamped, real value is off the scale
    }
    tft.drawString(String(theTemp), x, theY + (theOffsetY * 2));
  }
  tft.setTextPadding(0);
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
void handleTempRangeColour(int16_t theVal) {
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
  bool isNight = today && !current->is_day;

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
**                Get the Meteocons bitmap for a WMO weather code
**                mini = true for the 50px forecast set, false for the 100px set
***************************************************************************************/
#define MC(name) (mini ? mc_mini_##name : mc_##name)

const uint8_t *getMeteoconBitmap(uint16_t id, bool today, bool mini) {
  bool isNight = today && !current->is_day;

  if (id == 0) return isNight ? MC(clearnight) : MC(sunny);
  if (id == 1) return isNight ? MC(mostlyclearnight) : MC(mostlysunny);
  if (id == 2) return isNight ? MC(partlycloudynight) : MC(partlycloudy);
  if (id == 3) return MC(cloudy);
  if (id == 45) return MC(fog);
  if (id == 48) return MC(hazy);                         // depositing rime fog
  if (id == 51 || id == 53) return MC(chancerain);      // light / moderate drizzle
  if (id == 55) return MC(rain);                        // dense drizzle
  if (id == 56 || id == 57) return MC(chancesleet);     // freezing drizzle
  if (id == 61) return MC(chancerain);
  if (id == 63 || id == 65) return MC(rain);
  if (id == 66 || id == 67) return MC(sleet);           // freezing rain
  if (id == 71) return MC(flurries);
  if (id == 73 || id == 75) return MC(snow);
  if (id == 77) return MC(chanceflurries);              // snow grains
  if (id == 80) return MC(chancerain);
  if (id == 81 || id == 82) return MC(rain);
  if (id == 85) return MC(chancesnow);
  if (id == 86) return MC(snow);
  if (id == 95) return MC(chancestorms);
  if (id == 96 || id == 99) return MC(tstorms);         // thunderstorm with hail

  return MC(unknown);
}

#undef MC

/***************************************************************************************
**                          Draw screen section separator line
***************************************************************************************/
void drawSeparator(uint16_t y) {
  tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
}

/***************************************************************************************
**                          Determine place to split a line
***************************************************************************************/
int splitIndex(String text, int maxWidth) {
  int index = 0;
  while ((text.indexOf(' ', index) >= 0) && (index <= text.length() / 2)) {
    index = text.indexOf(' ', index) + 1;
  }
  if (index) index--;

  // No space found (a single long word like "Thunderstorm"), or the first
  // half is still too wide — fall back to the widest prefix that actually
  // fits maxWidth so it wraps onto two lines instead of running off-screen.
  if (maxWidth && (!index || tft.textWidth(text.substring(0, index)) > maxWidth)) {
    index = text.length();
    while (index > 1 && tft.textWidth(text.substring(0, index)) > maxWidth) {
      index--;
    }
  }

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
**                    Progress ring, top-left corner
**  Smooth-edged like KlippyMon's gauges: dark grey track, green filling
**  clockwise from 12 o'clock. Shows a download in progress, and fills while
**  the weather area is held for a refresh. (Replaces a navy fillSegment pie
**  that was all but invisible on black.)
***************************************************************************************/
#define RING_X 22
#define RING_Y 22
#define RING_R 16   // outer radius
#define RING_IR 10  // inner radius — 7px thick

void drawProgressRing(uint8_t percent) {
  // TFT_eSPI angles: 0 = 6 o'clock, clockwise — so 12 o'clock is 180.
  // drawArc (not drawSmoothArc) so the moving end stays square as it grows.
  tft.drawArc(RING_X, RING_Y, RING_R, RING_IR, 0, 360, TFT_DARKGREY, TFT_BLACK);
  if (percent >= 100) {
    tft.drawArc(RING_X, RING_Y, RING_R, RING_IR, 0, 360, TFT_GREEN, TFT_BLACK);
  } else if (percent > 0) {
    tft.drawArc(RING_X, RING_Y, RING_R, RING_IR, 180, (180 + percent * 36 / 10) % 360, TFT_GREEN, TFT_BLACK);
  }
}

void eraseProgressRing() {
  tft.fillCircle(RING_X, RING_Y, RING_R + 1, TFT_BLACK);  // +1 for the anti-aliased edge
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
  const byte barXPosBase = 219;  // last bar ends at x 237, flush with the UV bars below
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
    // Look up whichever location is currently active
    if (lookupCityName(altLocation)) runBdcOnce = true;
  }
}

/***************************************************************************************
**             Fetch and cache the city name for location 1 or location 2
**  alt = false → location 1, true → location 2. Doesn't have to be the one on
**  screen — a settings Save uses this to cache the off-screen location's name.
**  Returns true if a name is cached (already, or just fetched).
***************************************************************************************/
bool lookupCityName(bool alt) {
  // Reuse the cached name if we already have one — Nominatim asks clients to cache results.
  // Cache lives in RAM only: cleared on reboot, and per location when a Save moves it.
  String &cached = alt ? theCityLocation2 : theCityLocation;
  if (cached != "Unknown") return true;

  BDCcurrent = new BDC_current;
  String lat = alt ? latitude2 : latitude;
  String lon = alt ? longitude2 : longitude;
  bool BDCparsed = bcd.getBDCLocation(BDCcurrent, lat, lon, NOMINATIM_LANG);
  if (BDCparsed) {
    if (BDCcurrent->city.isEmpty() && !BDCcurrent->locality.isEmpty())
      BDCcurrent->city = BDCcurrent->locality;
    String cityName = BDCcurrent->city;
    if (cityName.length() > 26) cityName = "Local Weather";
    cached = cityName;
  }
  delete BDCcurrent;  // free on failure too, or every failed lookup leaks one struct
  return BDCparsed;
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
  // Nothing changed while the splash was up — redraw from memory rather than
  // setting booted, which re-ran the whole boot download and progress bars
  redrawScreen(false);
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
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
    See more at https://github.com/ThingPulse/esp8266-weather-station-color
    Hardware kit: https://thingpulse.com/product/esp32-wifi-color-display-kit-grande/
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
