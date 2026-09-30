#include "moje_lcd.h"
#include <WiFi.h>
#include <time.h>
#include "Arduino.h"
#include "Audio.h"
#include <WebServer.h>

const char* ssid = "multimedia_Cegielnia18";
const char* password = "Cegieldanci91";

unsigned long t_akt = 0;
unsigned long t_poprz = 0;
unsigned long t_odc = 0;
unsigned long suma = 0;
unsigned long t_min_poln = 0;
unsigned long godz = 0;
unsigned long minuta = 0;
unsigned long t = 0;
 
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26
int VOLUME = 21;
String ADDRESS = "http://62.133.128.18:8040/listen.pls";
Audio audio;
void audio_info(const char* info) {
    Serial.printf("%s\n", info);
}

WebServer server(80);

void setup() {
    set_up();

    WiFi.begin(ssid, password);

    // Czekanie na połączenie
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    clearLCD();
    printLCD("Czekam");
    }
    Serial.begin(115200);
    
    Serial.print("Adres IP serwera: ");
       Serial.println(WiFi.localIP());

    configTime(3600, 3600, "pool.ntp.org");

    // Pobranie czasu w setup
    struct tm timeinfo;
    // Czekamy chwilę na synchronizację czasu z serwerem
    while (!getLocalTime(&timeinfo)) {
    delay(100);
    }



    // Wyświetlanie tekstów

    setCursor(3, 0);
    printLCD("Godzina");

    setCursor(0, 1);
    char buffer[16];
    godz = timeinfo.tm_hour;
    minuta = timeinfo.tm_min;
    t = (godz * 60UL + minuta * 1UL) * 60000UL;
    sprintf(buffer, "%02d:%02d", godz, minuta);
    printLCD(buffer);
    delay(1000);
    
    // radio
      // Definicja strony głównej (ścieżka "/")
  server.on("/", []() {
    // Sprawdzamy, czy użytkownik wysłał tekst w formularzu (pole "newVolume")
    if (server.hasArg("newVolume")) {
      VOLUME = server.arg("newVolume").toInt(); // Zapamiętujemy nową wartość
      audio.setVolume(VOLUME);
    }

    if (server.hasArg("newAddress")) {
      ADDRESS = server.arg("newAddress"); // Zapamiętujemy nową wartość
      audio.connecttohost(ADDRESS.c_str());
    }

    // Budujemy stronę HTML z aktualną wartością i formularzem
    String html = "<h1>Glosnosc: " + String(VOLUME) + "</h1>";
    html += "<form action='/' method='GET'>";
    html += "<input type='text' name='newVolume' placeholder='Wpisz wartosc'>";
    html += "<input type='submit' value='Zapisz'>";
    html += "</form>";
    server.send(200, "text/html", html); // Wysyłamy stronę do przeglądarki
  });

  // Uruchomienie serwera
  server.begin();

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(VOLUME); // default 0...21
  audio.connecttohost(ADDRESS.c_str());
    
    
}

void loop() {
  t_akt = millis();
  t_odc = t_akt - t_poprz;
  t_poprz = t_akt;
  suma = suma + t_odc;
  if(suma >= 60000UL)
  {
    t = (t + 60000UL) % 86400000UL;
    suma = suma - 60000UL;
    clearLCD();
    char buffer[16];
    sprintf(buffer, "%02d:%02d", t/60000UL/60UL, t/60000UL - t/60000UL/60UL * 60);
    printLCD(buffer);
  }
    
  // Obsługa przychodzących połączeń
  server.handleClient();
  audio.loop();
  vTaskDelay(1);


}
