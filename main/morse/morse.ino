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

const int led12 = 25;  // Primer relé
int estadoLed12 = 0;
const String morseCode[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",  // Letras A-J
  "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",    // Letras K-T
  "..-", "...-", ".--", "-..-", "-.--", "--..",                           // Letras U-Z
  "-----", ".----", "..---", "...--", "....-",                            // Números 0-4
  ".....", "-....", "--...", "---..", "----."                             // Números 5-9
};

// Función que maneja los mensajes nuevos
void mensajesNuevos(int numerosMensajes) {
  for (int i = 0; i < numerosMensajes; i++) {
    String chat_id = bot.messages[i].chat_id;  // Cambiar a long
    String text = bot.messages[i].text;
    String patron = "";  // Inicializamos la variable patron
    for (int posicion = 0; posicion < text.length(); posicion++) {
      char letra = toupper(text[posicion]);
      if (letra >= 'A' && letra <= 'Z') {
        patron = letraA_Morse(letra);  // Obtener el código Morse de la letra
      } else if (letra >= '0' && letra <= '9') {
        patron = numeroA_Morse(letra);  // Obtener el código Morse del número
      }
      if (patron.length() > 0) {
        patronMorse(patron);  // Llamar a la función para emitir el patrón
      }
      
digitalWrite(led12, LOW);
      delay(1000);
    }
    
  }
}

// Función para obtener el código Morse de una letra
String letraA_Morse(char letra) {
  return morseCode[letra - 'A'];  // Calcular el índice correcto para letras
}

// Función para obtener el código Morse de un número
String numeroA_Morse(char numero) {
  return morseCode[(numero - '0') + 26];  // Calcular el índice para números
}

// Función para emitir el código Morse
void patronMorse(String patron) {
  for (int i = 0; i < patron.length(); i++) {
    char caracter = patron[i];
    if (caracter == '.') {
      punto();
    } else if (caracter == '-') {
      barra();
    }
  }
}

// Función para representar un punto en Morse
void punto() {
  digitalWrite(led12, HIGH);
  delay(200);  // Duración del punto
  digitalWrite(led12, LOW);
  delay(200);  // Pausa entre puntos
}

// Función para representar una barra en Morse
void barra() {
  digitalWrite(led12, HIGH);
  delay(600);  // Duración de la barra
  digitalWrite(led12, LOW);
  delay(200);  // Pausa entre barras
}

void setup() {
  Serial.begin(115200);
  pinMode(led12, OUTPUT);
  digitalWrite(led12, HIGH);  // Inicializar el LED

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Esperar a que se conecte el WiFi
  }

  // Enviar mensaje de bienvenida solo una vez
  bot.sendMessage(ID_Chat, "Conversor texto a morse iniciado.");
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
