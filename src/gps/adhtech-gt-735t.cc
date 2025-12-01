#ifdef GPS_ADHTECH_GT_735T

#include "./common.h"
#include "../debug.h"

TinyGPSPlus gps;

// Função auxiliar para enviar comandos UBX de forma segura
// Evita problemas com caracteres nulos (\x00) dentro de strings
void sendUBX(const uint8_t *progmem_command, size_t len) {
    for (size_t i = 0; i < len; i++) {
        Serial1.write(pgm_read_byte(progmem_command + i));
    }
    Serial1.flush(); // Garante o envio antes de continuar
}

// Definição dos comandos em PROGMEM para economizar RAM
const uint8_t DISABLE_GLL[] PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x05,0x3A};
const uint8_t DISABLE_GSA[] PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x01,0x00,0x01,0x01,0x01,0x01,0x06,0x41};
const uint8_t DISABLE_GSV[] PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x01,0x00,0x01,0x01,0x01,0x01,0x07,0x48};
const uint8_t DISABLE_VTG[] PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x01,0x00,0x01,0x01,0x01,0x01,0x09,0x56};
const uint8_t DISABLE_ZDA[] PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x5B};
const uint8_t ENABLE_GGA[]  PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x05,0x38};
const uint8_t ENABLE_RMC[]  PROGMEM = {0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x01,0x01,0x01,0x01,0x01,0x01,0x09,0x54};
const uint8_t SAVE_CONFIG[] PROGMEM = {0xB5,0x62,0x06,0x09,0x0C,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x80};

// Forward declaration
void wakeful_delay(unsigned long ms);
void consume_gps();

bool setup_gps() {
    // GPS is on the ProMicro's UART (Serial1)
    // RX: pin 0; TX: pin 1
    Serial1.begin(9600);
    
    // CORREÇÃO: Limpar buffer inicial
    delay(10);
    while(Serial1.available()) Serial1.read();

    // Verificação real de hardware: Espera até receber pelo menos 1 byte do GPS
    // Isso confirma que o GPS está ligado e transmitindo
    unsigned long start_wait = millis();
    bool gps_detected = false;
    
    DEBUGLN(F("Aguardando dados do GPS..."));
    
    while (millis() - start_wait < 5000) { // Espera até 5 segundos por dados
        if (Serial1.available()) {
            gps_detected = true;
            break;
        }
    }

    if (!gps_detected) {
		return false;
        DEBUGLN(F("ALERTA: GPS nao enviou dados nos primeiros 5s (Verifique cabos RX/TX)"));
        // Não retornamos false aqui para tentar configurar mesmo assim, 
        // mas é um forte indicativo de problema elétrico.
    }

    // Configuração do GPS
    // Usamos sendUBX e wakeful_delay para não travar o buffer

    sendUBX(DISABLE_GLL, 16);
    wakeful_delay(100); // CORREÇÃO: wakeful_delay em vez de delay

    sendUBX(DISABLE_GSA, 16);
    wakeful_delay(100);

    sendUBX(DISABLE_GSV, 16);
    wakeful_delay(100);

    sendUBX(DISABLE_VTG, 16);
    wakeful_delay(100);

    sendUBX(DISABLE_ZDA, 16);
    wakeful_delay(100);

    sendUBX(ENABLE_GGA, 16);
    wakeful_delay(100);

    sendUBX(ENABLE_RMC, 16);
    wakeful_delay(100);
    
    // Save configuration
    sendUBX(SAVE_CONFIG, 20);
    wakeful_delay(250);

    return true;
}

void wakeful_delay(unsigned long ms) {
    unsigned long limit = millis() + ms;
    do {
        consume_gps();
    } while(millis() < limit);
}

void consume_gps() {
    // CORREÇÃO: Leitura byte a byte é mais eficiente e segura que buffer de array
    while(Serial1.available() > 0) {
        char c = Serial1.read();
        gps.encode(c);
        
        #ifdef DEBUG_NMEA
        DEBUG_STREAM.write(c);
        #endif
    }
}

#endif