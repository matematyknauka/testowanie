#include "moje_lcd.h"
#include <WiFi.h>
#include <time.h>

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



void setup() {
    
    WiFi.begin(ssid, password);
  
    // Czekanie na połączenie
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    clearLCD();
    printLCD("Czekam");
    }
    clearLCD();
    printLCD("Polaczono");
    clearLCD();

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

    
}
