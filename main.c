#include <Wire.h>
#include "LMP91000.h"

LMP91000 lmp;  // Declaração única do objeto LMP91000

const int adc_pin = A0;  // Pino ADC do Arduino Nano
double adc_ref = 5.0;    // Tensão de referência do ADC do Arduino Nano (5V)
uint8_t adc_bits = 10;   // Resolução do ADC do Arduino Nano (10 bits)

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Configuração inicial do LMP91000
  lmp.setMENB(2);  // Pino de habilitação I2C (ajuste conforme necessário)
  lmp.setGain(LMP91000_TIA_GAIN_14K);  // Ajuste o ganho do TIA conforme necessário
  lmp.setRLoad(LMP91000_RLOAD_100OHM);  // Ajuste a resistência de carga conforme necessário
  lmp.setIntRefSource();  // Usar a fonte de referência interna
  lmp.setIntZ(LMP91000_INT_Z_50PCT);  // Ajuste a tensão de referência interna
  lmp.setBias(0);  // Sem polarização para início
  
  lmp.setMode(LMP91000_OP_MODE_AMPEROMETRIC);  // Modo amperométrico
}

void loop() {
  double voltage, current;
  uint16_t adc_value;

  // Voltametria Cíclica
  Serial.println("Voltametria Ciclica:");
  for (double v = -0.6; v <= 0.6; v += 0.01) {
    lmp.setBias(v * 100);
    delay(10);

    adc_value = analogRead(adc_pin);
    voltage = (adc_value * adc_ref) / (1 << adc_bits);
    current = (voltage / 1000.0);  // Calcular corrente baseado em tensão medida

    Serial.print(v);
    Serial.print(",");
    Serial.println(current);

    delay(10);
  }

  for (double v = 0.6; v >= -0.6; v -= 0.01) {
    lmp.setBias(v * 100);
    delay(10);

    adc_value = analogRead(adc_pin);
    voltage = (adc_value * adc_ref) / (1 << adc_bits);
    current = (voltage / 1000.0);  // Calcular corrente baseado em tensão medida

    Serial.print(v);
    Serial.print(",");
    Serial.println(current);

    delay(10);
  }

  delay(5000); // Pausa entre voltametria e onda quadrada

  // Onda Quadrada
  Serial.println("Onda Quadrada:");
  double high_potential = 0.5;
  double low_potential = -0.5;
  double step = 0.01;
  
  for (int i = 0; i < 10; i++) {  // 10 ciclos de onda quadrada
    for (double v = low_potential; v <= high_potential; v += step) {
      lmp.setBias(v * 100);
      delay(10);

      adc_value = analogRead(adc_pin);
      voltage = (adc_value * adc_ref) / (1 << adc_bits);
      current = (voltage / 1000.0);  // Calcular corrente baseado em tensão medida

      Serial.print(v);
      Serial.print(",");
      Serial.println(current);
    }

    for (double v = high_potential; v >= low_potential; v -= step) {
      lmp.setBias(v * 100);
      delay(10);

      adc_value = analogRead(adc_pin);
      voltage = (adc_value * adc_ref) / (1 << adc_bits);
      current = (voltage / 1000.0);  // Calcular corrente baseado em tensão medida

      Serial.print(v);
      Serial.print(",");
      Serial.println(current);
    }
  }

  delay(5000); // Pausa entre ciclos
}
