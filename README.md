# 🌊 Marine Height Logger (LidarBox)

![Status](https://img.shields.io/badge/status-funcional-success)
![Platform](https://img.shields.io/badge/framework-PlatformIO-orange)
![MCU](https://img.shields.io/badge/hardware-Arduino_ProMicro-blue)
![License](https://img.shields.io/badge/license-EPL_2.0-lightgrey)

**Desenvolvido por:** Robota da UFSC

## 📖 Visão Geral

O **Marine Height Logger** é um instrumento oceanográfico de baixo custo projetado para registrar a variação de altura da superfície da água (ondas e marés). O sistema utiliza tecnologia **LiDAR** para medir a distância até a água, correlacionando esses dados com posicionamento global (**GPS**) e dados inerciais (**IMU**) para compensar o movimento da boia ou embarcação (Heave/Pitch/Roll).

O firmware foi otimizado para alta performance de escrita, utilizando um **buffer de saída** para salvar dados em lotes no cartão SD, minimizando a latência e evitando gargalos de I/O.

---

## 🛠️ Hardware e Eletrônica

### 1. Lista de Componentes (BOM)

| Componente | Modelo | Função | Protocolo |
| :--- | :--- | :--- | :--- |
| **MCU** | Arduino ProMicro (ATmega32u4) | Processamento Central | - |
| **LiDAR** | Benewake TF02-Pro *ou* SF11 | Distância (Altura) | I²C (Endereço `0x10`) |
| **GPS** | GlobalSat EM-506 | Tempo e Posição | Serial (UART) |
| **IMU** | Pololu MinIMU-9 v5 (LSM6) | Acelerômetro/Giroscópio | I²C |
| **Storage** | SparkFun SD Breakout | Datalogging | SPI |
| **Debug** | LED Genérico | Status Visual | Digital (Pino 5) |

> [!WARNING]
> **Atenção com o LiDAR TF02-Pro:** Este sensor vem de fábrica configurado para Serial. É **obrigatório** reconfigurá-lo para I²C (endereço `0x10`) utilizando o software do fabricante antes da montagem final.

### 2. Pinagem (Conexões)

Baseado na versão atual do firmware (`src/main.cc`):

| Periférico | Pinos Módulo | Pino Arduino | Observação |
| :--- | :--- | :--- | :--- |
| **I²C Bus** | SDA / SCL | **D2 / D3** | LiDAR e IMU compartilham este barramento |
| **GPS** | RX / TX | **TX0 / RX1** | Serial Hardware |
| **SD Card** | CS | **D10** | Chip Select |
| **SD Card** | MOSI | **D16** | SPI MOSI |
| **SD Card** | MISO | **D14** | SPI MISO |
| **SD Card** | SCK | **D15** | SPI Clock |
| **LED Debug** | Anodo (+) | **D5** | LED externo de status |

---

## 🚥 Tabela de Diagnóstico (LEDs)

O sistema não possui tela (headless). O status é comunicado através do **LED Externo (Pino 5)** e do **LED RX interno**, que piscam em sincronia.

### ✅ Status Normal
* **10 Piscadas Rápidas:** Boot concluído com sucesso. Todos os sensores foram detectados e o arquivo de log foi criado.
* **Aceso/Piscando durante operação:** Indica atividade de escrita no buffer ou no cartão SD.

### ❌ Códigos de Erro (Loop Infinito)
Se o sistema falhar durante a inicialização, ele entrará em um loop infinito, piscando o código de erro repetidamente (ciclos de 300ms).

| Nº Piscadas | Código Interno | Significado | Solução Provável |
| :---: | :--- | :--- | :--- |
| **2x** | `ERR_NO_LIDAR` | **LiDAR não encontrado** | Verifique cabos I²C (D2/D3) e alimentação (5V). Confirme o endereço `0x10`. |
| **3x** | `ERR_NO_GPS_LOCK` | **Falha de GPS** | O GPS não respondeu aos comandos iniciais ou baud rate incorreto. |
| **4x** | `ERR_IMU_FAIL` | **Falha no IMU** | O acelerômetro (LSM6) não foi detectado no barramento I²C. |
| **5x** | `ERR_SD_FAIL` | **Falha Física no SD** | Cartão não inserido, mal contatado ou formato inválido. |
| **6x** | `ERR_SD_CREATE_FAIL`| **Erro de Arquivo** | Cartão detectado, mas não foi possível criar o arquivo `LOG_xxxx.CSV` (Cartão cheio?). |

---

## 💾 Detalhes do Firmware e Dados

### Arquivos de Log
O sistema cria arquivos sequenciais na raiz do cartão:
* `LOG_0000.CSV`
* `LOG_0001.CSV`
* ...

### Estrutura do CSV
O arquivo contém as seguintes colunas separadas por tabulação ou vírgula:
1. `gmt_date` / `gmt_time` (Data/Hora GPS)
2. `num_sats` (Qualidade do sinal)
3. `longitude` / `latitude` / `altitude`
4. `laser_altitude_cm` (Leitura bruta do LiDAR)
5. `tilt_deg` (Inclinação calculada)
6. `accel_x/y/z` (Dados brutos do acelerômetro)
7. `gyro_x/y/z` (Dados brutos do giroscópio)

### Segurança de Dados (Buffer)
Para economizar tempo de processamento, o sistema mantém os dados em RAM e realiza a escrita física no cartão SD (**flush**) apenas a cada **20 linhas de medição**.

> [!CAUTION]
> **Ao desligar o equipamento:** Após a última atividade intensa do LED, aguarde cerca de 3 a 5 segundos antes de remover a energia. Isso garante que o último bloco de 20 linhas foi salvo corretamente no cartão, evitando corrupção do arquivo.

### Resiliência (Watchdog I²C)
O código implementa `Wire.setWireTimeout(3000, true)`. Isso impede que o microcontrolador trave completamente caso um sensor I²C (como o IMU ou LiDAR) pare de responder ou desconecte durante a operação.

---

## 🚀 Como Compilar e Enviar

Este projeto utiliza **PlatformIO**.

1. **Clone o repositório:**
   ```bash
   git clone [https://github.com/seu-usuario/Marine_Height_Logger.git](https://github.com/seu-usuario/Marine_Height_Logger.git)
   
2. **Instale as dependências (automático via platformio.ini):**

* `SD`
* `TinyGPSPlus`
* `LSM6 (Pololu)`

3. **Compile e faça o upload:**
    ```Bash
    pio run --target upload

---

##📝 Licença

Este projeto está licenciado sob a EPL-2.0.