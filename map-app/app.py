from flask import Flask, render_template
import folium
import pandas as pd

app = Flask(__name__)

@app.route('/')
def index():
    # Sample Data (replace with your input)
    data = {
        "latitude": [42.4440, 42.4455, 42.4461, 42.4422],
        "longitude": [-76.5019, -76.5033, -76.4995, -76.5007],
        "connected": ["yes", "no", "yes", "no"],
        "timestamp": [
            "2025-04-25 08:30:00",
            "2025-04-25 08:31:00",
            "2025-04-25 08:32:00",
            "2025-04-25 08:33:00"
        ]
    }
    df = pd.DataFrame(data)

    # Create a map
    m = folium.Map(location=[df["latitude"].mean(), df["longitude"].mean()], zoom_start=15)

    for _, row in df.iterrows():
        status = row["connected"].lower()
        color = "green" if status == "yes" else "red"
        icon = "ok" if status == "yes" else "remove"
        
        popup_html = f"""
        <b>Connected:</b> {row['connected']}<br>
        <b>Time:</b> {row['timestamp']}
        """
        
        folium.Marker(
            location=[row["latitude"], row["longitude"]],
            popup=folium.Popup(popup_html, max_width=300),
            icon=folium.Icon(color=color, icon=icon, prefix="glyphicon")
        ).add_to(m)

    # Save as HTML inside templates folder
    m.save("templates/map.html")

    return render_template('map.html')

if __name__ == "__main__":
    app.run(debug=True)
