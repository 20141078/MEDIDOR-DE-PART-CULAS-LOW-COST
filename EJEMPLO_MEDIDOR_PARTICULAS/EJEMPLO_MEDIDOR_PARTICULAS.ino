/*SICHA HUAMAN RUDY GABRIEL 
 * METEORÓLOGO DE LA UNALM
 * PROTOTIPO 2 DE CALIDAD DE AIRE Y VARIABLES METEOROLÓGICAS
 * CUENTA DE GITHUB https://github.com/20141078/INSTRUMENTAL_LOW_COST
 * CUENTA EN THINGSPEAK https://thingspeak.com/channels/1047170 
 * PROYECTO REFERENTE A LOS CURSOS 
 * MONITOREO Y CONTROL DE LA CONTAMINACIÓN ATMOSFÉRICA
 * TEORÍA INSTRUMENTAL DE VARIABLES METEOROLÓGICAS
 */

/*
 * PARA USAR LA PLACA ESP32 requiere colocar este link: https://dl.espressif.com/dl/package_esp32_index.json en Archivo/Preferencias/Localización del proyecto
 * 
 */

//// REQUIERE INSTALAR LA LIBRERIA THINGSPEAK 

//// INCLUYENDO SENSORES POR BLOQUES 

///////////////////////////////////////////////////////////
//// 1RO IOT THINGSPEAK 
//// esp32
#include "ThingSpeak.h"
#include "WiFi.h"
WiFiClient cliente;

//// DEFINIR LOS PÁRAMETROS DE CONEXIÓN PLACA-NUBE 
//// AQUI CAMBIAR POR EL ID Y CLAVE DEL WIFI, ADEMÁS DEL CANAL Y APYKEY DE THIONGSPEAK
const char* ssid = "XXXXXX";               //// SSID ROUTER.
const char* password = "XXXXX";       //// Contraseña de router.

unsigned long channelID = 1128079;   //// Cambiar con el ID del canal 
const char* WriteAPIKey = "XXXXXXXXXXXX";  //// Cambiar con el Write API Key del canal.

///////////////////////////////////////////////////////////
/// 2DO. PLANTOWER PMS7003
#include "PMS.h"
#include "Wire.h"
PMS pms(Serial1);
PMS::DATA data;
#define RXD2 26
#define TXD2 25

///////////////////////////////////////////////////////////
////// 3RO. DHT22
#include "Arduino.h"
#include "DHT.h"
#define DHTPIN 5     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
#define DHTTYPE DHT21   // DHT 21, DHT 23
DHT dht(DHTPIN, DHTTYPE);

//// LCD
#include <LiquidCrystal_I2C.h>//Libreria para LCD I2C
#include <Wire.h>//libreria requrida para usar SDA y SCL
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
  Serial.begin(115200);
  
///////////////////////////////////////////////////////////
 ///// THINGSPEAK
  Serial.println("CONFIGURACIÓN NUBE:");

  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");

  ThingSpeak.begin(cliente);

///////////////////////////////////////////////////////////  
  //// PMS7003
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pms.passiveMode();

///////////////////////////////////////////////////////////
 //// DHT22
   dht.begin();

 //// LCD

  lcd.begin(20, 4);//Inicializar la LCD 20x4
  lcd.backlight();//Encender la luz de fondo

}

void loop()
{
  delay(2000);
  
///////////////////////////////////////////////////////////
  //// Lecturas de sensores
  sensores();

///////////////////////////////////////////////////////////
  ThingSpeak.writeFields(channelID,WriteAPIKey);
  Serial.println("Datos enviados a ThingSpeak!");
  //// delay(10000);
  delay(10000);
  
}

void sensores() {

///////////////////////////////////////////////////////////
//// PMS 7003

  //ACTIVACIÓN DEL VENTILADOR, Y TIEMPO DE ESTABILIZACIÓN DE 30 SEGUNDOS;
  Serial.println("Ventilador activado");
  pms.wakeUp();
  delay(20000);

  //ESTABLECIENDO SOLICITUD DE LAS LECTURAS;
  pms.requestRead();
  //LECTURA DE LOS DATOS, ADEMÁS TIEMPO EN OBTENCIÓN DE LECTURAS;
  if (pms.readUntil(data))
  {
    
    //// RECORDAR LAS ESPECIFICACIONES DEL SENSOR
    //// https://download.kamami.com/p564008-p564008-PMS7003%20series%20data%20manua_English_V2.5.pdf
    
    float PM_1   =   (data.PM_AE_UG_1_0);    //// PM 1 
    float PM_2_5 =   (data.PM_AE_UG_2_5);    //// PM 2.5
    float PM_10  =   (data.PM_AE_UG_10_0);   //// PM 10

    Serial.print("PM 1.0 (ug/m3): ");  Serial.println(PM_1);
    Serial.print("PM 2.5 (ug/m3): ");  Serial.println(PM_2_5);
    Serial.print("PM 10.0 (ug/m3): "); Serial.println(PM_10);

///////////////////////////////////////////////////////////
    ///// DHT22
  float h = dht.readHumidity();
  float t = dht.readTemperature();   
    Serial.print("TEMPERATURA: ");
    Serial.println(t);
    Serial.print("HUMEDAD: ");
    Serial.println(h);

    Serial.println("-----------------------");

   // CÓDIGOS DE LA PANTALLA 
   lcd.clear();
   // PRIMERA FILA
   lcd.setCursor(0,0);
   lcd.print("T: ");
   lcd.print(t,1);
   lcd.print(" oC");
  // PRIMERA FILA
   lcd.setCursor(11,0);
   lcd.print("HR: ");
   lcd.print(h,0);
   lcd.print(" %");
  // SEGUNDA FILA
   lcd.setCursor(0,1);
   lcd.print("PM 1: ");
   lcd.print(PM_1,0); //1 decimal
  // SEGUNDA FILA
   lcd.setCursor(10,1);
   lcd.print("PM 2.5: ");
   lcd.print(PM_2_5,0); //1 decimal
  // TERCERA FILA
   lcd.setCursor(0,2);
   lcd.print("PM 10: ");
   lcd.print(PM_10,0);
   lcd.setCursor(12,2);
   lcd.print("ug/m3 ");

// INCA
  int INCA = PM_2_5*100/50;
    if(INCA<=50){
    lcd.setCursor(0,3);
    lcd.print("BUENA");
    lcd.setCursor(12,3);
    lcd.print(":)");

  } else if(INCA>50 && INCA<=100){
    lcd.setCursor(0,3);
    lcd.print("MODERADO");
    lcd.setCursor(17,3);
    lcd.print(":/");
    
  } else if(INCA>100 && INCA <= 500){
    lcd.setCursor(0,3);
    lcd.print("MALO");
    lcd.setCursor(17,3);
    lcd.print(":(");
    
  } else{
    lcd.setCursor(0,3);
    lcd.print("VUEC");
    lcd.setCursor(17,3);
    lcd.print(":X");
  }
  
    delay(200);
  
///////////////////////////////////////////////////////////
    //// Envio a thingspeak
    //// formato (FIELD, VARIABLE)
    ThingSpeak.setField (1,PM_1);
    ThingSpeak.setField (2,PM_2_5);
    ThingSpeak.setField (3,PM_10);
    ThingSpeak.setField (4,t);
    ThingSpeak.setField (5,h);  

    delay(5000);

  }
  else
  {
    Serial.println("LECTURA FALLIDA.");
  }
  
  //// APAGAR EL VENTILADOR;
    pms.sleep();

}
