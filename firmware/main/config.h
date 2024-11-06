#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h> // Ensure Arduino definitions are available

// ------------------------------
// Debugging Settings
// ------------------------------
#define ENABLE_DEBUG_PRINTS false  // Enable/Disable debug prints
#define ENABLE_DATA_PRINTS true   // Enable/Disable data readings

// ------------------------------
// Calibration Settings
// ------------------------------
#define PERFORM_MQ_CALIBRATION false // Set to true to perform calibration of MQ sensors

// ------------------------------
// Logging Settings
// ------------------------------
#define LOG_FILE_NAME "dataLog.csv" // Name of the log file on SD card (changed to .csv)
#define LOG_TO_SD true              // Enable/Disable logging to SD card

// ------------------------------
// Pin Definitions
// ------------------------------
const int redPin = 3;     // Digital Pin D3 for Red LED
const int greenPin = 5;   // Digital Pin D5 for Green LED
const int bluePin = 6;    // Digital Pin D6 for Blue LED
const int buzzerPin = 7;  // Digital Pin D7 for Piezo Buzzer
const int buttonPin = A6; // Analog Pin A6 for Push Button

const int mq8Pin = A0;     // Analog Pin A0 for MQ-8 Sensor (H₂)
const int mq136Pin = A1;   // Analog Pin A1 for MQ-136 Sensor (H₂S)
const int mq137Pin = A2;   // Analog Pin A2 for MQ-137 Sensor (NH₃)
const int mq9bPin = A3;    // Analog Pin A3 for MQ-9b Sensor (CO & CH₄)
const int relayPin = 4;    // Digital Pin 4 for Relay Control (MQ-9b Heater Voltage)

// RTC Chip Select Pin
const uint8_t DS3234_CS_PIN = 10; // DS3234 Chip-select pin

// SD Card Chip Select Pin
const uint8_t chipSelect = 8; // SD card CS pin

// ------------------------------
// Standard Colors Definitions
// ------------------------------
const int RED[3]     = {255, 0, 0};
const int GREEN[3]   = {0, 255, 0};

// ------------------------------
// Sampling Settings
// ------------------------------
#define SAMPLING_INTERVAL_MS 5000 // Interval between sensor readings in milliseconds (5 seconds)

// ------------------------------
// Alarm Thresholds
// ------------------------------
#define CO2_THRESHOLD_PPM      1000  // CO2 threshold in ppm
#define TEMP_THRESHOLD_C       35.0  // Temperature threshold in Celsius
#define HUMIDITY_THRESHOLD_RH  70.0  // Humidity threshold in %RH
#define H2_THRESHOLD_PPM       100   // H₂ threshold in ppm
#define H2S_THRESHOLD_PPM      10    // H₂S threshold in ppm
#define NH3_THRESHOLD_PPM      25    // NH₃ threshold in ppm
#define CH4_THRESHOLD_PPM      100   // CH₄ threshold in ppm
#define CO_THRESHOLD_PPM       50    // CO threshold in ppm

// ------------------------------
// I2C LCD Settings
// ------------------------------
#define LCD_I2C_ADDRESS 0x27 // I2C address of the LCD
#define LCD_COLUMNS 16       // Number of columns on the LCD
#define LCD_ROWS 2           // Number of rows on the LCD

// ------------------------------
// Serial Communication Settings
// ------------------------------
#define SERIAL_BAUD_RATE 115200 // Baud rate for serial communication

// ------------------------------
// Debounce Settings
// ------------------------------
#define DEBOUNCE_DELAY_MS 50 // Debounce delay in milliseconds

#endif // CONFIG_H
