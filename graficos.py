import serial
import matplotlib.pyplot as plt
import csv
import time

# Configuração da porta serial (ajuste conforme necessário)
ser = serial.Serial('COM15', 115200, timeout=1)  # Substitua 'COM15' pelo nome da porta apropriada
time.sleep(2)  # Aguardar inicialização da porta serial

# Inicializando variáveis
vc_data = []
sqwv_data = []
collecting_vc = False
collecting_sqwv = False

# Coletando dados
try:
    while True:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8').strip()
            if "Voltametria Ciclica:" in line:
                collecting_vc = True
                collecting_sqwv = False
                print("Iniciando coleta de Voltametria Cíclica")
                continue
            elif "Onda Quadrada:" in line:
                collecting_vc = False
                collecting_sqwv = True
                print("Iniciando coleta de Onda Quadrada")
                continue

            if collecting_vc or collecting_sqwv:
                try:
                    voltage, current = map(float, line.split(','))
                    if collecting_vc:
                        vc_data.append((voltage, current))
                    elif collecting_sqwv:
                        sqwv_data.append((voltage, current))
                except ValueError:
                    print(f"Erro ao processar a linha: {line}")
                    continue

            # Condição de saída: Limite de dados ou falta de novas leituras por tempo
            if len(vc_data) > 1000 or len(sqwv_data) > 1000:
                print("Limite de dados atingido")
                break

        # Verificação de timeout (opcional para encerrar coleta se não houver dados novos)
        if collecting_vc and not ser.in_waiting:
            print("Timeout na coleta de Voltametria Cíclica")
            break
        elif collecting_sqwv and not ser.in_waiting:
            print("Timeout na coleta de Onda Quadrada")
            break

finally:
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
if vc_data:
    vc_voltages = [v[0] for v in vc_data]
    vc_currents = [v[1] for v in vc_data]

    plt.figure()
    plt.plot(vc_voltages, vc_currents, label="Voltametria Cíclica")
    plt.xlabel('Voltage (V)')
    plt.ylabel('Current (A)')
    plt.title('Voltametria Cíclica')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('voltametria_ciclica.png')
    plt.show()
else:
    print("Nenhum dado coletado para Voltametria Cíclica.")

# Plotando Onda Quadrada
if sqwv_data:
    sqwv_voltages = [v[0] for v in sqwv_data]
    sqwv_currents = [v[1] for v in sqwv_data]

    plt.figure()
    plt.plot(sqwv_voltages, sqwv_currents, label="Onda Quadrada")
    plt.xlabel('Voltage (V)')
    plt.ylabel('Current (A)')
    plt.title('Onda Quadrada')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('onda_quadrada.png')
    plt.show()
else:
    print("Nenhum dado coletado para Onda Quadrada.")
