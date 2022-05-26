/* 
  Green Air Explorer Kit
  Future Education Modena 2022

  Monitora: 
  - Umidità /Temperatura (Shield MKR Env)
  - Umidità  del terreno (Grove Sensore Conduzione o Capacitivo analogico)
  - Anidride Carbonica (MH-Z19B) //Da modificare
  - Particolato (Grove Dust Sensor)
*/

//ATTENZIONE: non spostare queste definizioni
//*******************************************
#define SECRET_SSID "FEM_WiFi"
#define SECRET_PASS "0h4orXc@yS3do"
#define CHIAVE_CLOUD "Qk2xOVKxuxtqie6cofTq"

//Variabili
float temp_aria = 0;
float umid_aria = 0;
float luminosita = 0;
int umid_terreno = 0;
int anid_carbonica = 0;
float particolato = 0;

//Collegamento alla piattaforma GL-Blocks
#include "GL-Blocks-Basic-GAX.h"
#include "GL-Blocks-WiFi.h"
#include "GL-Blocks-Sensori-GAX.h"
#include "GL-Blocks-Dashboard-GAX.h"
//*******************************************

/**
 * Preperazione di Arduino: setup() 
 * Eseguito una sola volta.
 */
void setup() {

  Accendi_Serial_Monitor();
  
  Accendi_LED_Per(1);

  Prepara_Arduino();
  
  Accendi_LED_Per(2);

  Prepara_Sensori();

  Accendi_LED_Per(3);

  Connetti_WIFI();  
  
  Accendi_LED_Per(4);    

}

/**
 * Arduino ciclo infinito
 */
void loop() {    

  Accendi_LED_Per(3);
  
  temp_aria = Misura_Temperatura_Aria();
  
  umid_aria = Misura_Umidita();

  luminosita = Misura_Illuminazione();

  umid_terreno = Misura_Umidita_Terreno();

  anid_carbonica = Misura_CO2_Analogico();

  //tvoc = Misura_TVOC();

  Misura_Particolato();

  Accendi_LED_Per(2);
      
  Mostra_Valori_Serial_Monitor();

  Connetti_WIFI();
  
  Aspetta_Secondi(1);

  Trasmetti_Dati_Cloud();    
     
  Aspetta_Secondi(30);
}
