#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h> // Ensure Arduino definitions are available

// ------------------------------
// Pin Definitions
// ------------------------------
const int redPin = 3;      // Digital Pin D3 for Red LED
const int greenPin = 5;    // Digital Pin D5 for Green LED
const int bluePin = 6;     // Digital Pin D6 for Blue LED
const int buzzerPin = 7;   // Digital Pin D7 for Piezo Buzzer
const int buttonPin = A6;  // Analog Pin A6 for Push Button

const int mq8Pin = A0;     // Analog Pin A0 for MQ-8 Sensor (H2)
const int mq136Pin = A1;   // Analog Pin A1 for MQ-136 Sensor (H2S)
const int mq137Pin = A2;   // Analog Pin A2 for MQ-137 Sensor (NH3)
const int mq9bPin = A3;   // Analog Pin A3 for MQ-9b Sensor (CO & CH4)

// RTC Chip Select Pin
const uint8_t DS3234_CS_PIN = 10; // DS3234 Chip-select pin

// SD Card Chip Select Pin
const uint8_t chipSelect = 8; // SD card CS pin

// ------------------------------
// Standard Colors Definitions
// ------------------------------
const int RED[3]     = {255, 0, 0};
const int GREEN[3]   = {0, 255, 0};
const int BLUE[3]    = {0, 0, 255};
const int MAGENTA[3] = {255, 0, 255};
const int CYAN[3]    = {0, 255, 255};
const int YELLOW[3]  = {255, 255, 0};
const int WHITE[3]   = {255, 255, 255};

// Array of Pointers to Standard Colors
const int* standardColors[] = {RED, GREEN, BLUE, MAGENTA, CYAN, YELLOW, WHITE};
const int numStandardColors = sizeof(standardColors) / sizeof(standardColors[0]);

// ------------------------------
// Variables for RGB values
// ------------------------------
extern int redValue;
extern int greenValue;
extern int blueValue;

// ------------------------------
// Sampling Settings
// ------------------------------
#define SAMPLING_INTERVAL_MS 5000  // Interval between sensor readings in milliseconds (5 seconds)

// ------------------------------
// Alarm Thresholds
// ------------------------------
#define CO2_THRESHOLD_PPM      1000  // CO2 threshold in ppm
#define TEMP_THRESHOLD_C       35.0  // Temperature threshold in Celsius
#define HUMIDITY_THRESHOLD_RH  70.0  // Humidity threshold in %RH

// ------------------------------
// Logging Settings
// ------------------------------
#define LOG_FILE_NAME "dataLog.txt"   // Name of the log file on SD card
#define LOG_TO_SD  true               // Enable/Disable logging to SD card
#define LOG_TO_SERIAL true            // Enable/Disable logging to Serial (RPi)

// ------------------------------
// Debugging Settings
// ------------------------------
#define DEBUG_MODE false  // Enable/Disable debug prints

// ------------------------------
// I2C LCD Settings
// ------------------------------
#define LCD_I2C_ADDRESS 0x27  // I2C address of the LCD
#define LCD_COLUMNS 16        // Number of columns on the LCD
#define LCD_ROWS    2         // Number of rows on the LCD

// ------------------------------
// Serial Communication Settings
// ------------------------------
#define SERIAL_BAUD_RATE 115200  // Baud rate for serial communication

// ------------------------------
// Debounce Settings
// ------------------------------
#define DEBOUNCE_DELAY_MS 50    // Debounce delay in milliseconds

#endif // CONFIG_H
