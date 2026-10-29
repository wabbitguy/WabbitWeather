// WebConfig.h — settings web page at http://<HOSTNAME in lowercase>.local (wabbitweather.local)
// Save updates the All_Settings.h globals, writes /config.txt and sets configUpdated + configChanges

#pragma once

// ── Config file ────────────────────────────────────────────────────────────
#define CONFIG_FILE "/config.txt"

// ── configChanges bits: what the last Save changed, so only that is re-fetched
#define CFG_LOC1_MOVED    0x01  // location 1 lat/lon changed → new city name (+ weather if on screen)
#define CFG_LOC2_MOVED    0x02  // location 2 lat/lon changed → same, for location 2
#define CFG_UNITS_CHANGED 0x04  // imperial ↔ metric → convert what's in memory

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
  { "LIGHTGREY",   0xD69A },
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
void startWebConfig(const char* hostName);  // call once at end of setup()
void handleWebConfig();   // call every loop()
