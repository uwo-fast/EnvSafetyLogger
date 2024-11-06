/***************************************************************************
  EnvSafetyMonitor - Environmental Safety Monitoring System

  This program monitors environmental parameters using SCD41, ENS160, and MQ-8 sensors,
  displays data on a 2x16 I2C LCD, logs data to an SD card, and triggers alarms
  based on threshold values. The LCD menu allows scrolling through different
  parameter screens using a push button.

***************************************************************************/

#include <Arduino.h>
#include "config.h"

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RtcDS3234.h>
#include <SparkFun_SCD4x_Arduino_Library.h> // SCD41 Library
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include "ScioSense_ENS160.h" // ENS160 library
#include <MQUnifiedsensor.h>  // MQ-8 sensor library

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
SCD4x scd41;

// Initialize ENS160 Sensor
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);

// Definitions for MQ-8 Sensor
#define MQ8_DEVICE "Arduino Nano"
#define MQ8_VOLTAGE_RESOLUTION 5
#define MQ8_PIN mq8Pin // Using mq8Pin defined in config.h
#define MQ8_TYPE "MQ-8"
#define MQ8_ADC_BIT_RESOLUTION 10
#define MQ8_RATIO_CLEAN_AIR 70 // RS / R0 = 70 ppm

// Declare MQ-8 Sensor Object
MQUnifiedsensor MQ8(MQ8_DEVICE, MQ8_VOLTAGE_RESOLUTION, MQ8_ADC_BIT_RESOLUTION, MQ8_PIN, MQ8_TYPE);

// Global Variables
File dataFile;

// Sensor Values from SCD41
int co2 = 0;
float temperature = 0.0;
float humidity = 0.0;
char timestamp[20]; // Use a character array for timestamp

// Sensor Values from ENS160
int aqi = 0;
int tvoc = 0;
int eco2 = 0;

// Sensor Value from MQ-8
float h2_ppm = 0.0;

// Button Debounce Variables
bool buttonState = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// Current LED Color Index
int currentColorIndex = 0;

// Alarm Variables
bool alarmActive = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_DURATION_MS = 500;

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

// Screen 0: Title and Timestamp
LiquidLine screen0_line1(0, 0, "EnvSafetyMonitor");
LiquidLine screen0_line2(0, 1, timestamp);
LiquidScreen screen0(screen0_line1, screen0_line2);

// Screen1: CO2 ppm
LiquidLine screen1_line1(0, 0, "CO2 ppm:");
LiquidLine screen1_line2(0, 1, co2, "ppm");
LiquidScreen screen1(screen1_line1, screen1_line2);

// Screen2: Temperature C
LiquidLine screen2_line1(0, 0, "Temp C:");
LiquidLine screen2_line2(0, 1, temperature, "C");
LiquidScreen screen2(screen2_line1, screen2_line2);

// Screen3: Humidity %RH
LiquidLine screen3_line1(0, 0, "Humidity:");
LiquidLine screen3_line2(0, 1, humidity, "%RH");
LiquidScreen screen3(screen3_line1, screen3_line2);

// Screen4: AQI
LiquidLine screen4_line1(0, 0, "AQI:");
LiquidLine screen4_line2(0, 1, aqi, "");
LiquidScreen screen4(screen4_line1, screen4_line2);

// Screen5: TVOC ppb
LiquidLine screen5_line1(0, 0, "TVOC ppb:");
LiquidLine screen5_line2(0, 1, tvoc, "ppb");
LiquidScreen screen5(screen5_line1, screen5_line2);

// Screen6: eCO2 ppm
LiquidLine screen6_line1(0, 0, "eCO2 ppm:");
LiquidLine screen6_line2(0, 1, eco2, "ppm");
LiquidScreen screen6(screen6_line1, screen6_line2);

// Screen7: H2 ppm
LiquidLine screen7_line1(0, 0, "H2 ppm:");
LiquidLine screen7_line2(0, 1, h2_ppm, "ppm");
LiquidScreen screen7(screen7_line1, screen7_line2);

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && LOG_TO_SERIAL)
        ;

    initializePins();

    // Initialize I2C
    Wire.begin();

    // Initialize SD card
    if (LOG_TO_SD)
    {
        if (!SD.begin(chipSelect))
        {
#if DEBUG_MODE
            Serial.println(F("SD Card initialization failed!"));
#endif
            while (true)
                ; // Halt if SD card fails to initialize
        }
#if DEBUG_MODE
        Serial.println(F("SD Card initialized."));
#endif
    }

    // Initialize RTC
    SPI.begin();
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid())
    {
#if DEBUG_MODE
        Serial.println(F("RTC lost confidence in the DateTime!"));
#endif
        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
#if DEBUG_MODE
        Serial.println(F("RTC was not actively running, starting now"));
#endif
        Rtc.SetIsRunning(true);
    }

    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3234SquareWavePin_ModeNone);

    // Initialize SCD41 sensor
    if (!scd41.begin())
    {
        Serial.println(F("SCD41 sensor not detected. Check wiring."));
        while (true)
            ; // Halt if SCD41 fails to initialize
    }
#if DEBUG_MODE
    Serial.println(F("SCD41 initialized."));
#endif
    scd41.startPeriodicMeasurement();
    delay(100); // Short delay after SCD41 initialization to avoid I2C conflict

    // Initialize ENS160 sensor
    Serial.print(F("ENS160..."));
    ens160.begin();
    Serial.println(ens160.available() ? F("done.") : F("failed!"));
    if (ens160.available())
    {
#if DEBUG_MODE
        Serial.print(F("\tRev: "));
        Serial.print(ens160.getMajorRev());
        Serial.print(F("."));
        Serial.print(ens160.getMinorRev());
        Serial.print(F("."));
        Serial.println(ens160.getBuild());
#endif

        Serial.print(F("\tStandard mode "));
        Serial.println(ens160.setMode(ENS160_OPMODE_STD) ? F("done.") : F("failed!"));
    }

    // Initialize MQ-8 Sensor
    // Set math model to calculate the PPM concentration and the value of constants
    MQ8.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ8.setA(976.97);
    MQ8.setB(-0.688); // Configure the equation to calculate H2 concentration

    MQ8.init();

    // Calibration
#if DEBUG_MODE
    Serial.print("Calibrating MQ-8, please wait.");
#endif
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        MQ8.update(); // Read the voltage from the analog pin
        calcR0 += MQ8.calibrate(MQ8_RATIO_CLEAN_AIR);
#if DEBUG_MODE
        Serial.print(".");
#endif
    }
    MQ8.setR0(calcR0 / 10);
#if DEBUG_MODE
    Serial.println(" done!");
#endif

    if (isinf(calcR0))
    {
        Serial.println("Warning: Connection issue, MQ-8 R0 is infinite (Open circuit detected). Please check your wiring and supply.");
        while (1)
            ;
    }
    if (calcR0 == 0)
    {
        Serial.println("Warning: Connection issue found, MQ-8 R0 is zero (Analog pin shorts to ground). Please check your wiring and supply.");
        while (1)
            ;
    }
    MQ8.serialDebug(DEBUG_MODE);

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Initialize LiquidMenu
    initializeLCDMenu();
    menu.init(); // Important: Initialize the menu after adding screens

    // Initialize Alarms
    initializeAlarms();

    // Initialize random seed
    randomSeed(analogRead(0));

    Serial.println(F("System initialized."));
}

void loop()
{
    static unsigned long lastMillis = 0;

    // Handle button press continuously
    handleButtonPress();

    // Only read sensors every `SAMPLING_INTERVAL_MS`
    if (millis() - lastMillis >= SAMPLING_INTERVAL_MS)
    {
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

    // Handle non-blocking alarm buzzer
    if (alarmActive)
    {
        if (millis() - alarmStartTime >= ALARM_DURATION_MS)
        {
            digitalWrite(buzzerPin, LOW);
            alarmActive = false;
        }
    }
}

void initializePins()
{
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

void initializeAlarms()
{
    // Alarms are already initialized in initializePins()
}

void initializeLCDMenu()
{
    // Set decimal places for floats and integers as needed
    screen0_line2.set_decimalPlaces(0); // Timestamp is a string, no decimals
    screen1_line2.set_decimalPlaces(0); // CO2 ppm
    screen2_line2.set_decimalPlaces(1); // Temperature C
    screen3_line2.set_decimalPlaces(1); // Humidity %RH
    screen4_line2.set_decimalPlaces(0); // AQI
    screen5_line2.set_decimalPlaces(0); // TVOC ppb
    screen6_line2.set_decimalPlaces(0); // eCO2 ppm
    screen7_line2.set_decimalPlaces(2); // H2 ppm

    // Add screens to menu
    menu.add_screen(screen0);
    menu.add_screen(screen1);
    menu.add_screen(screen2);
    menu.add_screen(screen3);
    menu.add_screen(screen4);
    menu.add_screen(screen5);
    menu.add_screen(screen6);
    menu.add_screen(screen7);
}

void readSensors()
{
    if (!Rtc.IsDateTimeValid())
    {
#if DEBUG_MODE
        Serial.println(F("RTC lost confidence in the DateTime!"));
#endif
    }

    RtcDateTime now = Rtc.GetDateTime();

    // Update timestamp
    snprintf(timestamp, sizeof(timestamp), "%02u/%02u/%04u %02u:%02u:%02u",
             now.Month(), now.Day(), now.Year(),
             now.Hour(), now.Minute(), now.Second());

    // Read SCD41 sensor
    if (scd41.readMeasurement())
    {
        co2 = scd41.getCO2();
        temperature = scd41.getTemperature();
        humidity = scd41.getHumidity();
    }
    else
    {
#if DEBUG_MODE
        Serial.println(F("Failed to read from SCD41 sensor."));
#endif
    }

    // Read ENS160 sensor
    if (ens160.available())
    {
        ens160.set_envdata(temperature, humidity);
        ens160.measure(true);
        ens160.measureRaw(true);

        aqi = ens160.getAQI();
        tvoc = ens160.getTVOC();
        eco2 = ens160.geteCO2();
    }
    else
    {
#if DEBUG_MODE
        Serial.println(F("ENS160 not available."));
#endif
    }

    // Read MQ-8 Sensor
    MQ8.update();              // Read the voltage from the analog pin
    h2_ppm = MQ8.readSensor(); // Get the H2 concentration in ppm
}

void handleButtonPress()
{
    int reading = digitalRead(buttonPin);

    if (reading != lastButtonState)
    {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY_MS)
    {
        if (reading != buttonState)
        {
            buttonState = reading;
            if (buttonState == LOW)
            { // Button pressed
                menu.next_screen();
                changeLEDColor(); // Change LED color on button press
            }
        }
    }

    lastButtonState = reading;
}

void changeLEDColor()
{
    int newColorIndex = random(numStandardColors);
    currentColorIndex = newColorIndex;

    redValue = standardColors[currentColorIndex][0];
    greenValue = standardColors[currentColorIndex][1];
    blueValue = standardColors[currentColorIndex][2];

    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
}

void updateLCDMenu()
{
    menu.update();
}

void checkAndTriggerAlarms()
{
    bool alarmTriggered = false;

    if (co2 > CO2_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        Serial.println(F("ALARM: High CO2 detected!"));
    }

    if (temperature > TEMP_THRESHOLD_C)
    {
        alarmTriggered = true;
        Serial.println(F("ALARM: High Temperature detected!"));
    }

    if (humidity > HUMIDITY_THRESHOLD_RH)
    {
        alarmTriggered = true;
        Serial.println(F("ALARM: High Humidity detected!"));
    }

    if (alarmTriggered)
    {
        if (!alarmActive) // Start the alarm if not already active
        {
            alarmActive = true;
            alarmStartTime = millis();
            digitalWrite(buzzerPin, HIGH);
        }
    }
}

void logSensorData()
{
    if (LOG_TO_SD)
    {
        dataFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
        if (dataFile)
        {
            dataFile.print(timestamp);
            dataFile.print(", CO2: ");
            dataFile.print(co2);
            dataFile.print(" ppm");
            dataFile.print(", Temp: ");
            dataFile.print(temperature);
            dataFile.print(" C");
            dataFile.print(", Humidity: ");
            dataFile.print(humidity);
            dataFile.print(" %RH");
            dataFile.print(", AQI: ");
            dataFile.print(aqi);
            dataFile.print(", TVOC: ");
            dataFile.print(tvoc);
            dataFile.print(" ppb");
            dataFile.print(", eCO2: ");
            dataFile.print(eco2);
            dataFile.print(" ppm");
            dataFile.print(", H2: ");
            dataFile.print(h2_ppm);
            dataFile.print(" ppm");
            dataFile.println();
            dataFile.close();
        }
        else
        {
            Serial.println(F("Error opening file for logging."));
        }
    }

    if (LOG_TO_SERIAL)
    {
        Serial.print(F("{\"timestamp\":\""));
        Serial.print(timestamp);
        Serial.print(F("\", \"CO2\":"));
        Serial.print(co2);
        Serial.print(F(", \"Temperature\":"));
        Serial.print(temperature, 2);
        Serial.print(F(", \"Humidity\":"));
        Serial.print(humidity, 2);
        Serial.print(F(", \"AQI\":"));
        Serial.print(aqi);
        Serial.print(F(", \"TVOC\":"));
        Serial.print(tvoc);
        Serial.print(F(", \"eCO2\":"));
        Serial.print(eco2);
        Serial.print(F(", \"H2\":"));
        Serial.print(h2_ppm, 2);
        Serial.println(F("}"));
    }
}
