/*
 * Funzioni e costanti per la gestione del MH-Z19B
 * Sensore CO2
 */

//Sensor constants

#define MHZ14A 14
#define  MHZ19B 19

#define MHZ14A_PREHEATING_TIME 3 * 60 * 1000L
//#define MHZ19B_PREHEATING_TIME 3 * 60 * 1000L
#define MHZ19B_PREHEATING_TIME 10 * 1000L  //SOLO PER I TEST

#define MHZ14A_RESPONSE_TIME 60 * 1000L
#define MHZ19B_RESPONSE_TIME 120 * 1000L

#define MHZ19_STATUS_NO_RESPONSE -2
#define MHZ19_STATUS_CHECKSUM_MISMATCH -3
#define MHZ19_STATUS_INCOMPLETE -4
#define MHZ19_STATUS_NOT_READY -5
#define MHZ19_STATUS_PWM_NOT_CONFIGURED -6
#define MHZ19_STATUS_SERIAL_NOT_CONFIGURED -7

//Max reading value: check the label on the sensor!
#define MHZ19_MAX_PPM_READING 5000

const int MHZType = MHZ19B;  //Modificare se cambia il tipo

byte MHZTemperature = 0;
unsigned long lastRequest = 0;

/**
 * Attende un tempo fisso di pre-riscaldamento
 */
boolean MHZPreHeating(void) {
  
  if (MHZType == MHZ14A) 
    return millis() < MHZ14A_PREHEATING_TIME;
  else if (MHZType == MHZ19B) 
    return millis() < MHZ19B_PREHEATING_TIME;
  else return false;
}

/**
 * Determina se l'MH-Z è pronto per l'accesso.
 * TRUE se non è in preheating e se è passato tempo sufficiente
 * dal precedente accesso
 */
boolean MHZIsReady() {

  //Se in preheating, non è pronto
  if (MHZPreHeating()) return false;

  //Se è la prima richiesta, è ready
  if (lastRequest == 0) return true;
  
  long elapsed = millis() - lastRequest;
  if (MHZType == MHZ14A)    
    return elapsed > MHZ14A_RESPONSE_TIME;
  else if (MHZType == MHZ19B)
    return elapsed > MHZ19B_RESPONSE_TIME;
  else return false;
}

/**
 * Inizializza l'MH-Z CO2 sensor
 */
void MHZSetup(void) {
  
  //Serial.println("MHZ19B");

  if (MHZPreHeating()) {
    Serial.print("MH-Z19 Pre-heating");
    while (MHZPreHeating()) {
      Serial.print(".");
      delay(5000);
    }
    Serial.println();
  }
  
}

/**
 * Inizializza l'MH-Z CO2 sensor con la funzione PWM
 */

void MHZSetupWithPWM(int pwm_pin) {
  
  pinMode(pwm_pin, INPUT);
  delay(100);

  MHZSetup();
}

/**
 * Verifica il Checksum della comunicazione ricevuta
 */
byte MHZGetCheckSum(byte* packet) {
  //Serial.println("  getCheckSum()");
  byte i;
  unsigned char checksum = 0;
  for (i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}

/**
 * Legge i dati di misurazione dalla UART
 * USA Serial1 in Arduino MKR
 */
int MHZReadCO2UART(void) {
  if (!MHZIsReady()) {
    Serial.println("Sensor not ready");
    return MHZ19_STATUS_NOT_READY;
  }
  Serial.println("Read CO2 from UART");
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  byte response[9];  // for answer

  Serial.print("Sending request to UART");
  Serial1.write(cmd, 9);  // request PPM CO2
  //Serial1.flush();
  lastRequest = millis();

  // clear the buffer
  memset(response, 0, 9);

  int waited = 0;
  while (Serial1.available() == 0) {
    Serial.print(".");
    delay(1000);  // wait a short moment to avoid false reading
    if (waited++ > 10) {
      Serial.println("No response after 10 seconds");
      //Serial1.flush();      
      return MHZ19_STATUS_NO_RESPONSE;
    }
  }
  Serial.println();

  // The serial stream can get out of sync. The response starts with 0xff, try
  // to resync.
  // TODO: I think this might be wrong any only happens during initialization?
  boolean skip = false;
  while (Serial1.available() > 0 && (unsigned char)Serial1.peek() != 0xFF) {
    if (!skip) {
      Serial.print("Skipping unexpected readings:");
      skip = true;
    }
    Serial.print(" ");
    Serial.print(Serial1.peek(), HEX);
    Serial1.read();
  }
  if (skip) Serial.println();

  if (Serial1.available() > 0) {
    int count = Serial1.readBytes(response, 9);
    if (count < 9) {
      //Serial1.flush();
      return MHZ19_STATUS_INCOMPLETE;
    }
  } else {
    //Serial1.flush();
    return MHZ19_STATUS_INCOMPLETE;
  }

  
  // print out the response in hexa
  Serial.print("  << ");
  for (int i = 0; i < 9; i++) {
    Serial.print(response[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
  
  // checksum
  byte check = MHZGetCheckSum(response);
  if (response[8] != check) {
    Serial.println("MHZ: Checksum not OK!");
    Serial.print("MHZ: Received: ");
    Serial.println(response[8], HEX);
    Serial.print("MHZ: Should be: ");
    Serial.println(check, HEX);
    //temperature = MHZ19_STATUS_CHECKSUM_MISMATCH;
    //Serial1.flush();
    return MHZ19_STATUS_CHECKSUM_MISMATCH;
  }

  int ppm_uart = 256 * (int)response[2] + response[3]; 

  MHZTemperature = response[4] - 44;  // - 40;

  byte status = response[5];
  
  //Serial.print(F(" # PPM UART: "));
  //Serial.println(ppm_uart);
  //Serial.print(F(" # Temperature: "));
  //Serial.println(MHZTemperature);

  // Is always 0 for version 14a  and 19b
  // Version 19a?: status != 0x40
  if (status != 0) {
    Serial.print("Status maybe not OK:");
    Serial.println(status, HEX);
  } else {
    Serial.print("Status  OK:");
    Serial.println(status, HEX);
  }

  //Serial1.flush();
  return ppm_uart;

}
