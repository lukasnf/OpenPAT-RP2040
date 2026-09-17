#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// Erstelle ein ADS1115 Objekt
Adafruit_ADS1115 ads; 

// pin definitions
const int MOSFET_GATE = 26; // GPIO 26 for MOSFET gate
const int L_DRIVE = 27; // GPIO 27 for L drive

// ADS pins definitions
const int ADS_PE_HIGH = 0; // PE_TEST_RE
const int ADS_PE_LOW  = 1; // PE_TEST
const int ADS_L_T     = 2; // L_T
const int ADS_N_T     = 3; // N_T

// constants
const float PE_CURRENT_A = 0.2016; // current in Amperes for PE TEST
const float R6 = 1000.0; // R6 resistance in Ohms
const float RP2040_VCC = 3.3;       // output voltage of the RP2040 (3.3V)

void setup() {
  Serial.begin(115200);

  pinMode(MOSFET_GATE, OUTPUT);
  pinMode(L_DRIVE, OUTPUT);
  
  // Base state: turn off MOSFET and L drive
  digitalWrite(MOSFET_GATE, LOW);
  digitalWrite(L_DRIVE, LOW);

  // Initialize the ADS1115
  if (!ads.begin()) {
    Serial.println("Error: ADS1115 not found.");
    while (1);
  }

  ads.setGain(GAIN_ONE); // Set gain to 1 for ±4.096V range

  Serial.println("System ready. Press 'p' for PE-Test, 'l' for L/N-Test");
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    
    if (input == 'P' || input == 'p') {
      measurePE();
    } 
    else if (input == 'L' || input == 'l') {
      measureLN();
    }
  }
}

void measurePE() {
  Serial.println("\n Starting PE-Test...");

   // differential measurement between ADS_PE_TEST and ADS_PE_TEST_RE
  int16_t adc0_1 = ads.readADC_Differential_0_1();
  
  // calculate the voltage (1 Bit = .125 mV)
  float voltage_PE = (adc0_1 * 0.125) / 1000.0;

  // calculating the resistance of PE
  float resistance_PE = voltage_PE / PE_CURRENT_A;

  // output the results
  Serial.print("Voltaged Drop: "); Serial.print(voltage_PE, 4); Serial.println(" V");
  Serial.print("PE Resistance:   "); Serial.print(resistance_PE, 3); Serial.println(" Ohm");
}


void measureLN() {
  Serial.println("\n Starting L/N-Test...");

  // turn on Mosfet
  digitalWrite(MOSFET_GATE, HIGH);

  // apply voltage to L (via R6)
  digitalWrite(L_DRIVE, HIGH);
  delay(10);

  // measure voltage across L_T and N_T
  int16_t adcL = ads.readADC_SingleEnded(ADS_L_T);
  int16_t adcN = ads.readADC_SingleEnded(ADS_N_T);

  // convert raw ADC values to Volts
  float v_L = (adcL * 0.125) / 1000.0;
  float v_N = (adcN * 0.125) / 1000.0;

  // calculate voltage drop across the DUT
  float voltage_DUT = v_L - v_N;

  // calculate the current through the DUT 
  float current_DUT = (RP2040_VCC - v_L) / R6;

  // calculate the resistance of the DUT
  float resistance_LN = -1; // -1 as default value
  if (current_DUT > 0.0001) { // Prevent division by zero if no load is connected
    resistance_LN = voltage_DUT / current_DUT;
  }

  // Turn everything off 
  digitalWrite(L_DRIVE, LOW);
  digitalWrite(MOSFET_GATE, LOW);

  Serial.print("Voltage at L_T: "); Serial.print(v_L, 3); Serial.println(" V");
  Serial.print("Voltage at N_T: "); Serial.print(v_N, 3); Serial.println(" V");
  
  if (resistance_LN >= 0) {
    Serial.print("L/N Resistance: "); Serial.print(resistance_LN, 1); Serial.println(" Ohm");
  } else {
    Serial.println("L/N Resistance: No continuity");
  }
}