# Firmware

*EnvSafetyMonitor - Environmental Safety Monitoring System*

## Table of Contents
- [Introduction](#introduction)
- [System Overview](#system-overview)
- [Hardware Components](#hardware-components)
- [Software Structure](#software-structure)
  - [Configuration (`config.h`)](#configuration-configh)
  - [Main Program (`main.ino`)](#main-program-mainino)
- [Sensor Initialization and Calibration](#sensor-initialization-and-calibration)
- [Data Logging](#data-logging)
- [Alarm System](#alarm-system)
- [LCD Menu Interface](#lcd-menu-interface)
- [Pin Configuration](#pin-configuration)
- [Usage Instructions](#usage-instructions)
- [Troubleshooting](#troubleshooting)
- [Conclusion](#conclusion)

## Introduction

EnvSafetyMonitor is an Environmental Safety Monitoring System designed to continuously monitor various environmental parameters to ensure safety and compliance. The system leverages multiple sensors to detect levels of gases, temperature, and humidity, providing real-time data visualization, logging, and alarm notifications.

## System Overview

The system integrates several components:
- **Sensors:** SCD41 (CO₂, temperature, humidity), ENS160 (AQI, TVOC, eCO₂), MQ series (H₂, H₂S, NH₃, CH₄, CO).
- **Display:** 2x16 I2C LCD for real-time data visualization.
- **Data Logging:** SD card module for recording sensor data in CSV format.
- **Alarms:** Buzzer and RGB LED indicators for alerting based on predefined thresholds.
- **User Interface:** Push button to navigate through different data screens on the LCD.

## Hardware Components

- **Arduino Nano:** The microcontroller that orchestrates all operations.
- **SCD41 Sensor:** Measures CO₂ concentration, temperature, and humidity.
- **ENS160 Sensor:** Provides Air Quality Index (AQI), Total Volatile Organic Compounds (TVOC), and equivalent CO₂ (eCO₂) readings.
- **MQ Series Sensors:**
  - **MQ-8:** Detects Hydrogen (H₂) concentrations.
  - **MQ-136:** Measures Hydrogen Sulfide (H₂S) levels.
  - **MQ-137:** Monitors Ammonia (NH₃) concentrations.
  - **MQ-9b:** Detects Methane (CH₄) and Carbon Monoxide (CO) levels.
- **2x16 I2C LCD:** Displays sensor readings and system status.
- **SD Card Module:** Logs sensor data for future analysis.
- **RGB LEDs:** Visual indicators for system status (Green for normal, Red for alarms).
- **Piezo Buzzer:** Auditory alarm for threshold breaches.
- **Relay Module:** Controls the heating element for the MQ-9b sensor.
- **Push Button:** Allows users to navigate through LCD screens.
- **RTC Module (DS3234):** Keeps accurate time for timestamping logs.

## Software Structure

The software is divided into two main files: `config.h` and `main.ino`.

### Configuration (`config.h`)

This header file contains all configurable settings, pin definitions, thresholds, and other constants used throughout the EnvSafetyMonitor system.

#### Debugging Settings
- `ENABLE_DEBUG_PRINTS`: Toggle for enabling/disabling debug messages.
- `ENABLE_DATA_PRINTS`: Toggle for enabling/disabling data output to Serial.

#### Calibration Settings
- `PERFORM_MQ_CALIBRATION`: Determines whether to calibrate MQ sensors on startup.

#### Logging Settings
- `LOG_FILE_NAME`: Name of the CSV log file on the SD card.
- `LOG_TO_SD`: Enables/disables logging sensor data to the SD card.

#### Pin Definitions
- **LEDs:**
  - `redPin`: Connected to Red LED.
  - `greenPin`: Connected to Green LED.
  - `bluePin`: Connected to Blue LED.
- **Buzzer and Button:**
  - `buzzerPin`: Connected to Piezo Buzzer.
  - `buttonPin`: Connected to Push Button.
- **Sensors:**
  - `mq8Pin`: Connected to MQ-8 Sensor.
  - `mq136Pin`: Connected to MQ-136 Sensor.
  - `mq137Pin`: Connected to MQ-137 Sensor.
  - `mq9bPin`: Connected to MQ-9b Sensor.
- **Relay:**
  - `relayPin`: Controls the Relay for MQ-9b Heater Voltage.
- **Communication:**
  - `DS3234_CS_PIN`: Chip-select pin for RTC module.
  - `chipSelect`: Chip-select pin for SD card module.

#### Standard Colors Definitions
- `RED`: RGB values for red color.
- `GREEN`: RGB values for green color.

#### Sampling Settings
- `SAMPLING_INTERVAL_MS`: Time interval between consecutive sensor readings.

#### Alarm Thresholds
Defines the maximum allowable values for various parameters before triggering alarms:
- `CO2_THRESHOLD_PPM`
- `TEMP_THRESHOLD_C`
- `HUMIDITY_THRESHOLD_RH`
- `H2_THRESHOLD_PPM`
- `H2S_THRESHOLD_PPM`
- `NH3_THRESHOLD_PPM`
- `CH4_THRESHOLD_PPM`
- `CO_THRESHOLD_PPM`

#### I2C LCD Settings
- `LCD_I2C_ADDRESS`: I2C address of the LCD.
- `LCD_COLUMNS`: Number of columns on the LCD.
- `LCD_ROWS`: Number of rows on the LCD.

#### Serial Communication Settings
- `SERIAL_BAUD_RATE`: Baud rate for Serial communication.

#### Debounce Settings
- `DEBOUNCE_DELAY_MS`: Delay for debouncing the push button.

### Main Program (`main.ino`)

The `main.ino` file contains the core logic of the EnvSafetyMonitor system, including sensor initialization, calibration, data reading, logging, alarm handling, and user interface management.

#### Initialization
- **LCD and Menu:**
  - Initializes the I2C LCD and sets up the LiquidMenu for navigating through different data screens.
- **RTC:**
  - Initializes the Real-Time Clock module and sets the current time if invalid.
- **Sensors:**
  - Initializes and optionally calibrates all connected sensors (SCD41, ENS160, MQ-8, MQ-136, MQ-137, MQ-9b).
- **SD Card:**
  - Sets up the SD card module and creates a CSV log file with appropriate headers if it doesn't exist.
- **Alarms and Indicators:**
  - Sets the RGB LEDs to green indicating normal operation and ensures the relay starts in the HIGH state for MQ-9b sensor.

#### Loop Function
- **Button Handling:**
  - Continuously checks for button presses to navigate through the LCD screens with debounce to prevent false triggers.
- **Relay Control:**
  - Manages the relay to switch between CO and CH₄ detection phases for the MQ-9b sensor at defined intervals.
- **Sensor Reading and Logging:**
  - At every `SAMPLING_INTERVAL_MS`, reads data from all sensors, updates the LCD display, checks for alarm conditions, and logs data to the SD card and Serial.
- **Alarm Management:**
  - Activates the buzzer and changes the LED color to red if any sensor reading exceeds its threshold. The buzzer remains active for a defined duration.

#### Functions
- `initializePins()`: Configures all hardware pins (LEDs, buzzer, button, relay) and sets their initial states.
- `initializeAlarms()`: Sets up alarm-related configurations (currently handled in `initializePins()`).
- `initializeLCDMenu()`: Sets decimal places for sensor readings and adds all screens to the LiquidMenu.
- `readSensors()`: Reads data from all sensors and updates global variables.
- `handleButtonPress()`: Implements button press handling with debounce for LCD navigation.
- `checkAndTriggerAlarms()`: Compares sensor readings against thresholds and activates alarms if necessary.
- `logSensorData()`: Logs sensor data to the SD card in CSV format and optionally to Serial in JSON format.
- `updateLCDMenu()`: Refreshes the LCD display based on the current menu screen.
- `controlMQ9bRelay()`: Manages the relay to alternate between CH₄ and CO detection phases.

## Sensor Initialization and Calibration

Each sensor is initialized with specific configurations tailored to its detection capabilities. Calibration is essential to ensure accurate readings:

- **SCD41 Sensor:** Begins periodic measurement upon successful initialization.
- **ENS160 Sensor:** Configured to operate in standard mode and begins measuring environmental data.
- **MQ Series Sensors:** 
  - Set regression methods and calibration equations.
  - If `PERFORM_MQ_CALIBRATION` is `true`, the system calibrates each MQ sensor by taking multiple readings in clean air conditions and calculates the R0 value, which is then stored in EEPROM for future use.
  - If calibration is disabled, the system retrieves the R0 value from EEPROM.

## Data Logging

Sensor data is logged in two ways:

1. **SD Card Logging:**
   - Data is recorded in a CSV file (`dataLog.csv`) on the SD card.
   - Each entry includes a timestamp and readings from all sensors.
   - The system ensures the CSV file has appropriate headers upon creation.
   
2. **Serial Logging:**
   - If `ENABLE_DATA_PRINTS` is enabled, sensor data is output to Serial in JSON format for real-time monitoring or integration with other systems.

## Alarm System

The alarm system ensures that any hazardous conditions are promptly notified:

- **Threshold Checking:**
  - Each sensor reading is compared against predefined thresholds.
  
- **Alarm Activation:**
  - If any reading exceeds its threshold, the system:
    - Activates the piezo buzzer for `ALARM_DURATION_MS` milliseconds.
    - Changes the RGB LED color to red to indicate an alarm state.
  
- **Alarm Management:**
  - The buzzer operates in a non-blocking manner, allowing the system to continue normal operations while the alarm is active.
  - After the alarm duration, the buzzer is turned off, and the LED reverts to green if no other alarms are active.

## LCD Menu Interface

The LCD provides a user-friendly interface to monitor environmental data:

- **LiquidMenu Integration:**
  - Utilizes the LiquidMenu library to create multiple screens displaying different sensor readings.
  
- **Navigation:**
  - Users can navigate through screens using a push button. Each press cycles to the next screen.
  
- **Displayed Data:**
  - **Screen 0:** System title and current timestamp.
  - **Screen 1-11:** Individual sensor readings including CO₂, temperature, humidity, AQI, TVOC, eCO₂, H₂, H₂S, NH₃, CH₄, and CO concentrations.

## Pin Configuration

Proper pin configuration ensures reliable communication between the Arduino and connected peripherals:

- **Digital Pins:**
  - `3`: Red LED
  - `5`: Green LED
  - `6`: Blue LED
  - `7`: Piezo Buzzer
  - `4`: Relay Control for MQ-9b Heater
  - `10`: RTC Module Chip Select
  - `8`: SD Card Module Chip Select
  
- **Analog Pins:**
  - `A0`: MQ-8 Sensor (H₂)
  - `A1`: MQ-136 Sensor (H₂S)
  - `A2`: MQ-137 Sensor (NH₃)
  - `A3`: MQ-9b Sensor (CO & CH₄)
  - `A6`: Push Button

## Usage Instructions

1. **Setup Hardware:**
   - Connect all sensors to their respective pins as defined in `config.h`.
   - Ensure the SD card is properly inserted into the SD card module.
   - Connect the RTC module to the designated chip-select pin.
   - Attach the RGB LEDs, buzzer, and relay to their respective pins.
   - Connect the push button to `A6` with appropriate pull-up/down resistors.

2. **Configure Software:**
   - Adjust settings in `config.h` as needed, such as enabling/disabling debug prints, calibration, and data logging.
   - Set appropriate threshold values for each sensor based on environmental safety requirements.

3. **Calibration:**
   - If `PERFORM_MQ_CALIBRATION` is set to `true`, calibrate the MQ sensors in clean air conditions to establish baseline readings.
   - After calibration, set `PERFORM_MQ_CALIBRATION` to `false` to use the stored R0 values from EEPROM.

4. **Running the System:**
   - Upload the `main.ino` sketch to the Arduino Nano.
   - Upon startup, the system initializes all components and begins monitoring.
   - Navigate through different sensor data screens using the push button.
   - Monitor alarms via the buzzer and RGB LED indicators.

5. **Data Access:**
   - Retrieve the `dataLog.csv` file from the SD card for analysis using spreadsheet software or other data processing tools.
   - Real-time data can be viewed via Serial Monitor if enabled.

## Troubleshooting

- **SD Card Initialization Failed:**
  - Ensure the SD card is properly inserted.
  - Verify the chip-select pin is correctly connected.
  - Check the SD card format (should be FAT16/FAT32).

- **RTC Lost Confidence in DateTime:**
  - Check RTC module connections.
  - Ensure the RTC has a backup battery for maintaining time.

- **Sensor Not Detected:**
  - Verify wiring connections for each sensor.
  - Ensure sensors are powered correctly.
  - Check for I2C address conflicts.

- **Alarms Not Triggering:**
  - Confirm that sensor readings exceed the set thresholds.
  - Ensure the buzzer and LED pins are correctly connected and functioning.

- **LCD Not Displaying Data:**
  - Verify I2C connections.
  - Check the LCD address and dimensions in `config.h`.
  - Ensure the LiquidMenu library is correctly initialized.

## Conclusion

EnvSafetyMonitor provides a comprehensive solution for real-time environmental monitoring, combining accurate sensor data collection, intuitive user interfaces, reliable data logging, and prompt alarm notifications. Proper setup and calibration are essential for optimal performance, ensuring the system effectively safeguards against hazardous environmental conditions.

For further enhancements, consider integrating wireless data transmission for remote monitoring or expanding the system with additional sensors to cover a broader range of environmental parameters.

