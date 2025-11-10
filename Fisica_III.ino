#include <WiFi.h>   
#include <PubSubClient.h>  

#define SSID "OPPO A58" // WifiSSID  
#define PASS "123456789" // contraseña de Wifi  
#define TOKEN "BBUS-ZRIXRsYmJv2xPaUQMR9pN60p3pVgYQ" // TOKEN de Ubidots  
#define MQTT_CLIENT_NAME "Proyecto_MQTT" // MQTT nombre del cliente  
#define DEVICE_LABEL "esp32"  // Nombre del Device_Label

#define VARIABLE_LABEL_SENSOR "sensor"   // Variable_LABEL en Ubidots  
#define VARIABLE_LABEL_TANQUE "tanque"  
#define VARIABLE_LABEL_ALMACEN "almacen"  

#define ULN18 15  // 250mL - Pines del ULN2803  
#define ULN17 2   // 500mL  
#define ULN16 0   // 750mL   
#define ULN15 4   // 1L  
#define ULN14 16  // 1,25L  
#define ULN13 17  // 1,5L    
#define ULN12 5   // 1,75L  
#define ULN11 18  // 2L  
#define T2N3904 19      // Control del ULN2803
#define ALMACEN_PIN 22  // Control para llenar el tanque
#define TANQUE_PIN 23   // Control para vaciar el tanque

#define LED_PIN1 13  // LEDs indicadores
#define LED_PIN2 12  
#define LED_PIN3 14  
#define LED_PIN4 27  
#define LED_PIN5 26  
#define LED_PIN6 25  
#define LED_PIN7 33  
#define LED_PIN8 32  

int pinG15 = 0;  // Estado de los valores del ULN2803
int pinG2  = 0;  
int pinG0  = 0;   
int pinG4  = 0;   
int pinG16 = 0;   
int pinG17 = 0;   
int pinG5  = 0;  
int pinG18 = 0;  
float sensor = 0;

const float NIVEL_MINIMO = 12.5;     // Límites mínimo y máximo del tanque
const float NIVEL_MAXIMO = 100.0;

WiFiClient ubidots;                  // Comunicación Wifi
PubSubClient client(ubidots);  
char mqttBroker[] = "industrial.api.ubidots.com";  
char payload[150];  
char topic[150];  
char str_sensor[10];  
bool estadoTanque  = false;  
bool estadoAlmacen = false;  

void callback(char* topic, byte* payload, unsigned int length)  // PubSubClient - Comunicación con Ubidots 
{  
  payload[length] = '\0';  
  String msg = String((char*)payload);  
  Serial.print("Mensaje recibido: ");  
  Serial.println(msg);  
  Serial.print("Tópico: ");  
  Serial.println(topic);  

  if (String(topic).indexOf(VARIABLE_LABEL_TANQUE) != -1)   // Control de la bomba TANQUE  
  {  
    if (msg == "1" || msg == "1.0")
    {  
      digitalWrite(TANQUE_PIN, HIGH);  
      estadoTanque = true; 
      Serial.println("Bomba del tanque : ON ");  
    }   
    else
    {  
      digitalWrite(TANQUE_PIN, LOW);  
      estadoTanque = false;
      Serial.println("Bomba del tanque : OFF "); 
    }  
  }  

  if (String(topic).indexOf(VARIABLE_LABEL_ALMACEN) != -1)   // Control de la bomba ALMACÉN  
  {  
    if (msg == "1" || msg == "1.0")   
    {  
      digitalWrite(ALMACEN_PIN, HIGH);  
      estadoAlmacen = true; 
      Serial.print("Bomba del almacen : ON ");  
    }   
    else  
    {  
      digitalWrite(ALMACEN_PIN, LOW);  
      estadoAlmacen = false;  
      Serial.print("Bomba del almacen : OFF "); 
    }  
  }  
}  

void reconnect()            // Reconexión a Ubidots  
{
  while (!client.connected()) 
  { 
    Serial.println("Sin conexión a Ubidots... :c");  
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, ""))
    {  
      Serial.println("Conectado a Ubidots");  
      sprintf(topic, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL_TANQUE);    // Comando del pulsador de Ubidots 
      client.subscribe(topic);  
      sprintf(topic, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL_ALMACEN);   // Comando del pulsador de Ubidots
      client.subscribe(topic);  
      Serial.println("Tienes el control :D");  
    }
    else
    {  
      Serial.print(client.state());  
      Serial.println(" Reintentando...");  
      delay(2000);  
    }  
  }  
}  

void setup()   
{  
  Serial.begin(115200);  
  WiFi.begin(SSID, PASS);  
  Serial.print("Conectando a Red ... ");  
  
  while (WiFi.status() != WL_CONNECTED)  
  {  
    delay(500);  
    Serial.print(".");  
  }  
  Serial.println("Conectado (ᵔ ᵕ ᵔ) ");  
  Serial.print("IP: ");  
  Serial.println(WiFi.localIP());  
  client.setServer(mqttBroker, 1883);  
  client.setCallback(callback);  

  pinMode(T2N3904, OUTPUT);           
  pinMode(TANQUE_PIN, OUTPUT); 
  pinMode(ALMACEN_PIN, OUTPUT); 
  digitalWrite(TANQUE_PIN, LOW); 
  digitalWrite(ALMACEN_PIN, LOW); 

  pinMode(ULN18, INPUT_PULLUP);  
  pinMode(ULN17, INPUT_PULLUP);  
  pinMode(ULN16, INPUT_PULLUP);  
  pinMode(ULN15, INPUT_PULLUP);  
  pinMode(ULN14, INPUT_PULLUP);  
  pinMode(ULN13, INPUT_PULLUP);  
  pinMode(ULN12, INPUT_PULLUP);  
  pinMode(ULN11, INPUT_PULLUP);  

  pinMode(LED_PIN1, OUTPUT);  
  pinMode(LED_PIN2, OUTPUT);  
  pinMode(LED_PIN3, OUTPUT);  
  pinMode(LED_PIN4, OUTPUT);  
  pinMode(LED_PIN5, OUTPUT);  
  pinMode(LED_PIN6, OUTPUT);  
  pinMode(LED_PIN7, OUTPUT);  
  pinMode(LED_PIN8, OUTPUT);  
}  

void loop()   
{  
  if (!client.connected()) 
  {  
     reconnect ();  
  }  
  client.loop();  

  digitalWrite(T2N3904, HIGH);  
  delay(100);  
  pinG15 = digitalRead(ULN18);  
  pinG2  = digitalRead(ULN17);  
  pinG0  = digitalRead(ULN16);  
  pinG4  = digitalRead(ULN15);  
  pinG16 = digitalRead(ULN14);  
  pinG17 = digitalRead(ULN13);  
  pinG5  = digitalRead(ULN12);  
  pinG18 = digitalRead(ULN11);  
  delay(100);  
  digitalWrite(T2N3904, LOW);  

  if (pinG15 == LOW && pinG2 == LOW && pinG0 == LOW && pinG4 == LOW && pinG16 == LOW && pinG17 == LOW && pinG5  == LOW && pinG18 == LOW)  
  {  
    sensor = 100;  
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, HIGH);  
    digitalWrite(LED_PIN4, HIGH);  
    digitalWrite(LED_PIN5, HIGH);  
    digitalWrite(LED_PIN6, HIGH);  
    digitalWrite(LED_PIN7, HIGH);  
    digitalWrite(LED_PIN8, HIGH);  
    digitalWrite(ALMACEN_PIN, LOW);
  }

  else if (pinG15 == LOW && pinG2 == LOW && pinG0 == LOW && pinG4 == LOW && pinG16 == LOW && pinG17 == LOW && pinG5 == LOW && pinG18 == HIGH) 
  {  
    sensor = 87.5;  
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, HIGH);  
    digitalWrite(LED_PIN4, HIGH);  
    digitalWrite(LED_PIN5, HIGH);  
    digitalWrite(LED_PIN6, HIGH);  
    digitalWrite(LED_PIN7, HIGH);  
    digitalWrite(LED_PIN8, LOW);  
  }

  else if (pinG15 == LOW && pinG2 == LOW && pinG0 == LOW && pinG4 == LOW && pinG16 == LOW && pinG17 == LOW && pinG5 == HIGH && pinG18 == HIGH) 
  {  
    sensor = 75;  
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, HIGH);  
    digitalWrite(LED_PIN4, HIGH);  
    digitalWrite(LED_PIN5, HIGH);  
    digitalWrite(LED_PIN6, HIGH);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW);  
  }

  else if (pinG15 == LOW && pinG2 == LOW && pinG0 == LOW && pinG4 == LOW && pinG16 == LOW && pinG17 == HIGH && pinG5 == HIGH && pinG18 == HIGH)
  {  
    sensor = 62.5; 
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, HIGH);  
    digitalWrite(LED_PIN4, HIGH);  
    digitalWrite(LED_PIN5, HIGH);  
    digitalWrite(LED_PIN6, LOW);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW);  
  }  

  else if (pinG15 == LOW && pinG2 == LOW && pinG0 == LOW && pinG4 == LOW && pinG16 == HIGH && pinG17 == HIGH && pinG5 == HIGH && pinG18 == HIGH)  
  {  
    sensor = 50; 
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, HIGH);  
    digitalWrite(LED_PIN4, HIGH);  
    digitalWrite(LED_PIN5, LOW);  
    digitalWrite(LED_PIN6, LOW);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW);
  }  

  else if (pinG15 == LOW && pinG2 == LOW && pinG0 == LOW && pinG4 == HIGH && pinG16 == HIGH && pinG17 == HIGH && pinG5 == HIGH && pinG18 == HIGH)  
  {  
    sensor = 37.5;  
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, HIGH);  
    digitalWrite(LED_PIN4, LOW);  
    digitalWrite(LED_PIN5, LOW);  
    digitalWrite(LED_PIN6, LOW);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW);  
  }  
  
  else if (pinG15 == LOW && pinG2 == LOW && pinG0 == HIGH && pinG4 == HIGH && pinG16 == HIGH && pinG17 == HIGH && pinG5 == HIGH && pinG18 == HIGH)  
  {  
    sensor = 25;  
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, HIGH);  
    digitalWrite(LED_PIN3, LOW);  
    digitalWrite(LED_PIN4, LOW);  
    digitalWrite(LED_PIN5, LOW);  
    digitalWrite(LED_PIN6, LOW);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW);  
  }  

  else if (pinG15 == LOW && pinG2 == HIGH && pinG0 == HIGH && pinG4 == HIGH && pinG16 == HIGH && pinG17 == HIGH && pinG5 == HIGH && pinG18 == HIGH)  
  {  
    sensor = 12.5;  
    digitalWrite(LED_PIN1, HIGH);  
    digitalWrite(LED_PIN2, LOW);  
    digitalWrite(LED_PIN3, LOW);  
    digitalWrite(LED_PIN4, LOW);  
    digitalWrite(LED_PIN5, LOW);  
    digitalWrite(LED_PIN6, LOW);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW);  
    digitalWrite(TANQUE_PIN, LOW);
    digitalWrite(ALMACEN_PIN, HIGH);

  }  

  else if (pinG15 == HIGH && pinG2 == HIGH && pinG0 == HIGH && pinG4 == HIGH && pinG16 == HIGH && pinG17 == HIGH && pinG5 == HIGH && pinG18 == HIGH)   
  {  
    sensor = 0;  
    digitalWrite(LED_PIN1, LOW);  
    digitalWrite(LED_PIN2, LOW);  
    digitalWrite(LED_PIN3, LOW);  
    digitalWrite(LED_PIN4, LOW);  
    digitalWrite(LED_PIN5, LOW);  
    digitalWrite(LED_PIN6, LOW);  
    digitalWrite(LED_PIN7, LOW);  
    digitalWrite(LED_PIN8, LOW); 
  }  

  if (sensor <= NIVEL_MINIMO)
  {
    Serial.println("¡Nivel mínimo!");
    sensor = NIVEL_MINIMO;
  }
    else if (sensor >= NIVEL_MAXIMO) 
    {  
      Serial.println("¡Nivel máximo!");
    } 

  sprintf(topic, "/v1.6/devices/%s", DEVICE_LABEL);  
  dtostrf(sensor, 4, 2, str_sensor);  
  sprintf(payload, "{\"%s\": {\"value\": %s}}", VARIABLE_LABEL_SENSOR, str_sensor);  
  client.publish(topic, payload);  
  Serial.println( String("Sensor: ") + sensor + " porciento " );

  delay(1000);  
}