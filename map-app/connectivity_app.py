import streamlit as st
import pandas as pd
import json
import pydeck as pdk

st.set_page_config(page_title="LoRaWAN Connectivity Map", layout="wide")

st.title("📡 LoRaWAN Connectivity Map")
st.markdown("*Jack Long, 2025*")
st.markdown("Upload your LoRaWAN Tracker's GPS log (.txt) from the onboard SD card to view connectivity info.")
st.markdown("""
🔗 [View the source on GitHub](https://github.com/jack‑d‑long/lorawan-map)
""")

@st.cache_data
def load_data(file_bytes) -> pd.DataFrame:
    lines   = file_bytes.readlines()
    entries = [json.loads(line.decode("utf-8")) for line in lines]
    records = []
    for e in entries:
        lat = e.get("latitude")
        lon = e.get("longitude")
        if lat is None or lon is None:
            continue

        # connectivity: 1 ⇒ yes, else ⇒ no
        conn = "yes" if e.get("LoRaWANSend", 0) == 1 else "no"

        # timestamp
        y,mo,d = e.get("year",0), e.get("month",0), e.get("day",0)
        h,mi,s = e.get("hour",0), e.get("minute",0), e.get("second",0)
        ts = (f"{y:04d}-{mo:02d}-{d:02d} {h:02d}:{mi:02d}:{s:02d}"
              if (y and mo and d) else "Unknown")

        # scale PDOP
        raw_pdop = e.get("pdop")
        pdop     = raw_pdop / 100 if raw_pdop is not None else None

        records.append({
            "latitude": lat,
            "longitude": lon,
            "connected": conn,
            "timestamp": ts,
            "pdop": pdop
        })

    return pd.DataFrame(records)

uploaded = st.file_uploader("Upload TXT file", type="txt")
if not uploaded:
    st.info("Awaiting TXT upload…")
    st.stop()

df = load_data(uploaded).dropna(subset=["latitude","longitude"])
if df.empty:
    st.warning("No valid GPS points found.")
    st.stop()

st.success(f"Loaded {len(df):,} points.")

# add styling & display columns
df["color"]        = df["connected"].map({"yes":[0,200,0],"no":[200,0,0]})
df["radius"]       = df["connected"].apply(lambda c: 15 if c=="yes" else 5)
df["lat_display"]  = df["latitude"].round(6).astype(str)
df["lon_display"]  = df["longitude"].round(6).astype(str)
df["pdop_display"] = df["pdop"].apply(lambda x: f"{x:.2f}" if pd.notna(x) else "Unknown")

# center on most recent
latest = df.iloc[-1]
view   = pdk.ViewState(
    latitude  = latest["latitude"],
    longitude = latest["longitude"],
    zoom      = 15,
    pitch     = 0
)

# GPU‐accelerated scatterplot
scatter = pdk.Layer(
    "ScatterplotLayer",
    data            = df,
    get_position    = "[longitude, latitude]",
    get_fill_color  = "color",
    get_radius      = "radius",
    pickable        = True,
    opacity         = 0.7
)

# extended tooltip
tooltip = {
    "html": (
        "<b>Connected:</b> {connected} <br/>"
        "<b>Time:</b> {timestamp} <br/>"
        "<b>Lat:</b> {lat_display} <br/>"
        "<b>Lon:</b> {lon_display} <br/>"
        "<b>PDOP:</b> {pdop_display}"
    ),
    "style": {"backgroundColor":"steelblue","color":"white"}
}

deck = pdk.Deck(
    layers             = [scatter],
    initial_view_state = view,
    tooltip            = tooltip
)

st.pydeck_chart(deck)
