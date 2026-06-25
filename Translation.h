// =============================================================================
// Translation.h — WabbitWeather localisation
// =============================================================================
//
//
// 2. Remove (or comment out) the matching string definitions from All_Settings.h
//    — shortDOW[], sunStr, cloudStr, humidityStr, moonPhase[] — they live here now.
//
// 3. Replace hardcoded strings in WabbitWeather.ino and WebConfig.cpp with the
//    constant names defined below.  A comment beside each one shows the original
//    source location so you can find them quickly with Ctrl+F.
//
// 4. For the Open-Meteo weather description displayed on screen (current->description
//    and current->main), use wmoDescription(id) and wmoMain(id) instead.
//    Open-Meteo returns WMO numeric codes, not text — the English strings you see
//    come from inside the OpenMeteo parser.  The functions at the bottom of this
//    file return translated text directly from the WMO code so you stay consistent
//    regardless of what the parser sets.
//
// FONT NOTE
// ---------
// The existing NotoSansBold15 / NotoSansBold36 bitmap fonts cover the full
// Latin-1 Supplement block (U+00C0–U+00FF), so accented characters for French,
// German, Spanish, and Dutch work without regenerating fonts.
// =============================================================================

#ifndef TRANSLATION_H
#define TRANSLATION_H

// -----------------------------------------------------------------------------
// Default to English if nothing is defined
// -----------------------------------------------------------------------------
#if !defined(LANG_EN) && !defined(LANG_FR) && !defined(LANG_DE) && \
    !defined(LANG_ES) && !defined(LANG_NL) && !defined(LANG_PT) && \
    !defined(LANG_TR)
  #define LANG_EN
#endif

// =============================================================================
//  ENGLISH
// =============================================================================
#ifdef LANG_EN

// Open-Meteo language code (passed to Nominatim for place name lookup)
#define NOMINATIM_LANG "en"

// ── Calendar ─────────────────────────────────────────────────────────────────
// Was: const String shortDOW[8] in All_Settings.h
static const char* shortDOW[8] = { "???", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

static const char* shortMonth[12] = {
  "Jan","Feb","Mar","Apr","May","Jun",
  "Jul","Aug","Sep","Oct","Nov","Dec"};

static const char* moonPhase[8] = {
  "New", "Waxing", "1st qtr", "Waxing", "Full", "Waning", "Last qtr", "Waning"
};
// ── Astronomy panel ───────────────────────────────────────────────────────────
// Was: const char sunStr[], cloudStr[], humidityStr[] in All_Settings.h
const char sunStr[] = "Sun";
const char cloudStr[] = "Cloud";
const char humidityStr[] = "Humidity";

// ── Astronomy rise/set labels ─────────────────────────────────────────────────
const char riseStr[] = "SR:";
const char setStr[]  = "SS:";

// ── Current weather panel ─────────────────────────────────────────────────────
// Was: "Updated: " prefix built inline in drawCurrentWeather()
const char updatedStr[] = "Updated: ";

// Wind speed units — appended to the wind speed value
// Was: " m/s" / " mph" inline in drawCurrentWeather()
const char windMetric[] = " m/s";
const char windImperial[] = " mph";

// Pressure units
// Was: " in" / " hPa" inline in drawCurrentWeather()
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";

// Temperature unit symbol (small font uses 'o' not the degree glyph)
// Was: "oC" / "oF" inline in drawCurrentWeather()
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";

// ── 6hr forecast panel ───────────────────────────────────────────────────────
// Was: "6Hr Forecast" / "Pop%:" / "Temp:" / " Dew:" in handleHourlyFrame()
const char forecastTitleStr[] = "6Hr Forecast";
const char popLabelStr[] = "Pop%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Dew:";

// ── Boot / progress messages ──────────────────────────────────────────────────
// Was: hardcoded strings in setup() drawProgress() calls
const char bootUpdatingTime[] = "Updating time...";
const char bootUpdatingConditions[] = "Updating conditions...";
const char bootDone[] = "Done...";
const char bootConnectedWifi[] = "Connected to WiFi";
const char bootFetchingWeather[] = "Fetching weather data...";
const char bootFlashOK[] = "Flash FS available!";
const char bootFlashFail[] = "Flash FS initialisation failed!";
const char bootRetry[] = "Fetch failed, retrying in 60 seconds...";

// ── Splash screen credits ─────────────────────────────────────────────────────
// Was: hardcoded in setup()
const char creditOriginal[] = "Original by: blog.squix.org";
const char creditBodmer[] = "Adapted by: Bodmer";
const char creditWabbit[] = "Open-Meteo by: Wabbitguy";

// ── WebConfig UI strings ──────────────────────────────────────────────────────
// Was: hardcoded inside buildPage() in WebConfig.cpp

// Page / header
const char wcTitle[] = "WabbitWeather";
const char wcSaved[] = "&#10003; Settings saved and applied.";

// WiFi reset modal
const char wcWifiResetTitle[] = "&#x26A0; Reset WiFi?";
const char wcWifiResetBody[] = "Erases saved WiFi credentials and reboots into "
                               "setup mode. Reconnect via the WabbitWeather hotspot.";
const char wcWifiResetYes[] = "Yes, reset";
const char wcWifiResetCancel[] = "Cancel";
const char wcWifiResetBtn[] = "&#x26A0; WiFi Reset";
const char wcWifiResetting[] = "Resetting WiFi...";
const char wcWifiResetHotspot[] = "Rebooting. Connect to the <strong>WabbitWeather</strong> hotspot.";

// Section headings
const char wcSecLocation[] = "Location";
const char wcSecDisplay[] = "Display Settings";
const char wcSecTempColours[] = "Temperature Threshold Colours";
const char wcSecPrecipColours[] = "Precipitation Threshold Colours";

// Location fields
const char wcLatLabel[] = "Latitude";
const char wcLonLabel[] = "Longitude";

// Display toggles / labels
const char wcUnitsLabel[] = "Temperature Units";
const char wcImperial[] = "Imperial (&deg;F)";
const char wcMetric[] = "Metric (&deg;C)";
const char wcClock24[] = "24 hour clock";
const char wcAutoDim[] = "Auto dim at dusk";
const char wcNightBright[] = "Night brightness (0-255)";
const char wcNightBrightHint[] = "0 = display off, 1 = barely visible";
const char wcShowBaro[] = "Show barometric pressure";
const char wcShowPrecip[] = "Show precip probability";
const char wcShowUV[] = "Show UV index";
const char wcShow6Hr[] = "Show 6hr forecast";
const char wcForecastStart[]   = "Forecast start";
const char wcForecastToday[]   = "Today";
const char wcForecastTomorrow[] = "Tomorrow";
const char wcWeatherHeadings[] = "Weather Headings";
const char wcAstroHeadings[] = "Astrology Headings";

// Temperature threshold labels
const char wcHighTempThresh[] = "High temp threshold (&deg;)";
const char wcLowTempThresh[] = "Low temp threshold (&deg;)";
const char wcHighTempColour[] = "High temp colour";
const char wcMidTempColour[] = "Mid temp colour";
const char wcLowTempColour[] = "Low temp colour";

// Precipitation threshold labels
const char wcHighPrecipThresh[] = "High precip threshold (%)";
const char wcLowPrecipThresh[] = "Low precip threshold (%)";
const char wcHighPOPColour[] = "High POP colour";
const char wcMidPOPColour[] = "Mid POP colour";
const char wcLowPOPColour[] = "Low POP colour";

// Buttons
const char wcSaveBtn[] = "Update &amp; Save";
const char wcCancelBtn[] = "Cancel";

// ── WMO weather code → display text ──────────────────────────────────────────
// Use wmoDescription(id) where you currently use current->description
// Use wmoMain(id)        where you currently use current->main
inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case 0: return "Clear sky";
    case 1: return "Mostly clear";
    case 2: return "Partly cloudy";
    case 3: return "Overcast";
    case 45: return "Foggy";
    case 48: return "Icy fog";
    case 51: return "Light drizzle";
    case 53: return "Drizzle";
    case 55: return "Heavy drizzle";
    case 56: return "Light freezing drizzle";
    case 57: return "Freezing drizzle";
    case 61: return "Light rain";
    case 63: return "Rain";
    case 65: return "Heavy rain";
    case 66: return "Light freezing rain";
    case 67: return "Freezing rain";
    case 71: return "Light snow";
    case 73: return "Snow";
    case 75: return "Heavy snow";
    case 77: return "Snow grains";
    case 80: return "Light showers";
    case 81: return "Showers";
    case 82: return "Heavy showers";
    case 85: return "Snow showers";
    case 86: return "Heavy snow showers";
    case 95: return "Thunderstorm";
    case 96: return "Thunderstorm w/ hail";
    case 99: return "Thunderstorm w/ hail";
    default: return "Unknown";
  }
}
inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1) return "Clear";
  if (id == 2 || id == 3) return "Clouds";
  if (id == 45 || id == 48) return "Fog";
  if (id >= 51 && id <= 57) return "Drizzle";
  if (id >= 61 && id <= 67) return "Rain";
  if (id >= 71 && id <= 77) return "Snow";
  if (id >= 80 && id <= 82) return "Rain";
  if (id == 85 || id == 86) return "Snow";
  if (id == 95 || id == 96 || id == 99) return "Thunderstorm";
  return "Unknown";
}

#endif  // LANG_EN


// =============================================================================
//  FRENCH
// =============================================================================
#ifdef LANG_FR

#define NOMINATIM_LANG "fr"

static const char* shortMonth[12] = {
  "Jan","Fév","Mar","Avr","Mai","Jun",
  "Jul","Aoû","Sep","Oct","Nov","Déc"};

static const char* shortDOW[8] = { "???", "DIM", "LUN", "MAR", "MER", "JEU", "VEN", "SAM" };

const char sunStr[] = "Soleil";
const char cloudStr[] = "Nuages";
const char humidityStr[] = "Humidite";  // é needs Noto Latin-1 — safe to use: "Humidit\xe9"

static const char* moonPhase[8] = {
  "Nouvelle", "Croissant", "1er qrt", "Croissant",
  "Pleine", "Decroissant", "Der qrt", "Decroissant"
};

const char riseStr[] = "SR:";
const char setStr[] = "SS:";

const char updatedStr[] = "Mis a jour: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";

const char forecastTitleStr[] = "Previsions 6h";
const char popLabelStr[] = "Prec%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Rose:";

const char bootUpdatingTime[] = "Mise a jour heure...";
const char bootUpdatingConditions[] = "Telechargement meteo...";
const char bootDone[] = "Termine...";
const char bootConnectedWifi[] = "WiFi connecte";
const char bootFetchingWeather[] = "Chargement meteo...";
const char bootFlashOK[] = "Flash FS pret!";
const char bootFlashFail[] = "Erreur Flash FS!";
const char bootRetry[] = "Echec, nouvel essai dans 60s...";

const char creditOriginal[] = "Original: blog.squix.org";
const char creditBodmer[] = "Adapte par: Bodmer";
const char creditWabbit[] = "Open-Meteo par: Wabbitguy";

const char wcTitle[] = "WabbitWeather";
const char wcSaved[] = "&#10003; Parametres enregistres.";
const char wcWifiResetTitle[] = "&#x26A0; Reinitialiser WiFi ?";
const char wcWifiResetBody[] = "Efface les identifiants WiFi et redémarre en mode configuration.";
const char wcWifiResetYes[] = "Oui, reinitialiser";
const char wcWifiResetCancel[] = "Annuler";
const char wcWifiResetBtn[] = "&#x26A0; WiFi Reset";
const char wcWifiResetting[] = "Reinitialisation WiFi...";
const char wcWifiResetHotspot[] = "Redemarrage. Connectez-vous au hotspot <strong>WabbitWeather</strong>.";
const char wcSecLocation[] = "Emplacement";
const char wcSecDisplay[] = "Parametres d'affichage";
const char wcSecTempColours[] = "Couleurs de temperature";
const char wcSecPrecipColours[] = "Couleurs de precipitation";
const char wcLatLabel[] = "Latitude";
const char wcLonLabel[] = "Longitude";
const char wcUnitsLabel[] = "Unites de temperature";
const char wcImperial[] = "Imperiales (&deg;F)";
const char wcMetric[] = "Metriques (&deg;C)";
const char wcClock24[] = "Horloge 24h";
const char wcAutoDim[] = "Gradation automatique au crepuscule";
const char wcNightBright[] = "Luminosite nocturne (0-255)";
const char wcNightBrightHint[] = "0 = ecran eteint";
const char wcShowBaro[] = "Afficher la pression";
const char wcShowPrecip[] = "Afficher la probabilite de pluie";
const char wcShowUV[] = "Afficher l'indice UV";
const char wcShow6Hr[] = "Previsions sur 6h";
const char wcForecastStart[]    = "Debut previsions";
const char wcForecastToday[]    = "Aujourd'hui";
const char wcForecastTomorrow[] = "Demain";
const char wcWeatherHeadings[] = "Couleur titres meteo";
const char wcAstroHeadings[] = "Couleur titres astronomie";
const char wcHighTempThresh[] = "Seuil temperature haute (&deg;)";
const char wcLowTempThresh[] = "Seuil temperature basse (&deg;)";
const char wcHighTempColour[] = "Couleur temp haute";
const char wcMidTempColour[] = "Couleur temp moyenne";
const char wcLowTempColour[] = "Couleur temp basse";
const char wcHighPrecipThresh[] = "Seuil precipitation haute (%)";
const char wcLowPrecipThresh[] = "Seuil precipitation basse (%)";
const char wcHighPOPColour[] = "Couleur POP haute";
const char wcMidPOPColour[] = "Couleur POP moyenne";
const char wcLowPOPColour[] = "Couleur POP basse";
const char wcSaveBtn[] = "Enregistrer";
const char wcCancelBtn[] = "Annuler";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case 0: return "Ciel clair";
    case 1: return "Peu nuageux";
    case 2: return "Partiellement nuageux";
    case 3: return "Couvert";
    case 45: return "Brouillard";
    case 48: return "Brouillard givrant";
    case 51: return "Bruine legere";
    case 53: return "Bruine";
    case 55: return "Bruine forte";
    case 56: return "Bruine verglagante legere";
    case 57: return "Bruine verglacante";
    case 61: return "Pluie legere";
    case 63: return "Pluie";
    case 65: return "Forte pluie";
    case 66: return "Pluie verglacante legere";
    case 67: return "Pluie verglacante";
    case 71: return "Neige legere";
    case 73: return "Neige";
    case 75: return "Forte neige";
    case 77: return "Grains de neige";
    case 80: return "Averses legeres";
    case 81: return "Averses";
    case 82: return "Fortes averses";
    case 85: return "Averses de neige";
    case 86: return "Fortes averses de neige";
    case 95: return "Orage";
    case 96: return "Orage avec grele";
    case 99: return "Orage avec grele";
    default: return "Inconnu";
  }
}
inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1) return "Clair";
  if (id == 2 || id == 3) return "Nuages";
  if (id == 45 || id == 48) return "Brouillard";
  if (id >= 51 && id <= 57) return "Bruine";
  if (id >= 61 && id <= 67) return "Pluie";
  if (id >= 71 && id <= 77) return "Neige";
  if (id >= 80 && id <= 82) return "Pluie";
  if (id == 85 || id == 86) return "Neige";
  if (id == 95 || id == 96 || id == 99) return "Orage";
  return "Inconnu";
}

#endif  // LANG_FR


// =============================================================================
//  GERMAN
// =============================================================================
#ifdef LANG_DE

#define NOMINATIM_LANG "de"

static const char* shortMonth[12] = {
  "Jan","Feb","Mär","Apr","Mai","Jun",
  "Jul","Aug","Sep","Okt","Nov","Dez"};// German

static const char* shortDOW[8] = { "???", "SO", "MO", "DI", "MI", "DO", "FR", "SA" };

const char sunStr[] = "Sonne";
const char cloudStr[] = "Wolken";
const char humidityStr[] = "Luftfeuchte";

static const char* moonPhase[8] = {
  "Neumond", "Zunehmend", "Halbmond", "Zunehmend",
  "Vollmond", "Abnehmend", "Halbmond", "Abnehmend"
};

const char riseStr[] = "SR:";
const char setStr[] = "SS:";

const char updatedStr[] = "Aktualisiert: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";

const char forecastTitleStr[] = "6-Std-Vorhersage";
const char popLabelStr[] = "Nied%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Tau:";

const char bootUpdatingTime[] = "Uhrzeit wird aktualisiert...";
const char bootUpdatingConditions[] = "Wetterdaten werden geladen...";
const char bootDone[] = "Fertig...";
const char bootConnectedWifi[] = "WiFi verbunden";
const char bootFetchingWeather[] = "Wetterdaten werden abgerufen...";
const char bootFlashOK[] = "Flash FS bereit!";
const char bootFlashFail[] = "Flash FS Fehler!";
const char bootRetry[] = "Fehler, neuer Versuch in 60s...";

const char creditOriginal[] = "Original: blog.squix.org";
const char creditBodmer[] = "Angepasst von: Bodmer";
const char creditWabbit[] = "Open-Meteo von: Wabbitguy";

const char wcTitle[] = "WabbitWeather";
const char wcSaved[] = "&#10003; Einstellungen gespeichert.";
const char wcWifiResetTitle[] = "&#x26A0; WiFi zuruecksetzen?";
const char wcWifiResetBody[] = "Loescht WLAN-Daten und startet im Konfigurationsmodus neu.";
const char wcWifiResetYes[] = "Ja, zuruecksetzen";
const char wcWifiResetCancel[] = "Abbrechen";
const char wcWifiResetBtn[] = "&#x26A0; WiFi Reset";
const char wcWifiResetting[] = "WiFi wird zurueckgesetzt...";
const char wcWifiResetHotspot[] = "Neustart. Verbinde mit dem Hotspot <strong>WabbitWeather</strong>.";
const char wcSecLocation[] = "Standort";
const char wcSecDisplay[] = "Anzeigeeinstellungen";
const char wcSecTempColours[] = "Temperaturfarbschwellen";
const char wcSecPrecipColours[] = "Niederschlagsfarbschwellen";
const char wcLatLabel[] = "Breitengrad";
const char wcLonLabel[] = "Laengengrad";
const char wcUnitsLabel[] = "Temperatureinheit";
const char wcImperial[] = "Imperial (&deg;F)";
const char wcMetric[] = "Metrisch (&deg;C)";
const char wcClock24[] = "24-Stunden-Uhr";
const char wcAutoDim[] = "Automatisch abdunkeln bei Dunkelheit";
const char wcNightBright[] = "Nachtshelligkeit (0-255)";
const char wcNightBrightHint[] = "0 = Display aus";
const char wcShowBaro[] = "Luftdruck anzeigen";
const char wcShowPrecip[] = "Niederschlagswahrscheinlichkeit anzeigen";
const char wcShowUV[] = "UV-Index anzeigen";
const char wcShow6Hr[] = "6-Std-Vorhersage anzeigen";
const char wcForecastStart[]    = "Vorhersage ab";
const char wcForecastToday[]    = "Heute";
const char wcForecastTomorrow[] = "Morgen";
const char wcWeatherHeadings[] = "Farbe Wetter-Titel";
const char wcAstroHeadings[] = "Farbe Astronomie-Titel";
const char wcHighTempThresh[] = "Hohe Temp-Schwelle (&deg;)";
const char wcLowTempThresh[] = "Niedrige Temp-Schwelle (&deg;)";
const char wcHighTempColour[] = "Farbe hohe Temp";
const char wcMidTempColour[] = "Farbe mittlere Temp";
const char wcLowTempColour[] = "Farbe niedrige Temp";
const char wcHighPrecipThresh[] = "Hohe Niederschlagsschwelle (%)";
const char wcLowPrecipThresh[] = "Niedrige Niederschlagsschwelle (%)";
const char wcHighPOPColour[] = "Farbe hoher POP";
const char wcMidPOPColour[] = "Farbe mittlerer POP";
const char wcLowPOPColour[] = "Farbe niedriger POP";
const char wcSaveBtn[] = "Speichern";
const char wcCancelBtn[] = "Abbrechen";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case 0: return "Klarer Himmel";
    case 1: return "Ueberwiegend klar";
    case 2: return "Teilweise bewoelkt";
    case 3: return "Bedeckt";
    case 45: return "Neblig";
    case 48: return "Gefrierender Nebel";
    case 51: return "Leichter Nieseln";
    case 53: return "Nieseln";
    case 55: return "Starkes Nieseln";
    case 56: return "Leichter Eisregen";
    case 57: return "Eisregen";
    case 61: return "Leichter Regen";
    case 63: return "Regen";
    case 65: return "Starker Regen";
    case 66: return "Leichter gefrierender Regen";
    case 67: return "Gefrierender Regen";
    case 71: return "Leichter Schneefall";
    case 73: return "Schneefall";
    case 75: return "Starker Schneefall";
    case 77: return "Schneekörner";
    case 80: return "Leichte Schauer";
    case 81: return "Schauer";
    case 82: return "Starke Schauer";
    case 85: return "Schneeschauer";
    case 86: return "Starke Schneeschauer";
    case 95: return "Gewitter";
    case 96: return "Gewitter mit Hagel";
    case 99: return "Gewitter mit Hagel";
    default: return "Unbekannt";
  }
}
inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1) return "Klar";
  if (id == 2 || id == 3) return "Wolken";
  if (id == 45 || id == 48) return "Nebel";
  if (id >= 51 && id <= 57) return "Nieseln";
  if (id >= 61 && id <= 67) return "Regen";
  if (id >= 71 && id <= 77) return "Schnee";
  if (id >= 80 && id <= 82) return "Regen";
  if (id == 85 || id == 86) return "Schnee";
  if (id == 95 || id == 96 || id == 99) return "Gewitter";
  return "Unbekannt";
}

#endif  // LANG_DE


// =============================================================================
//  SPANISH
// =============================================================================
#ifdef LANG_ES

#define NOMINATIM_LANG "es"

static const char* shortMonth[12] = {"Ene","Feb","Mar","Abr","May","Jun",
                               "Jul","Ago","Sep","Oct","Nov","Dic"};

static const char* shortDOW[8] = { "???", "DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB" };

const char sunStr[] = "Sol";
const char cloudStr[] = "Nubes";
const char humidityStr[] = "Humedad";

static const char* moonPhase[8] = {
  "Nueva", "Creciente", "Cuarto", "Creciente",
  "Llena", "Menguante", "Cuarto", "Menguante"
};

const char riseStr[] = "SR:";
const char setStr[] = "SS:";

const char updatedStr[] = "Actualizado: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";

const char forecastTitleStr[] = "Pronostico 6h";
const char popLabelStr[] = "Prec%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Roc:";

const char bootUpdatingTime[] = "Actualizando hora...";
const char bootUpdatingConditions[] = "Actualizando condiciones...";
const char bootDone[] = "Listo...";
const char bootConnectedWifi[] = "WiFi conectado";
const char bootFetchingWeather[] = "Obteniendo datos...";
const char bootFlashOK[] = "Flash FS listo!";
const char bootFlashFail[] = "Error en Flash FS!";
const char bootRetry[] = "Fallo, reintentando en 60s...";

const char creditOriginal[] = "Original: blog.squix.org";
const char creditBodmer[] = "Adaptado por: Bodmer";
const char creditWabbit[] = "Open-Meteo por: Wabbitguy";

const char wcTitle[] = "WabbitWeather";
const char wcSaved[] = "&#10003; Configuracion guardada.";
const char wcWifiResetTitle[] = "&#x26A0; Restablecer WiFi?";
const char wcWifiResetBody[] = "Borra las credenciales WiFi y reinicia en modo configuracion.";
const char wcWifiResetYes[] = "Si, restablecer";
const char wcWifiResetCancel[] = "Cancelar";
const char wcWifiResetBtn[] = "&#x26A0; WiFi Reset";
const char wcWifiResetting[] = "Restableciendo WiFi...";
const char wcWifiResetHotspot[] = "Reiniciando. Conéctese al hotspot <strong>WabbitWeather</strong>.";
const char wcSecLocation[] = "Ubicacion";
const char wcSecDisplay[] = "Configuracion de pantalla";
const char wcSecTempColours[] = "Colores de temperatura";
const char wcSecPrecipColours[] = "Colores de precipitacion";
const char wcLatLabel[] = "Latitud";
const char wcLonLabel[] = "Longitud";
const char wcUnitsLabel[] = "Unidades de temperatura";
const char wcImperial[] = "Imperial (&deg;F)";
const char wcMetric[] = "Metrico (&deg;C)";
const char wcClock24[] = "Reloj de 24 horas";
const char wcAutoDim[] = "Atenuar al anochecer";
const char wcNightBright[] = "Brillo nocturno (0-255)";
const char wcNightBrightHint[] = "0 = pantalla apagada";
const char wcShowBaro[] = "Mostrar presion barometrica";
const char wcShowPrecip[] = "Mostrar probabilidad de precipitacion";
const char wcShowUV[] = "Mostrar indice UV";
const char wcShow6Hr[] = "Mostrar pronostico de 6h";
const char wcForecastStart[]    = "Inicio pronostico";
const char wcForecastToday[]    = "Hoy";
const char wcForecastTomorrow[] = "Manana";
const char wcWeatherHeadings[] = "Color titulos meteo";
const char wcAstroHeadings[] = "Color titulos astronomia";
const char wcHighTempThresh[] = "Umbral temperatura alta (&deg;)";
const char wcLowTempThresh[] = "Umbral temperatura baja (&deg;)";
const char wcHighTempColour[] = "Color temp alta";
const char wcMidTempColour[] = "Color temp media";
const char wcLowTempColour[] = "Color temp baja";
const char wcHighPrecipThresh[] = "Umbral precipitacion alta (%)";
const char wcLowPrecipThresh[] = "Umbral precipitacion baja (%)";
const char wcHighPOPColour[] = "Color POP alto";
const char wcMidPOPColour[] = "Color POP medio";
const char wcLowPOPColour[] = "Color POP bajo";
const char wcSaveBtn[] = "Guardar";
const char wcCancelBtn[] = "Cancelar";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case 0: return "Cielo despejado";
    case 1: return "Mayormente despejado";
    case 2: return "Parcialmente nublado";
    case 3: return "Nublado";
    case 45: return "Niebla";
    case 48: return "Niebla helada";
    case 51: return "Llovizna ligera";
    case 53: return "Llovizna";
    case 55: return "Llovizna intensa";
    case 56: return "Llovizna helada ligera";
    case 57: return "Llovizna helada";
    case 61: return "Lluvia ligera";
    case 63: return "Lluvia";
    case 65: return "Lluvia intensa";
    case 66: return "Lluvia helada ligera";
    case 67: return "Lluvia helada";
    case 71: return "Nevada ligera";
    case 73: return "Nevada";
    case 75: return "Nevada intensa";
    case 77: return "Granizo menudo";
    case 80: return "Chubascos ligeros";
    case 81: return "Chubascos";
    case 82: return "Chubascos intensos";
    case 85: return "Chubascos de nieve";
    case 86: return "Chubascos de nieve intensos";
    case 95: return "Tormenta";
    case 96: return "Tormenta con granizo";
    case 99: return "Tormenta con granizo";
    default: return "Desconocido";
  }
}
inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1) return "Despejado";
  if (id == 2 || id == 3) return "Nubes";
  if (id == 45 || id == 48) return "Niebla";
  if (id >= 51 && id <= 57) return "Llovizna";
  if (id >= 61 && id <= 67) return "Lluvia";
  if (id >= 71 && id <= 77) return "Nieve";
  if (id >= 80 && id <= 82) return "Lluvia";
  if (id == 85 || id == 86) return "Nieve";
  if (id == 95 || id == 96 || id == 99) return "Tormenta";
  return "Desconocido";
}

#endif  // LANG_ES


// =============================================================================
//  DUTCH
// =============================================================================
#ifdef LANG_NL

#define NOMINATIM_LANG "nl"

static const char* shortMonth[12] = {
  "Jan","Feb","Mrt","Apr","Mei","Jun",
  "Jul","Aug","Sep","Okt","Nov","Dec"};

static const char* shortDOW[8] = { "???", "ZON", "MAA", "DIN", "WOE", "DON", "VRI", "ZAT" };

const char sunStr[] = "Zon";
const char cloudStr[] = "Bewolking";
const char humidityStr[] = "Vochtigheid";

static const char* moonPhase[8] = {
  "Nieuw", "Wassend", "Kwartier", "Wassend",
  "Vol", "Afnemend", "Kwartier", "Afnemend"
};

const char riseStr[] = "SR:";
const char setStr[] = "SS:";

const char updatedStr[] = "Bijgewerkt: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";

const char forecastTitleStr[] = "Voorspelling 6u";
const char popLabelStr[] = "Neer%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Dauw:";

const char bootUpdatingTime[] = "Tijd bijwerken...";
const char bootUpdatingConditions[] = "Weersomstandigheden laden...";
const char bootDone[] = "Klaar...";
const char bootConnectedWifi[] = "Verbonden met WiFi";
const char bootFetchingWeather[] = "Weerdata ophalen...";
const char bootFlashOK[] = "Flash FS gereed!";
const char bootFlashFail[] = "Flash FS fout!";
const char bootRetry[] = "Mislukt, opnieuw in 60s...";

const char creditOriginal[] = "Origineel: blog.squix.org";
const char creditBodmer[] = "Aangepast door: Bodmer";
const char creditWabbit[] = "Open-Meteo door: Wabbitguy";

const char wcTitle[] = "WabbitWeather";
const char wcSaved[] = "&#10003; Instellingen opgeslagen.";
const char wcWifiResetTitle[] = "&#x26A0; WiFi resetten?";
const char wcWifiResetBody[] = "Verwijdert WiFi-gegevens en herstart in configuratiemodus.";
const char wcWifiResetYes[] = "Ja, resetten";
const char wcWifiResetCancel[] = "Annuleren";
const char wcWifiResetBtn[] = "&#x26A0; WiFi Reset";
const char wcWifiResetting[] = "WiFi wordt gereset...";
const char wcWifiResetHotspot[] = "Herstarten. Verbind met het <strong>WabbitWeather</strong> hotspot.";
const char wcSecLocation[] = "Locatie";
const char wcSecDisplay[] = "Weergave-instellingen";
const char wcSecTempColours[] = "Temperatuurdrempelkleuren";
const char wcSecPrecipColours[] = "Neerslagdrempelkleuren";
const char wcLatLabel[] = "Breedtegraad";
const char wcLonLabel[] = "Lengtegraad";
const char wcUnitsLabel[] = "Temperatuureenheid";
const char wcImperial[] = "Imperiaal (&deg;F)";
const char wcMetric[] = "Metrisch (&deg;C)";
const char wcClock24[] = "24-uursklok";
const char wcAutoDim[] = "Automatisch dimmen bij schemering";
const char wcNightBright[] = "Nachthelderheid (0-255)";
const char wcNightBrightHint[] = "0 = scherm uit";
const char wcShowBaro[] = "Luchtdruk weergeven";
const char wcShowPrecip[] = "Neerslagkans weergeven";
const char wcShowUV[] = "UV-index weergeven";
const char wcShow6Hr[] = "Voorspelling 6u weergeven";
const char wcForecastStart[]    = "Voorspelling vanaf";
const char wcForecastToday[]    = "Vandaag";
const char wcForecastTomorrow[] = "Morgen";
const char wcWeatherHeadings[] = "Kleur weertitels";
const char wcAstroHeadings[] = "Kleur astronomietitels";
const char wcHighTempThresh[] = "Hoge temp-drempel (&deg;)";
const char wcLowTempThresh[] = "Lage temp-drempel (&deg;)";
const char wcHighTempColour[] = "Kleur hoge temp";
const char wcMidTempColour[] = "Kleur gemiddelde temp";
const char wcLowTempColour[] = "Kleur lage temp";
const char wcHighPrecipThresh[] = "Hoge neerslagdrempel (%)";
const char wcLowPrecipThresh[] = "Lage neerslagdrempel (%)";
const char wcHighPOPColour[] = "Kleur hoge POP";
const char wcMidPOPColour[] = "Kleur gemiddelde POP";
const char wcLowPOPColour[] = "Kleur lage POP";
const char wcSaveBtn[] = "Opslaan";
const char wcCancelBtn[] = "Annuleren";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case 0: return "Heldere lucht";
    case 1: return "Overwegend helder";
    case 2: return "Gedeeltelijk bewolkt";
    case 3: return "Bewolkt";
    case 45: return "Mist";
    case 48: return "IJsmist";
    case 51: return "Lichte motregen";
    case 53: return "Motregen";
    case 55: return "Zware motregen";
    case 56: return "Lichte ijzel";
    case 57: return "IJzel";
    case 61: return "Lichte regen";
    case 63: return "Regen";
    case 65: return "Zware regen";
    case 66: return "Lichte ijsregen";
    case 67: return "IJsregen";
    case 71: return "Lichte sneeuw";
    case 73: return "Sneeuw";
    case 75: return "Zware sneeuw";
    case 77: return "Sneeuwkorrels";
    case 80: return "Lichte buien";
    case 81: return "Buien";
    case 82: return "Zware buien";
    case 85: return "Sneeuwbuien";
    case 86: return "Zware sneeuwbuien";
    case 95: return "Onweer";
    case 96: return "Onweer met hagel";
    case 99: return "Onweer met hagel";
    default: return "Onbekend";
  }
}
inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1) return "Helder";
  if (id == 2 || id == 3) return "Wolken";
  if (id == 45 || id == 48) return "Mist";
  if (id >= 51 && id <= 57) return "Motregen";
  if (id >= 61 && id <= 67) return "Regen";
  if (id >= 71 && id <= 77) return "Sneeuw";
  if (id >= 80 && id <= 82) return "Regen";
  if (id == 85 || id == 86) return "Sneeuw";
  if (id == 95 || id == 96 || id == 99) return "Onweer";
  return "Onbekend";
}

#endif  // LANG_NL

// =============================================================================
//  PORTUGUESE
// =============================================================================
#ifdef LANG_PT

#define NOMINATIM_LANG "pt"

static const char* shortDOW[8] = { "???", "DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB" };

const char sunStr[]      = "Sol";
const char cloudStr[]    = "Nuvens";
const char humidityStr[] = "Humidade";

static const char* moonPhase[8] = {
  "Nova", "Crescente", "Quarto", "Crescente",
  "Cheia", "Minguante", "Quarto", "Minguante"
};

const char riseStr[] = "SR:";
const char setStr[]  = "SS:";

const char updatedStr[]    = "Atualizado: ";
const char windMetric[]    = " m/s";
const char windImperial[]  = " mph";
const char pressImperial[] = " in";
const char pressMetric[]   = " hPa";
const char tempMetric[]    = "oC";
const char tempImperial[]  = "oF";

static const char* shortMonth[12] = {
  "Jan","Fev","Mar","Abr","Mai","Jun",
  "Jul","Ago","Set","Out","Nov","Dez"};

const char forecastTitleStr[] = "Previsao 6h";
const char popLabelStr[]      = "Prec%:";
const char tempLabelStr[]     = "Temp:";
const char dewLabelStr[]      = " Orv:";

const char bootUpdatingTime[]       = "Atualizando hora...";
const char bootUpdatingConditions[] = "Atualizando condicoes...";
const char bootDone[]               = "Concluido...";
const char bootConnectedWifi[]      = "WiFi conectado";
const char bootFetchingWeather[]    = "Obtendo dados...";
const char bootFlashOK[]            = "Flash FS pronto!";
const char bootFlashFail[]          = "Erro no Flash FS!";
const char bootRetry[]              = "Falha, tentando em 60s...";

const char creditOriginal[]  = "Original: blog.squix.org";
const char creditBodmer[]    = "Adaptado por: Bodmer";
const char creditWabbit[]    = "Open-Meteo por: Wabbitguy";

const char wcTitle[]            = "WabbitWeather";
const char wcSaved[]            = "&#10003; Configuracoes salvas.";
const char wcWifiResetTitle[]   = "&#x26A0; Redefinir WiFi?";
const char wcWifiResetBody[]    = "Apaga credenciais WiFi e reinicia no modo configuracao.";
const char wcWifiResetYes[]     = "Sim, redefinir";
const char wcWifiResetCancel[]  = "Cancelar";
const char wcWifiResetBtn[]     = "&#x26A0; WiFi Reset";
const char wcWifiResetting[]    = "Redefinindo WiFi...";
const char wcWifiResetHotspot[] = "Reiniciando. Conecte-se ao hotspot <strong>WabbitWeather</strong>.";
const char wcSecLocation[]      = "Localizacao";
const char wcSecDisplay[]       = "Configuracoes de exibicao";
const char wcSecTempColours[]   = "Cores de temperatura";
const char wcSecPrecipColours[] = "Cores de precipitacao";
const char wcLatLabel[]         = "Latitude";
const char wcLonLabel[]         = "Longitude";
const char wcUnitsLabel[]       = "Unidade de temperatura";
const char wcImperial[]         = "Imperial (&deg;F)";
const char wcMetric[]           = "Metrico (&deg;C)";
const char wcClock24[]          = "Relogio 24h";
const char wcAutoDim[]          = "Diminuir ao anoitecer";
const char wcNightBright[]      = "Brilho noturno (0-255)";
const char wcNightBrightHint[]  = "0 = tela apagada";
const char wcShowBaro[]         = "Mostrar pressao barometrica";
const char wcShowPrecip[]       = "Mostrar probabilidade de chuva";
const char wcShowUV[]           = "Mostrar indice UV";
const char wcShow6Hr[]          = "Previsao de 6h";
const char wcForecastStart[]    = "Inicio previsao";
const char wcForecastToday[]    = "Hoje";
const char wcForecastTomorrow[] = "Amanha";
const char wcWeatherHeadings[]  = "Cor titulos meteorologia";
const char wcAstroHeadings[]    = "Cor titulos astronomia";
const char wcHighTempThresh[]   = "Limite temp alta (&deg;)";
const char wcLowTempThresh[]    = "Limite temp baixa (&deg;)";
const char wcHighTempColour[]   = "Cor temp alta";
const char wcMidTempColour[]    = "Cor temp media";
const char wcLowTempColour[]    = "Cor temp baixa";
const char wcHighPrecipThresh[] = "Limite precipitacao alta (%)";
const char wcLowPrecipThresh[]  = "Limite precipitacao baixa (%)";
const char wcHighPOPColour[]    = "Cor POP alto";
const char wcMidPOPColour[]     = "Cor POP medio";
const char wcLowPOPColour[]     = "Cor POP baixo";
const char wcSaveBtn[]          = "Salvar";
const char wcCancelBtn[]        = "Cancelar";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case  0: return "Ceu limpo";
    case  1: return "Predominantemente limpo";
    case  2: return "Parcialmente nublado";
    case  3: return "Nublado";
    case 45: return "Neblina";
    case 48: return "Neblina gelada";
    case 51: return "Garoa leve";
    case 53: return "Garoa";
    case 55: return "Garoa intensa";
    case 56: return "Garoa gelada leve";
    case 57: return "Garoa gelada";
    case 61: return "Chuva leve";
    case 63: return "Chuva";
    case 65: return "Chuva intensa";
    case 66: return "Chuva gelada leve";
    case 67: return "Chuva gelada";
    case 71: return "Neve leve";
    case 73: return "Neve";
    case 75: return "Neve intensa";
    case 77: return "Graos de neve";
    case 80: return "Pancadas leves";
    case 81: return "Pancadas de chuva";
    case 82: return "Pancadas intensas";
    case 85: return "Pancadas de neve";
    case 86: return "Pancadas de neve intensas";
    case 95: return "Tempestade";
    case 96: return "Tempestade com granizo";
    case 99: return "Tempestade com granizo";
    default: return "Desconhecido";
  }
}

inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1)             return "Limpo";
  if (id == 2 || id == 3)             return "Nuvens";
  if (id == 45 || id == 48)           return "Neblina";
  if (id >= 51 && id <= 57)           return "Garoa";
  if (id >= 61 && id <= 67)           return "Chuva";
  if (id >= 71 && id <= 77)           return "Neve";
  if (id >= 80 && id <= 82)           return "Chuva";
  if (id == 85 || id == 86)           return "Neve";
  if (id == 95 || id == 96 || id==99) return "Tempestade";
  return "Desconhecido";
}

#endif // LANG_PT

// =============================================================================
//  TURKISH
// =============================================================================
#ifdef LANG_TR

#define NOMINATIM_LANG "tr"

static const char* shortDOW[8] = { "???", "PAZ", "PZT", "SAL", "CAR", "PER", "CUM", "CMT" };

const char sunStr[]      = "Gunes";
const char cloudStr[]    = "Bulut";
const char humidityStr[] = "Nem";

static const char* moonPhase[8] = {
  "Yeni", "Hilal", "Ilk Cey", "Hilal",
  "Dolun", "Son Cey", "Son Cey", "Hilal"
};

const char riseStr[] = "SR:";
const char setStr[]  = "SS:";

const char updatedStr[]    = "Guncellendi: ";
const char windMetric[]    = " m/s";
const char windImperial[]  = " mph";
const char pressImperial[] = " in";
const char pressMetric[]   = " hPa";
const char tempMetric[]    = "oC";
const char tempImperial[]  = "oF";

static const char* shortMonth[12] = {
  "Oca","Sub","Mar","Nis","May","Haz",
  "Tem","Agu","Eyl","Eki","Kas","Ara"};

const char forecastTitleStr[] = "6s Tahmin";
const char popLabelStr[]      = "Yag%:";
const char tempLabelStr[]     = "Sic:";
const char dewLabelStr[]      = " Ciy:";

const char bootUpdatingTime[]       = "Saat guncelleniyor...";
const char bootUpdatingConditions[] = "Hava durumu yukleniyor...";
const char bootDone[]               = "Tamamlandi...";
const char bootConnectedWifi[]      = "WiFi baglandi";
const char bootFetchingWeather[]    = "Hava verisi aliniyor...";
const char bootFlashOK[]            = "Flash FS hazir!";
const char bootFlashFail[]          = "Flash FS hatasi!";
const char bootRetry[]              = "Basarisiz, 60s sonra tekrar...";

const char creditOriginal[]  = "Orijinal: blog.squix.org";
const char creditBodmer[]    = "Uyarlayan: Bodmer";
const char creditWabbit[]    = "Open-Meteo: Wabbitguy";

const char wcTitle[]            = "WabbitWeather";
const char wcSaved[]            = "&#10003; Ayarlar kaydedildi.";
const char wcWifiResetTitle[]   = "&#x26A0; WiFi Sifirla?";
const char wcWifiResetBody[]    = "WiFi bilgilerini siler ve yapilandirma modunda yeniden baslatir.";
const char wcWifiResetYes[]     = "Evet, sifirla";
const char wcWifiResetCancel[]  = "Iptal";
const char wcWifiResetBtn[]     = "&#x26A0; WiFi Reset";
const char wcWifiResetting[]    = "WiFi sifirlanıyor...";
const char wcWifiResetHotspot[] = "Yeniden baslatiliyor. <strong>WabbitWeather</strong> hotspotuna baglanin.";
const char wcSecLocation[]      = "Konum";
const char wcSecDisplay[]       = "Ekran Ayarlari";
const char wcSecTempColours[]   = "Sicaklik Esik Renkleri";
const char wcSecPrecipColours[] = "Yagis Esik Renkleri";
const char wcLatLabel[]         = "Enlem";
const char wcLonLabel[]         = "Boylam";
const char wcUnitsLabel[]       = "Sicaklik Birimi";
const char wcImperial[]         = "Emperyal (&deg;F)";
const char wcMetric[]           = "Metrik (&deg;C)";
const char wcClock24[]          = "24 saat formati";
const char wcAutoDim[]          = "Alacakaranlıkta karart";
const char wcNightBright[]      = "Gece parlaklik (0-255)";
const char wcNightBrightHint[]  = "0 = ekran kapali";
const char wcShowBaro[]         = "Barometre goster";
const char wcShowPrecip[]       = "Yagis olasiligi goster";
const char wcShowUV[]           = "UV indeksi goster";
const char wcShow6Hr[]          = "6 saatlik tahmin goster";
const char wcForecastStart[]    = "Tahmin baslangici";
const char wcForecastToday[]    = "Bugun";
const char wcForecastTomorrow[] = "Yarin";
const char wcWeatherHeadings[]  = "Hava durumu baslik rengi";
const char wcAstroHeadings[]    = "Astronomi baslik rengi";
const char wcHighTempThresh[]   = "Yuksek sicaklik esigi (&deg;)";
const char wcLowTempThresh[]    = "Dusuk sicaklik esigi (&deg;)";
const char wcHighTempColour[]   = "Yuksek sicaklik rengi";
const char wcMidTempColour[]    = "Orta sicaklik rengi";
const char wcLowTempColour[]    = "Dusuk sicaklik rengi";
const char wcHighPrecipThresh[] = "Yuksek yagis esigi (%)";
const char wcLowPrecipThresh[]  = "Dusuk yagis esigi (%)";
const char wcHighPOPColour[]    = "Yuksek YOO rengi";
const char wcMidPOPColour[]     = "Orta YOO rengi";
const char wcLowPOPColour[]     = "Dusuk YOO rengi";
const char wcSaveBtn[]          = "Kaydet";
const char wcCancelBtn[]        = "Iptal";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case  0: return "Acik hava";
    case  1: return "Cok az bulutlu";
    case  2: return "Parcali bulutlu";
    case  3: return "Bulutlu";
    case 45: return "Sisli";
    case 48: return "Buzlu sis";
    case 51: return "Hafif cicek yagmuru";
    case 53: return "Cicek yagmuru";
    case 55: return "Yogun cicek yagmuru";
    case 56: return "Hafif buzlu cicek yagmuru";
    case 57: return "Buzlu cicek yagmuru";
    case 61: return "Hafif yagmur";
    case 63: return "Yagmur";
    case 65: return "Yogun yagmur";
    case 66: return "Hafif buzlu yagmur";
    case 67: return "Buzlu yagmur";
    case 71: return "Hafif kar";
    case 73: return "Kar";
    case 75: return "Yogun kar";
    case 77: return "Kar taneleri";
    case 80: return "Hafif saganak";
    case 81: return "Saganak yagmur";
    case 82: return "Yogun saganak";
    case 85: return "Karli saganak";
    case 86: return "Yogun karli saganak";
    case 95: return "Firtina";
    case 96: return "Dolu ile firtina";
    case 99: return "Dolu ile firtina";
    default: return "Bilinmiyor";
  }
}

inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1)             return "Acik";
  if (id == 2 || id == 3)             return "Bulutlu";
  if (id == 45 || id == 48)           return "Sis";
  if (id >= 51 && id <= 57)           return "Cicek yagmuru";
  if (id >= 61 && id <= 67)           return "Yagmur";
  if (id >= 71 && id <= 77)           return "Kar";
  if (id >= 80 && id <= 82)           return "Yagmur";
  if (id == 85 || id == 86)           return "Kar";
  if (id == 95 || id == 96 || id==99) return "Firtina";
  return "Bilinmiyor";
}

#endif // LANG_TR

#endif  // TRANSLATION_H