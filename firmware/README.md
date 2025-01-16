# Firmware Docs

Welcome to the firmware docs! Here you will find...fuinish

## Software Structure

The software is divided into four main files:`main.ino`, `config.h`, `mqGas.h` and `alarmThresholds.h`.

### `main.ino`

The program monitors environmental safety parameters using various sensors and logs data to an SD card and serial output.

#### Program Flow

1. Initialize pins, sensors, and libraries
2. Read sensor data every `SAMPLING_INTERVAL_MS`
3. Update LCD menu display and check/triggers alarms
4. Log data to SD card and serial output

#### Key Components

- **Sensor Initialization and Calibration**:
  - SCD41: Begins periodic measurement upon successful initialization
  - ENS160: Configured to operate in standard mode
  - MQ Series: Calibrated using `PERFORM_MQ_CALIBRATION` flag, analog values mapped to ppm using exponential regression, R0 value stored in EEPROM
- **Alarm System**:
  - Threshold-based alarm triggering (DANGER/WARNING)
  - Activates piezo buzzer and changes RGB LED color to red upon alarm
- **Logging**:
  - SD card (CSV): Records sensor data with timestamp
  - Serial output (JSON): Real-time monitoring or integration with other systems
- **LCD Menu Interface**:
  - Utilizes LiquidMenu library for multiple screens displaying sensor readings
  - Navigation via push button, with screens for system title, timestamp, and individual sensor readings

### `config.h`

This header file contains all configurable settings, pin definitions, thresholds, and other constants used throughout the EnvSafetyMonitor system.

- Debugging Settings
  - `ENABLE_DEBUG_PRINTS`: Toggle for enabling/disabling debug messages.
  - `ENABLE_DATA_PRINTS`: Toggle for enabling/disabling data output to Serial.
- Calibration Settings
  - `PERFORM_MQ_CALIBRATION`: Determines whether to calibrate MQ sensors on startup. Note that once this is enabled you should boot the system, allow calibration, then reupload with this setting disabled during production use.
- Logging Settings
  - `LOG_FILE_NAME`: Name of the CSV log file on the SD card.
  - `LOG_TO_SD`: Enables/disables logging sensor data to the SD card.
- Standard Colors Definitions
  - `RED`: RGB values for red color.
  - `GREEN`: RGB values for green color.
- Sampling Settings
  - `SAMPLING_INTERVAL_MS`: Time interval between consecutive sensor readings.
- I2C LCD Settings
  - `LCD_I2C_ADDRESS`: I2C address of the LCD.
  - `LCD_COLUMNS`: Number of columns on the LCD.
  - `LCD_ROWS`: Number of rows on the LCD.
- Serial Communication Settings
  - `SERIAL_BAUD_RATE`: Baud rate for Serial communication.
- Debounce Settings
  - `DEBOUNCE_DELAY_MS`: Delay for debouncing the push button.

### `alarmThresholds.h`

The following thresholds are used to trigger warning and danger alarms for various environmental parameters.

#### Temperature & Humidity Thresholds

- Temperature
  - `TEMP_WARNING_THRESHOLD_C`: 30.0°C - Temperature warning threshold
  - `TEMP_DANGER_THRESHOLD_C`: 35.0°C - Temperature danger threshold
  - Temperatures around 30°C increase the risk of heat stress, while temperatures at or above 35°C pose a higher risk of heat-related illnesses.
- Relative Humidity (RH)
  - `HUMIDITY_WARNING_THRESHOLD_RH`: 60.0%RH - Humidity warning threshold
  - `HUMIDITY_DANGER_THRESHOLD_RH`: 80.0%RH - Humidity danger threshold
    High humidity levels reduce the body's ability to cool itself, with increased risk at 80% humidity.

#### Gas Concentration Thresholds

- Carbon Dioxide (CO2)

  - `CO2_WARNING_THRESHOLD_PPM`: 5000 ppm - CO₂ warning threshold
  - `CO2_DANGER_THRESHOLD_PPM`: 15000 ppm - CO₂ danger threshold
  - **Based on:** OSHA's PEL for CO₂ is 5,000 ppm over an 8-hour workday, with concentrations above 15,000 ppm causing respiratory issues.

- Hydrogen (H2)

  - `H2_WARNING_THRESHOLD_PPM`: 4000 ppm - H₂ warning threshold
  - `H2_DANGER_THRESHOLD_PPM`: 8000 ppm - H₂ danger threshold
  - **Based on:** Hydrogen is highly flammable, with warning and danger thresholds set at 10% and 20% of the Lower Explosive Limit (LEL).

- Hydrogen Sulfide (H2S)

  - `H2S_WARNING_THRESHOLD_PPM`: 10 ppm - H₂S warning threshold
  - `H2S_DANGER_THRESHOLD_PPM`: 50 ppm - H₂S danger threshold
  - **Based on:** OSHA's PEL for H₂S is 10 ppm over an 8-hour workday, with concentrations above 15 ppm causing eye irritation.

- Ammonia (NH3)

  - `NH3_WARNING_THRESHOLD_PPM`: 25 ppm - NH₃ warning threshold
  - `NH3_DANGER_THRESHOLD_PPM`: 50 ppm - NH₃ danger threshold
  - **Based on:** OSHA's PEL for ammonia is 50 ppm and 25 ppm over an 8-hour workday, with levels above 50 ppm causing eye and respiratory irritation.

- Methane (CH4)

  - `CH4_WARNING_THRESHOLD_PPM`: 5000 ppm - CH₄ warning threshold
  - `CH4_DANGER_THRESHOLD_PPM`: 10000 ppm - CH₄ danger threshold
  - **Based on:** Methane is flammable, with warning and danger thresholds set at 10% and 20% of the Lower Explosive Limit (LEL).

- Carbon Monoxide (CO)
  - `CO_WARNING_THRESHOLD_PPM`: 35 ppm - CO warning threshold
  - `CO_DANGER_THRESHOLD_PPM`: 50 ppm - CO danger threshold
  - **Based on:** OSHA's PEL for CO is 50 ppm and 25 ppm over an 8-hour workday, with levels above 50 ppm causing headaches and dizziness.

### `mqgas.h`

The MQ gas sensors detect specific gases by measuring the change in their internal resistance when exposed to different gas concentrations. The resistance value (`RS`) changes in proportion to the gas concentration, allowing for the calculation of the gas's parts per million (ppm) level.

#### Regression Formula

The MQ sensors use a logarithmic formula to relate the sensor resistance ratio (`RS / R0`) to gas concentration:

```c
ppm = a * (RS / R0)^b
```

where:

- a and b are constants specific to each sensor and gas, derived from calibration and datasheet values.
- RS is the sensor resistance in the current environment.
- R0 is the baseline resistance of the sensor in clean air, obtained through calibration.

#### Sensor Resistance (RS) Calculation

RS is calculated from the sensor's analog voltage reading as follows:

```c
RS = ((V_supply - V_sensor) \* RL) / V_sensor
```

where:

- V_sensor is the voltage at the sensor’s output.
- V_supply is the supply voltage to the sensor.
- RL is the load resistance in series with the sensor.

#### Baseline Resistance (R0) Calibration

R0 represents the sensor’s resistance in a known clean air environment and is used as a baseline for gas concentration calculations. During calibration:

- The sensor's RS is measured in clean air.
- R0 is calculated as the average RS in this environment and is saved in EEPROM to maintain consistency across power cycles.

#### Clean Air Ratio

The clean air ratio (RS / R0 in clean air) is a reference value specific to each sensor. This ratio helps establish the baseline resistance and is used during R0 calibration.

#### Sensor Definitions

- **MQ-8 Sensor (Hydrogen, H₂)**

  - `MQ8_TYPE`: "MQ-8"
  - `MQ8_RATIO_CLEAN_AIR`: 70
  - `A_VALUE_MQ8`: 71.7592
  - `B_VALUE_MQ8`: -0.9760
  - `EEPROM_ADDRESS_MQ8_R0`: 0

- **MQ-136 Sensor (Hydrogen Sulfide, H₂S)**

  - `MQ136_TYPE`: "MQ-136"
  - `MQ136_RATIO_CLEAN_AIR`: 3.6
  - `A_VALUE_MQ136`: 1.0715
  - `B_VALUE_MQ136`: -0.8877
  - `EEPROM_ADDRESS_MQ136_R0`: 4

- **MQ-137 Sensor (Ammonia, NH₃)**

  - `MQ137_TYPE`: "MQ-137"
  - `MQ137_RATIO_CLEAN_AIR`: 4.2
  - `A_VALUE_MQ137`: 1.0613
  - `B_VALUE_MQ137`: -0.8905
  - `EEPROM_ADDRESS_MQ137_R0`: 8

- **MQ-9b Sensor (Carbon Monoxide, CO & Methane, CH₄)**
  - `MQ9B_TYPE`: "MQ-9"
  - `MQ9B_RATIO_CLEAN_AIR`: 9.6
  - `A_VALUE_MQ9B_CH4`: 4.9059
  - `B_VALUE_MQ9B_CH4`: -0.6699
  - `A_VALUE_MQ9B_CO`: 4.8726
  - `B_VALUE_MQ9B_CO`: -0.6989
  - `EEPROM_ADDRESS_MQ9B_R0`: 12
  - `SETTLE_TIME_TO_CO`: 20000 ms
  - `SETTLE_TIME_TO_CH4`: 10000 ms
