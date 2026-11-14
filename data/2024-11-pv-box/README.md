# PV-box logs, November 2024

Readings logged by the monitor inside the portable solar-powered station described
in the paper (see the [main README](../../README.md#citation)), with the scripts
that plot them.

| File | Logged | Rows |
| --- | --- | --- |
| `DATALOG_11072024.csv` | 2024-11-07 09:00 to 16:34 | 5,480 |
| `DATALOG1.CSV` | 2024-11-14 13:02 to 14:03 | 3,493 |
| `newData/DATALOG.CSV` | 2024-11-06 10:57 to 2024-11-14 13:01 | 20,884 |
| `newData/DATALOG1.CSV` | 2024-11-14 13:02 to 14:41 | 3,509 |
| `newData/DATALOG2.CSV` | 2024-11-14 14:42 to 23:55 | 6,287 |
| `newData/DATALOG3.CSV` | 2024-11-14 23:56 to 2024-11-17 21:26 | 50,209 |

Each row is one reading, every 5 s: timestamp, CO₂ (ppm), temperature (°C),
humidity (%RH), AQI, TVOC (ppb), eCO₂ (ppm), and H₂, H₂S, NH₃, CH₄ and CO (ppm).
The MQ-9B alternates between its CO and CH₄ heater phases, so each row has one of
those two and the other is empty.

## Plots

- `plot_env.py` plots CO₂, temperature, humidity and TVOC from
  `DATALOG_11072024.csv` into `environment_data.png`.
- `plot_gas_smoothed.py` plots the gases, smoothed with a rolling mean, from
  `DATALOG1.CSV` into `gas_concentrations_smoothed.png`.

Each script has a `show_title` switch; the `_notitle` images are the versions
without titles. Both need `pandas` and `matplotlib`.
