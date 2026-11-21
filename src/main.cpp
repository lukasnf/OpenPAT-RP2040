#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads; 

// pin definitions
const int MOSFET_GATE = 26; // GPIO 26 for MOSFET gate
const int L_DRIVE = 27; // GPIO 27 for L drive
int Samples = 32;

// ADS pins definitions
const int ADS_L_T  = 2; // L_T
const int ADS_N_T  = 3; // N_T

// constants
const float PE_CURRENT_A = 0.195; // current in Amperes for PE TEST
const float R6 = 990.0; // R6 resistance in Ohms
const float RP2040_VCC = 3.224; // output voltage of the RP2040 (3.3V)

void measurePE();
void measureLN();
void setup() {
  Serial.begin(115200);
  
  while (!Serial) {
    delay(10);
  }
  
  delay(2000); 
  Serial.println("\n--- BOOTING OpenPAT-RP2040 ---");

  pinMode(MOSFET_GATE, OUTPUT);
  pinMode(L_DRIVE, OUTPUT);
  digitalWrite(MOSFET_GATE, LOW);
  digitalWrite(L_DRIVE, LOW);

  Wire1.setSDA(2); 
  Wire1.setSCL(3); 
  Wire1.begin();
  
  Serial.println("I2C1 booted");

  if (!ads.begin(0x48, &Wire1)) {
    while (1) {
      Serial.println("Error: ADS1115 not found!");
      delay(1000);
    }
  }

  ads.setGain(GAIN_ONE);
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
    else if (input == 'Q' || input == 'q') {
      digitalWrite(L_DRIVE, LOW);
      digitalWrite(MOSFET_GATE, LOW);
      Serial.println("\n Standby.");
    }
  }
}

void measurePE() {
  Serial.println("\n Starting PE-Test...");

  int32_t sum = 0.0;
  int64_t sumsq = 0.0;

  for(int i = 0; i < Samples ; i++ ){
    // differential measurement between ADS_PE_TEST and ADS_PE_TEST_RE
    int16_t adc0_1 = ads.readADC_Differential_0_1();
    
    sum += adc0_1;
    sumsq += (adc0_1 * adc0_1);
  }
  double mean_c = double(sum) / Samples;
  double variance = (sumsq - Samples * (mean_c * mean_c)) / (Samples - 1);
  if (variance < 0) {
    variance = 0;
  }
  double sigma_c = sqrt(variance);
  double sigma_m = sigma_c / sqrt(Samples);
  double lsb = ads.computeVolts(1);
  float mean_v = mean_c * lsb;
  float sigma_v = sigma_c * lsb;
  float sigma_m_v = sigma_m * lsb;


  float resistance_PE = fabs(mean_v) / PE_CURRENT_A;

  // output the results
  Serial.printf("U_PE  = %.3f mV +/- %.3f mV (sd = %.3f mV, N = %d)\n", mean_v * 1000, sigma_m_v * 1000, sigma_v * 1000, Samples);
  Serial.printf("R_PE  = %.4f Ohm\n", resistance_PE);
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

  // output the results
  Serial.print("Voltage at L_T: "); Serial.print(v_L, 3); Serial.println(" V");
  Serial.print("Voltage at N_T: "); Serial.print(v_N, 3); Serial.println(" V");
  
  if (resistance_LN >= 0) {
    Serial.print("L/N Resistance: "); Serial.print(resistance_LN, 1); Serial.println(" Ohm");
  } else {
    Serial.println("L/N Resistance: No continuity");
  }
}