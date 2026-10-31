// =============================================================================
// Translation.h — all screen and web page text; the language is chosen in All_Settings.h
// NotoSansBold15 text can use accents (Latin-1 + Turkish); Font 2/4 text must stay plain ASCII.
// =============================================================================

#ifndef TRANSLATION_H
#define TRANSLATION_H

// -----------------------------------------------------------------------------
// Default to English if nothing is defined
// -----------------------------------------------------------------------------
#if !defined(LANG_EN) && !defined(LANG_FR) && !defined(LANG_DE) && \
    !defined(LANG_ES) && !defined(LANG_NL) && !defined(LANG_PT) && \
    !defined(LANG_TR) && !defined(LANG_IT)
  #define LANG_EN
#endif

// =============================================================================
//  ENGLISH
// =============================================================================
#ifdef LANG_EN

// Language for the Nominatim place name lookup
#define NOMINATIM_LANG "en"

// ── Calendar ─────────────────────────────────────────────────────────────────
static const char* shortDOW[8] = { "???", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

static const char* shortMonth[12] = {
  "Jan","Feb","Mar","Apr","May","Jun",
  "Jul","Aug","Sep","Oct","Nov","Dec"};

static const char* moonPhase[8] = {
  "New", "Waxing", "1st qtr", "Waxing", "Full", "Waning", "Last qtr", "Waning"
};
// ── Astronomy panel ───────────────────────────────────────────────────────────
const char sunStr[] = "Sun";
const char cloudStr[] = "Cloud";
const char humidityStr[] = "Humidity";

// ── Astronomy rise/set labels ─────────────────────────────────────────────────
const char riseStr[] = "SR:";
const char setStr[]  = "SS:";

// ── Current weather panel ─────────────────────────────────────────────────────
const char updatedStr[] = "Updated: ";
// Replaces updatedStr when the weather hasn't refreshed for an hour (FAILURE_WARNING_SECS)
const char failedStr[] = "Failed: ";

// Wind speed units
const char windMetric[] = " m/s";
const char windImperial[] = " mph";

// Pressure units
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";

// Temperature unit symbol (small font uses 'o' not the degree glyph)
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

// ── 6hr forecast panel ───────────────────────────────────────────────────────
const char forecastTitleStr[] = "6Hr Forecast";
const char popLabelStr[] = "Pop%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Dew:";

// ── Boot / progress messages ──────────────────────────────────────────────────
const char bootUpdatingTime[] = "Updating time...";
const char bootUpdatingConditions[] = "Updating conditions...";
const char bootDone[] = "Done...";
const char bootConnectedWifi[] = "Connected to WiFi";
const char bootFetchingWeather[] = "Fetching weather data...";
const char bootFlashOK[] = "Flash FS available!";
const char bootFlashFail[] = "Flash FS initialisation failed!";
const char bootRetry[] = "Fetch failed, retrying in 60 seconds...";
const char strAPActive[] = "Access Point Active";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Scan to join Wi-Fi";  // under the setup QR codes
const char strAPStep2[] = "2. Scan to open setup";
const char strLocalWeather[] = "Local Weather";  // city name too long to show

// ── Splash screen credits ─────────────────────────────────────────────────────
const char creditOriginal[] = "Original by: blog.squix.org";
const char creditBodmer[] = "Adapted by: Bodmer";
const char creditWabbit[] = "Open-Meteo by: Wabbitguy";

// ── WebConfig UI strings ──────────────────────────────────────────────────────

// Page / header
const char wcSaved[] = "&#10003; Settings saved and applied.";

// WiFi reset modal
const char wcWifiResetTitle[] = "&#x26A0; Reset WiFi?";
const char wcWifiResetBody[] = "Erases saved WiFi credentials and reboots into "
                               "setup mode. Reconnect via the " HOSTNAME " hotspot.";
const char wcWifiResetYes[] = "Yes, reset";
const char wcWifiResetCancel[] = "Cancel";
const char wcWifiResetBtn[] = "&#x26A0; Reset WiFi";
const char wcWifiResetting[] = "Resetting WiFi...";
const char wcWifiResetHotspot[] = "Rebooting. Connect to the <strong>" HOSTNAME "</strong> hotspot.";

// Section headings
const char wcSecLocation[] = "Location";
const char wcLocName[] = "name on display";  // after "TZ1 " on the settings page
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
const char wcAutoDim[] = "Auto dim at sunset";
const char wcNightBright[] = "Night brightness (0-255)";
const char wcNightBrightHint[] = "0 = display off, 1 = barely visible";
const char wcShowBaro[] = "Show barometric pressure";
const char wcShowPrecip[] = "Show precip probability";
const char wcShowUV[] = "Show UV index";
const char wcShow6Hr[] = "Show 6hr forecast";
const char wcForecastStart[]   = "Forecast start";
const char wcForecastToday[]   = "Today";
const char wcForecastTomorrow[] = "Tomorrow";
const char wcIconSet[]         = "Weather icons";
const char wcIconStandard[]    = "Standard";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Invert colours";
const char wcInvertHint[]      = "Colour inversion correction";
const char wcPressure[]        = "Pressure";
const char wcPressSeaLevel[]   = "Sea level";
const char wcPressStation[]    = "Station";
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
const char humidityStr[] = "Humidité";

static const char* moonPhase[8] = {
  "Nouvelle", "Croissant", "1er qrt", "Croissant",
  "Pleine", "Décroissant", "Der qrt", "Décroissant"
};

const char riseStr[] = "SR:";
const char setStr[] = "SS:";

const char updatedStr[] = "Mis à jour: ";
const char failedStr[] = "Échec: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

const char forecastTitleStr[] = "Prévisions 6h";
const char popLabelStr[] = "Préc%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Rosée:";

const char bootUpdatingTime[] = "Mise à jour heure...";
const char bootUpdatingConditions[] = "Téléchargement météo...";
const char bootDone[] = "Terminé...";
const char bootConnectedWifi[] = "WiFi connecté";
const char bootFetchingWeather[] = "Chargement météo...";
const char bootFlashOK[] = "Flash FS prêt!";
const char bootFlashFail[] = "Erreur Flash FS!";
const char bootRetry[] = "Échec, nouvel essai dans 60s...";
const char strAPActive[] = "Point d'accès actif";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Scannez pour le Wi-Fi";  // under the setup QR codes
const char strAPStep2[] = "2. Scannez pour configurer";
const char strLocalWeather[] = "Météo locale";  // city name too long to show

const char creditOriginal[] = "Original: blog.squix.org";
const char creditBodmer[] = "Adapté par: Bodmer";
const char creditWabbit[] = "Open-Meteo par: Wabbitguy";

const char wcSaved[] = "&#10003; Paramètres enregistrés.";
const char wcWifiResetTitle[] = "&#x26A0; Réinitialiser WiFi ?";
const char wcWifiResetBody[] = "Efface les identifiants WiFi et redémarre en mode configuration.";
const char wcWifiResetYes[] = "Oui, réinitialiser";
const char wcWifiResetCancel[] = "Annuler";
const char wcWifiResetBtn[] = "&#x26A0; Réinitialiser WiFi";
const char wcWifiResetting[] = "Réinitialisation WiFi...";
const char wcWifiResetHotspot[] = "Redémarrage. Connectez-vous au hotspot <strong>" HOSTNAME "</strong>.";
const char wcSecLocation[] = "Emplacement";
const char wcLocName[] = "nom affiché";  // after "TZ1 " on the settings page
const char wcSecDisplay[] = "Paramètres d'affichage";
const char wcSecTempColours[] = "Couleurs de température";
const char wcSecPrecipColours[] = "Couleurs de précipitation";
const char wcLatLabel[] = "Latitude";
const char wcLonLabel[] = "Longitude";
const char wcUnitsLabel[] = "Unités de température";
const char wcImperial[] = "Impériales (&deg;F)";
const char wcMetric[] = "Métriques (&deg;C)";
const char wcClock24[] = "Horloge 24h";
const char wcAutoDim[] = "Atténuation auto au coucher du soleil";
const char wcNightBright[] = "Luminosité nocturne (0-255)";
const char wcNightBrightHint[] = "0 = écran éteint";
const char wcShowBaro[] = "Afficher la pression";
const char wcShowPrecip[] = "Afficher la probabilité de pluie";
const char wcShowUV[] = "Afficher l'indice UV";
const char wcShow6Hr[] = "Prévisions sur 6h";
const char wcForecastStart[]    = "Début prévisions";
const char wcForecastToday[]    = "Aujourd'hui";
const char wcForecastTomorrow[] = "Demain";
const char wcIconSet[]         = "Icônes météo";
const char wcIconStandard[]    = "Standard";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Inverser les couleurs";
const char wcInvertHint[]      = "Correction de l'inversion des couleurs";
const char wcPressure[]        = "Pression";
const char wcPressSeaLevel[]   = "Niveau de la mer";
const char wcPressStation[]    = "Station";
const char wcWeatherHeadings[] = "Couleur titres météo";
const char wcAstroHeadings[] = "Couleur titres astronomie";
const char wcHighTempThresh[] = "Seuil température haute (&deg;)";
const char wcLowTempThresh[] = "Seuil température basse (&deg;)";
const char wcHighTempColour[] = "Couleur temp haute";
const char wcMidTempColour[] = "Couleur temp moyenne";
const char wcLowTempColour[] = "Couleur temp basse";
const char wcHighPrecipThresh[] = "Seuil précipitation haute (%)";
const char wcLowPrecipThresh[] = "Seuil précipitation basse (%)";
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
    case 51: return "Bruine légère";
    case 53: return "Bruine";
    case 55: return "Bruine forte";
    case 56: return "Bruine verglaçante légère";
    case 57: return "Bruine verglaçante";
    case 61: return "Pluie légère";
    case 63: return "Pluie";
    case 65: return "Forte pluie";
    case 66: return "Pluie verglaçante légère";
    case 67: return "Pluie verglaçante";
    case 71: return "Neige légère";
    case 73: return "Neige";
    case 75: return "Forte neige";
    case 77: return "Grains de neige";
    case 80: return "Averses légères";
    case 81: return "Averses";
    case 82: return "Fortes averses";
    case 85: return "Averses de neige";
    case 86: return "Fortes averses de neige";
    case 95: return "Orage";
    case 96: return "Orage avec grêle";
    case 99: return "Orage avec grêle";
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
const char failedStr[] = "Fehler: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

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
const char strAPActive[] = "Zugangspunkt aktiv";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Scannen für WLAN";  // under the setup QR codes
const char strAPStep2[] = "2. Scannen zum Einrichten";
const char strLocalWeather[] = "Lokales Wetter";  // city name too long to show

const char creditOriginal[] = "Original: blog.squix.org";
const char creditBodmer[] = "Angepasst von: Bodmer";
const char creditWabbit[] = "Open-Meteo von: Wabbitguy";

const char wcSaved[] = "&#10003; Einstellungen gespeichert.";
const char wcWifiResetTitle[] = "&#x26A0; WiFi zurücksetzen?";
const char wcWifiResetBody[] = "Löscht WLAN-Daten und startet im Konfigurationsmodus neu.";
const char wcWifiResetYes[] = "Ja, zurücksetzen";
const char wcWifiResetCancel[] = "Abbrechen";
const char wcWifiResetBtn[] = "&#x26A0; WiFi zurücksetzen";
const char wcWifiResetting[] = "WiFi wird zurückgesetzt...";
const char wcWifiResetHotspot[] = "Neustart. Verbinde mit dem Hotspot <strong>" HOSTNAME "</strong>.";
const char wcSecLocation[] = "Standort";
const char wcLocName[] = "Anzeigename";  // after "TZ1 " on the settings page
const char wcSecDisplay[] = "Anzeigeeinstellungen";
const char wcSecTempColours[] = "Temperaturfarbschwellen";
const char wcSecPrecipColours[] = "Niederschlagsfarbschwellen";
const char wcLatLabel[] = "Breitengrad";
const char wcLonLabel[] = "Längengrad";
const char wcUnitsLabel[] = "Temperatureinheit";
const char wcImperial[] = "Imperial (&deg;F)";
const char wcMetric[] = "Metrisch (&deg;C)";
const char wcClock24[] = "24-Stunden-Uhr";
const char wcAutoDim[] = "Bei Sonnenuntergang abdunkeln";
const char wcNightBright[] = "Nachthelligkeit (0-255)";
const char wcNightBrightHint[] = "0 = Display aus";
const char wcShowBaro[] = "Luftdruck anzeigen";
const char wcShowPrecip[] = "Niederschlagswahrscheinlichkeit anzeigen";
const char wcShowUV[] = "UV-Index anzeigen";
const char wcShow6Hr[] = "6-Std-Vorhersage anzeigen";
const char wcForecastStart[]    = "Vorhersage ab";
const char wcForecastToday[]    = "Heute";
const char wcForecastTomorrow[] = "Morgen";
const char wcIconSet[]         = "Wettersymbole";
const char wcIconStandard[]    = "Standard";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Farben invertieren";
const char wcInvertHint[]      = "Korrektur der Farbumkehrung";
const char wcPressure[]        = "Luftdruck";
const char wcPressSeaLevel[]   = "Meeresniveau";
const char wcPressStation[]    = "Standort";
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
    case 1: return "Überwiegend klar";
    case 2: return "Teilweise bewölkt";
    case 3: return "Bedeckt";
    case 45: return "Neblig";
    case 48: return "Gefrierender Nebel";
    case 51: return "Leichtes Nieseln";
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

static const char* shortDOW[8] = { "???", "DOM", "LUN", "MAR", "MIÉ", "JUE", "VIE", "SÁB" };

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
const char failedStr[] = "Fallo: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

const char forecastTitleStr[] = "Pronóstico 6h";
const char popLabelStr[] = "Prec%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Roc:";

const char bootUpdatingTime[] = "Actualizando hora...";
const char bootUpdatingConditions[] = "Actualizando condiciones...";
const char bootDone[] = "Listo...";
const char bootConnectedWifi[] = "WiFi conectado";
const char bootFetchingWeather[] = "Obteniendo datos...";
const char bootFlashOK[] = "¡Flash FS listo!";
const char bootFlashFail[] = "¡Error en Flash FS!";
const char bootRetry[] = "Fallo, reintentando en 60s...";
const char strAPActive[] = "Punto de acceso activo";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Escanee para el Wi-Fi";  // under the setup QR codes
const char strAPStep2[] = "2. Escanee para configurar";
const char strLocalWeather[] = "Tiempo local";  // city name too long to show

const char creditOriginal[] = "Original: blog.squix.org";
const char creditBodmer[] = "Adaptado por: Bodmer";
const char creditWabbit[] = "Open-Meteo por: Wabbitguy";

const char wcSaved[] = "&#10003; Configuración guardada.";
const char wcWifiResetTitle[] = "&#x26A0; ¿Restablecer WiFi?";
const char wcWifiResetBody[] = "Borra las credenciales WiFi y reinicia en modo configuración.";
const char wcWifiResetYes[] = "Sí, restablecer";
const char wcWifiResetCancel[] = "Cancelar";
const char wcWifiResetBtn[] = "&#x26A0; Restablecer WiFi";
const char wcWifiResetting[] = "Restableciendo WiFi...";
const char wcWifiResetHotspot[] = "Reiniciando. Conéctese al hotspot <strong>" HOSTNAME "</strong>.";
const char wcSecLocation[] = "Ubicación";
const char wcLocName[] = "nombre en pantalla";  // after "TZ1 " on the settings page
const char wcSecDisplay[] = "Configuración de pantalla";
const char wcSecTempColours[] = "Colores de temperatura";
const char wcSecPrecipColours[] = "Colores de precipitación";
const char wcLatLabel[] = "Latitud";
const char wcLonLabel[] = "Longitud";
const char wcUnitsLabel[] = "Unidades de temperatura";
const char wcImperial[] = "Imperial (&deg;F)";
const char wcMetric[] = "Métrico (&deg;C)";
const char wcClock24[] = "Reloj de 24 horas";
const char wcAutoDim[] = "Atenuar al ponerse el sol";
const char wcNightBright[] = "Brillo nocturno (0-255)";
const char wcNightBrightHint[] = "0 = pantalla apagada";
const char wcShowBaro[] = "Mostrar presión barométrica";
const char wcShowPrecip[] = "Mostrar probabilidad de precipitación";
const char wcShowUV[] = "Mostrar índice UV";
const char wcShow6Hr[] = "Mostrar pronóstico de 6h";
const char wcForecastStart[]    = "Inicio pronóstico";
const char wcForecastToday[]    = "Hoy";
const char wcForecastTomorrow[] = "Mañana";
const char wcIconSet[]         = "Iconos del tiempo";
const char wcIconStandard[]    = "Estándar";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Invertir colores";
const char wcInvertHint[]      = "Corrección de inversión de color";
const char wcPressure[]        = "Presión";
const char wcPressSeaLevel[]   = "Nivel del mar";
const char wcPressStation[]    = "Estación";
const char wcWeatherHeadings[] = "Color títulos meteo";
const char wcAstroHeadings[] = "Color títulos astronomía";
const char wcHighTempThresh[] = "Umbral temperatura alta (&deg;)";
const char wcLowTempThresh[] = "Umbral temperatura baja (&deg;)";
const char wcHighTempColour[] = "Color temp alta";
const char wcMidTempColour[] = "Color temp media";
const char wcLowTempColour[] = "Color temp baja";
const char wcHighPrecipThresh[] = "Umbral precipitación alta (%)";
const char wcLowPrecipThresh[] = "Umbral precipitación baja (%)";
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
const char failedStr[] = "Mislukt: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

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
const char strAPActive[] = "Toegangspunt actief";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Scan voor wifi";  // under the setup QR codes
const char strAPStep2[] = "2. Scan om in te stellen";
const char strLocalWeather[] = "Lokaal weer";  // city name too long to show

const char creditOriginal[] = "Origineel: blog.squix.org";
const char creditBodmer[] = "Aangepast door: Bodmer";
const char creditWabbit[] = "Open-Meteo door: Wabbitguy";

const char wcSaved[] = "&#10003; Instellingen opgeslagen.";
const char wcWifiResetTitle[] = "&#x26A0; WiFi resetten?";
const char wcWifiResetBody[] = "Verwijdert WiFi-gegevens en herstart in configuratiemodus.";
const char wcWifiResetYes[] = "Ja, resetten";
const char wcWifiResetCancel[] = "Annuleren";
const char wcWifiResetBtn[] = "&#x26A0; WiFi resetten";
const char wcWifiResetting[] = "WiFi wordt gereset...";
const char wcWifiResetHotspot[] = "Herstarten. Verbind met het <strong>" HOSTNAME "</strong> hotspot.";
const char wcSecLocation[] = "Locatie";
const char wcLocName[] = "naam op scherm";  // after "TZ1 " on the settings page
const char wcSecDisplay[] = "Weergave-instellingen";
const char wcSecTempColours[] = "Temperatuurdrempelkleuren";
const char wcSecPrecipColours[] = "Neerslagdrempelkleuren";
const char wcLatLabel[] = "Breedtegraad";
const char wcLonLabel[] = "Lengtegraad";
const char wcUnitsLabel[] = "Temperatuureenheid";
const char wcImperial[] = "Imperiaal (&deg;F)";
const char wcMetric[] = "Metrisch (&deg;C)";
const char wcClock24[] = "24-uursklok";
const char wcAutoDim[] = "Dimmen bij zonsondergang";
const char wcNightBright[] = "Nachthelderheid (0-255)";
const char wcNightBrightHint[] = "0 = scherm uit";
const char wcShowBaro[] = "Luchtdruk weergeven";
const char wcShowPrecip[] = "Neerslagkans weergeven";
const char wcShowUV[] = "UV-index weergeven";
const char wcShow6Hr[] = "Voorspelling 6u weergeven";
const char wcForecastStart[]    = "Voorspelling vanaf";
const char wcForecastToday[]    = "Vandaag";
const char wcForecastTomorrow[] = "Morgen";
const char wcIconSet[]         = "Weericonen";
const char wcIconStandard[]    = "Standaard";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Kleuren omkeren";
const char wcInvertHint[]      = "Correctie voor kleurinversie";
const char wcPressure[]        = "Luchtdruk";
const char wcPressSeaLevel[]   = "Zeeniveau";
const char wcPressStation[]    = "Station";
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

static const char* shortDOW[8] = { "???", "DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SÁB" };

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
const char failedStr[]     = "Falha: ";
const char windMetric[]    = " m/s";
const char windImperial[]  = " mph";
const char pressImperial[] = " in";
const char pressMetric[]   = " hPa";
const char tempMetric[]    = "oC";
const char tempImperial[]  = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

static const char* shortMonth[12] = {
  "Jan","Fev","Mar","Abr","Mai","Jun",
  "Jul","Ago","Set","Out","Nov","Dez"};

const char forecastTitleStr[] = "Previsão 6h";
const char popLabelStr[]      = "Prec%:";
const char tempLabelStr[]     = "Temp:";
const char dewLabelStr[]      = " Orv:";

const char bootUpdatingTime[]       = "Atualizando hora...";
const char bootUpdatingConditions[] = "Atualizando condições...";
const char bootDone[]               = "Concluído...";
const char bootConnectedWifi[]      = "WiFi conectado";
const char bootFetchingWeather[]    = "Obtendo dados...";
const char bootFlashOK[]            = "Flash FS pronto!";
const char bootFlashFail[]          = "Erro no Flash FS!";
const char bootRetry[]              = "Falha, tentando em 60s...";
const char strAPActive[] = "Ponto de acesso ativo";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Escaneie para o Wi-Fi";  // under the setup QR codes
const char strAPStep2[] = "2. Escaneie para configurar";
const char strLocalWeather[] = "Tempo local";  // city name too long to show

const char creditOriginal[]  = "Original: blog.squix.org";
const char creditBodmer[]    = "Adaptado por: Bodmer";
const char creditWabbit[]    = "Open-Meteo por: Wabbitguy";

const char wcSaved[]            = "&#10003; Configurações salvas.";
const char wcWifiResetTitle[]   = "&#x26A0; Redefinir WiFi?";
const char wcWifiResetBody[]    = "Apaga credenciais WiFi e reinicia no modo configuração.";
const char wcWifiResetYes[]     = "Sim, redefinir";
const char wcWifiResetCancel[]  = "Cancelar";
const char wcWifiResetBtn[]     = "&#x26A0; Redefinir WiFi";
const char wcWifiResetting[]    = "Redefinindo WiFi...";
const char wcWifiResetHotspot[] = "Reiniciando. Conecte-se ao hotspot <strong>" HOSTNAME "</strong>.";
const char wcSecLocation[]      = "Localização";
const char wcLocName[]          = "nome na tela";  // after "TZ1 " on the settings page
const char wcSecDisplay[]       = "Configurações de exibição";
const char wcSecTempColours[]   = "Cores de temperatura";
const char wcSecPrecipColours[] = "Cores de precipitação";
const char wcLatLabel[]         = "Latitude";
const char wcLonLabel[]         = "Longitude";
const char wcUnitsLabel[]       = "Unidade de temperatura";
const char wcImperial[]         = "Imperial (&deg;F)";
const char wcMetric[]           = "Métrico (&deg;C)";
const char wcClock24[]          = "Relógio 24h";
const char wcAutoDim[]          = "Diminuir ao pôr do sol";
const char wcNightBright[]      = "Brilho noturno (0-255)";
const char wcNightBrightHint[]  = "0 = tela apagada";
const char wcShowBaro[]         = "Mostrar pressão barométrica";
const char wcShowPrecip[]       = "Mostrar probabilidade de chuva";
const char wcShowUV[]           = "Mostrar índice UV";
const char wcShow6Hr[]          = "Previsão de 6h";
const char wcForecastStart[]    = "Início previsão";
const char wcForecastToday[]    = "Hoje";
const char wcForecastTomorrow[] = "Amanhã";
const char wcIconSet[]         = "Ícones do tempo";
const char wcIconStandard[]    = "Padrão";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Inverter cores";
const char wcInvertHint[]      = "Correção de inversão de cores";
const char wcPressure[]        = "Pressão";
const char wcPressSeaLevel[]   = "Nível do mar";
const char wcPressStation[]    = "Estação";
const char wcWeatherHeadings[]  = "Cor títulos meteorologia";
const char wcAstroHeadings[]    = "Cor títulos astronomia";
const char wcHighTempThresh[]   = "Limite temp alta (&deg;)";
const char wcLowTempThresh[]    = "Limite temp baixa (&deg;)";
const char wcHighTempColour[]   = "Cor temp alta";
const char wcMidTempColour[]    = "Cor temp média";
const char wcLowTempColour[]    = "Cor temp baixa";
const char wcHighPrecipThresh[] = "Limite precipitação alta (%)";
const char wcLowPrecipThresh[]  = "Limite precipitação baixa (%)";
const char wcHighPOPColour[]    = "Cor POP alto";
const char wcMidPOPColour[]     = "Cor POP médio";
const char wcLowPOPColour[]     = "Cor POP baixo";
const char wcSaveBtn[]          = "Salvar";
const char wcCancelBtn[]        = "Cancelar";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case  0: return "Céu limpo";
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
    case 77: return "Grãos de neve";
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

static const char* shortDOW[8] = { "???", "PAZ", "PZT", "SAL", "ÇAR", "PER", "CUM", "CMT" };

const char sunStr[]      = "Güneş";
const char cloudStr[]    = "Bulut";
const char humidityStr[] = "Nem";

static const char* moonPhase[8] = {
  "Yeni", "Hilal", "İlk Çeyrek", "Şişkin",
  "Dolunay", "Şişkin", "Son Çeyrek", "Hilal"
};

const char riseStr[] = "SR:";
const char setStr[]  = "SS:";

const char updatedStr[]    = "Güncellendi: ";
const char failedStr[]     = "Hata: ";
const char windMetric[]    = " m/s";
const char windImperial[]  = " mph";
const char pressImperial[] = " in";
const char pressMetric[]   = " hPa";
const char tempMetric[]    = "oC";
const char tempImperial[]  = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

static const char* shortMonth[12] = {
  "Oca","Şub","Mar","Nis","May","Haz",
  "Tem","Ağu","Eyl","Eki","Kas","Ara"};

const char forecastTitleStr[] = "6s Tahmin";
const char popLabelStr[]      = "Yağ%:";
const char tempLabelStr[]     = "Sıc:";
const char dewLabelStr[]      = " Çiy:";

const char bootUpdatingTime[]       = "Saat güncelleniyor...";
const char bootUpdatingConditions[] = "Hava durumu yükleniyor...";
const char bootDone[]               = "Tamamlandı...";
const char bootConnectedWifi[]      = "WiFi bağlandı";
const char bootFetchingWeather[]    = "Hava verisi alınıyor...";
const char bootFlashOK[]            = "Flash FS hazır!";
const char bootFlashFail[]          = "Flash FS hatası!";
const char bootRetry[]              = "Başarısız, 60s sonra tekrar...";
const char strAPActive[] = "Erişim noktası aktif";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Wi-Fi için tarayın";  // under the setup QR codes
const char strAPStep2[] = "2. Kurulum için tarayın";
const char strLocalWeather[] = "Yerel Hava";  // city name too long to show

const char creditOriginal[]  = "Orijinal: blog.squix.org";
const char creditBodmer[]    = "Uyarlayan: Bodmer";
const char creditWabbit[]    = "Open-Meteo: Wabbitguy";

const char wcSaved[]            = "&#10003; Ayarlar kaydedildi.";
const char wcWifiResetTitle[]   = "&#x26A0; WiFi Sıfırla?";
const char wcWifiResetBody[]    = "WiFi bilgilerini siler ve yapılandırma modunda yeniden başlatır.";
const char wcWifiResetYes[]     = "Evet, sıfırla";
const char wcWifiResetCancel[]  = "İptal";
const char wcWifiResetBtn[]     = "&#x26A0; WiFi Sıfırla";
const char wcWifiResetting[]    = "WiFi sıfırlanıyor...";
const char wcWifiResetHotspot[] = "Yeniden başlatılıyor. <strong>" HOSTNAME "</strong> hotspotuna bağlanın.";
const char wcSecLocation[]      = "Konum";
const char wcLocName[]          = "ekranda görünen ad";  // after "TZ1 " on the settings page
const char wcSecDisplay[]       = "Ekran Ayarları";
const char wcSecTempColours[]   = "Sıcaklık Eşik Renkleri";
const char wcSecPrecipColours[] = "Yağış Eşik Renkleri";
const char wcLatLabel[]         = "Enlem";
const char wcLonLabel[]         = "Boylam";
const char wcUnitsLabel[]       = "Sıcaklık Birimi";
const char wcImperial[]         = "Emperyal (&deg;F)";
const char wcMetric[]           = "Metrik (&deg;C)";
const char wcClock24[]          = "24 saat formatı";
const char wcAutoDim[]          = "Gün batımında karart";
const char wcNightBright[]      = "Gece parlaklığı (0-255)";
const char wcNightBrightHint[]  = "0 = ekran kapalı";
const char wcShowBaro[]         = "Barometre göster";
const char wcShowPrecip[]       = "Yağış olasılığı göster";
const char wcShowUV[]           = "UV indeksi göster";
const char wcShow6Hr[]          = "6 saatlik tahmin göster";
const char wcForecastStart[]    = "Tahmin başlangıcı";
const char wcForecastToday[]    = "Bugün";
const char wcForecastTomorrow[] = "Yarın";
const char wcIconSet[]         = "Hava durumu simgeleri";
const char wcIconStandard[]    = "Standart";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Renkleri ters çevir";
const char wcInvertHint[]      = "Renk ters çevirme düzeltmesi";
const char wcPressure[]        = "Basınç";
const char wcPressSeaLevel[]   = "Deniz seviyesi";
const char wcPressStation[]    = "İstasyon";
const char wcWeatherHeadings[]  = "Hava durumu başlık rengi";
const char wcAstroHeadings[]    = "Astronomi başlık rengi";
const char wcHighTempThresh[]   = "Yüksek sıcaklık eşiği (&deg;)";
const char wcLowTempThresh[]    = "Düşük sıcaklık eşiği (&deg;)";
const char wcHighTempColour[]   = "Yüksek sıcaklık rengi";
const char wcMidTempColour[]    = "Orta sıcaklık rengi";
const char wcLowTempColour[]    = "Düşük sıcaklık rengi";
const char wcHighPrecipThresh[] = "Yüksek yağış eşiği (%)";
const char wcLowPrecipThresh[]  = "Düşük yağış eşiği (%)";
const char wcHighPOPColour[]    = "Yüksek YOO rengi";
const char wcMidPOPColour[]     = "Orta YOO rengi";
const char wcLowPOPColour[]     = "Düşük YOO rengi";
const char wcSaveBtn[]          = "Kaydet";
const char wcCancelBtn[]        = "İptal";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case  0: return "Açık hava";
    case  1: return "Çok az bulutlu";
    case  2: return "Parçalı bulutlu";
    case  3: return "Bulutlu";
    case 45: return "Sisli";
    case 48: return "Buzlu sis";
    case 51: return "Hafif çisenti";
    case 53: return "Çisenti";
    case 55: return "Yoğun çisenti";
    case 56: return "Hafif buzlu çisenti";
    case 57: return "Buzlu çisenti";
    case 61: return "Hafif yağmur";
    case 63: return "Yağmur";
    case 65: return "Yoğun yağmur";
    case 66: return "Hafif buzlu yağmur";
    case 67: return "Buzlu yağmur";
    case 71: return "Hafif kar";
    case 73: return "Kar";
    case 75: return "Yoğun kar";
    case 77: return "Kar taneleri";
    case 80: return "Hafif sağanak";
    case 81: return "Sağanak yağmur";
    case 82: return "Yoğun sağanak";
    case 85: return "Karlı sağanak";
    case 86: return "Yoğun karlı sağanak";
    case 95: return "Fırtına";
    case 96: return "Dolu ile fırtına";
    case 99: return "Dolu ile fırtına";
    default: return "Bilinmiyor";
  }
}

inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1)             return "Açık";
  if (id == 2 || id == 3)             return "Bulutlu";
  if (id == 45 || id == 48)           return "Sis";
  if (id >= 51 && id <= 57)           return "Çisenti";
  if (id >= 61 && id <= 67)           return "Yağmur";
  if (id >= 71 && id <= 77)           return "Kar";
  if (id >= 80 && id <= 82)           return "Yağmur";
  if (id == 85 || id == 86)           return "Kar";
  if (id == 95 || id == 96 || id==99) return "Fırtına";
  return "Bilinmiyor";
}

#endif // LANG_TR

// =============================================================================
//  ITALIAN
// =============================================================================
#ifdef LANG_IT

#define NOMINATIM_LANG "it"

static const char* shortMonth[12] = {
  "Gen","Feb","Mar","Apr","Mag","Giu",
  "Lug","Ago","Set","Ott","Nov","Dic"};

static const char* shortDOW[8] = { "???", "DOM", "LUN", "MAR", "MER", "GIO", "VEN", "SAB" };

const char sunStr[] = "Sole";
const char cloudStr[] = "Nuvole";
const char humidityStr[] = "Umidità";

static const char* moonPhase[8] = {
  "Nuova", "Crescente", "Primo qrt", "Crescente",
  "Piena", "Calante", "Ultimo qrt", "Calante"
};

const char riseStr[] = "SR:";
const char setStr[] = "SS:";

const char updatedStr[] = "Aggiornato: ";
const char failedStr[] = "Errore: ";
const char windMetric[] = " m/s";
const char windImperial[] = " mph";
const char pressImperial[] = " in";
const char pressMetric[] = " hPa";
const char tempMetric[] = "oC";
const char tempImperial[] = "oF";
const char amStr[] = "AM";
const char pmStr[] = "PM";

const char forecastTitleStr[] = "Previsioni 6h";
const char popLabelStr[] = "Prec%:";
const char tempLabelStr[] = "Temp:";
const char dewLabelStr[] = " Rug:";

const char bootUpdatingTime[] = "Aggiornamento ora...";
const char bootUpdatingConditions[] = "Aggiornamento meteo...";
const char bootDone[] = "Fatto...";
const char bootConnectedWifi[] = "WiFi connesso";
const char bootFetchingWeather[] = "Download dati meteo...";
const char bootFlashOK[] = "Flash FS pronto!";
const char bootFlashFail[] = "Errore Flash FS!";
const char bootRetry[] = "Errore, nuovo tentativo tra 60s...";
const char strAPActive[] = "Access point attivo";  // Wi-Fi setup screen
const char strAPStep1[] = "1. Scansiona per il Wi-Fi";  // under the setup QR codes
const char strAPStep2[] = "2. Scansiona per configurare";
const char strLocalWeather[] = "Meteo locale";  // city name too long to show

const char creditOriginal[] = "Originale: blog.squix.org";
const char creditBodmer[] = "Adattato da: Bodmer";
const char creditWabbit[] = "Open-Meteo da: Wabbitguy";

const char wcSaved[] = "&#10003; Impostazioni salvate e applicate.";
const char wcWifiResetTitle[] = "&#x26A0; Reimpostare il WiFi?";
const char wcWifiResetBody[] = "Cancella le credenziali WiFi salvate e riavvia in modalità "
                               "configurazione. Riconnettiti tramite l'hotspot " HOSTNAME ".";
const char wcWifiResetYes[] = "Sì, reimposta";
const char wcWifiResetCancel[] = "Annulla";
const char wcWifiResetBtn[] = "&#x26A0; Reimposta WiFi";
const char wcWifiResetting[] = "Reimpostazione WiFi...";
const char wcWifiResetHotspot[] = "Riavvio in corso. Connettiti all'hotspot <strong>" HOSTNAME "</strong>.";
const char wcSecLocation[] = "Posizione";
const char wcLocName[] = "nome sul display";  // after "TZ1 " on the settings page
const char wcSecDisplay[] = "Impostazioni display";
const char wcSecTempColours[] = "Colori della temperatura";
const char wcSecPrecipColours[] = "Colori delle precipitazioni";
const char wcLatLabel[] = "Latitudine";
const char wcLonLabel[] = "Longitudine";
const char wcUnitsLabel[] = "Unità di temperatura";
const char wcImperial[] = "Imperiali (&deg;F)";
const char wcMetric[] = "Metriche (&deg;C)";
const char wcClock24[] = "Orologio 24 ore";
const char wcAutoDim[] = "Attenua al tramonto";
const char wcNightBright[] = "Luminosità notturna (0-255)";
const char wcNightBrightHint[] = "0 = display spento, 1 = appena visibile";
const char wcShowBaro[] = "Mostra pressione barometrica";
const char wcShowPrecip[] = "Mostra probabilità di precipitazioni";
const char wcShowUV[] = "Mostra indice UV";
const char wcShow6Hr[] = "Mostra previsioni 6 ore";
const char wcForecastStart[]    = "Inizio previsioni";
const char wcForecastToday[]    = "Oggi";
const char wcForecastTomorrow[] = "Domani";
const char wcIconSet[]         = "Icone meteo";
const char wcIconStandard[]    = "Standard";
const char wcIconMeteocons[]   = "Meteocons";
const char wcInvert[]          = "Inverti colori";
const char wcInvertHint[]      = "Correzione dell'inversione dei colori";
const char wcPressure[]        = "Pressione";
const char wcPressSeaLevel[]   = "Livello del mare";
const char wcPressStation[]    = "Stazione";
const char wcWeatherHeadings[] = "Colore titoli meteo";
const char wcAstroHeadings[] = "Colore titoli astronomia";
const char wcHighTempThresh[] = "Soglia temperatura alta (&deg;)";
const char wcLowTempThresh[] = "Soglia temperatura bassa (&deg;)";
const char wcHighTempColour[] = "Colore temp alta";
const char wcMidTempColour[] = "Colore temp media";
const char wcLowTempColour[] = "Colore temp bassa";
const char wcHighPrecipThresh[] = "Soglia precipitazioni alta (%)";
const char wcLowPrecipThresh[] = "Soglia precipitazioni bassa (%)";
const char wcHighPOPColour[] = "Colore POP alta";
const char wcMidPOPColour[] = "Colore POP media";
const char wcLowPOPColour[] = "Colore POP bassa";
const char wcSaveBtn[] = "Aggiorna e salva";
const char wcCancelBtn[] = "Annulla";

inline const char* wmoDescription(uint16_t id) {
  switch (id) {
    case 0: return "Sereno";
    case 1: return "Poco nuvoloso";
    case 2: return "Parzialmente nuvoloso";
    case 3: return "Coperto";
    case 45: return "Nebbia";
    case 48: return "Nebbia gelata";
    case 51: return "Pioggerella leggera";
    case 53: return "Pioggerella";
    case 55: return "Pioggerella intensa";
    case 56: return "Pioggerella gelata leggera";
    case 57: return "Pioggerella gelata";
    case 61: return "Pioggia leggera";
    case 63: return "Pioggia";
    case 65: return "Pioggia forte";
    case 66: return "Pioggia gelata leggera";
    case 67: return "Pioggia gelata";
    case 71: return "Neve leggera";
    case 73: return "Neve";
    case 75: return "Neve forte";
    case 77: return "Neve granulosa";
    case 80: return "Rovesci leggeri";
    case 81: return "Rovesci";
    case 82: return "Rovesci forti";
    case 85: return "Rovesci di neve";
    case 86: return "Forti rovesci di neve";
    case 95: return "Temporale";
    case 96: return "Temporale con grandine";
    case 99: return "Temporale con grandine";
    default: return "Sconosciuto";
  }
}
inline const char* wmoMain(uint16_t id) {
  if (id == 0 || id == 1) return "Sereno";
  if (id == 2 || id == 3) return "Nuvole";
  if (id == 45 || id == 48) return "Nebbia";
  if (id >= 51 && id <= 57) return "Pioggerella";
  if (id >= 61 && id <= 67) return "Pioggia";
  if (id >= 71 && id <= 77) return "Neve";
  if (id >= 80 && id <= 82) return "Pioggia";
  if (id == 85 || id == 86) return "Neve";
  if (id == 95 || id == 96 || id == 99) return "Temporale";
  return "Sconosciuto";
}

#endif  // LANG_IT

#endif  // TRANSLATION_H