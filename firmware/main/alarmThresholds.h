// ------------------------------
// Alarm Thresholds
// ------------------------------

#define TEMP_WARNING_THRESHOLD_C 30.0 // Temperature warning threshold in Celsius
#define TEMP_DANGER_THRESHOLD_C 35.0  // Temperature danger threshold in Celsius
// Sustained temperatures around 30°C increase the risk of heat stress, particularly for those not acclimatized.
// Temperatures at or above 35°C pose a higher risk of heat-related illnesses, such as heat exhaustion or heat stroke.
#define HUMIDITY_WARNING_THRESHOLD_RH 60.0 // Humidity warning threshold in %RH
#define HUMIDITY_DANGER_THRESHOLD_RH 80.0  // Humidity danger threshold in %RH
// High humidity levels (above 60%) reduce the body's ability to cool itself through sweating.
// At 80% humidity, this risk is heightened as sweat evaporation slows, increasing heat stress potential.
// TODO: Future work should integrate Wet Bulb Globe Temperature (WBGT) measurements for a more comprehensive heat stress assessment.

#define CO2_WARNING_THRESHOLD_PPM 5000 // CO₂ warning threshold in ppm
#define CO2_DANGER_THRESHOLD_PPM 15000 // CO₂ danger threshold in ppm
// OSHA's PEL for CO₂ is 5,000 ppm over an 8-hour workday. Short term exposure is 30,000 ppm,
// concentrations above 15,000 ppm can cause respiratory issues and cognitive effects.
// Source: https://www.osha.gov/annotated-pels/table-z-1

#define H2_WARNING_THRESHOLD_PPM 4000 // H₂ warning threshold in ppm
#define H2_DANGER_THRESHOLD_PPM 8000  // H₂ danger threshold in ppm
// Hydrogen is highly flammable; its lower explosive limit (LEL) is 4% by volume in air (40,000 ppm).
// Warning set at 10% LEL (4,000 ppm) and danger at 20% LEL (8,000 ppm) to provide early alerts and prevent hazardous levels.
// Source: https://www.engineeringtoolbox.com/explosive-concentration-limits-d_423.html

#define H2S_WARNING_THRESHOLD_PPM 10 // H₂S warning threshold in ppm
#define H2S_DANGER_THRESHOLD_PPM 50  // H₂S danger threshold in ppm
// OSHA's PEL for H₂S is 10 ppm over an 8-hour workday.
// Concentrations above 15 ppm can cause eye irritation and respiratory issues.
// Source: https://www.osha.gov/annotated-pels/table-z-2

#define NH3_WARNING_THRESHOLD_PPM 25 // NH₃ warning threshold in ppm
#define NH3_DANGER_THRESHOLD_PPM 50  // NH₃ danger threshold in ppm
// OSHA's PEL for ammonia is 50 ppm and 25 ppm over an 8-hour workday.
// Levels above 50 ppm can cause eye and respiratory irritation.
// Source: https://www.osha.gov/annotated-pels/table-z-1

#define CH4_WARNING_THRESHOLD_PPM 5000 // CH₄ warning threshold in ppm
#define CH4_DANGER_THRESHOLD_PPM 10000 // CH₄ danger threshold in ppm
// Methane (CH₄) is flammable, with an explosive range between 5% and 15% by volume in air (50,000 - 150,000 ppm).
// Warning set at 10% of the Lower Explosive Limit (LEL) (5,000 ppm) and danger set at 20% of the LEL (10,000 ppm).
// Source: https://minearc.com/methane-health-and-safety-hazards-fact-sheet/

#define CO_WARNING_THRESHOLD_PPM 35 // CO warning threshold in ppm
#define CO_DANGER_THRESHOLD_PPM 50  // CO danger threshold in ppm
// OSHA's PEL for CO is 50 ppm and 25 ppm over an 8-hour workday.
// Levels above 50 ppm can cause headaches and dizziness.
// Source: https://www.osha.gov/annotated-pels/table-z-1
