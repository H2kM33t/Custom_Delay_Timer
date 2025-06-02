Project Title: Dual Relay Timer Controller with LCD and Keypad
Overview:
This Arduino-based system controls two AC relays (AC1 and AC2) in an alternating manner based on a user-defined delay. The delay can be set using a 4x4 keypad, and real-time status is displayed on a 16x2 I2C LCD. Relay states and delay settings are stored in EEPROM to retain them across power cycles.

Hardware Components:
Arduino board (e.g., Uno)
16x2 I2C LCD (0x27 I2C address)
4x4 Keypad
Two relays connected to PC0 and PC1 (digital pins A0 and A1)
EEPROM (built-in on most Arduino boards)
Contactor
AC loads (connected through the relays)

Key Functionalities:
LCD Display:

Line 1: Displays current delay and active relay (AC1 or AC2).

Line 2: Shows elapsed time since the last relay switch.

Keypad Input:

Numbers 0 to 9 set the delay in seconds (1–9 seconds or 0 = 0s, though you sanitize delay > 9s).

Delay is saved to EEPROM for persistence.

Relay Control:

Alternates between AC1 and AC2 at each delay interval.

Relay states are managed using direct port manipulation for speed and efficiency (PORTC).

Current relay state is saved in EEPROM.

EEPROM Use:

Stores:

KEYPAD_DELAY (in ms) at address 0.

Relay state (0 for AC1, 1 for AC2) at the next address.

Timing Logic:
millis() is used for non-blocking delay control.

System checks whether the elapsed time since last toggle exceeds the set delay.

If yes, it toggles the relay and updates the display accordingly.

Code Highlights:
PORTC control enables faster switching:

PORTC &= ~(1 << PC0); // Set PC0 LOW (ON for AC1)
PORTC |= (1 << PC1);  // Set PC1 HIGH (OFF for AC2)
Uses Keypad and LiquidCrystal_I2C libraries for ease of integration.

Updates display and saves delay without rebooting or interrupting the relay logic.