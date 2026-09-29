#include <Wire.h>
#include <WiFi.h>
#include <time.h>

const char* ssid = "nazwa";
const char* password = "";

unsigned long t_akt = 0;
unsigned long t_poprz = 0;
unsigned long t_odc = 0;
unsigned long suma = 0;
unsigned long t_min_poln = 0;
unsigned long godz = 0;
unsigned long minuta = 0;
unsigned long t = 0;

const uint8_t lcdAddr = 0x27;

// Podstawowa funkcja wysyłająca bajt do LCD
void lcd_send(uint8_t value, uint8_t mode) {
    uint8_t highnibble = value & 0xF0;
    uint8_t lownibble  = (value << 4) & 0xF0;

    uint8_t data[4];
    data[0] = highnibble | 0x0C | mode; // EN = 1, Backlight = 1
    data[1] = highnibble | 0x08 | mode; // EN = 0, Backlight = 1
    data[2] = lownibble  | 0x0C | mode; // EN = 1, Backlight = 1
    data[3] = lownibble  | 0x08 | mode; // EN = 0, Backlight = 1

    for (int i = 0; i < 4; i++) {
        Wire.beginTransmission(lcdAddr);
        Wire.write(data[i]);
        Wire.endTransmission();
        delayMicroseconds(50); // Opóźnienie stabilizujące dla fizycznego HD44780
    }
}

// Ustawianie kursora (wiersz 0 lub 1, kolumna 0-15)
void setCursor(uint8_t col, uint8_t row) {
    uint8_t adresy[] = {0x80, 0xC0};
    lcd_send(adresy[row] + col, 0);
}

// Funkcja wypisująca tekst
void printLCD(String tresc) {
    for (int i = 0; i < tresc.length(); i++) {
        lcd_send(tresc[i], 1);
    }
}

// Czyszczenie ekranu
void clearLCD() {
    lcd_send(0x01, 0);
    delay(2);
}

void setup() {
    Wire.begin();
    delay(50);

    // Sekwencja inicjalizacji HD44780 w trybie 4-bitowym
    lcd_send(0x33, 0);
    delay(5);
    lcd_send(0x32, 0);
    delay(5);
    
    // Konfiguracja pracy
    lcd_send(0x28, 0); // 4-bit, 2 linie, font 5x8
    lcd_send(0x0C, 0); // Ekran włączony, kursor wyłączony
    lcd_send(0x06, 0); // Inkrementacja kursora
    clearLCD();

    WiFi.begin(ssid, password);
  
    // Czekanie na połączenie
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    clearLCD();
    printLCD("Czekam");
    }
    clearLCD();
    printLCD("Polaczono");

    configTime(3600, 3600, "pool.ntp.org");

    // Pobranie czasu w setup
    struct tm timeinfo;
    // Czekamy chwilę na synchronizację czasu z serwerem
    while (!getLocalTime(&timeinfo)) {
    delay(100);
    }
    // wifi koniec

    

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
