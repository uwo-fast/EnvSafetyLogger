# Docs

_EnvSafetyMonitor - Environmental Safety Monitoring System_

The [`assets`](assets/) folder contains images related to design and build; [`datasheets`](mq-datasheets/) contains datasheets for hardware components used in the build.

## Table of Contents

- [Docs](#docs)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [System Overview](#system-overview)
  - [Hardware Components](#hardware-components)
  - [Software Dependancies](#software-dependancies)
  - [Usage Instructions](#usage-instructions)
  - [Troubleshooting](#troubleshooting)

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

## Software Dependancies

Consult the following table for the various library dependancies.

| Dependency                                                                                   | Source                                          |
| -------------------------------------------------------------------------------------------- | ----------------------------------------------- |
| [SPI](https://www.arduino.cc/en/Reference/SPI)                                               | Included by default in the Arduino IDE          |
| [Wire](https://www.arduino.cc/en/Reference/Wire)                                             | Included by default in the Arduino IDE          |
| [EEPROM](https://www.arduino.cc/en/Reference/EEPROM)                                         | Included by default in the Arduino IDE          |
| [Math.h](https://www.arduino.cc/reference/en/language/functions/math/)                       | Included by default in the Arduino IDE          |
| [SD](https://www.arduino.cc/en/Reference/SD)                                                 | Search `SD` in Lib Manager                      |
| [Rtc (Makuna)](https://github.com/Makuna/Rtc)                                                | Search `rtc makuna` in Lib Manager              |
| [SparkFun_SCD4x_Arduino_Library](https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library) | Search `SparkFun SCD4x` in Lib Manager          |
| [ScioSense_ENS160](https://github.com/adafruit/ENS160_driver)                                | Search `Adafruit ENS160` in Lib Manager         |
| [MQSensorsLib](https://github.com/miguel5612/MQSensorsLib)                                   | Search `mqunified` in Lib Manager               |
| [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)                        | Search `LiquidCrystal I2C Frank` in Lib Manager |
| [LiquidMenu](https://github.com/VasilKalchev/LiquidMenu)                                     | Search `LiquidMenu` in Lib Manager              |

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
  - Ensure the LiquidMenu library is correctly configured
    - Ensure you have modified the LiquidMenu library header to use the I2C library
    - Ensure the I2C LCD library is also installed and included in the project
  - Verify I2C connections.
  - Check the LCD address and dimensions in `config.h`.
