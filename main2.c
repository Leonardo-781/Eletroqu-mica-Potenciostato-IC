#include <Wire.h>
#include "LMP91000.h"


#define LOG_INFO 1
#define LOG_DEBUG 2
#define LOG_LEVEL LOG_INFO

// Configurações do LMP91000 e ADC do Arduino
const int ADC_PIN = A0;  // Pino ADC usado no Arduino
const double ADC_REF = 5.0;  // Referência de tensão do Arduino
const uint8_t ADC_BITS = 10;  // Resolução do ADC do Arduino

// Coeficientes de Calibração (pode ter q mudar)
const float A_COEFF = -146.63;
const float B_COEFF = 7.64;

// Configurações para Voltametria
const double VOLTAGE_STEP = 0.01;
const int DELAY_TIME = 10;
const double V_START = -0.6;
const double V_END = 0.6;
const int NUM_CYCLES = 10;
const double RESISTANCE = 10000.0; // Resistência usada para calcular corrente

// Classe para Gerenciamento do LMP91000
class LMP91000Controller {
public:
    LMP91000Controller(uint8_t menbPin) : lmp(), menbPin(menbPin) {}

    void setup() {
        lmp.setMENB(menbPin);
        lmp.setGain(LMP91000_TIA_GAIN_14K);
        lmp.setRLoad(LMP91000_RLOAD_100OHM);
        lmp.setIntRefSource();
        lmp.setIntZ(LMP91000_INT_Z_50PCT);
        lmp.setBias(0);
        lmp.setMode(LMP91000_OP_MODE_AMPEROMETRIC);
    }

    void setBias(double voltage) {
        // Como não temos DAC no Arduino, ajustamos o bias diretamente
        lmp.setBias(voltage * 100);
    }

private:
    LMP91000 lmp;
    uint8_t menbPin;
};

// Sistema de Logging
void logMessage(int level, const char* message) {
    if (level <= LOG_LEVEL) {
        Serial.println(message);
    }
}

// Implementação da Média Móvel Simples
double readVoltage(int numSamples = 10) {
    double sum = 0;
    for (int i = 0; i < numSamples; i++) {
        uint16_t adc_value = analogRead(ADC_PIN);
        double voltage = (adc_value * ADC_REF) / (pow(2, ADC_BITS) - 1);
        sum += A_COEFF + B_COEFF * voltage;
        delay(1);  // delay para estabilidade 
    }
    return sum / numSamples;
}

double readCurrent() {
    double voltage = readVoltage();
    return voltage / RESISTANCE; //  resposavel pelo calculo de corrente base na resistencia
}

// Função para Executar Varredura com Registro de Dados dereva fuciona #fe
void executeSweep(LMP91000Controller &controller, double voltage) {
    controller.setBias(voltage);
    delay(DELAY_TIME);

    double current = readCurrent();

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "V: %.2f V, I: %.6f A", voltage, current);
    logMessage(LOG_INFO, buffer);
}

// Funções de Voltametria
void cyclicVoltammetry(LMP91000Controller &controller) {
    logMessage(LOG_INFO, "Iniciando Voltametria Cíclica");
    for (double v = V_START; v <= V_END; v += VOLTAGE_STEP) {
        executeSweep(controller, v);
    }
    for (double v = V_END; v >= V_START; v -= VOLTAGE_STEP) {
        executeSweep(controller, v);
    }
}

void squareWaveVoltammetry(LMP91000Controller &controller) {
    logMessage(LOG_INFO, "Iniciando Voltametria de Onda Quadrada");
    double high_potential = 0.5;
    double low_potential = -0.5;

    for (int i = 0; i < NUM_CYCLES; i++) {
        for (double v = low_potential; v <= high_potential; v += VOLTAGE_STEP) {
            executeSweep(controller, v);
        }
        for (double v = high_potential; v >= low_potential; v -= VOLTAGE_STEP) {
            executeSweep(controller, v);
        }
    }
}

// Setup e Loop Principal
void setup() {
    Serial.begin(115200);
    Wire.begin();

    LMP91000Controller controller(2);  // Ajuste o pino conforme necessário
    controller.setup();
}

void loop() {
    LMP91000Controller controller(2);
    cyclicVoltammetry(controller);
    delay(5000);
    squareWaveVoltammetry(controller);
    delay(5000);
}
