# Diagnostic Tool for Super Service Center

## Make sure when uploading the code in Arduino IDE to make sure too change the board type to "Arduino Duemilanove or Diecimila"
## Make sure to use an FDTI breakout to upload the code and connect the pins to the 5-pin holes labeled GND, GND, 5V, RXI, TXO, DTR.
## Then upload the SuperServiceCenterLCD.ino file to the lcd and it should work more information below 


## Overview
This Arduino sketch is designed for the Super Service Center Diagnostic Tool at the Betty Brinn Children's Museum. It controls an LCD display to provide diagnostic feedback for parts testing, using a magnetic switch to trigger the testing process. The tool displays a progress bar, indicates whether a part is good or needs replacement, and provides auditory feedback via tones.

## Author
- **Pete Prodoehl** <pprodoehl@bbcmkids.org>
- **Date**: 2015-07-28

## Based On
This code is based on the Serial Enabled LCD Kit by Jim Lindblom from SparkFun Electronics. Relevant resources:
- [GitHub Repository](https://github.com/mr1337357/Serial-LCD-Kit)
- [SparkFun Product Page](https://www.sparkfun.com/products/10097)
- [SparkFun Tutorial](https://www.sparkfun.com/tutorials/289)
- **License**: Beerware - Retain the notice and use freely. If you meet Jim Lindblom, consider buying him a beer.

## Dependencies
The code uses the following Arduino libraries:
- **LiquidCrystal**: For controlling the LCD display [](http://www.arduino.cc/en/Tutorial/LiquidCrystal).
  - Credits: Limor Fried, Tom Igoe, David A. Mellis
- **EEPROM**: For storing settings like backlight, baud rate, and splash screen state.
- **TrueRandom**: For generating random decisions during part testing.
- **Bounce**: For debouncing the magnetic switch input.

## Hardware Setup
### Arduino Pin Definitions
- **RS Pin**: 2
- **RW Pin**: 3
- **EN Pin**: 4
- **D4 Pin**: 5
- **D5 Pin**: 6
- **D6 Pin**: 7
- **D7 Pin**: 8
- **Backlight Pin (BLPin)**: 9 (PWM for brightness control)
- **Magnetic Switch Pin (MagPin)**: 10 (with pull-up resistor)
- **Buzzer Pin (BeepPin)**: 11 (for auditory feedback)

### LCD Configuration
- **Default Size**: 16 columns, 2 rows
- **Custom Character**: Defined for potential use (e.g., progress bar or icons)

## EEPROM Usage
The sketch stores configuration settings in EEPROM to persist across power cycles:
- **LCD_BACKLIGHT_ADDRESS (1)**: Backlight brightness (0–255)
- **BAUD_ADDRESS (2)**: Serial baud rate setting (0–10, mapping to 300–115200 baud)
- **SPLASH_SCREEN_ADDRESS (3)**: Splash screen enable/disable (0 or 1)
- **ROWS_ADDRESS (4)**: Number of LCD rows
- **COLUMNS_ADDRESS (5)**: Number of LCD columns

## Special Commands
The tool supports special commands via serial input, prefixed with `SPECIAL_COMMAND (254)`:
- **Clear Display**: `254 1`
- **Move Cursor Right**: `254 20`
- **Move Cursor Left**: `254 16`
- **Scroll Right**: `254 28`
- **Scroll Left**: `254 24`
- **Display On**: `254 12` (if off)
- **Display Off**: `254 8`
- **Underline Cursor On**: `254 14`
- **Underline Cursor Off**: `254 12` (if underline is on)
- **Blinking Cursor On**: `254 13`
- **Blinking Cursor Off**: `254 12` (if blinking is on)
- **Set Cursor Position**: `254 128 <position>`
- **Toggle Splash Screen**: `254 30`
- **Set Backlight**: `254 128 <brightness>` (0–255)
- **Set Baud Rate**: `254 129 <setting>` (0–10)

## Functionality
### Setup
- Initializes serial communication based on EEPROM-stored baud rate (defaults to 9600 if unset).
- Configures the LCD (16x2 by default).
- Sets backlight brightness from EEPROM.
- Displays a splash screen ("STARTING DIAGNOSTICS...") with a 1300 Hz tone for 2 seconds if enabled.
- Initializes the magnetic switch with a 50ms debounce and the buzzer pin.

### Main Loop
- Monitors the magnetic switch for a falling edge (activation).
- On activation:
  - Displays "TESTING PART..." with a 900 Hz tone.
  - Shows a progress bar animation using block characters (ASCII 255).
  - Randomly decides if the part is good or bad (33% chance of "PART IS GOOD", 67% chance of "REPLACE PART").
  - Plays a tone (700 Hz for good, 500 Hz for bad) and displays the result for 2.5 seconds.
- Default display: "SCAN TO TEST PART...".

### Progress Bar
- Animates a full row of block characters (16 columns) on the second row, moving left to right and back, twice.
- Each step includes a 100 Hz tone for 10ms and a 35ms delay for animation speed.

## Key Functions
- **SpecialCommands()**: Processes serial commands for controlling the LCD (e.g., cursor movement, display on/off).
- **LCDDisplay(char)**: Displays a character at the current cursor position, handling row/column wrapping.
- **setBacklight(uint8_t)**: Sets backlight brightness via PWM and saves to EEPROM.
- **setBaudRate(uint8_t)**: Sets serial baud rate based on a setting value (0–10) and saves to EEPROM.
- **progressbar()**: Animates the progress bar with block characters and tones.

## Usage Notes
- Connect a magnetic switch to pin 10 to trigger part testing.
- Use a buzzer on pin 11 for auditory feedback.
- Serial commands can be sent to control the LCD dynamically.
- The random decision for part testing can be adjusted by modifying the `random(1, 3)` range or logic in the `loop()` function.
- To disable the splash screen, send the command `254 30` or modify the EEPROM value at `SPLASH_SCREEN_ADDRESS`.
