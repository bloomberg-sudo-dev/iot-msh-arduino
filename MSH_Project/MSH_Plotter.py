import pandas as pd
import matplotlib.pyplot as plt
import gspread
from oauth2client.service_account import ServiceAccountCredentials
from matplotlib.dates import DateFormatter

# 1. Authenticate and Authorize access to Google Sheets
scope = [
    'https://spreadsheets.google.com/feeds',
    'https://www.googleapis.com/auth/drive'
]

# Replace 'msh-project-446918-79d9805ac71d.json' with the actual path to your service account JSON file
credentials = ServiceAccountCredentials.from_json_keyfile_name('', scope)
client = gspread.authorize(credentials)

# 2. Open the Google Sheet by URL
sheet_url = ''
sheet = client.open_by_url(sheet_url)

# 3. Get the first worksheet
worksheet = sheet.sheet1

# 4. Get all values from the worksheet as a list of lists
data = worksheet.get_all_values()

# 5. Convert the data to a Pandas DataFrame
df = pd.DataFrame(data[1:], columns=data[0])  # Use the first row as column headers, skip it for data

# 6. Parse the "Time" column into datetime objects
if "Time" in df.columns:
    df["Time"] = pd.to_datetime(df["Time"], format="%Y-%m-%d %H:%M:%S", errors='coerce')  # Handle invalid formats gracefully

# Drop rows with NaN in the "Time" column (if any)
df = df.dropna(subset=["Time"])

# Convert other columns to numeric, handling non-numeric values
for column in df.columns[1:]:  # Skip "Time" column
    # Replace commas with dots in string representations
    df[column] = df[column].replace(',', '.', regex=True)  # [1][2][5]
    # Convert to numeric, handling remaining invalid values
    df[column] = pd.to_numeric(df[column], errors='coerce')  # [1][4][7]

# 7. Plotting scatter plots with time on x-axis
plt.figure(figsize=(12, 8))

for column in df.columns[1:]:  # Skip "Time" column for y-axis data
    plt.scatter(df["Time"], df[column], label=column)

# Format the x-axis to show readable dates/times
date_format = DateFormatter("%Y-%m-%d %H:%M:%S")
plt.gca().xaxis.set_major_formatter(date_format)
plt.gcf().autofmt_xdate()  # Automatically rotate date labels for better readability

# Add labels, title, and legend
plt.xlabel("Time")
plt.ylabel("Spectral Intensity")
plt.title("Spectral Data Over Time")
plt.legend(loc='upper right')
plt.grid(True)
plt.show()
