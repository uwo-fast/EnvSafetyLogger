# MQ Gas Sensors Overview

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