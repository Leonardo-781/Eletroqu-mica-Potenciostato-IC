import serial
import matplotlib.pyplot as plt
import csv
import time

# Configuração da porta serial (ajuste conforme necessário)
ser = serial.Serial('COM3', 115200)  # Substitua 'COM3' pelo nome da porta apropriada
time.sleep(2)  # Aguardar inicialização da porta serial

# Inicializando variáveis
vc_data = []
sqwv_data = []
collecting_vc = False
collecting_sqwv = False

# Coletando dados
while True:
    if ser.in_waiting:
        line = ser.readline().decode('utf-8').strip()
        if "Voltametria Ciclica:" in line:
            collecting_vc = True
            collecting_sqwv = False
            continue
        elif "Onda Quadrada:" in line:
            collecting_vc = False
            collecting_sqwv = True
            continue
        
        if collecting_vc or collecting_sqwv:
            try:
                voltage, current = map(float, line.split(','))
                if collecting_vc:
                    vc_data.append((voltage, current))
                elif collecting_sqwv:
                    sqwv_data.append((voltage, current))
            except ValueError:
                continue

        if len(vc_data) > 1000 or len(sqwv_data) > 1000:  # Limita a quantidade de dados
            break

# Desconecta da porta serial
ser.close()

# Salvando dados em CSV
with open('voltametria_ciclica.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['Voltage (V)', 'Current (A)'])
    writer.writerows(vc_data)

with open('onda_quadrada.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['Voltage (V)', 'Current (A)'])
    writer.writerows(sqwv_data)

# Plotando Voltametria Cíclica
vc_voltages = [v[0] for v in vc_data]
vc_currents = [v[1] for v in vc_data]

plt.figure()
plt.plot(vc_voltages, vc_currents)
plt.xlabel('Voltage (V)')
plt.ylabel('Current (A)')
plt.title('Voltametria Cíclica')
plt.grid(True)
plt.savefig('voltametria_ciclica.png')
plt.show()

# Plotando Onda Quadrada
sqwv_voltages = [v[0] for v in sqwv_data]
sqwv_currents = [v[1] for v in sqwv_data]

plt.figure()
plt.plot(sqwv_voltages, sqwv_currents)
plt.xlabel('Voltage (V)')
plt.ylabel('Current (A)')
plt.title('Onda Quadrada')
plt.grid(True)
plt.savefig('onda_quadrada.png')
plt.show()
