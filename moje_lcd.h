#include <Wire.h>

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
void set_up() {
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
}
