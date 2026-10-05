# OpenPAT-RP2040

> **⚠️ SAFETY DISCLAIMER:** This project is for educational and preliminary diagnostic purposes only. While it tests electrical safety parameters, it is **not** a certified Portable Appliance Tester (PAT). It does not replace legally required safety tests conducted with calibrated and certified equipment. Always assume appliances are dangerous until proven otherwise.

**OpenPAT-RP2040** is a low-voltage Portable Appliance Tester based on the RP2040 microcontroller. It is designed to safely measure Protective Earth (PE) continuity at standard test currents (200mA) and verify Phase-to-Neutral (L-N) resistance on **unpowered** appliances. 

Operating completely without high measurement voltages, this design allows for safe preliminary testing of appliances. 

> **Note:** This project is currently under active development. The schematics and PCB layout have been designed using KiCad and manufactured via JLCPCB.

---

## 🛠 Hardware Architecture

The measurement circuitry is designed around the RP2040 and relies on a dedicated analog frontend for precise, low-voltage readings. Based on the schematic, the core components and sub-circuits include:

* **Microcontroller:** RP2040
* **Analog-to-Digital Converter (ADC):** ADS1115 connected via I2C (`I2C_SCL`, `I2C_SDA`) to read the measurement nodes (`PE_TEST`, `PE_TEST_RE`, `L_T`, `N_T`).
* **PE Test Current Source:** A constant current source generating approximately 200mA. This is achieved using an LM317 linear regulator (U1) combined with a 6.2Ω / 1W resistor (R1).
* **PE Continuity Measurement:** Employs a **4-wire (Kelvin) measurement method** to accurately determine resistance by eliminating the voltage drop across the test leads. This is implemented using separate connections for force (`PE_FORCE_IN`, `PE_OUT1`) and sense (`PE_SENSE_1`, which connects to the protected `PE_TEST_RE` node).
* **L-N Resistance Measurement:** Controlled via RP2040 GPIOs. An NMOS transistor (Q1, controlled by GP26) and a switched pull-up (via GP27 and a 1kΩ resistor) are used to configure the circuit to measure the resistance between the Phase and Neutral test points via the L/N_TEST1 connector.
* **Input Protection:** All sensitive analog measurement lines are protected against transients. The circuit uses 10kΩ series resistors (R2, R3, R4, R5) and BAT54S dual Schottky diodes (D1, D2, D3, D4) to safely clamp the ADC inputs to the 3.3V rail and Ground.

## 📂 Repository Contents
<img width="631" height="490" alt="Bildschirmfoto 2026-10-05 um 15 35 09" src="https://github.com/user-attachments/assets/ea2fb498-5255-4306-849e-52d807fd6b38" />

<img width="1732" height="1044" alt="image" src="https://github.com/user-attachments/assets/b9f0d478-d94d-4992-8590-68096dfa7ec6" />

...

## 📄 License

This repository contains both hardware design files and software source code, which are licensed differently:

* **Hardware:** All hardware design files (schematics, PCB layouts) are released under the [CERN Open Hardware Licence Version 2 - Strongly Reciprocal (CERN-OHL-S v2)](LICENSE).
* **Software:** All source code is released under the [MIT License](LICENSE-MIT).


This README was drafted with AI assistance. All technical information has been manually verified by the author.

Lukas F.
