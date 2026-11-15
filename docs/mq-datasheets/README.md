# MQ Gas Sensors Overview

## Datasheets

The board uses Winsen's manuals for its four MQ sensors. Winsen's copyright notice
forbids redistributing them, so they are linked rather than stored here. Each link
is an Internet Archive snapshot that returns the original file; the SHA-256 lets you
check a copy from anywhere else. The same editions are pinned, with further
mirrors, in the
[MQGasSensors datasheet manifest](https://github.com/CameronBrooks11/MQGasSensors/blob/main/datasheets/manifest.toml).

| Sensor | Manual | SHA-256 |
| --- | --- | --- |
| MQ-8 | [Flammable Gas Sensor MQ-8, version 1.3](https://web.archive.org/web/20170811200808id_/https://cdn.sparkfun.com/datasheets/Sensors/Biometric/MQ-8%20Ver1.3%20-%20Manual.pdf) | `8124fae76c565ba830762bf39b5a29f5db274b301f900e730bbca730ab6e95d9` |
| MQ-9B | [Toxic Gas Sensor MQ-9B, version 1.4](https://web.archive.org/web/20201204150834id_/https://cdn.sparkfun.com/assets/d/f/5/e/2/MQ-9B_Ver1.4__-_Manual.pdf) | `fd23194f927fafdc4934ac2298ab5533a0630a1dc1cab756bc7bd2e29b8756b8` |
| MQ-136 | [Hydrogen Sulfide Gas Sensor MQ136, version 1.4](https://web.archive.org/web/20210927224428id_/https://www.winsen-sensor.com/d/files/PDF/Semiconductor%20Gas%20Sensor/MQ136%20(Ver1.4)%20-%20Manual.pdf) | `bed5d7b8e15965551bb796345a6b926ea3996cb72f2df3c54fd1723d5d627502` |
| MQ-137 | [Ammonia Gas Sensor MQ137, version 1.4](https://web.archive.org/web/20231207012210id_/https://www.winsen-sensor.com/d/files/PDF/Semiconductor%20Gas%20Sensor/MQ137%20(Ver1.4)%20-%20Manual.pdf) | `e376b682b2846915311ad364884fb4bc47b570d4fde9d2017322fe02e7076e34` |

MQ gas sensors, such as the MQ-8, are low-cost, widely used sensors that detect various gases by measuring the change in conductivity of a SnO2-sensitive material in response to gas concentration. This change in conductivity can be interpreted as a resistance change (Rs), which increases or decreases depending on the target gas.

## Sensor Operation and Calibration

1. **Structure and Operation**:
   - MQ sensors consist of a heating element and a SnO2-sensitive layer. The sensor requires two voltage inputs: a heater voltage (VH) to maintain the sensor's optimal temperature and a circuit voltage (VC) for measurement.
   - In clean air, the sensor's resistance (Rs) is at a baseline level, and the sensor outputs a corresponding voltage (VRL) across an external load resistor (RL). When exposed to a target gas, the conductivity increases, causing Rs to change based on the gas concentration.

2. **Calibration**:
   - Each sensor requires calibration to determine its baseline resistance, R0, which represents the resistance in clean air. Calibration steps:
     - Expose the sensor to clean air and measure Rs.
     - Calculate R0 using the clean air ratio, a predefined constant (e.g., MQ-8 clean air ratio is 70), as follows:
       R0 = Rs / Clean Air Ratio
   - This value of R0 is unique to each sensor and serves as a reference for future measurements.

3. **Using R0 for Gas Measurement**:
   - During operation, Rs is measured in the presence of the target gas, and the Rs/R0 ratio is calculated. This ratio can be used with a regression equation derived from the sensor’s datasheet to estimate gas concentration (PPM).
   - The general form of the regression equation is:
     PPM = A * (Rs / R0) ^ B
     where A and B are gas-specific constants provided by the datasheet or determined experimentally.

4. **Typical Sensitivity and Environment Effects**:
   - MQ sensors are sensitive to various gases, but their sensitivity varies depending on the gas. For example, the MQ-8 is primarily sensitive to hydrogen but also responds to gases like methane (CH4) and carbon monoxide (CO) to a lesser extent.
   - Temperature and humidity changes can affect Rs, so periodic recalibration or compensation may be necessary for consistent results, especially in variable environments.

## Important Parameters

- **Clean Air Ratio**: A predefined constant that indicates the expected Rs/R0 ratio in clean air.
- **R0**: The baseline sensor resistance in clean air, determined during calibration.
- **Rs**: The sensor resistance measured in the presence of a target gas.
- **A and B Constants**: Regression constants used to calculate gas concentration based on Rs/R0.