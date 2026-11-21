# OpenPAT-RP2040

> **⚠️ SAFETY DISCLAIMER:** This project is for educational and preliminary diagnostic purposes only. While it tests electrical safety parameters, it is **not** a certified Portable Appliance Tester (PAT). It does not replace legally required safety tests conducted with calibrated and certified equipment. Always assume appliances are dangerous until proven otherwise.

**OpenPAT-RP2040** is a low-voltage Portable Appliance Tester based on the RP2040 microcontroller. It is designed to safely measure Protective Earth (PE) continuity at a standard test current of 200 mA and to verify the Phase-to-Neutral (L-N) resistance of **unpowered** appliances.

Operating completely without high measurement voltages, this design allows for safe preliminary testing of appliances.

> **Note:** This project is under active development. Accuracy measurements and calibration data are coming soon. The schematics and PCB layout were designed in KiCad and manufactured via JLCPCB.

---

## 🛠 Hardware Architecture

* **Microcontroller:** RP2040 (RP2040 Zero board)
* **Analog-to-Digital Converter (ADC):** ADS1115, connected via I2C (`I2C_SCL`, `I2C_SDA`), reads the measurement nodes `PE_TEST`, `PE_TEST_RE`, `L_T` and `N_T`.
* **PE Test Current Source:** A constant current source of approximately 200 mA, built from an LM317 linear regulator (U1) and a 6.2 Ω / 1 W resistor (R1).
* **PE Continuity Measurement:** A **4-wire (Kelvin) measurement** eliminates the voltage drop across the test leads and contacts.
  * Force: `PE_FORCE_IN` → appliance PE conductor → `PE_OUT1` pin 2 (GND)
  * Sense: `PE_SENSE_1` (`PE_TEST_RE`) and `PE_OUT1` pin 1 (`PE_TEST`)
* **L-N Resistance Measurement:** Controlled via RP2040 GPIOs. GP27 drives the L terminal through R6 (1 kΩ), and an NMOS transistor (Q1, controlled by GP26) connects N to ground during the measurement.
* **Input Protection:** All analog measurement lines are protected with 10 kΩ series resistors (R2–R5) and BAT54S dual Schottky diodes (D1–D4), which clamp the ADC inputs to the 3.3 V rail and ground.

## 🔩 Components

| Component | Value / Type |
| :--- | :--- |
| Microcontroller | RP2040 Zero |
| ADC | ADS1115 |
| Voltage regulator | LM317 |
| N-channel MOSFET | IRLML2502 (20 V, 4.2 A, SOT-23) |
| Schottky diode | BAT54S-7-F (30 V, 0.2 A, SOT-23) |
| Capacitor | 100 nF (50 V, X7R, RM 2.5) |
| Terminal block | MKDS 1.5/2-5.08 (2-pin, RM 5.08) |
| Current-setting resistor (R1) | 6.2 Ω (1 W) |
| Standard resistors | 10 kΩ, 1 kΩ |

## 📐 Schematic & PCB

<img width="631" height="490" alt="Schematic" src="https://github.com/user-attachments/assets/ea2fb498-5255-4306-849e-52d807fd6b38" />

<img width="1818" height="1118" alt="PCB layout" src="https://github.com/user-attachments/assets/3fd09da4-3e47-48c3-98b1-30cde7f4cd65" />

## ⚙️ How It Works

### PE continuity (4-wire measurement)

The LM317 always keeps 1.25 V between its OUT and ADJ pins. With R1 = 6.2 Ω between them, the current is fixed at

I = 1.25 V / 6.2 Ω ≈ 200 mA

independent of the load (measured: 195 mA due to component tolerances). This current flows through the appliance's protective earth conductor. Two separate sense leads measure the voltage drop directly at the conductor. Since almost no current flows through the sense leads, their resistance and the contact resistances do not affect the result. The ADS1115 measures the voltage differentially in a single conversion:

R_PE = U_sense / I

In the ±4.096 V range, one ADC step is 125 µV, which corresponds to about 0.64 mΩ.

### L-N resistance (voltage divider)

GP27 drives 3.3 V through R6 (1 kΩ) into L. Q1 (controlled by GP26) connects N to ground only during the measurement. The ADC measures the voltages at L and N:

I = (U_GP27 − U_L) / R6

R_LN = (U_L − U_N) / I

Measuring U_N removes the on-resistance of Q1 from the result.

### Input protection

Every measurement line has a 10 kΩ series resistor and a BAT54S dual Schottky diode. If a line rises above about 3.6 V or falls below about −0.3 V, the diodes conduct to 3.3 V or ground, and the excess voltage drops across the resistor. This protects the ADC against small overvoltages and residual charge in the appliance.

## ⚠️ Limitations

- **Not mains-safe:** The device is designed for disconnected appliances only. `PE_FORCE_IN`, the L drive path (GP27 via 1 kΩ) and the drain of Q1 (20 V max) are not protected against external voltages.
- **Limited compliance voltage:** With a 5 V USB supply, the LM317 dropout (≈ 1.5–2 V) and the 1.25 V reference leave about 1.75 V for the appliance and leads. Above about 8 Ω in total, the current source can no longer hold 200 mA.
- **PE current is always on:** The current source cannot be switched off by the firmware.
- **Fixed constants:** The test current, R6 and the GPIO voltage are stored as measured constants; drift is not detected.
- **Single readings:** The firmware currently takes one ADC reading per measurement (no averaging).

## 🗺️ Roadmap

- [ ] Calibration against reference resistors and uncertainty analysis
- [ ] Averaging with standard deviation output
- [ ] Voltage check before each test (abort if the appliance is powered)
- [ ] Detection when the current source is out of regulation
- [ ] Hardware rev. 2: TVS diodes, reverse-protection diode for the LM317, switchable PE current source

## 📄 License

This repository contains both hardware design files and software source code, which are licensed differently:

* **Hardware:** All hardware design files (schematics, PCB layouts) are released under the [CERN Open Hardware Licence Version 2 – Strongly Reciprocal (CERN-OHL-S v2)](LICENSE).
* **Software:** All source code is released under the [MIT License](LICENSE-MIT).

---

This README was drafted with AI assistance. All technical information has been reviewed by the author.

Lukas F.
