import streamlit as st
import pandas as pd
import pydeck as pdk
import folium
import json
from streamlit_folium import st_folium

st.set_page_config(page_title="LoRa Connectivity Map", layout="wide")

st.title("📡 LoRa Connectivity Map")
st.markdown("Upload your LoRaWAN Tracker's GPS log (.txt) from the onboard SD card and get an interactive map.")

# --- 1) Fast, cached parser ---
@st.cache_data
def load_data(file_bytes) -> pd.DataFrame:
    lines = file_bytes.readlines()
    entries = [json.loads(line.decode()) for line in lines]
    records = []
    for e in entries:
        lat = e.get("latitude")
        lon = e.get("longitude")
        if lat is None or lon is None:
            continue
        # connectivity & timestamp
        conn = "yes" if "true" in e else "no"
        y,mo,d = e.get("year",0), e.get("month",0), e.get("day",0)
        h,mi,s = e.get("hour",0), e.get("minute",0), e.get("second",0)
        ts = (
            f"{y:04d}-{mo:02d}-{d:02d} "
            f"{h:02d}:{mi:02d}:{s:02d}"
        ) if (y and mo and d) else "Unknown"
        records.append({
            "latitude": lat,
            "longitude": lon,
            "connected": conn,
            "timestamp": ts
        })
    df = pd.DataFrame(records)
    return df

uploaded = st.file_uploader("Upload TXT file", type="txt")

if uploaded:
    df = load_data(uploaded)   # cached parser from before
    if df.empty:
        st.warning("No valid points.")
        st.stop()
    st.success(f"Loaded {len(df)} points.")

    
    # 5️⃣ Prepare colors
    df["color"] = df["connected"].map({
        "yes": [0, 200, 0],
        "no":  [200, 0, 0]
    })

    df["radius"] = df["connected"].apply(lambda c: 15 if c == "yes" else 5)

    # 6️⃣ Initial camera view at the most recent measurement
    latest = df.iloc[-1]
    initial_view = pdk.ViewState(
        latitude  = latest["latitude"],
        longitude = latest["longitude"],
        zoom      = 15,
        pitch     = 0
    )

    # 7️⃣ Create the GPU‐accelerated scatter layer
    scatter = pdk.Layer(
    "ScatterplotLayer",
    data            = df,
    get_position    = "[longitude, latitude]",
    get_fill_color  = "color",
    get_radius      = "radius",   # ← now driven by df["radius"]
    pickable        = True,
    opacity         = 0.7
    )



    # -- Deck with tooltip showing connectivity + time --
    deck = pdk.Deck(
        layers            = [scatter],
        initial_view_state= initial_view,
        tooltip = {
            "html": "<b>Connected:</b> {connected} <br/> <b>Time:</b> {timestamp}",
            "style": {"backgroundColor": "steelblue", "color": "white"}
        }
    )

    st.pydeck_chart(deck)
else:
    st.info("Upload your TXT file to render 15 000+ points at GPU speeds.")