# MQ sensor datasheets

The four MQ sensors on the board are documented in Winsen's manuals. Winsen's
copyright notice forbids redistributing them, so they are not stored here. The
[MQGasSensors datasheet manifest](https://github.com/CameronBrooks11/MQGasSensors/blob/main/datasheets/manifest.toml)
pins each edition by size and SHA-256, with an ordered list of mirrors: Internet
Archive snapshots first, then Software Heritage and live URLs. In a clone
of [MQGasSensors](https://github.com/CameronBrooks11/MQGasSensors), this command
downloads them and keeps only copies whose bytes match:

```text
just datasheets
```

| Sensor | Manifest id | SHA-256 |
| --- | --- | --- |
| MQ-8 | `mq-8-winsen-v1.3` | `8124fae76c565ba830762bf39b5a29f5db274b301f900e730bbca730ab6e95d9` |
| MQ-9B | `mq-9b-winsen-v1.4` | `fd23194f927fafdc4934ac2298ab5533a0630a1dc1cab756bc7bd2e29b8756b8` |
| MQ-136 | `mq-136-winsen-v1.4` | `bed5d7b8e15965551bb796345a6b926ea3996cb72f2df3c54fd1723d5d627502` |
| MQ-137 | `mq-137-winsen-v1.4` | `e376b682b2846915311ad364884fb4bc47b570d4fde9d2017322fe02e7076e34` |

## What the firmware computes

Calibration in clean air sets R0 = Rs / clean-air ratio, where Rs is the
sensor's resistance. The ratios, in `firmware/main/mqGas.h`, are 70 (MQ-8), 3.6
(MQ-136), 4.2 (MQ-137) and 9.6 (MQ-9B). Each reading is then converted with
ppm = a * (Rs / R0)^b, using the a and b for that gas in `mqGas.h`.

The Winsen manuals define R0 as the resistance in clean air: the
MQ-9B manual's Fig. 3 caption reads "R0 means resistance of sensor in clean
air". See [#1](https://github.com/uwo-fast/multi-gas-monitor/issues/1).
