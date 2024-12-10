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

const int led13 = 33;  // Tercer relé
const int led15 = 27;  // Primero relé
const int led16 = 26;  // Segundo relé
const int led17 = 32;  // Cuarto relé

const int ledPins[] = { 27, 26, 33, 32 };  // Relé 2, 3, 4 y 5
const int numReles = 4;

// Genera un número secreto aleatorio
int numeroSecreto[] = { random(0, 10), random(0, 10), random(0, 10), random(0, 10) };

void generarNumeroSecreto() {
  for (int i = 0; i < 4; i++) {
    numeroSecreto[i] = random(0, 10);
  }
}

void apagarTodo() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

bool in_array(int value, int array[], int size) {
  for (int i = 0; i < size; i++) {
    if (array[i] == value) {
      return true;
    }
  }
  return false;
}

String mostrarResultado() {
  String number = "";
  for (int i = 0; i < 4; i++) {
    number += String(numeroSecreto[i]);
  }
  return number;
}

#define DHT_TYPE DHT22
DHT dht(21, DHT_TYPE);//ahi se pone el pin
bool movimientoDetectado = false;

void mensajesNuevos(int numerosMensajes) {
  for (int i = 0; i < numerosMensajes; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    bot.sendMessage(chat_id, "Repetido: " + text);

    if (text == "pista") {
      bot.sendMessage(chat_id, "No estoy programado para darte pistas.");
    } else if (text == "reiniciar") {
      bot.sendMessage(chat_id, "Reiniciando número...");
      generarNumeroSecreto();
      apagarTodo();
    } else if (text == "respuesta") {
      bot.sendMessage(chat_id, "El número secreto es: " + mostrarResultado());
    } else {
      for (int j = 0; j < numReles; j++) {
        if (text.length() > j) {
          delay(1000);
          if (numeroSecreto[j] == text[j] - '0') {
            digitalWrite(ledPins[j], LOW);
            bot.sendMessage(chat_id, "El dígito: " + String(text[j]) + " es correcto.");
          } else if (in_array(text[j] - '0', numeroSecreto, 4)) {
            bot.sendMessage(chat_id, "El dígito: " + String(text[j]) + " se encuentra en el número, pero no está en la posición correcta.");
          } else {
            bot.sendMessage(chat_id, "El dígito: " + String(text[j]) + " no se encuentra en el número.");
            digitalWrite(ledPins[j], HIGH);
          }
        }
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led13, OUTPUT);
  pinMode(led15, OUTPUT);
  pinMode(led16, OUTPUT);
  pinMode(led17, OUTPUT);

  digitalWrite(led13, HIGH);
  digitalWrite(led15, HIGH);
  digitalWrite(led16, HIGH);
  digitalWrite(led17, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  generarNumeroSecreto();

  // Enviar mensaje de bienvenida solo una vez
  bot.sendMessage(ID_Chat, "Iniciando juego de adivinar el número secreto");
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
