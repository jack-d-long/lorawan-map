import streamlit as st
import pandas as pd
import json
import pydeck as pdk

st.set_page_config(page_title="LoRa Connectivity Map", layout="wide")

st.title("📡 LoRa Connectivity Map")
st.markdown("Upload your LoRaWAN Tracker's GPS log (.txt) and get an interactive map.")

# --- 1) Fast, cached parser ---
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
        conn_flag = e.get("LoRaWANSend", 0)
        conn      = "yes" if conn_flag == 1 else "no"

        # build timestamp
        y, mo, d = e.get("year",0), e.get("month",0), e.get("day",0)
        h, mi, s = e.get("hour",0), e.get("minute",0), e.get("second",0)
        if y and mo and d:
            ts = f"{y:04d}-{mo:02d}-{d:02d} {h:02d}:{mi:02d}:{s:02d}"
        else:
            ts = "Unknown"

        records.append({
            "latitude": lat,
            "longitude": lon,
            "connected": conn,
            "timestamp": ts
        })

    return pd.DataFrame(records)

# --- upload step ---
uploaded = st.file_uploader("Upload TXT file", type="txt")
if not uploaded:
    st.info("Awaiting Tracker TXT upload. Insert your Tracker's SD card, then drag and drop its contents here.")
    st.stop()

# --- load & clean ---
df = load_data(uploaded)
df = df.dropna(subset=["latitude","longitude"])
if df.empty:
    st.warning("No valid GPS points found.")
    st.stop()

st.success(f"Loaded {len(df):,} points.")

# --- color & radius columns ---
df["color"]  = df["connected"].map({"yes":[0,200,0], "no":[200,0,0]})
df["radius"] = df["connected"].apply(lambda c: 15 if c=="yes" else 5)

# --- center on most recent measurement ---
latest = df.iloc[-1]
view   = pdk.ViewState(
    latitude  = latest["latitude"],
    longitude = latest["longitude"],
    zoom      = 15,
    pitch     = 0
)

# --- GPU‐accelerated scatter layer ---
scatter = pdk.Layer(
    "ScatterplotLayer",
    data            = df,
    get_position    = "[longitude, latitude]",
    get_fill_color  = "color",
    get_radius      = "radius",
    pickable        = True,
    opacity         = 0.7
)

# --- assemble and render Deck.gl map ---
deck = pdk.Deck(
    layers             = [scatter],
    initial_view_state = view,
    tooltip = {
        "html": "<b>Connected:</b> {connected} <br/><b>Time:</b> {timestamp}",
        "style": {"backgroundColor":"steelblue","color":"white"}
    }
)

st.pydeck_chart(deck)
