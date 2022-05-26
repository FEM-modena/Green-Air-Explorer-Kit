/****
 * GL Blocks
 * Air Data Challenge
 * Green Lab - Future Education Modena 2022
 * 
 * Funzioni Basic
 */

#include <Wire.h>  

#define PIN_LED1 5

//Collegare RESET al pin 7 con un Jumper
#define PIN_RESET 4

/****************************
 Funzioni di piattaforma
 ***************************/

/**
 * Inizializza la seriale 
 */
void Accendi_Serial_Monitor(void) 
{
  Serial.begin(9600);
  //while (!Serial); //Needed for native USB port only
  delay(2000); //Attesa setup della seriale
}

/**
 * Funzione di setup
 */
void Prepara_Arduino(void) 
{
  digitalWrite(PIN_RESET, HIGH);
  delay(100);
  pinMode(PIN_RESET, OUTPUT);
  
  pinMode(PIN_LED1, OUTPUT);
  digitalWrite(PIN_LED1, LOW);

  delay(2000); //Tempo per aprire il serial monitor...
  Serial.println("Green Air Explorer MKR");
  
  Wire.begin(); //Inzializza I2C per l'ENV shield 

}

/**
 * Lampeggia il BUILT IN lED per un numero di volte
 */

void Accendi_LED_Per(byte volte) 
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
void Mostra_Valori_Serial_Monitor()
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

/**
 * Wrapper della funzione delay()
 */
void Aspetta_Secondi(int sec) 
{
  delay(1000*sec);
}

//Reset programmatico
void Resetta() {
  digitalWrite(PIN_RESET, LOW);
}
