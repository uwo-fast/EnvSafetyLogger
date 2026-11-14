fqbn := "arduino:megaavr:nona4809"

default:
    @just --list

# Install the pinned Nano Every core and the firmware's libraries with arduino-cli (CI runs this)
setup:
    arduino-cli core update-index
    arduino-cli core install arduino:megaavr@1.8.8
    arduino-cli lib update-index
    arduino-cli lib install "Rtc by Makuna@2.5.0" "SparkFun SCD4x Arduino Library@1.1.2" "ENS160 - Adafruit Fork@3.0.1" "MQUnifiedsensor@3.0.0" "LiquidMenu@1.6.3" "LiquidCrystal I2C@1.1.2" "SD@1.3.0"

# LiquidMenu defaults to the parallel LCD library; the firmware drives an I2C LCD.
# These flags select LiquidCrystal_I2C for the whole build, sketch and libraries
# alike, in place of editing LiquidMenu_config.h.
liquidmenu_i2c := "-DLIQUIDMENU_LIBRARY=LiquidCrystal_I2C_LIBRARY -DDisplayClass=LiquidCrystal_I2C"

# Compile the firmware for the Arduino Nano Every
check:
    arduino-cli compile --fqbn {{fqbn}} --warnings default --build-property "compiler.cpp.extra_flags={{liquidmenu_i2c}}" firmware/main
