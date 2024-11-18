/***************************************************************************
  MQ Sensor Definitions and Characteristics

  Each MQ sensor in this setup detects specific gases based on a change in
  its internal resistance when exposed to different gas concentrations.
  The resistance value (`RS`) changes in proportion to the gas concentration,
  and this is used to calculate the gas's parts per million (ppm) level.
  All a and b values are derived from the sensor datasheet sensitivity 
  characteristics using a derived regression formula.

  Breakdown of how these characteristics are handled in software:

  1. **Regression Formula**:
     The MQ sensors use a logarithmic formula to relate the sensor resistance ratio (`RS / R0`) to gas concentration:
     ```
     ppm = a * (RS / R0)^b
     ```
     where:
     - `a` and `b` are constants specific to each sensor and gas, derived from calibration / datasheet.
     - `RS` is the sensor resistance in the current environment.
     - `R0` is the baseline resistance of the sensor in clean air, obtained through calibration.

  2. **Sensor Resistance (`RS`) Calculation**:
     `RS` is calculated from the sensor's analog voltage reading as follows:
     ```
     RS = ((V_supply - V_sensor) * RL) / V_sensor
     ```
     where:
     - `V_sensor` is the voltage at the sensor’s output.
     - `V_supply` is the supply voltage to the sensor.
     - `RL` is the load resistance in series with the sensor.

  3. **Baseline Resistance (`R0`) Calibration**:
     `R0` represents the sensor’s resistance in a known clean air environment and is used as a baseline for gas concentration calculations. During calibration:
     - The sensor's `RS` is measured in clean air.
     - `R0` is calculated as the average `RS` in this environment and is saved in EEPROM to maintain consistency across power cycles.

  4. **Clean Air Ratio**:
     The clean air ratio (`RS / R0` in clean air) is a reference value specific to each sensor. This ratio helps establish the baseline resistance and is used during `R0` calibration.

  This configuration allows accurate ppm calculation, compensating for environmental changes and sensor characteristics, ensuring reliable gas concentration readings.

***************************************************************************/

#ifndef MQGAS_H
#define MQGAS_H

// General MQ Sensor Definitions
#define DEVICE "Arduino Nano"
#define VOLTAGE_RESOLUTION 5
#define ADC_BIT_RESOLUTION 10

// Definitions for MQ-8 Sensor (Hydrogen, H₂)
#define MQ8_TYPE "MQ-8"
#define MQ8_RATIO_CLEAN_AIR 70  // Clean air ratio (RS/R0) in clean air
#define A_VALUE_MQ8 71.7592      // Regression coefficient for hydrogen (H₂)
#define B_VALUE_MQ8 -0.9760      // Regression exponent for hydrogen (H₂)
#define EEPROM_ADDRESS_MQ8_R0 0 // EEPROM address for MQ-8 R0

// Definitions for MQ-136 Sensor (Hydrogen Sulfide, H₂S)
#define MQ136_TYPE "MQ-136"
#define MQ136_RATIO_CLEAN_AIR 3.6 // Clean air ratio (RS/R0) in clean air
#define A_VALUE_MQ136 1.0715      // Regression coefficient for hydrogen sulfide (H₂S)
#define B_VALUE_MQ136 -0.8877      // Regression exponent for hydrogen sulfide (H₂S)
#define EEPROM_ADDRESS_MQ136_R0 4 // EEPROM address for MQ-136 R0

// Definitions for MQ-137 Sensor (Ammonia, NH₃)
#define MQ137_TYPE "MQ-137"
#define MQ137_RATIO_CLEAN_AIR 4.2 // Clean air ratio (RS/R0) in clean air
#define A_VALUE_MQ137 1.0613     // Regression coefficient for ammonia (NH₃)
#define B_VALUE_MQ137 -0.8905      // Regression exponent for ammonia (NH₃)
#define EEPROM_ADDRESS_MQ137_R0 8 // EEPROM address for MQ-137 R0

// Definitions for MQ-9b Sensor (Carbon Monoxide, CO & Methane, CH₄)
#define MQ9B_TYPE "MQ-9"
#define MQ9B_RATIO_CLEAN_AIR 9.6  // Clean air ratio (RS/R0) in clean air
#define A_VALUE_MQ9B_CH4 4.9059   // Regression coefficient for methane (CH₄)
#define B_VALUE_MQ9B_CH4 -0.6699   // Regression exponent for methane (CH₄)
#define A_VALUE_MQ9B_CO 4.8726    // Regression coefficient for carbon monoxide (CO)
#define B_VALUE_MQ9B_CO -0.6989    // Regression exponent for carbon monoxide (CO)
#define EEPROM_ADDRESS_MQ9B_R0 12 // EEPROM address for MQ-9b R0
#define SETTLE_TIME_TO_CO 20000   // Settle time in milliseconds for CO measurement after switching heater voltage
#define SETTLE_TIME_TO_CH4 10000  // Settle time in milliseconds for CH4 measurement after switching heater voltage

#endif // MQGAS_H