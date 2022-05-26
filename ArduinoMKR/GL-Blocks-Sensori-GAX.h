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
  - Particolato PM10 e 2.5 (NOVA SDS-11) //Pin 13 RX e 14 TX Arduino NKR Serial1
  */

//Libreria ENV Shield
#include <Arduino_MKRENV.h>

//Libreria per MH-Z19  //Da sostituire con sensore Grove
#include "MH-Z19.h"
#define PIN_MHZ19_ANALOG A0

int last_ppmCO2;

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

  // Start up del MH-Z19
  MHZSetup();

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
 *  Legge la CO2 dal MH-Z19B
 *  utilizzando la connessione seriale.
 */
int Misura_CO2_Seriale()
{
  //Lettura CO2 MH-Z19B
  int res = MHZReadCO2UART();  
  //Se il risultato è positivo, si tratta di una misura
  if (res >= 0) 
    last_ppmCO2 = res;
  
  //altrimenti di un errore
  else {
    res = last_ppmCO2;
    Serial.print("MH-Z19B ritorna:");        
    Serial.print(res);
  }
  return res;
}

/** 
 *  Legge la CO2 dal MH-Z19B
 *  il pin analogico
 */
int Misura_CO2_Analogico()
{
  long va = analogRead(PIN_MHZ19_ANALOG);
  return (int)map(va, 0, 512, 0, MHZ19_MAX_PPM_READING);
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
