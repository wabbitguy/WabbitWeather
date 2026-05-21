#pragma once

// WebConfig.h — Wabbitwx web configuration portal
// Accessible at http://wabbitweather.local or http://<IP> on port 80
//
// Requires in your .ino:
//   #include <WebServer.h>
//   #include <ESPmDNS.h>
//   These are already in your .ino so no changes needed there.
//
// In setup() at the very end, add:
//   loadConfig();    // load saved settings (writes defaults if no file exists)
//   startWebConfig(); // start mDNS + web server
//
// In loop() add:
//   handleWebConfig(); // must be called every loop iteration
//
// When the user hits "Update & Save" the globals in All_Settings.h are updated
// immediately and saved to /config.txt on LittleFS.
// Check the configUpdated flag in loop() to trigger a display refresh:
//   if (configUpdated) { configUpdated = false; updateData(); }

// ── Config file ────────────────────────────────────────────────────────────
#define CONFIG_FILE "/config.txt"

// ── Flag set by web handler when user saves settings ──────────────────────
// Declare in your .ino globals: volatile bool configUpdated = false;
// WebConfig.cpp will set it to true when the user hits Update & Save.

// ── TFT colour lookup table ────────────────────────────────────────────────
struct TFTColour {
  const char* name;
  uint32_t    value;
};

static const TFTColour TFT_COLOURS[] = {
  { "BLACK",       0x0000 },
  { "NAVY",        0x000F },
  { "DARKGREEN",   0x03E0 },
  { "DARKCYAN",    0x03EF },
  { "MAROON",      0x7800 },
  { "PURPLE",      0x780F },
  { "OLIVE",       0x7BE0 },
  { "LIGHTGREY",   0xC618 },
  { "DARKGREY",    0x7BEF },
  { "BLUE",        0x001F },
  { "GREEN",       0x07E0 },
  { "CYAN",        0x07FF },
  { "RED",         0xF800 },
  { "MAGENTA",     0xF81F },
  { "YELLOW",      0xFFE0 },
  { "WHITE",       0xFFFF },
  { "ORANGE",      0xFDA0 },
  { "GREENYELLOW", 0xB7E0 },
  { "PINK",        0xFE19 },
  { "BROWN",       0x9A60 },
  { "GOLD",        0xFEA0 },
  { "SILVER",      0xC618 },
  { "SKYBLUE",     0x867D },
  { "VIOLET",      0x915C }
};
static const uint8_t TFT_COLOUR_COUNT = sizeof(TFT_COLOURS) / sizeof(TFT_COLOURS[0]);

// ── Colour helpers ─────────────────────────────────────────────────────────
inline uint32_t colourFromName(const String& name) {
  for (uint8_t i = 0; i < TFT_COLOUR_COUNT; i++)
    if (name == TFT_COLOURS[i].name) return TFT_COLOURS[i].value;
  return 0xFFFF; // default white
}

inline const char* nameFromColour(uint32_t value) {
  for (uint8_t i = 0; i < TFT_COLOUR_COUNT; i++)
    if (value == TFT_COLOURS[i].value) return TFT_COLOURS[i].name;
  return "TFT_WHITE";
}

// ── Public API ─────────────────────────────────────────────────────────────
void loadConfig();        // reads /config.txt, writes defaults if missing
void saveConfig();        // writes current globals to /config.txt
void startWebConfig();    // call once at end of setup()
void handleWebConfig();   // call every loop()
