/****
 * GL Blocks
 * Green Air Explorer
 * Green Lab - Future Education Modena 2022
 * 
 * Blocchi Lettura Sensori
 */

 /**
  - Umidità /Temperatura (Shield MKR Env)
  - Umidità  del terreno ( Capacitivo analogico) su A1
  - Illuminazione (Shield MKR Env) 
  - Anidride Carbonica (MH-Z19B) lettura analogica su A0
  - Particolato PM10 e 2.5 (Grove Dust Sensor) lettura PWM su D3
  - Anidride Carbonica (Grove CO2 - Umid / Temp ) digitale I2C
  */

//Libreria ENV Shield
#include <Arduino_MKRENV.h>

//Libreria per Grove CO2 Hum/Temp SCD30
//https://wiki.seeedstudio.com/Grove-CO2_Temperature_Humidity_Sensor-SCD30/
#include "SCD30.h"
float last_ppmCO2;

#define PIN_UMIDITA_TERRENO A1

#define PIN_SENSORE_PARTICOLATO 3
//Periodo di lettura
int INTERVALLO_PARTICOLATO = 60000; //In millisecondi

//Tempo di ultima lettura particolato
unsigned long durataImpulsoLow = 0;
long ultimaLetturaDust;

/*************************************
 Funzioni dei BLOCCHI sensore
 *************************************/

/**
 * Avvia i sensori configurati per il MKR
 */
void Prepara_Sensori(void) {
    
  //Inizializza L'ENV Shield
  if (!ENV.begin()) {
    Serial.println("Errore durante l'avvio del MKR Shield");
    while (1);
  }   

  //Avvio del Grove CO2 Temp/Hum SCD30
  scd30.initialize();

  //Inizio temporizzazione particolato
  pinMode(PIN_SENSORE_PARTICOLATO, INPUT);
  ultimaLetturaDust = millis();
}


/**
 * Legge la temperatura
 */
float Misura_Temperatura_Aria() 
{
  return ENV.readTemperature();
}

/**
 * Legge l'umidità dell'aria
 */
float Misura_Umidita() 
{
  return ENV.readHumidity();
}

/**
 * Legge l'illuminazione
 */
float Misura_Illuminazione() 
{
  return ENV.readIlluminance();
}

/**
 * Legge l'umidità del terreno
 */ 
int Misura_Umidita_Terreno()
{
  long va = analogRead(PIN_UMIDITA_TERRENO);
  int ht = (int)map(va, 0, 1024, 100, 0); // se Sensore capacitivo
  return ht;
}

/**
 * Legge l'intensità luminosa
 */
float Misura_Luce()
{
  return ENV.readIlluminance();
}

/** 
 *  Legge la CO2 dall SCD30
 *  dalla connessione I2C
 */
float Misura_CO2()
{
  //Lettura Dati
  float res;
  float result[3] = {0};
  
  //Se il risultato è disponibile, lo legge
  if(scd30.isAvailable()) {
    scd30.getCarbonDioxideConcentration(result);
    res = result[0];
    last_ppmCO2 = res;
  }
  else {
    Serial.print("Valori CO2 non disponibili e non aggiornati");        
    res = last_ppmCO2;        
  }    
  return res;
}

/**
 * Legge le misure del particolato da NOVA PM
 * Utilizzando l'interfaccia seriale
 */
void Misura_Particolato()
{
  unsigned long durata = pulseIn(PIN_SENSORE_PARTICOLATO, LOW);
  durataImpulsoLow = durataImpulsoLow + durata;

  if ((millis() - ultimaLetturaDust) > INTERVALLO_PARTICOLATO) {
    float ratio = durataImpulsoLow/(INTERVALLO_PARTICOLATO * 10.0);
    particolato = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;

    durataImpulsoLow = 0;
    ultimaLetturaDust = millis();
  }
}
