# LoRaWAN Tracker
*Modified Spring 2025 by Jack Long for MAE 4220: IoT, Cornell University*
*Originally by Alex Coy, ECE 4950 TA for Spring 2021*

A one‑stop repository for everything you need to build, deploy, and visualize data from our GPS LoRaWAN tracker:

- **Map App** (`map‑app/`)  
  A lightweight web application for ingesting the raw `.txt` logs off the tracker’s onboard SD card and rendering an interactive, real‑time map of your device’s journey.

- **Tracker Hardware** (`tracker‑hardware/`)  
  - **Shell** (`tracker‑hardware/shell_CAD/`)  
    Fully parametric OpenSCAD/STEP files and detailed drawings for 3D‑printing or CNC fabrication of the tracker enclosure.  
  - **Firmware** (`tracker‑hardware/firmware/`)  
    PlatformIO‑compatible C/C++ code for the microcontroller: SD‑card logger, GNSS reader, LoRaWAN uplink, and low‑power sleep modes with GUI.
  - **Purple Board** (`tracker‑hardware/firmware/`)
    KiCAD-compatible files for manufacture and assembly of the EERL 'Purple Board' PCB by Alex Coy, current as of May 2025. 
---

## Repository Structure

```plaintext
lorawan-map/
├── map-app/                   # Web application
│   ├── connectivity_app.py    # Streamlit entrypoint
│   ├── requirements.txt       # Python dependencies
│   ├── templates/             # HTML templates
│
├── tracker-hardware/          
│   ├── shell_CAD/                   # OpenSCAD & STEP enclosure files
│   ├── purple_board/                # EERL purple board docs
│   └── firmware/              
│       ├── feather_sd_sam-m8q-/     # C/C++ source with all proprietary headers
│
├── .gitignore                 
└── README.md                  # ← you are here

```

## The LoRaWAN Tracker 


###  Features

- **Battery‑aware operation**: real‑time voltage monitoring and on‑screen battery bar  
- **Dual scan modes** (“Drive” vs “Walk”) with adaptive transmit intervals (15 s vs 60 s)  
- **Non‑blocking UI controls**: five‑button D‑pad menu navigation with hardware‑debounced inputs  
- **Persistent storage**: 32 kB FRAM for network session state + SD card for full JSON logging  
- **Plug‑and‑play web front‑end**: drag‑and‑drop `.txt` logs to visualize GPS tracks, timestamps and sensor graphs  

---

###  Hardware Requirements

- **Microcontroller**: Adafruit Feather M0 LoRa ([RFM95, 915 MHz](https://www.adafruit.com/product/3178))  
- **"Purple Board"**: EERL PCB (source included)
- **Display**: [SH1106 128×64 I²C OLED](https://www.aliexpress.us/item/3256806846801359.html?src=google&pdp_npi=4%40dis%21USD%216.32%213.11%21%21%21%21%21%40%2112000039161970280%21ppc%21%21%21&src=google&albch=shopping&acnt=708-803-3821&isdl=y&slnk=&plac=&mtctp=&albbt=Google_7_shopping&aff_platform=google&aff_short_key=UneMJZVf&gclsrc=aw.ds&albagn=888888&ds_e_adid=&ds_e_matchtype=&ds_e_device=c&ds_e_network=x&ds_e_product_group_id=&ds_e_product_id=en3256806846801359&ds_e_product_merchant_id=5388525816&ds_e_product_country=US&ds_e_product_language=en&ds_e_product_channel=online&ds_e_product_store_id=&ds_url_v=2&albcp=20123152476&albag=&isSmbAutoCall=false&needSmbHouyi=false&gad_source=1&gad_campaignid=20127768206&gbraid=0AAAAAD6I-hE0CYYoGWDb82JdLxJdh6HZj&gclid=Cj0KCQjwrPHABhCIARIsAFW2XBMoG-_e8-ALzsrb52M93WnFTvrb6fn9T6qEIRYuo83KG5EKEKTS-N8aAq3UEALw_wcB&gatewayAdapt=glo2usa)
- **GPS**: u‑blox SAM‑M8Q module ([Sparkfun breakout](https://www.sparkfun.com/sparkfun-gps-breakout-chip-antenna-sam-m8q-qwiic.html))  
- **Storage**: [MicroSD card breakout](https://www.amazon.com/UMLIFE-Interface-Conversion-Compatible-Raspberry/dp/B0989SM146) + MCCI Catena cFram32k FRAM (optional)
- **Controls**: [PCA9554 I²C D‑Pad](https://www.sparkfun.com/sparkfun-qwiic-directional-pad.html) + [12mm latching pushbutton](https://www.amazon.com/Gebildet-Latcting-Lockable-Trumpet-Doorbell/dp/B07YDCJFZJ?source=ps-sl-shoppingads-lpcontext&ref_=fplfs&psc=1&smid=A86AD06S3OMVQ&gQT=0)
- **Power**: 3.7 V 1S/3C LiPo battery (800-1000 mAh)   
- **Enclosure**: 3D‑printed shell (source included)  

---

### Software Requirements

- **Arduino IDE**   
- Libraries:  
  - `Arduino_LoRaWAN_ttn` + `lmic`  
  - `Adafruit GFX` & `Adafruit SH110X`  
  - `SparkFun u‑blox GNSS` & `TinyGPS`  
  - `SparkFun I2C Expander`  
  - `MCCI Catena FrAM`  
- **Node.js** (v14+) & **npm** for the web app  
- Web dependencies: `express`, `react`, `leaflet`, `chart.js`

---

## Usage

**1. Clone the repo**
```
git clone https://github.com/jack-d-long/lorawan-map.git
cd lorawan-map
```

**2. Build the tracker**
- Purchase components, print shell, assemble. 
- Flash firmware with Arduino IDE

**3. Use the tracker**
- Collect data on LoRaWAN connectivity in your local area

**4. Set up the map app**
- Create and activate a Python environment 

```
python3 -m venv venv
source venv/bin/activate

```

- Install dependencies

```
pip install --upgrade pip
pip install \
  streamlit \
  pandas \
  pydeck \
  jsonschema  # optional: validate JSON if you extend
```

**5. Read your stored data card with the map app**
- Run the app

**6. Read your data on the cloud via TTN**

- Set up TTN by following Labs 3, 4, 5, and 6 of [this tutorial](https://pages.github.coecis.cornell.edu/LPWAN-Training/training-labs/lab3), or equivalent
- Pipe your data via MQTT to the database of your choice for live retrieval 



```
streamlit run connectivity_app.py

```
- In your browser, go to http://localhost:8501
- Upload your GPS/LoRaWAN log file (.txt newline‑delimited JSON).
- Watch the map render instantly—and pan, zoom or hover for details.