# Docs

Build and usage notes. The MQ sensor datasheets and the gas calculation are in
[MQ sensor datasheets](mq-datasheets/README.md).

## Hardware

The sensors are listed in the [README](../README.md#sensors-and-components) and
every part in the bill of materials, [`BOM_v0.ods`](BOM_v0.ods). The rest of the
circuit:

- A DS3234 real-time clock on SPI, chip select D10.
- The SD card on SPI, chip select D8.
- A 2x16 LCD on I2C at address 0x27.
- An Omron G5Q-1 relay for the MQ-9B heater, its coil switched by a 2N7000
  MOSFET, with a 1N4148 diode across the coil.

### Pins

From `firmware/main/config.h`:

| Function | Pin |
| --- | --- |
| RGB LED red | D3 |
| RGB LED green | D5 |
| RGB LED blue | D6 |
| Buzzer | D7 |
| MQ-9B heater relay (2N7000 gate) | D4 |
| Push button | A6 |
| MQ-8 | A0 |
| MQ-136 | A1 |
| MQ-137 | A2 |
| MQ-9B | A3 |
| RTC chip select | D10 |
| SD card chip select | D8 |

The firmware sets the button pin to `INPUT_PULLUP` and counts a LOW reading as a
press, so it expects the button to connect A6 to GND. The schematic instead draws
the button to +5 V with a 10k pull-down (R10). The two disagree.

## Software Dependencies

The firmware builds with the Arduino Nano Every core and these libraries. The
versions are the ones CI builds with (`just setup` installs them).

| Dependency | Library Manager name | Version |
| --- | --- | --- |
| Arduino megaAVR boards (Nano Every core) | `arduino:megaavr` | 1.8.8 |
| SPI, Wire, EEPROM | included with the board core | |
| [SD](https://github.com/arduino-libraries/SD) | `SD` | 1.3.0 |
| [Rtc](https://github.com/Makuna/Rtc) | `Rtc by Makuna` | 2.5.0 |
| [SparkFun SCD4x](https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library) | `SparkFun SCD4x Arduino Library` | 1.1.2 |
| [ScioSense ENS160](https://github.com/adafruit/ENS160_driver) | `ENS160 - Adafruit Fork` | 3.0.1 |
| [MQSensorsLib](https://github.com/miguel5612/MQSensorsLib) | `MQUnifiedsensor` | 3.0.0 |
| [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C) | `LiquidCrystal I2C` | 1.1.2 |
| [LiquidMenu](https://github.com/VasilKalchev/LiquidMenu) | `LiquidMenu` | 1.6.3 |

LiquidMenu has to be set to use the I2C LCD library; see
[Building the firmware](../README.md#building-the-firmware).

## Usage Instructions

1. **Configure:** the settings are in `config.h` and the alarm levels in
   `alarmThresholds.h`; [`firmware/README.md`](../firmware/README.md) describes
   each one.

2. **Calibrate:** set `PERFORM_MQ_CALIBRATION` to `true`, upload, and run the
   monitor in clean air. For each MQ sensor the firmware averages R0 over 10
   readings and stores it in EEPROM. Then set `PERFORM_MQ_CALIBRATION` to
   `false` and upload again, so the firmware reads R0 from EEPROM. The ppm
   coefficients are not yet checked against a reference gas; see
   [Calibration status](../README.md#calibration-status).

3. **Run:** upload `main.ino` to the Arduino Nano Every. The push button pages
   through the LCD screens. The RGB LED and buzzer show the alarm level:

   | Level | LED | Buzzer |
   | --- | --- | --- |
   | Normal | green | off |
   | Warning | orange | on and off every 1 s |
   | Danger | red | on |

4. **Data:** the log file (`LOG_FILE_NAME` in `config.h`) on the SD card is a
   CSV. With `ENABLE_DATA_PRINTS` set to `true`, the firmware also prints each
   reading on the serial port at 115200 baud, one JSON object per line.

## Troubleshooting

- **SD Card Initialization Failed:**

  - Ensure the SD card is properly inserted.
  - Verify the chip-select pin is correctly connected.
  - Check the SD card format (should be FAT16/FAT32).

- **RTC Lost Confidence in DateTime:**

  - Check RTC module connections.
  - Ensure the RTC has a backup battery for maintaining time.

- **LCD Not Displaying Data:**
  - Ensure the LiquidMenu library is correctly configured
    - Ensure LiquidMenu is set to use the I2C library (see [Building the firmware](../README.md#building-the-firmware))
    - Ensure the I2C LCD library is also installed and included in the project
  - Verify I2C connections.
  - Check the LCD address and dimensions in `config.h`.
