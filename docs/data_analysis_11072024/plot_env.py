import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import os

# Parameters for easy adjustment
csv_file = os.path.join(os.path.dirname(__file__), "DATALOG_11072024.csv")
timestamp_col = "Timestamp"  # Column name for timestamp

# Manual axis limits for each environmental parameter
co2_ylim = (300, 500)  # Y-axis limits for CO2
temp_ylim = (5, 15)  # Y-axis limits for Temperature
humidity_ylim = (20, 50)  # Y-axis limits for Humidity
tvoc_ylim = (0, 500)  # Y-axis limits for TVOC

# Line properties for consistency in plot appearance
line_width = 1.2  # Line width for all plots
show_title = False  # Toggle to show or hide the title

# Colors for each environmental parameter plot
colors = {"CO2": "blue", "Temperature": "red", "Humidity": "green", "TVOC": "purple"}

# Load data from CSV and convert timestamp to datetime format
df = pd.read_csv(csv_file)
df[timestamp_col] = pd.to_datetime(df[timestamp_col])

# Calculate the average AQI and set the maximum AQI according to the ENS160 standard
avg_aqi = df["AQI"].mean()
max_aqi = 5  # Maximum possible AQI value for ENS160 standard

# Initialize the main plot figure and axis
fig, ax1 = plt.subplots(figsize=(13.5, 6))

# Plot each variable on separate y-axes with manual scaling and store lines and labels for the legend
lines, labels = [], []

# Plot CO2 with its own y-axis on the left
(line1,) = ax1.plot(
    df[timestamp_col],
    df["CO2_ppm"],
    label="CO2_ppm",
    color=colors["CO2"],
    linewidth=line_width,
)
ax1.set_ylabel("CO2_ppm", color=colors["CO2"])
ax1.tick_params(axis="y", labelcolor=colors["CO2"])
ax1.set_ylim(co2_ylim)
lines.append(line1)
labels.append("CO2_ppm")

# Plot Temperature with an additional y-axis on the right
ax2 = ax1.twinx()
(line2,) = ax2.plot(
    df[timestamp_col],
    df["Temperature_C"],
    label="Temperature_C",
    color=colors["Temperature"],
    linewidth=line_width,
)
ax2.set_ylabel("Temperature_C", color=colors["Temperature"])
ax2.tick_params(axis="y", labelcolor=colors["Temperature"])
ax2.set_ylim(temp_ylim)
lines.append(line2)
labels.append("Temperature_C")

# Plot Humidity with an additional offset y-axis
ax3 = ax1.twinx()
ax3.spines["right"].set_position(("outward", 60))
(line3,) = ax3.plot(
    df[timestamp_col],
    df["Humidity_%RH"],
    label="Humidity_%RH",
    color=colors["Humidity"],
    linewidth=line_width,
)
ax3.set_ylabel("Humidity_%RH", color=colors["Humidity"])
ax3.tick_params(axis="y", labelcolor=colors["Humidity"])
ax3.set_ylim(humidity_ylim)
lines.append(line3)
labels.append("Humidity_%RH")

# Plot TVOC with another offset y-axis
ax4 = ax1.twinx()
ax4.spines["right"].set_position(("outward", 120))
(line4,) = ax4.plot(
    df[timestamp_col],
    df["TVOC_ppb"],
    label="TVOC_ppb",
    color=colors["TVOC"],
    linewidth=line_width,
)
ax4.set_ylabel("TVOC_ppb", color=colors["TVOC"])
ax4.tick_params(axis="y", labelcolor=colors["TVOC"])
ax4.set_ylim(tvoc_ylim)
lines.append(line4)
labels.append("TVOC_ppb")

# Annotate average AQI information in the top right corner
ax1.annotate(
    f"Average AQI: {avg_aqi:.2f} / Max AQI: {max_aqi}",
    xy=(0.9, 0.9),
    xycoords="axes fraction",
    fontsize=10,
    color="orange",
    ha="right",
    va="top",
    bbox=dict(facecolor="white", edgecolor="orange", boxstyle="round,pad=0.3"),
)

# Place consolidated legend in the top-left corner of the plot, stacked vertically
fig.legend(
    lines, labels, loc="upper left", frameon=False, bbox_to_anchor=(0.1, 0.9), ncol=1
)

# Formatting for title, x-axis, and tick labels
if show_title:
    ax1.set_title("Environmental Data - CO2, Temperature, Humidity, and TVOC")
output_file_suffix = "" if show_title else "_notitle"
output_file = os.path.join(
    os.path.dirname(__file__), f"environment_data{output_file_suffix}.png"
)

ax1.xaxis.set_major_locator(
    mdates.HourLocator(interval=1)
)  # Hourly intervals for x-axis
ax1.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
plt.xticks(rotation=45, ha="right")  # Rotate x-axis labels for readability
fig.tight_layout()  # Adjust layout for clarity

# Save the plot as a PNG file
plt.savefig(output_file, dpi=300, bbox_inches="tight")
plt.close()
