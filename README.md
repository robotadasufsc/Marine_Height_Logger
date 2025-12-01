# 🌊 Marine Height Logger (LidarBox)

![Status](https://img.shields.io/badge/status-funcional-success)
![Platform](https://img.shields.io/badge/mcu-Arduino_ProMicro-blue)
![License](https://img.shields.io/badge/license-EPL_2.0-red)

**Desenvolvido por:** Robota da UFSC

## 📖 Visão Geral
O **Marine Height Logger** é um sistema embarcado de alta precisão para monitorar a variação de nível do mar. O dispositivo integra um **LiDAR** para medição de distância, **GPS** para geolocalização e tempo preciso, e um **IMU** para compensação de movimento (Tilt/Roll/Pitch).

Os dados são processados e salvos em um cartão SD em formato `.CSV` para fácil análise.

---

## 🛠️ Hardware e Eletrônica

### 1. Lista de Componentes (BOM)
| Componente | Modelo Específico | Função | Protocolo |
| :--- | :--- | :--- | :--- |
| **MCU** | Arduino ProMicro (ATmega32u4) | Controle Central | - |
| **IMU** | Pololu MinIMU-9 v5 (LSM6) | Acelerômetro/Giroscópio | I²C |
| **LiDAR** | Benewake TF02-Pro *ou* Lightware SF11 | Medição de Altura | I²C |
| **GPS** | GlobalSat EM-506 *ou* GP-735T | Posição e Relógio | Serial (UART) |
| **Storage** | SparkFun SD Breakout | Datalogging | SPI |

### 2. Pinagem (Conexões)
Baseado no firmware atual (`src/main.cc`):

| Módulo | Pino Módulo | Pino Arduino | Observação |
| :--- | :--- | :--- | :--- |
| **I²C Bus** | SDA / SCL | **D2 / D3** | LiDAR e IMU compartilham o bus |
| **GPS** | RX / TX | **TX0 / RX1** | Serial Hardware |
| **SD Card** | CS | **D10** | Chip Select |
| **SD Card** | MOSI | **D16** | SPI MOSI |
| **SD Card** | MISO | **D14** | SPI MISO |
| **SD Card** | SCK | **D15** | SPI Clock |

> [!WARNING]
> O **LiDAR TF02-Pro** vem de fábrica em modo Serial. Ele **deve** ser reconfigurado para I²C (endereço `0x10`) antes do uso.

> [📂 Ver Esquemático e PCB](hardware/circuit/) | [📦 Ver Modelos 3D do Case](hardware/3d_models/)

---

## 🚥 Tabela de Debug (Códigos de LED)
Como o dispositivo não possui tela, ele utiliza o LED interno (`LED_BUILTIN_RX`) para comunicar status e erros fatais.

### Inicialização
* **10 Piscadas Rápidas:** O sistema inicializou com sucesso, detectou todos os sensores e criou o arquivo de log. Está pronto para operar.

### Códigos de Erro (Loop Infinito)
Se o sistema encontrar uma falha crítica durante o boot, ele travará e piscará o LED repetidamente em sequências:

| Nº de Piscadas | Erro no Código | Significado / Solução |
| :---: | :--- | :--- |
| **1x** | `ERR_NO_LIDAR` | **LiDAR não encontrado.** Verifique cabos SDA/SCL e alimentação do sensor. Confirme se está em modo I²C. |
| **2x** | `ERR_NO_GPS_LOCK` | **Falha no GPS.** O sistema não conseguiu comunicação inicial com o módulo GPS. |
| **3x** | `ERR_IMU_FAIL` | **IMU não encontrado.** Verifique conexão com o Pololu MinIMU-9. |
| **4x** | `ERR_SD_FAIL` | **Cartão SD ausente/erro.** Verifique se o cartão está inserido e formatado corretamente. |
| **5x** | `ERR_SD_CREATE_FAIL` | **Erro de Arquivo.** O cartão foi lido, mas não foi possível criar o arquivo `LOG_XXXX.CSV` (cartão cheio ou protegido contra gravação). |

---

## 📊 Formato dos Dados
O arquivo gerado (`LOG_0000.CSV`) possui as seguintes colunas:
1. `gmt_date` (Data GPS)
2. `gmt_time` (Hora GPS)
3. `num_sats` (Nº Satélites)
4. `longitude` / `latitude`
5. `gps_altitude_m`
6. `SOG_kt` (Velocidade em nós)
7. `COG` (Curso sobre o solo)
8. `HDOP` (Precisão horizontal)
9. `laser_altitude_cm` (Leitura do LiDAR)
10. `tilt_deg` (Inclinação)
11. `accel_x/y/z` (Acelerômetro bruto)
12. `gyro_x/y/z` (Giroscópio bruto)

---

## 💻 Instalação do Firmware
Recomendado utilizar **PlatformIO**. As dependências principais são:
* `SD`
* `TinyGPSPlus`
* `LSM6` (Pololu)

```bash
# Clone o repositório e faça o upload
git clone [https://github.com/robotadasufsc/Marine_Height_Logger.git](https://github.com/robotadasufsc/Marine_Height_Logger.git)
cd Marine_Height_Logger
pio run --target upload