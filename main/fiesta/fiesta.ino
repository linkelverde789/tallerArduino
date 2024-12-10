#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"

// Reemplazar con los datos de tu red WiFi
#define WIFI_SSID "NOMBRE WIFI"
#define WIFI_PASSWORD "CONTRASEÑA WIFI"

// Token de Telegram BOT obtenido desde Botfather en Telegram
#define BOT_TOKEN "7844617143:AAFDJwBp9zBZoTQBzo80NjbB-NkoNi5jcqk"

const unsigned long tiempo = 1000;  // Tiempo medio entre mensajes de escaneo
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long tiempoAnterior;  // Última vez que se realizó el análisis de mensajes

#define ID_Chat "7765043093"  // ID de Chat, obtenido de Telegram


const int led12 = 25;  // Primer relé, antes 25 pero no hacía nada
const int led13 = 33;   // Segundo relé
const int led15 = 27;   // Tercer relé
const int led16 = 26;   // Cuarto relé
const int led17 = 32;   // Quinto relé
const int led18 = 15;   // Sexto relé
const int ldrPin = 25;  // Pin del sensor LDR
// const int dhtPin=21; //Por acordarme

int estadoLed12 = 0;
int estadoLed13 = 0;
int estadoLed15 = 0;
int estadoLed16 = 0;
int estadoLed17 = 0;
int estadoLed18 = 0;

#define DHT_PIN 21  //antes era 16... Al poner 16 entra en un bucle...
//ESP32-WROOM-32 --> DHT_PIN 21

const int ledPins[] = { 27, 26, 33, 32 };  // Relé 2, 3, 4 y 5
const int numReles = 4;

#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);
bool movimientoDetectado = false;
void mensajesNuevos(int numerosMensajes) {
  for (int i = 0; i < numerosMensajes; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    if (text == "fiesta") {
      for (int j = 0; j < 3; j++) {
        for (int i = 0; i < numReles; i++) {
          digitalWrite(ledPins[i], LOW);
          delay(75);
          digitalWrite(ledPins[i], HIGH);
          delay(75);
        }
      }
    } else {
      bot.sendMessage(chat_id, "Repetido: " + text);
    }

    if (text == "R1on") {
      estadoLed12 = 1;
      digitalWrite(led12, HIGH);
      bot.sendMessage(chat_id, "Relé1 encendido");
    }

    if (text == "R1off") {
      estadoLed12 = 0;
      digitalWrite(led12, LOW);
      bot.sendMessage(chat_id, "Relé1 apagado");
    }

    if (text == "R2on") {
      estadoLed15 = 1;
      digitalWrite(led15, LOW);
      bot.sendMessage(chat_id, "Relé2 encendido");
    }

    if (text == "R2off") {
      estadoLed15 = 0;
      digitalWrite(led15, HIGH);
      bot.sendMessage(chat_id, "Relé2 apagado");
    }

    if (text == "R3on") {
      estadoLed16 = 1;
      digitalWrite(led16, LOW);
      bot.sendMessage(chat_id, "Relé3 encendido");
    }

    if (text == "R3off") {
      estadoLed16 = 0;
      digitalWrite(led16, HIGH);
      bot.sendMessage(chat_id, "Relé3 apagado");
    }

    if (text == "R4on") {
      estadoLed13 = 1;
      digitalWrite(led13, LOW);
      bot.sendMessage(chat_id, "Relé4 encendido");
    }

    if (text == "R4off") {
      estadoLed13 = 0;
      digitalWrite(led13, HIGH);
      bot.sendMessage(chat_id, "Relé4 apagado");
    }

    if (text == "R5on") {
      estadoLed17 = 1;
      digitalWrite(led17, LOW);
      bot.sendMessage(chat_id, "Relé5 encendido");
    }

    if (text == "R5off") {
      estadoLed17 = 0;
      digitalWrite(led17, HIGH);
      bot.sendMessage(chat_id, "Relé5 apagado");
    }

    if (text == "R6on") {
      estadoLed18 = 1;
      digitalWrite(led18, LOW);
      bot.sendMessage(chat_id, "Relé6 encendido");
    }

    if (text == "R6off") {
      estadoLed18 = 0;
      digitalWrite(led18, HIGH);
      bot.sendMessage(chat_id, "Relé6 apagado");
    }

    if (text == "Datos") {
      float temperatura = dht.readTemperature();
      float humedad = dht.readHumidity();

      String respuesta = "Datos del sensor DHT22:\n";
      respuesta += "Temperatura: " + String(temperatura, 1) + " °C\n";
      respuesta += "Humedad: " + String(humedad, 1) + " %";
      bot.sendMessage(chat_id, respuesta, "");
    }

    if (text == "EstadoLDR") {
      int estadoLDR = digitalRead(ldrPin);
      if (estadoLDR == HIGH) {
        bot.sendMessage(chat_id, "El sensor LDR detecta luz.", "");
      } else {
        bot.sendMessage(chat_id, "El sensor LDR no detecta luz.", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led12, OUTPUT);
  pinMode(led13, OUTPUT);
  pinMode(led15, OUTPUT);
  pinMode(led16, OUTPUT);
  pinMode(led17, OUTPUT);
  pinMode(led18, OUTPUT);
  pinMode(ldrPin, INPUT);

  digitalWrite(led12, HIGH);
  digitalWrite(led13, HIGH);
  digitalWrite(led15, HIGH);
  digitalWrite(led16, HIGH);
  digitalWrite(led17, HIGH);
  digitalWrite(led18, HIGH);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Enviar mensaje de bienvenida solo una vez
  bot.sendMessage(ID_Chat, "Sistema preparado!!! Escribe algo para que lo repita.");
  dht.begin();
}

void loop() {
  if (millis() - tiempoAnterior > tiempo) {
    int numerosMensajes = bot.getUpdates(bot.last_message_received + 1);

    while (numerosMensajes) {
      mensajesNuevos(numerosMensajes);
      numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
    }

    tiempoAnterior = millis();
  }
}