# 📋 Guia de Operação em Campo: Marine Height Logger

Este documento contém instruções rápidas para operação, diagnóstico e segurança dos dados durante a utilização do equipamento em campo.

## 1. Checklist Pré-Operação
Antes de lacrar a caixa ou lançar o equipamento:

- [ ] **Cartão SD:** Inserido corretamente e formatado (FAT16/FAT32).
- [ ] **LiDAR:** Lentes limpas (sem sal, poeira ou obstruções).
- [ ] **Parafusos:** Verificar se todos os parafusos e velcro estão firmes.
- [ ] **Bateria:** Carga verificada.

---

## 2. Procedimento de Inicialização

1. **Ligar:** Alterne o botão para ligado.
2. **Aguardar:** O sistema leva alguns segundos para energizar o GPS e sensores.
3. **Confirmação Visual (LED):**Desconecte
   - O sistema deve emitir **10 piscadas rápidas**.
   - **Significado:** Boot OK + Sensores Detectados + Arquivo `.CSV` criado com sucesso.
4. **Aguardando GPS:** O LED piscará a cada 5 segundos esperando a conexão com os satelites, poderá levar de 30 segundos a 3 minutos. passado esse periodo reinicie e tente novamente. para o fix com os satélites tente deixá-lo em um local aberto e com o sensor gps lateral virado para cima.
5. **Operação:** O LED passará a piscar periodicamente(a cada 1 segundo), indicando que o buffer está enchendo e os dados estão sendo processados.

---

## 3. Procedimento de Desligamento (⚠️ CRÍTICO)

> **Risco de Perda de Dados**
> O sistema utiliza um *buffer* de escrita e só salva fisicamente no cartão SD a cada **20 segundos**. Cortar a energia abruptamente pode corromper o arquivo atual.

1. Observe o LED de status (externo).
2. Após uma sequência de piscadas (atividade de escrita).
3. Alterne o botão para desligado com segurança.

---

## 🆘 Diagnóstico de Erros (LED em Loop)

Se o LED entrar em um padrão repetitivo (pisca X vezes, pausa, repete), consulte a tabela abaixo:

| Piscadas | Componente Falho | Ação Corretiva |
| :---: | :--- | :--- |
| **2x** | **LiDAR** | Sensor não detectado. Verifique cabos SDA/SCL e alimentação. Confirme se está no endereço `0x10`. |
| **3x** | **GPS** | GPS não responde. Verifique cabos TX/RX ou se o módulo está queimado. |
| **4x** | **IMU** | Acelerômetro travado. Verifique conexão I²C com o Pololu MinIMU-9. |
| **5x** | **Cartão SD** | Cartão ausente ou mal encaixado no slot. |
| **6x** | **Erro de Arquivo** | Falha ao criar arquivo `LOG_xxxx.CSV`. Cartão pode estar cheio, protegido (trava física) ou corrompido. |

Tentativas de reparo:
* Ligue e desligue o equipamemto algumas vezes dando um intervalo de 1 segundo mínimo entre cada desligameto.
* Tente iniciar o equipamento sem o cartao SD, deixe entrar no loop de erro, desligue, reinsira o cartão e ligue novamente.
* Tente outras combinações desses métodos. 

---

## 📝 Notas Técnicas
* **Arquivos:** O sistema gera nomes sequenciais (`LOG_0000.CSV`, `LOG_0001.CSV`...). Ele **nunca** sobrescreve arquivos antigos.
* **Sinal de GPS:** Em dias nublados ou locais obstruídos, o GPS pode demorar para obter o *Fix*. O sistema gravará `NaN` ou `INVALID` nas colunas de GPS, mas continuará registrando dados do LiDAR e IMU normalmente.
* **Watchdog:** Se um sensor I²C travar durante a operação, o sistema tentará reiniciar o barramento automaticamente após 3 segundos (timeout), evitando o congelamento total do logger.