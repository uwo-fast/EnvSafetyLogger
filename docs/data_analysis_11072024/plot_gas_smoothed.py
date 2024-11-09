import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import os

# Parameters for easy adjustment
csv_file = os.path.join(os.path.dirname(__file__), "DATALOG_11072024.csv")
timestamp_col = "Timestamp"  # Column name for timestamp
rolling_window_size = 30  # Rolling window size for smoothing
show_title = False  # Toggle to show or hide the title

# Line properties for consistency in plot appearance
line_width = 1.2  # Line width for all plots

# Colors for each gas plot
colors = {"H2": "blue", "H2S": "red", "NH3": "green", "CH4": "purple", "CO": "orange"}

# Load data from CSV and convert timestamp to datetime format
df = pd.read_csv(csv_file)
df[timestamp_col] = pd.to_datetime(df[timestamp_col])

# Interpolate missing/NaN values in CH4_ppm and CO_ppm for continuity
df["CH4_ppm"] = df["CH4_ppm"].interpolate(method="linear")
df["CO_ppm"] = df["CO_ppm"].interpolate(method="linear")

# Apply a rolling mean to each gas concentration to smooth out the data
df["H2_ppm_smooth"] = df["H2_ppm"].rolling(window=rolling_window_size).mean()
df["H2S_ppm_smooth"] = df["H2S_ppm"].rolling(window=rolling_window_size).mean()
df["NH3_ppm_smooth"] = df["NH3_ppm"].rolling(window=rolling_window_size).mean()
df["CH4_ppm_smooth"] = df["CH4_ppm"].rolling(window=rolling_window_size).mean()
df["CO_ppm_smooth"] = df["CO_ppm"].rolling(window=rolling_window_size).mean()

# Create subplots with a broken y-axis to focus on specific ranges
fig, (ax1, ax2) = plt.subplots(
    2, 1, sharex=True, gridspec_kw={"height_ratios": [1, 1.3]}, figsize=(14, 8)
)

# Plot each gas on both axes to create a continuous view
for gas, color in colors.items():
    ax1.plot(
        df[timestamp_col],
        df[f"{gas}_ppm_smooth"],
        label=f"{gas}_ppm",
        color=color,
        linewidth=line_width,
    )
    ax2.plot(
        df[timestamp_col],
        df[f"{gas}_ppm_smooth"],
        label=f"{gas}_ppm",
        color=color,
        linewidth=line_width,
    )

# Set y-axis limits to create a "broken" effect
ax1.set_ylim(2.2, 4)  # Top subplot shows 2-4 range
ax2.set_ylim(0, 0.9)  # Bottom subplot shows 0-1 range

# Hide the spines between the broken axis
ax1.spines["bottom"].set_visible(False)
ax2.spines["top"].set_visible(False)

# Add diagonal lines to indicate the break in the y-axis
d = 0.008  # Size of diagonal lines for break indication
kwargs = dict(transform=fig.transFigure, color="k", clip_on=False)
ax1.plot(
    [0, 1],
    [0, 0],
    **kwargs,
    marker=[(1 - d, -d), (1, 0)],
    markersize=8,
    linestyle="None",
)
ax2.plot(
    [0, 1], [1, 1], **kwargs, marker=[(d, d), (0, 1)], markersize=8, linestyle="None"
)

# Place the legend in the top-left corner of the top plot
ax1.legend(loc="upper left", bbox_to_anchor=(0.0, 1.04), frameon=False, fontsize=16)

# Adjust title, labels, and ticks for clarity
if show_title:
    ax1.set_title(
        "Gas Concentrations - H2, H2S, NH3, CH4, CO (Smoothed with Rolling Mean, Broken Axis)",
        fontsize=16,
    )
output_file_suffix = "" if show_title else "_notitle"
output_file = os.path.join(
    os.path.dirname(__file__), f"gas_concentrations_smoothed{output_file_suffix}.png"
)

# Set y-axis labels with consistent font size for both axes
# ax1.set_ylabel("Gas Concentration (ppm)", fontsize=16)
ax2.set_ylabel("Gas Concentration (ppm)", fontsize=16)

# Ensure tick labels on both axes are consistent in size
ax1.tick_params(axis="y", labelsize=16)
ax2.tick_params(axis="y", labelsize=16)

ax2.xaxis.set_major_locator(
    mdates.HourLocator(interval=1)
)  # Hourly intervals for x-axis
ax2.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M"))
plt.xticks(rotation=45, ha="right", fontsize=16)  # Rotate x-axis labels for readability

# Save the plot as a PNG file
plt.savefig(output_file, dpi=300, bbox_inches="tight")
plt.close()
