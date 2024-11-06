/***************************************************************************
  EnvSafetyMonitor - Environmental Safety Monitoring System

  This program monitors environmental parameters using SCD41, ENS160, MQ-8, MQ-136, MQ-137,
  and MQ-9b sensors, displays data on a 2x16 I2C LCD, logs data to an SD card
  in CSV format, and triggers alarms based on threshold values. The LCD menu allows
  scrolling through different parameter screens using a push button.

***************************************************************************/

#include <Arduino.h>
#include "config.h"

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RtcDS3234.h>
#include <SparkFun_SCD4x_Arduino_Library.h>  // SCD41 Library
#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include "ScioSense_ENS160.h"  // ENS160 library
#include <MQUnifiedsensor.h>   // MQ sensor library
#include <EEPROM.h>            // EEPROM library

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

// Definitions for MQ Sensors
#define DEVICE "Arduino Nano"
#define VOLTAGE_RESOLUTION 5
#define ADC_BIT_RESOLUTION 10

// Definitions for MQ-8 Sensor
#define MQ8_TYPE "MQ-8"
#define MQ8_RATIO_CLEAN_AIR 70 // RS / R0 = 70 ppm
#define EEPROM_ADDRESS_MQ8_R0 0 // EEPROM address for MQ-8 R0

// Declare MQ-8 Sensor Object
MQUnifiedsensor MQ8(DEVICE, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, mq8Pin, MQ8_TYPE);

// Definitions for MQ-136 Sensor
#define MQ136_TYPE "MQ-136"
#define MQ136_RATIO_CLEAN_AIR 3.6 // RS / R0 = 3.6 ppm
#define EEPROM_ADDRESS_MQ136_R0 4 // EEPROM address for MQ-136 R0

// Declare MQ-136 Sensor Object
MQUnifiedsensor MQ136(DEVICE, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, mq136Pin, MQ136_TYPE);

// Definitions for MQ-137 Sensor
#define MQ137_TYPE "MQ-137"
#define MQ137_RATIO_CLEAN_AIR 4.2 // RS / R0 = 4.2 ppm
#define EEPROM_ADDRESS_MQ137_R0 8 // EEPROM address for MQ-137 R0

// Declare MQ-137 Sensor Object
MQUnifiedsensor MQ137(DEVICE, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, mq137Pin, MQ137_TYPE);

// Definitions for MQ-9b Sensor
#define MQ9B_TYPE "MQ-9"
#define MQ9B_RATIO_CLEAN_AIR 9.6 // RS / R0 = 9.6 ppm
#define EEPROM_ADDRESS_MQ9B_R0 12 // EEPROM address for MQ-9b R0

// Declare MQ-9b Sensor Object
MQUnifiedsensor MQ9B(DEVICE, VOLTAGE_RESOLUTION, ADC_BIT_RESOLUTION, mq9bPin, MQ9B_TYPE);

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

// Sensor Values from MQ Sensors
float h2_ppm = 0.0;
float h2s_ppm = 0.0;
float nh3_ppm = 0.0;
float ch4_ppm = 0.0; // Methane concentration
float co_ppm = 0.0;   // Carbon Monoxide concentration

// Button Debounce Variables
bool buttonState = HIGH;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

// Alarm Variables
bool alarmTriggered = false;
bool alarmActive = false;
unsigned long alarmStartTime = 0;
const unsigned long ALARM_DURATION_MS = 500;

// MQ-9b Relay Control Variables
unsigned long mq9bPreviousMillis = 0;
unsigned long mq9bInterval = 60000;  // 60 seconds for HIGH (5V) phase
bool mq9bHighState = true;           // Start in the HIGH (5V) phase

// Timing Variable for Sensor Sampling
unsigned long lastMillis = 0; // Declared globally

// Function Declarations
void initializePins();
void initializeAlarms();
void initializeLCDMenu();
void logSensorData();
void checkAndTriggerAlarms();
void updateLCDMenu();
void handleButtonPress();
void controlMQ9bRelay();

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

// Screen8: H2S ppm
LiquidLine screen8_line1(0, 0, "H2S ppm:");
LiquidLine screen8_line2(0, 1, h2s_ppm, "ppm");
LiquidScreen screen8(screen8_line1, screen8_line2);

// Screen9: NH3 ppm
LiquidLine screen9_line1(0, 0, "NH3 ppm:");
LiquidLine screen9_line2(0, 1, nh3_ppm, "ppm");
LiquidScreen screen9(screen9_line1, screen9_line2);

// Screen10: CH4 ppm
LiquidLine screen10_line1(0, 0, "CH4 ppm:");
LiquidLine screen10_line2(0, 1, ch4_ppm, "ppm");
LiquidScreen screen10(screen10_line1, screen10_line2);

// Screen11: CO ppm
LiquidLine screen11_line1(0, 0, "CO ppm:");
LiquidLine screen11_line2(0, 1, co_ppm, "ppm");
LiquidScreen screen11(screen11_line1, screen11_line2);

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && ENABLE_DATA_PRINTS); // Wait for serial if data printing is enabled

    initializePins();

    // Initialize I2C
    Wire.begin();

    // Initialize SD card
    if (LOG_TO_SD)
    {
        if (!SD.begin(chipSelect))
        {
#if ENABLE_DEBUG_PRINTS
            Serial.println(F("SD Card initialization failed!"));
#endif
            while (true)
                ; // Halt if SD card fails to initialize
        }
#if ENABLE_DEBUG_PRINTS
        Serial.println(F("SD Card initialized."));
#endif

        // Check if the log file exists
        if (!SD.exists(LOG_FILE_NAME))
        {
            // Create the file and write the header
            dataFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
            if (dataFile)
            {
                // Write CSV header
                dataFile.println("Timestamp,CO2_ppm,Temperature_C,Humidity_%RH,AQI,TVOC_ppb,eCO2_ppm,H2_ppm,H2S_ppm,NH3_ppm,CH4_ppm,CO_ppm");
                dataFile.close();
#if ENABLE_DEBUG_PRINTS
                Serial.println(F("CSV Header written to SD Card."));
#endif
            }
            else
            {
#if ENABLE_DEBUG_PRINTS
                Serial.println(F("Error creating log file on SD Card."));
#endif
            }
        }
    }

    // Initialize RTC
    SPI.begin();
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid())
    {
#if ENABLE_DEBUG_PRINTS
        Serial.println(F("RTC lost confidence in the DateTime!"));
#endif
        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
#if ENABLE_DEBUG_PRINTS
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
#if ENABLE_DEBUG_PRINTS
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
#if ENABLE_DEBUG_PRINTS
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
    MQ8.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ8.setA(976.97);
    MQ8.setB(-0.688); // Configure the equation to calculate H2 concentration

    MQ8.init();

    if (PERFORM_MQ_CALIBRATION)
    {
#if ENABLE_DEBUG_PRINTS
        Serial.print("Calibrating MQ-8, please wait.");
#endif
        float calcR0 = 0;
        for (int i = 1; i <= 10; i++)
        {
            MQ8.update(); // Read the voltage from the analog pin
            calcR0 += MQ8.calibrate(MQ8_RATIO_CLEAN_AIR);
#if ENABLE_DEBUG_PRINTS
            Serial.print(".");
#endif
        }
        float mq8_r0 = calcR0 / 10;
        MQ8.setR0(mq8_r0);
#if ENABLE_DEBUG_PRINTS
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
        // Save R0 to EEPROM
        EEPROM.put(EEPROM_ADDRESS_MQ8_R0, mq8_r0);
    }
    else
    {
        // Load R0 from EEPROM
        float mq8_r0 = 0.0;
        EEPROM.get(EEPROM_ADDRESS_MQ8_R0, mq8_r0);
        MQ8.setR0(mq8_r0);
    }
    MQ8.serialDebug(ENABLE_DEBUG_PRINTS);

    // Initialize MQ-136 Sensor
    MQ136.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ136.setA(36.737);
    MQ136.setB(-3.536); // Configure the equation to calculate H2S concentration

    MQ136.init();

    if (PERFORM_MQ_CALIBRATION)
    {
#if ENABLE_DEBUG_PRINTS
        Serial.print("Calibrating MQ-136, please wait.");
#endif
        float calcR0 = 0;
        for (int i = 1; i <= 10; i++)
        {
            MQ136.update(); // Read the voltage from the analog pin
            calcR0 += MQ136.calibrate(MQ136_RATIO_CLEAN_AIR);
#if ENABLE_DEBUG_PRINTS
            Serial.print(".");
#endif
        }
        float mq136_r0 = calcR0 / 10;
        MQ136.setR0(mq136_r0);
#if ENABLE_DEBUG_PRINTS
        Serial.println(" done!");
#endif

        if (isinf(calcR0))
        {
            Serial.println("Warning: Connection issue, MQ-136 R0 is infinite (Open circuit detected). Please check your wiring and supply.");
            while (1)
                ;
        }
        if (calcR0 == 0)
        {
            Serial.println("Warning: Connection issue found, MQ-136 R0 is zero (Analog pin shorts to ground). Please check your wiring and supply.");
            while (1)
                ;
        }
        // Save R0 to EEPROM
        EEPROM.put(EEPROM_ADDRESS_MQ136_R0, mq136_r0);
    }
    else
    {
        // Load R0 from EEPROM
        float mq136_r0 = 0.0;
        EEPROM.get(EEPROM_ADDRESS_MQ136_R0, mq136_r0);
        MQ136.setR0(mq136_r0);
    }
    MQ136.serialDebug(ENABLE_DEBUG_PRINTS);

    // Initialize MQ-137 Sensor
    MQ137.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ137.setA(116.602);
    MQ137.setB(-2.769); // Configure the equation to calculate NH3 concentration

    MQ137.init();

    if (PERFORM_MQ_CALIBRATION)
    {
#if ENABLE_DEBUG_PRINTS
        Serial.print("Calibrating MQ-137, please wait.");
#endif
        float calcR0 = 0;
        for (int i = 1; i <= 10; i++)
        {
            MQ137.update(); // Read the voltage from the analog pin
            calcR0 += MQ137.calibrate(MQ137_RATIO_CLEAN_AIR);
#if ENABLE_DEBUG_PRINTS
            Serial.print(".");
#endif
        }
        float mq137_r0 = calcR0 / 10;
        MQ137.setR0(mq137_r0);
#if ENABLE_DEBUG_PRINTS
        Serial.println(" done!");
#endif

        if (isinf(calcR0))
        {
            Serial.println("Warning: Connection issue, MQ-137 R0 is infinite (Open circuit detected). Please check your wiring and supply.");
            while (1)
                ;
        }
        if (calcR0 == 0)
        {
            Serial.println("Warning: Connection issue found, MQ-137 R0 is zero (Analog pin shorts to ground). Please check your wiring and supply.");
            while (1)
                ;
        }
        // Save R0 to EEPROM
        EEPROM.put(EEPROM_ADDRESS_MQ137_R0, mq137_r0);
    }
    else
    {
        // Load R0 from EEPROM
        float mq137_r0 = 0.0;
        EEPROM.get(EEPROM_ADDRESS_MQ137_R0, mq137_r0);
        MQ137.setR0(mq137_r0);
    }
    MQ137.serialDebug(ENABLE_DEBUG_PRINTS);

    // Initialize MQ-9b Sensor
    MQ9B.setRegressionMethod(1); // _PPM =  a*ratio^b
    MQ9B.setA(4269.6); // Methane (CH4) model
    MQ9B.setB(-2.648); // Methane (CH4) model

    MQ9B.init();

    if (PERFORM_MQ_CALIBRATION)
    {
#if ENABLE_DEBUG_PRINTS
        Serial.print("Calibrating MQ-9b, please wait.");
#endif
        float calcR0 = 0;
        for (int i = 1; i <= 10; i++)
        {
            MQ9B.update(); // Read the voltage from the analog pin
            calcR0 += MQ9B.calibrate(MQ9B_RATIO_CLEAN_AIR);
#if ENABLE_DEBUG_PRINTS
            Serial.print(".");
#endif
        }
        float mq9b_r0 = calcR0 / 10;
        MQ9B.setR0(mq9b_r0);
#if ENABLE_DEBUG_PRINTS
        Serial.println(" done!");
#endif

        if (isinf(calcR0))
        {
            Serial.println("Warning: Connection issue, MQ-9b R0 is infinite (Open circuit detected). Please check your wiring and supply.");
            while (1)
                ;
        }
        if (calcR0 == 0)
        {
            Serial.println("Warning: Connection issue found, MQ-9b R0 is zero (Analog pin shorts to ground). Please check your wiring and supply.");
            while (1)
                ;
        }
        // Save R0 to EEPROM
        EEPROM.put(EEPROM_ADDRESS_MQ9B_R0, mq9b_r0);
    }
    else
    {
        // Load R0 from EEPROM
        float mq9b_r0 = 0.0;
        EEPROM.get(EEPROM_ADDRESS_MQ9B_R0, mq9b_r0);
        MQ9B.setR0(mq9b_r0);
    }
    MQ9B.serialDebug(ENABLE_DEBUG_PRINTS);

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Initialize LiquidMenu
    initializeLCDMenu();
    menu.init(); // Important: Initialize the menu after adding screens

    // Initialize Alarms
    initializeAlarms();

    // Set LED to green initially
    analogWrite(redPin, GREEN[0]);
    analogWrite(greenPin, GREEN[1]);
    analogWrite(bluePin, GREEN[2]);

    // Initialize Relay for MQ-9b (Start in HIGH state - 5V)
    digitalWrite(relayPin, HIGH); // HIGH corresponds to 5V phase

#if ENABLE_DEBUG_PRINTS
    Serial.println(F("System initialized."));
#endif
}

void loop()
{
    unsigned long currentMillis = millis();

    // Handle button press continuously
    handleButtonPress();

    // Relay control for MQ-9b
    controlMQ9bRelay();

    // Only read sensors every `SAMPLING_INTERVAL_MS`
    if (currentMillis - lastMillis >= SAMPLING_INTERVAL_MS)
    {
        lastMillis = currentMillis; // Use currentMillis instead of millis()

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
        if (currentMillis - alarmStartTime >= ALARM_DURATION_MS)
        {
            digitalWrite(buzzerPin, LOW);
            alarmActive = false;
        }
    }

    // Set LED color depending on alarmTriggered
    if (alarmTriggered)
    {
        // Set LED to red
        analogWrite(redPin, RED[0]);
        analogWrite(greenPin, RED[1]);
        analogWrite(bluePin, RED[2]);
    }
    else
    {
        // Set LED to green
        analogWrite(redPin, GREEN[0]);
        analogWrite(greenPin, GREEN[1]);
        analogWrite(bluePin, GREEN[2]);
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

    // Relay Pin for MQ-9b
    pinMode(relayPin, OUTPUT);

    // Ensure alarms and relay are in their initial states
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, HIGH); // Start in HIGH (5V) position
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
    screen8_line2.set_decimalPlaces(2); // H2S ppm
    screen9_line2.set_decimalPlaces(2); // NH3 ppm
    screen10_line2.set_decimalPlaces(2); // CH4 ppm
    screen11_line2.set_decimalPlaces(2); // CO ppm

    // Add screens to menu
    menu.add_screen(screen0);
    menu.add_screen(screen1);
    menu.add_screen(screen2);
    menu.add_screen(screen3);
    menu.add_screen(screen4);
    menu.add_screen(screen5);
    menu.add_screen(screen6);
    menu.add_screen(screen7);
    menu.add_screen(screen8);
    menu.add_screen(screen9);
    menu.add_screen(screen10);
    menu.add_screen(screen11);
}

void readSensors()
{
    if (!Rtc.IsDateTimeValid())
    {
#if ENABLE_DEBUG_PRINTS
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
#if ENABLE_DEBUG_PRINTS
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
#if ENABLE_DEBUG_PRINTS
        Serial.println(F("ENS160 not available."));
#endif
    }

    // Read MQ-8 Sensor
    MQ8.update();              // Read the voltage from the analog pin
    h2_ppm = MQ8.readSensor(); // Get the H2 concentration in ppm

    // Read MQ-136 Sensor
    MQ136.update();               // Read the voltage from the analog pin
    h2s_ppm = MQ136.readSensor(); // Get the H2S concentration in ppm

    // Read MQ-137 Sensor
    MQ137.update();               // Read the voltage from the analog pin
    nh3_ppm = MQ137.readSensor(); // Get the NH3 concentration in ppm

    // Read MQ-9b Sensor
    MQ9B.update(); // Read the voltage from the analog pin

    if (mq9bHighState)
    {
        // HIGH (5V) phase: measure CH4
        MQ9B.setA(4269.6);
        MQ9B.setB(-2.648);
        ch4_ppm = MQ9B.readSensor();
        co_ppm = 0.0; // Reset CO ppm during CH4 measurement
    }
    else
    {
        // LOW (1.5V) phase: measure CO
        MQ9B.setA(599.65);
        MQ9B.setB(-2.244);
        co_ppm = MQ9B.readSensor();
        ch4_ppm = 0.0; // Reset CH4 ppm during CO measurement
    }
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
                // LED color no longer changes on button press
            }
        }
    }

    lastButtonState = reading;
}

void checkAndTriggerAlarms()
{
    alarmTriggered = false; // Reset alarmTriggered at the start

    if (co2 > CO2_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High CO2 detected!"));
    }

    if (temperature > TEMP_THRESHOLD_C)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High Temperature detected!"));
    }

    if (humidity > HUMIDITY_THRESHOLD_RH)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High Humidity detected!"));
    }

    if (h2_ppm > H2_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High H2 detected!"));
    }

    if (h2s_ppm > H2S_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High H2S detected!"));
    }

    if (nh3_ppm > NH3_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High NH3 detected!"));
    }

    if (ch4_ppm > CH4_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High CH4 detected!"));
    }

    if (co_ppm > CO_THRESHOLD_PPM)
    {
        alarmTriggered = true;
        if (ENABLE_DEBUG_PRINTS)
            Serial.println(F("ALARM: High CO detected!"));
    }

    if (alarmTriggered)
    {
        if (!alarmActive) // Start the buzzer if not already active
        {
            alarmActive = true;
            alarmStartTime = millis();
            digitalWrite(buzzerPin, HIGH);
            if (ENABLE_DEBUG_PRINTS)
                Serial.println(F("Alarm activated!"));
        }
    }
}

void logSensorData()
{
    // Log to SD card in CSV format
    if (LOG_TO_SD)
    {
        dataFile = SD.open(LOG_FILE_NAME, FILE_WRITE);
        if (dataFile)
        {
            // Format: timestamp,co2,temperature,humidity,aqi,tvoc,eco2,h2_ppm,h2s_ppm,nh3_ppm,ch4_ppm,co_ppm
            dataFile.print(timestamp);
            dataFile.print(",");
            dataFile.print(co2);
            dataFile.print(",");
            dataFile.print(temperature, 2);
            dataFile.print(",");
            dataFile.print(humidity, 2);
            dataFile.print(",");
            dataFile.print(aqi);
            dataFile.print(",");
            dataFile.print(tvoc);
            dataFile.print(",");
            dataFile.print(eco2);
            dataFile.print(",");
            dataFile.print(h2_ppm, 2);
            dataFile.print(",");
            dataFile.print(h2s_ppm, 2);
            dataFile.print(",");
            dataFile.print(nh3_ppm, 2);
            dataFile.print(",");
            dataFile.print(ch4_ppm, 2);
            dataFile.print(",");
            dataFile.print(co_ppm, 2);
            dataFile.println();
            dataFile.close();
        }
        else
        {
#if ENABLE_DEBUG_PRINTS
            Serial.println(F("Error opening file for logging."));
#endif
        }
    }

    // Log to Serial
    if (ENABLE_DATA_PRINTS)
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
        Serial.print(F(", \"H2S\":"));
        Serial.print(h2s_ppm, 2);
        Serial.print(F(", \"NH3\":"));
        Serial.print(nh3_ppm, 2);
        Serial.print(F(", \"CH4\":"));
        Serial.print(ch4_ppm, 2);
        Serial.print(F(", \"CO\":"));
        Serial.print(co_ppm, 2);
        Serial.println(F("}"));
    }
}

void updateLCDMenu()
{
    menu.update();
}

void controlMQ9bRelay()
{
    unsigned long currentMillis = millis();
    if (currentMillis - mq9bPreviousMillis >= mq9bInterval)
    {
        // Toggle relay state
        mq9bHighState = !mq9bHighState;
        digitalWrite(relayPin, mq9bHighState ? HIGH : LOW);
#if ENABLE_DEBUG_PRINTS
        Serial.print("Relay Switched: ");
        Serial.println(mq9bHighState ? "HIGH (CH4 detection)" : "LOW (CO detection)");
#endif

        // Update timing interval
        mq9bInterval = mq9bHighState ? 60000 : 90000; // 60s HIGH, 90s LOW
        mq9bPreviousMillis = currentMillis;
    }
}
