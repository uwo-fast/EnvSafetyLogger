#include <Arduino.h>
#include "config.h"

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RtcDS3234.h>
#include <SparkFun_SCD4x_Arduino_Library.h>  // SCD41 Library
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>

// Initialize RGB Values
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

// Initialize LCD
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS);

// Initialize LiquidMenu
LiquidMenu menu(lcd);

// Initialize RTC
RtcDS3234<SPIClass> Rtc(SPI, DS3234_CS_PIN);

// Initialize SCD41 Sensor
SCD4x mySensor;

// Global Variables
File dataFile;

// Sensor Values
int co2 = 0;
float temperature = 0.0;
float humidity = 0.0;
char timestamp[20]; // Use a character array for timestamp

// Button Debounce Variables
bool buttonState = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// Current LED Color Index
int currentColorIndex = 0;

// Function Declarations
void initializePins();
void initializeAlarms();
void initializeLCDMenu();
void logSensorData();
void checkAndTriggerAlarms();
void updateLCDMenu();
void handleButtonPress();
void changeLEDColor();
void readSensors();

// Create LiquidLine and LiquidScreen objects
LiquidLine line1(0, 0, "CO2:", co2, "ppm");
LiquidLine line2(0, 1, "Temp:", temperature, "C");

LiquidLine line3(0, 0, "Humidity:", humidity, "%RH");
LiquidLine line4(0, 1, "Time:", timestamp);

LiquidScreen screen1(line1, line2);
LiquidScreen screen2(line3, line4);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && LOG_TO_SERIAL);

    initializePins();

    // Initialize I2C
    Wire.begin();

    // Initialize SD card
    if (LOG_TO_SD) {
        if (!SD.begin(chipSelect)) {
            Serial.println("SD Card initialization failed!");
            while (true);
        }
        Serial.println("SD Card initialized.");
    }

    // Initialize RTC
    SPI.begin();
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid()) {
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning()) {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3234SquareWavePin_ModeNone);

    // Initialize SCD41 sensor
    if (!mySensor.begin()) {
        Serial.println("SCD41 sensor not detected. Check wiring.");
        while (true);
    }
    Serial.println("SCD41 initialized.");
    mySensor.startPeriodicMeasurement();
    delay(100); // Short delay after SCD41 initialization to avoid I2C conflict

    // Initialize LCD
    lcd.init();
    lcd.backlight();

    // Initialize LiquidMenu
    initializeLCDMenu();
    menu.init(); // Important: Initialize the menu after adding screens

    // Initialize Alarms
    initializeAlarms();

    // Initialize random seed
    randomSeed(analogRead(0));
}

void loop() {
    static unsigned long lastMillis = 0;

    // Handle button press continuously
    handleButtonPress();

    // Only read sensors every `SAMPLING_INTERVAL_MS`
    if (millis() - lastMillis >= SAMPLING_INTERVAL_MS) {
        lastMillis = millis();

        // Read sensors and update data
        readSensors();

        // Update LCD menu display
        updateLCDMenu();

        // Check and trigger alarms
        checkAndTriggerAlarms();

        // Log sensor data
        logSensorData();
    }
}

void initializePins() {
    // RGB LED Pins
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    // Buzzer Pin
    pinMode(buzzerPin, OUTPUT);

    // Button Pin
    pinMode(buttonPin, INPUT_PULLUP); // Assuming button connected to GND when pressed

    // Ensure alarms are off initially
    digitalWrite(buzzerPin, LOW);
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
}

void initializeAlarms() {
    // Alarms are already initialized in initializePins()
}

void initializeLCDMenu() {
    // Set decimal places for floats
    line2.set_decimalPlaces(1); // Temperature
    line3.set_decimalPlaces(1); // Humidity

    // Attach functions to lines if needed (e.g., for focus or interactivity)
    // line1.attach_function(1, someFunction);

    // Add screens to menu
    menu.add_screen(screen1);
    menu.add_screen(screen2);
}

void readSensors() {
    if (!Rtc.IsDateTimeValid()) {
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();

    // Update timestamp
    snprintf(timestamp, sizeof(timestamp), "%02u/%02u/%04u %02u:%02u:%02u",
             now.Month(), now.Day(), now.Year(),
             now.Hour(), now.Minute(), now.Second());

    if (mySensor.readMeasurement()) {
        co2 = mySensor.getCO2();
        temperature = mySensor.getTemperature();
        humidity = mySensor.getHumidity();
    } else {
        Serial.println("Failed to read from SCD41 sensor.");
    }
}

void handleButtonPress() {
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) { // Button pressed
                menu.next_screen();
                changeLEDColor();
            }
        }
    }

    lastButtonState = reading;
}

void changeLEDColor() {
    int newColorIndex = random(numStandardColors);
    currentColorIndex = newColorIndex;

    redValue = standardColors[currentColorIndex][0];
    greenValue = standardColors[currentColorIndex][1];
    blueValue = standardColors[currentColorIndex][2];

    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
}

void updateLCDMenu() {
    menu.update();
}

void checkAndTriggerAlarms() {
    bool alarmTriggered = false;

    if (co2 > CO2_THRESHOLD_PPM) {
        alarmTriggered = true;
        Serial.println("ALARM: High CO2 detected!");
    }

    if (temperature > TEMP_THRESHOLD_C) {
        alarmTriggered = true;
        Serial.println("ALARM: High Temperature detected!");
    }

    if (humidity > HUMIDITY_THRESHOLD_RH) {
        alarmTriggered = true;
        Serial.println("ALARM: High Humidity detected!");
    }

    if (alarmTriggered) {
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
    }
}

void logSensorData() {
    if (LOG_TO_SD) {
        dataFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
        if (dataFile) {
            dataFile.print(timestamp);
            dataFile.print(", CO2: "); dataFile.print(co2); dataFile.print(" ppm");
            dataFile.print(", Temp: "); dataFile.print(temperature); dataFile.print(" C");
            dataFile.print(", Humidity: "); dataFile.print(humidity); dataFile.println(" %RH");
            dataFile.close();
        } else {
            Serial.println("Error opening file for logging.");
        }
    }

    if (LOG_TO_SERIAL) {
        Serial.print("{\"timestamp\":\"");
        Serial.print(timestamp);
        Serial.print("\", \"CO2\":");
        Serial.print(co2);
        Serial.print(", \"Temperature\":");
        Serial.print(temperature, 1);
        Serial.print(", \"Humidity\":");
        Serial.print(humidity, 1);
        Serial.println("}");
    }
}
