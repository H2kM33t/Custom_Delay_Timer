#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Relay and timing
unsigned long KEYPAD_DELAY = 1000;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
bool currentRelayIsAC1 = true;  // true = AC1, false = AC2
char currentRelayLabel[4] = "AC1";

// EEPROM addresses
const int EEPROM_DELAY_ADDR = 0;
const int EEPROM_RELAY_ADDR = EEPROM_DELAY_ADDR + sizeof(KEYPAD_DELAY);

void setRelayState(bool isAC1) {
  if (isAC1) {
    PORTC &= ~(1 << PC0); // AC1 ON
    PORTC |= (1 << PC1);  // AC2 OFF
    strcpy(currentRelayLabel, "AC1");
  } else {
    PORTC |= (1 << PC0);  // AC1 OFF
    PORTC &= ~(1 << PC1); // AC2 ON
    strcpy(currentRelayLabel, "AC2");
  }
  currentRelayIsAC1 = isAC1;
}

void setup() {
  DDRC |= (1 << PC0) | (1 << PC1); // Output mode for both relays
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Load delay
  EEPROM.get(EEPROM_DELAY_ADDR, KEYPAD_DELAY);
  if (KEYPAD_DELAY > 9000) KEYPAD_DELAY = 1000;

  // Load relay state
  byte savedRelay = EEPROM.read(EEPROM_RELAY_ADDR);
  if (savedRelay > 1) savedRelay = 0; // sanitize
  setRelayState(savedRelay == 0);     // true if 0 => AC1

  lcd.clear();  // Start clean
}

void loop() {
  currentMillis = millis();
  unsigned long elapsedTime = currentMillis - previousMillis;

  // Keypad input
  char customKey = customKeypad.getKey();
  if (customKey >= '0' && customKey <= '9') {
    KEYPAD_DELAY = (customKey - '0') * 1000UL;
    EEPROM.put(EEPROM_DELAY_ADDR, KEYPAD_DELAY);
    Serial.print("Delay set to: ");
    Serial.println(KEYPAD_DELAY);
  }

  // Toggle relay after full delay only
  if (elapsedTime >= KEYPAD_DELAY) {
    previousMillis = currentMillis;
    setRelayState(!currentRelayIsAC1); // Toggle state
    EEPROM.write(EEPROM_RELAY_ADDR, currentRelayIsAC1 ? 0 : 1); // Save new state
  }

  // --- LCD Update ---
  lcd.setCursor(0, 0);
  lcd.print("Delay:");
  lcd.print(KEYPAD_DELAY / 1000);
  lcd.print("s   ");

  lcd.setCursor(12, 0);
  lcd.print(currentRelayLabel);

  lcd.setCursor(0, 1);
  lcd.print("Elapsed:");
  lcd.print((millis() - previousMillis) / 1000 + 1);
  lcd.print("s   ");
}
