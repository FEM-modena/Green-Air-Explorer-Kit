/* 
  Green Air Explorer Kit
  Future Education Modena 2022

  Monitora: 
  - Umidità /Temperatura (Shield MKR Env)
  - Umidità  del terreno (Grove Sensore Conduzione o Capacitivo analogico)
  - Anidride Carbonica (MH-Z19B) //Da modificare
  - Particolato (Grove Dust Sensor)
*/

///// NON SPOSTARE QUESTE DEFINIZIONI  /////
//*******************************************
#define SECRET_SSID "FEM_WiFi"
#define SECRET_PASS "0h4orXc@yS3do"
#define CHIAVE_CLOUD "Qk2xOVKxuxtqie6cofTq"

char dboard_server[] = "192.168.140.250"; // Indirizzo IP/Internet del Dashboard Server
int dboard_port = 8888;                   // Porta IP del server

//Variabili
float temp_aria = 0;
float umid_aria = 0;
float luminosita = 0;
int umid_terreno = 0;
float anid_carbonica = 0;
float particolato = 0;

//Collegare una resistenza da 1K in serie ad un led su questo pin
#define PIN_LED1 5
//Collegare RESET a questo pin con un jumper
#define PIN_RESET 4

//Collegamento alla funzione del led
void accendi_LED_per(byte volte);

//Collegamento alla piattaforma GL-Blocks
#include "GL-Blocks-WiFi.h"
#include "GL-Blocks-Dashboard-GAX.h"
//*******************************************
///// NON SPOSTARE QUESTE DEFINIZIONI  /////

#include <Wire.h>  

//Libreria ENV Shield
#include <Arduino_MKRENV.h>

//Libreria per Grove CO2 Hum/Temp SCD30
//https://wiki.seeedstudio.com/Grove-CO2_Temperature_Humidity_Sensor-SCD30/
#include "SCD30.h"
float last_ppmCO2;

//Collegare il sensore capacitivo al connettore analogico indicato
#define PIN_UMIDITA_TERRENO A1

//Collegare il sensore di particolato al connettore digitale indicato
#define PIN_SENSORE_PARTICOLATO 3
//Periodo di lettura
int INTERVALLO_PARTICOLATO = 60000; //In millisecondi: 1000 = 1 sec

//Misura del particolato proporzionale alla durata dell'impulso
unsigned long durataImpulsoLow = 0;
//Tempo di ultima lettura particolato
long ultimaLetturaDust;

/**
 * Preperazione di Arduino: setup() 
 * Eseguito una sola volta.
 */
void setup() {

  //Condifura il pin per il reset
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  pinMode(PIN_RESET, OUTPUT);

  //Attiva il Serial Monitor
  Serial.begin(9600);  
  delay(2000); //Attesa setup della seriale
  
  accendi_LED_per(1); //Lampeggia 1 volta

  pinMode(PIN_LED1, OUTPUT);
  digitalWrite(PIN_LED1, LOW);

  delay(2000); //Tempo per aprire il serial monitor...
  Serial.println("FEM - Green Air Explorer Kit");
  
  Wire.begin(); //Inzializza I2C per l'ENV shield 
  accendi_LED_per(2); //Lampeggia 2 volte

  ///// ATTIVAZIONE DEI SENSORI /////
 
  //Si connette L'ENV Shield
  if (!ENV.begin()) {
    Serial.println("Errore durante l'avvio del MKR Shield");
    while (1);
  }   

  //Avvio del Grove CO2 Temp/Hum SCD30
  scd30.initialize();

  //Inizio temporizzazione particolato
  pinMode(PIN_SENSORE_PARTICOLATO, INPUT);
  ultimaLetturaDust = millis();

  accendi_LED_per(3); //Lampeggia 3 volte

  // Connessione al WiFi: vedi il file GL-Blocks-WiFi.h
  Connetti_WIFI();  
  
  accendi_LED_per(4); //Lampeggia 4 volte: PRONTI   

}

/**
 * Ciclo delle operazioni da eseguire sempre
 */
void loop() {    

  accendi_LED_per(3);

  //Temperatura dall'ENV Shield
  temp_aria = ENV.readTemperature();

  //Umidità aria dall'ENV Shield
  umid_aria = ENV.readHumidity();

  //Luminosità dall'ENV Shield
  luminosita = ENV.readIlluminance();

  //Lettura sensore capacitivo umidità terreno
  long va = analogRead(PIN_UMIDITA_TERRENO);
  umid_terreno = (int)map(va, 0, 1024, 100, 0); //Da tarare

  //Misura dell'Anidride carbonica
  float result[3] = {0};
  //Se il risultato è disponibile, lo legge
  if(scd30.isAvailable()) {
    Serial.print("Misura CO2 disponibile");        
    scd30.getCarbonDioxideConcentration(result);
    anid_carbonica = result[0];
    last_ppmCO2 = anid_carbonica;
  }
  else {
    Serial.print("Misura CO2 non ancora disponibile: non aggiornato");        
    anid_carbonica = last_ppmCO2;        
  }    

  //Misura del particolato
  unsigned long durata = pulseIn(PIN_SENSORE_PARTICOLATO, LOW);
  durataImpulsoLow = durataImpulsoLow + durata;

  if ((millis() - ultimaLetturaDust) > INTERVALLO_PARTICOLATO) {
    float ratio = durataImpulsoLow/(INTERVALLO_PARTICOLATO * 10.0);
    particolato = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;

    durataImpulsoLow = 0;
    ultimaLetturaDust = millis();
  }  
  
  accendi_LED_per(2); //Lampeggia il led per 2 volte
      
  mostra_valori_serial_monitor();

  //Verifica se si è ancora connessi al WiFi
  Connetti_WIFI();
  
  delay(1000); //Attende 1 secondo

  //Invia i dati alla dashboard
  Trasmetti_Dati_Cloud();    

  //30 sec tra un ciclo e il prossimo
  delay(30000); 
}

/**
 * Lampeggia il LED sul pin PIN_LED1 per un numero di volte
 */
void accendi_LED_per(byte volte) 
{
  for (byte i=0; i<volte; i++) {
    digitalWrite(PIN_LED1, HIGH);
    delay(200);
    digitalWrite(PIN_LED1, LOW);
    delay(200);
  }
}

/**
 * Scrive i valori dei sensori sul serial Monitor (Serial)
 */
void mostra_valori_serial_monitor()
{
  Serial.println();    
  Serial.print("Temp. aria = ");
  Serial.print(temp_aria);
  Serial.println(" °C");

  Serial.print("Umid. aria = ");
  Serial.print(umid_aria);
  Serial.println(" %");

  Serial.print("Umid. terreno = ");
  Serial.print(umid_terreno);
  Serial.println(" %");

  Serial.print("Illuminazione = ");
  Serial.print(luminosita);
  Serial.println(" lux");

  Serial.print("Anid. carbonica = ");
  Serial.print(anid_carbonica);
  Serial.println(" ppm");
  
  Serial.print("Particolato = ");
  Serial.print(particolato);
  Serial.println(" ppm"); 
}
