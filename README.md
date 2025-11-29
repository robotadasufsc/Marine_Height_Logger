# 🌊 Marine Height Logger (LidarBox)

![Status](https://img.shields.io/badge/status-funcional-success)
![Platform](https://img.shields.io/badge/platform-Arduino_ProMicro-blue)
![License](https://img.shields.io/badge/license-EPL_2.0-red)

**Desenvolvido por:** Robota da UFSC

## 📖 Visão Geral
O **Marine Height Logger** é um instrumento de registro de dados para ambientes marítimos. Ele captura a variação de altura do nível d'água utilizando sensores LiDAR, correlacionando os dados com posicionamento GPS e compensação inercial (IMU).

Os dados são gravados em formato CSV no cartão SD para análise posterior, contendo carimbos de tempo, coordenadas e leituras dos sensores.

### Principais Funcionalidades
* **Datalogging Robusto:** Gravação de Data, Hora, GPS, LiDAR e IMU.
* **Flexibilidade de Hardware:** Suporte nativo para módulos Benewake TF02-Pro e Lightware SF11.
* **Feedback Visual:** Sistema de notificação de erros e status via LED.
* **Arquitetura Modular:** Drivers de sensores isolados para fácil manutenção.

---

## 🛠️ Hardware e Eletrônica

### 1. Lista de Componentes (BOM)
| Componente | Modelo | Função | Protocolo |
| :--- | :--- | :--- | :--- |
| **MCU** | Arduino ProMicro (ATmega32u4) | Controle | - |
| **IMU** | Pololu MinIMU-9 (LSM6) | Acelerômetro/Giroscópio | I²C |
| **LiDAR** | Benewake TF02-Pro *ou* SF11 | Distância | I²C |
| **GPS** | EM506 *ou* GT-735T | Posição/Tempo | Serial (UART) |
| **Armazenamento** | SparkFun SD Breakout | Logging | SPI |

### 2. Mapa de Conexões (Pinout)
Conexões baseadas no Arduino ProMicro / Leonardo:

**Barramento I²C (LiDAR + IMU)**
* **SDA:** Pino `D2`
* **SCL:** Pino `D3`
* *Nota:* O TF02 deve estar alimentado com 5V.

**GPS (Serial 1)**
* **RX (Módulo):** Conectar ao `TX0` do Arduino
* **TX (Módulo):** Conectar ao `RX1` do Arduino

**Cartão SD (SPI)**
* **CS:** Pino `D10`
* **MOSI:** Pino `D16`
* **MISO:** Pino `D14`
* **SCK:** Pino `D15`

### 3. Diagramas e PCB
> *Insira aqui uma imagem ou link para o esquemático elétrico*
> [📂 Ver arquivos de fabricação da PCB (Gerbers)](hardware/pcb)

---

## 🖨️ Mecânica e 3D
O projeto mecânico foi desenvolvido no OnShape para garantir estanqueidade e posicionamento correto dos sensores.

* **Arquivos Fonte:** [Acessar Projeto no OnShape](https://cad.onshape.com/documents/8c69aaf7bfe748ac84e2e23f/w/e7e4a977aaaffc7485234cd5/e/c591d70b899bdbf8e2ee1be5?renderMode=0&uiState=692b3cbd730f051df9b74f1f)
* **Impressão:** Recomenda-se PETG ou ABS.

---

## 💻 Estrutura do Firmware

O código é estruturado de forma modular em C++ (PlatformIO):

* **`src/main.cc`**: Loop principal. Gerencia a rotina `write_data_line` e tratamento de erros (`lock_and_report_error`).
* **LiDAR**:
    * Interface: `src/lidar/common.h`
    * Drivers: `src/lidar/benewake-tf02.cc` (Endereço 0x10) e `src/lidar/lightware-sf11-c.cc` (Endereço 0x55).
* **GPS**:
    * Implementa `TinyGPS++` através dos drivers em `src/gps/`.
* **IMU**:
    * Abstração do sensor LSM6 em `src/imu.cc`.

### Compilação e Upload
Este projeto utiliza **PlatformIO**.

1. Instale as dependências (definidas no `platformio.ini`).
2. Conecte a placa via USB.
3. Execute:
   ```bash
   pio run --target upload