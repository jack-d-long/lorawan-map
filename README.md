# LoRaWAN Tracker
*Modified Spring 2025 by Jack Long, Cornell University — MAE 4220: IoT.*

A one‑stop repository for everything you need to build, deploy, and visualize data from our GPS LoRaWAN tracker:

- **Map App** (`map‑app/`)  
  A lightweight web application for ingesting the raw `.txt` logs off the tracker’s onboard SD card and rendering an interactive, real‑time map of your device’s journey.

- **Tracker Hardware** (`tracker‑hardware/`)  
  - **CAD** (`tracker‑hardware/cad/`)  
    Fully parametric OpenSCAD/STEP files and detailed drawings for 3D‑printing or CNC fabrication of the tracker enclosure.  
  - **Firmware** (`tracker‑hardware/firmware/`)  
    PlatformIO‑compatible C/C++ code for the microcontroller: SD‑card logger, GNSS reader, LoRaWAN uplink, and low‑power sleep modes with GUI.

---

## 📂 Repository Structure

```plaintext
lorawan-map/
├── map-app/                   # Web application
│   ├── connectivity_app.py    #  entrypoint
│   ├── requirements.txt       # Python dependencies
│   ├── templates/             # HTML/Jinja templates
│
├── tracker-hardware/          
│   ├── cad/                   # OpenSCAD & STEP enclosure files
│   └── firmware/              
│       ├── feather_sd_sam-m8q-/               # C/C++ source with all proprietary headers
│
├── .gitignore                 
└── README.md                  # ← you are here
