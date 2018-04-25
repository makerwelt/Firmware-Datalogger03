#include <Wire.h>
#include <DHT11.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
RTC_DS1307 rtc;
DHT11 dht11(3);
int s_analogica_mq135 = 0;
const int chipSelect = 10; // En nuestra shield el pin SD es el 10.
char daysOfTheWeek[7][12] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
int temp = 1;  // Pin analógico A1 del Arduino donde conectaremos el pin de datos del sensor TMP36
float maxC = 0, minC = 100, maxF = 0, minF = 500, maxV = 0, minV = 5;  //Variables para ir comprobando maximos y minimos
int UVOUT = A5; // Output from the sensor
int REF_3V3 = A4; // 3.3V power on the Arduino board
int T2 = 0;
int TH1 = 0;
int AIR = 0;

void setup() {
  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  pinMode(4, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  T2 = digitalRead (4);
  TH1 = digitalRead (7);
  AIR = digitalRead (8);

  Serial.begin(9600);
  Wire.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // auto update from computer time
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // to set the time manualy
  pinMode(2, OUTPUT); // Pin para led de confirmacion de lectura escritura
  pinMode(3, INPUT_PULLUP) ; // DHT11 pin de lectura
  delay (500);
  Serial.print("Inicializando memoria SD..."); // Comprobamos que la SD esta preparada para realizar la copia de datos:
  pinMode(10, OUTPUT); // visualiza si existe SD y si esta bien inicializada:
  if (!SD.begin(chipSelect)) {
    Serial.println("Memoria Fallada, o no insertada"); // Si responde con fallo el programa no haria nada mas:
    return;
  }
  Serial.println("Memoria Inicializada.");
}

void loop() {
  digitalWrite(2, LOW) ;
  float voltaje, gradosC, gradosF; // Declaramos estas variables tipo float para guardar los valores de lectura
                                   // del sensor, así como las conversiones a realizar para convertir a grados
                                   // centígrados y a grados Fahrenheit
  voltaje = analogRead(1) * 0.004882814; // Con esta operación lo que hacemos es convertir el valor que nos devuelve
                                         // el analogRead(1) que va a estar comprendido entre 0 y 1023 a un valor
                                         // comprendido entre los 0.0 y los 5.0 voltios
  gradosC = (voltaje - 0.5) * 100.0; // Gracias a esta fórmula que viene en el datasheet del sensor podemos convertir
                                     // el valor del voltaje a grados centigrados
  gradosF = ((voltaje - 0.5) * 100.0) * (9.0 / 5.0) + 32.0; // Gracias a esta fórmula que viene en el datasheet del sensor podemos convertir
                                                            // el valor del voltaje a grados Fahrenheit
  delay(9450);
  Serial.println();
  DateTime now = rtc.now();
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(", ");
  Serial.print(now.day());
  Serial.print('/');
  Serial.print(now.month());
  Serial.print('/');
  Serial.print(now.year());
  Serial.print(", ");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.print(now.second());
  Serial.println("  ");
  if (T2 == 1){
    Serial.print ("Temp. Sensor de Contacto: ");
    Serial.print (gradosC);
    Serial.println (" ºC. ");
    }
  int err;
  float temp, hum;
  if ( TH1 == 1){
    if ((err = dht11.read(hum, temp)) == 0) {  // Si devuelve 0 es que ha leido bien
      Serial.print("Temperatura Ambiente: ");
      Serial.print(temp);
      Serial.print (" ºC., ");
      Serial.print(" Humedad Ambiente: ");
      Serial.print(hum);
      Serial.println(" %.");
      }
    else {
      Serial.println();
      Serial.print("Error Num :");
      Serial.print(err);
      Serial.println();
      }
  }
  if ( AIR == 1 ) {
    s_analogica_mq135 = analogRead(0);
    Serial.print(s_analogica_mq135, DEC);
    Serial.print(" ppm.");
    delay(250);
    if (s_analogica_mq135 <= 50) {
      Serial.println(" Calidad de Aire Buena.");
      delay(50);
    }
    if (s_analogica_mq135 >= 51 && s_analogica_mq135 <= 100) {
      Serial.println(" Calidad de Aire Regular.");
      delay(50);
    }
    if (s_analogica_mq135 >= 101 && s_analogica_mq135 <= 150) {
      Serial.println(" Calidad de Aire Mala.");
      delay(50);
    }
    if (s_analogica_mq135 >= 151 && s_analogica_mq135 <= 200) {
      Serial.println(" Calidad de Aire Muy mala.");
      delay(50);
    }
    if (s_analogica_mq135 >= 201) {
      Serial.println(" Calidad de Aire Extremadamente mala");
    }  
  }
  delay(1000);            //Recordad que solo lee una vez por segundo
  // Abrimos el fichero. Nota: solo se puede abrir un fichero a la vez. Para abrir un segundo fichero hay que cerrar el anterior.
  File dataFile = SD.open("test.txt", FILE_WRITE);
  if (dataFile) {
    Serial.print(" Escribiendo en test.txt...");
    dataFile.print(now.day());
    dataFile.print('/');
    dataFile.print(now.month());
    dataFile.print('/');
    dataFile.print(now.year());
    dataFile.print(", ");
    dataFile.print(now.hour());
    dataFile.print(':');
    dataFile.print(now.minute());
    dataFile.print(':');
    dataFile.print(now.second());
    dataFile.print(", ");
    if ( TH1 == 1){
      dataFile.print("Temperatura: ");
      dataFile.print(temp);
      dataFile.print(" ºC. ,");
      dataFile.print(" Humedad: ");
      dataFile.print(hum);
      dataFile.print(" %. ");
    }
    if ( T2 == 1){
      dataFile.print("temperatura: ");
      dataFile.println(gradosC);
    }
    if ( AIR == 1 ) {
      s_analogica_mq135 = analogRead(0);
      dataFile.print(s_analogica_mq135, DEC);
      dataFile.print(" ppm. ");
      delay(250);
      if (s_analogica_mq135 <= 50) {
        dataFile.println(" Calidad de Aire Buena.");
        delay(50);
      }
      if (s_analogica_mq135 >= 51 && s_analogica_mq135 <= 100) {
        dataFile.println(" Calidad de Aire Regular.");
        delay(50);
      }
      if (s_analogica_mq135 >= 101 && s_analogica_mq135 <= 150) {
        dataFile.println(" Calidad de Aire Mala.");
        delay(50);
      }
      if (s_analogica_mq135 >= 151 && s_analogica_mq135 <= 200) {
        dataFile.println(" Calidad de Aire Muy mala.");
        delay(50);
      }
      if (s_analogica_mq135 >= 201) {
        dataFile.println(" Calidad de Aire Extremadamente mala");
        delay(50);
      }
    }
    digitalWrite(2, HIGH) ;
    delay(50);
    // close the file:
    dataFile.close();
  }
}


