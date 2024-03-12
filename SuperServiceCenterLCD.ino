/*
 * Betty Brinn Children's Museum
 * Super Service Center
 * Diagnostic Tool
 *
 * Pete Prodoehl <pprodoehl@bbcmkids.org>
 * 2015-07-28
 *
 *
 * Based on Serial Enabled LCD Kit by Jim Lindblom - SparkFun Electronics
 * For more info see the links below:
 *
 * https://github.com/mr1337357/Serial-LCD-Kit
 *
 * https://www.sparkfun.com/products/10097
 *
 * https://www.sparkfun.com/tutorials/289
 *
 * License: Beerware - As long as you retain this notice you can
 * do whatever you want with this stuff. If you meet Jim some day,
 * and you think this stuff is worth it, you can buy him a beer
 * in return.
 *
 *
 * Uses the LiquidCrystal Arduino library to control the LCD
 * --------------------------------------------------------
 * http://www.arduino.cc/en/Tutorial/LiquidCrystal
 * Much thanks to: Limor Fried, Tom Igoe, David A. Mellis
 * for their fantastic work on this library!
 *
 */

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <TrueRandom.h>
#include <Bounce.h>

// --- EEPROM ADDRESS DEFINITIONS
#define LCD_BACKLIGHT_ADDRESS 1  // EEPROM address for backlight setting
#define BAUD_ADDRESS 2  // EEPROM address for Baud rate setting
#define SPLASH_SCREEN_ADDRESS 3 // EEPROM address for splash screen on/off
#define ROWS_ADDRESS 4  // EEPROM address for number of rows
#define COLUMNS_ADDRESS 5  // EEPROM address for number of columns

// --- SPECIAL COMMAND DEFINITIONS
#define BACKLIGHT_COMMAND 128  // 0x80
#define SPECIAL_COMMAND 254 // 0xFE
#define BAUD_COMMAND 129  // 0x81

// --- ARDUINO PIN DEFINITIONS
uint8_t RSPin = 2;
uint8_t RWPin = 3;
uint8_t ENPin = 4;
uint8_t D4Pin = 5;
uint8_t D5Pin = 6;
uint8_t D6Pin = 7;
uint8_t D7Pin = 8;
uint8_t BLPin = 9;
uint8_t MagPin = 10;
uint8_t BeepPin = 11;

Bounce MagSwitch = Bounce(MagPin, 50);

char inKey;  // Character received from serial input
uint8_t Cursor = 0;  // Position of cursor, 0 is top left, (rows*columns)-1 is bottom right
uint8_t LCDOnOff = 1;  // 0 if LCD is off
uint8_t blinky = 0;  // Is 1 if blinky cursor is on
uint8_t underline = 0; // Is 1 if underline cursor is on
uint8_t splashScreenEnable = 1;  // 1 means splash screen is enabled
uint8_t characters; // rows * columns
byte customchar[8] = {
  0, 17, 0, 0, 17, 14, 0
};

// initialize the LCD at pins defined above
LiquidCrystal lcd(RSPin, RWPin, ENPin, D4Pin, D5Pin, D6Pin, D7Pin);

/* ----------------------------------------------------------
 In the setup() function, we'll read the previous baud,
 screen size, backlight brightness, and splash screen state
 from EEPROM. Serial will be started at the proper baud, the
 LCD will be initialized, backlight turned on, and splash
 screen displayed (or not) according to the EEPROM states.
 ----------------------------------------------------------*/
void setup() {
  // initialize the serial communications:
  setBaudRate(EEPROM.read(BAUD_ADDRESS));

  // set up the LCD's number of rows and columns:
  lcd.begin(16, 2);

  // Set up the backlight
  pinMode(BLPin, OUTPUT);
  setBacklight(EEPROM.read(LCD_BACKLIGHT_ADDRESS));

  // Do splashscreen if set
  splashScreenEnable = EEPROM.read(SPLASH_SCREEN_ADDRESS);
  if (splashScreenEnable != 0)
  {
    lcd.print("STARTING ");
    lcd.setCursor(0, 1);
    lcd.print("DIGANOSTICS...");
    tone(BeepPin, 1300, 100);
    delay(2000);
    lcd.clear();
    Serial.println("boardworking");
  }
  randomSeed(analogRead(0));

  pinMode(MagPin, INPUT_PULLUP);
  pinMode(BeepPin, OUTPUT);

}

/*----------------------------------------------------------
 In loop(), we wait for a serial character to be
 received. Once received, the character is checked against
 all the special commands if it's not a special command the
 character (or tab, line feed, etc.) is displayed. If it is
 a special command another loop will be entered and we'll
 again wait for any further characters that are needed to
 finish the command.

 The code is looking for the magnetic switch to be activated,
 which causes the progress bar to display, and the choose if
 the part is good or bad, and plays the appropriate tone.
 Values and verbiage can be easily changed as desired.
 ----------------------------------------------------------*/
void loop() {

  MagSwitch.update();

  if (MagSwitch.fallingEdge()) {
  Serial.println("switchworking");
    lcd.setCursor(0, 0);
    lcd.print("TESTING PART...");
    tone(BeepPin, 900, 100);

    progressbar();

    delay(250);

    int decide = random(1, 3);

    lcd.clear();
    lcd.setCursor(0, 0);

    if (decide > 1) {
      lcd.print("REPLACE PART");
      tone(BeepPin, 500, 500);
    }
    else {
      lcd.print("PART IS GOOD");
      tone(BeepPin, 700, 200);
    }

    delay(2500);
    lcd.clear();
    delay(250);

  }

  lcd.setCursor(0, 0);
  lcd.print("SCAN TO ");
  lcd.setCursor(0, 1);
  lcd.print("TEST PART...");


}


/*----------------------------------------------------------
The progress bar is drawn using the code below. The call to
lcd.write(255) prints a block character. This can be changed
to any character desired. The delay in each loop determines
the speed of the animation, and the beeps can be altered if
so desired.
 ----------------------------------------------------------*/
void progressbar() {

  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.write(255);
    delay(35);
    tone(BeepPin, 100, 10);
  }
  for (int i = 16; i > 0; i--) {
    lcd.setCursor(i, 1);
    lcd.print(" ");
    delay(35);
    tone(BeepPin, 100, 10);
  }
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.write(255);
    delay(35);
    tone(BeepPin, 100, 10);
  }
  for (int i = 16; i > 0; i--) {
    lcd.setCursor(i, 1);
    lcd.print(" ");
    delay(35);
    tone(BeepPin, 100, 10);
  }


}







/* ----------------------------------------------------------
 SpecialCommands() is reached if SPECIAL_COMMAND is received.
 This function will wait for another character from the serial
 input and then perform the desired command. If a command is
 not recognized, nothing further happens and we jump back into
 loop().
 ----------------------------------------------------------*/
void SpecialCommands()
{
  // Wait for the next character
  while (Serial.available() == 0)
    ;
  inKey = Serial.read();
  // Clear Display
  if (inKey == 1)
  {
    Cursor = 0;
    lcd.clear();
  }
  // Move cursor right one
  else if (inKey == 20)
    Cursor++;
  // Move cursor left one
  else if (inKey == 16)
    Cursor--;
  // Scroll right
  else if (inKey == 28)
    lcd.scrollDisplayRight();
  // Scroll left
  else if (inKey == 24)
    lcd.scrollDisplayLeft();
  // Turn display on
  else if ((inKey == 12) && (LCDOnOff == 0))
  {
    LCDOnOff = 1;
    lcd.display();
  }
  // Turn display off
  else if (inKey == 8)
  {
    LCDOnOff = 0;
    lcd.noDisplay();
  }
  // Underline Cursor on
  else if (inKey == 14)
  {
    underline = 1;
    blinky = 0;
    lcd.noBlink();
    lcd.cursor();
  }
  // Underline Cursor off
  else if ((inKey == 12) && (underline == 1))
  {
    underline = 0;
    lcd.noCursor();
  }
  // Blinking box cursor on
  else if (inKey == 13)
  {
    lcd.noCursor();
    lcd.blink();
    blinky = 1;
    underline = 0;
  }
  // Blinking box cursor off
  else if ((inKey == 12) && (blinky = 1))
  {
    blinky = 0;
    lcd.noBlink();
  }
  // Set Cursor position
  else if ((inKey & 0xFF) == 128)
  {
    // Wait for the next character
    while (Serial.available() == 0)
      ;
    inKey = Serial.read();
    Cursor = inKey;
  }
  else if (inKey == 30)
  {
    if (splashScreenEnable)
      splashScreenEnable = 0;
    else
      splashScreenEnable = 1;
    EEPROM.write(SPLASH_SCREEN_ADDRESS, splashScreenEnable);
  }
}

/* ----------------------------------------------------------
 LCDDisplay() receives a single character and displays it
 depending on what value is in Cursor. We also do a bit of
 manipulation on Cursor, if it is beyond the screen size.
 Finally the Cursor is advanced one value, before the function
 is exited.
 ----------------------------------------------------------*/
void LCDDisplay(char character)
{
  int currentRow = 0;
  characters = 32;

  // If Cursor is beyond screen size, get it right
  while (Cursor >= characters)
    Cursor -= characters;
  while (Cursor < 0)
    Cursor += characters;

  if (Cursor >= 16)
    currentRow = Cursor / 16;

  lcd.setCursor(Cursor % 16, currentRow);
  lcd.write(character);

  Cursor++;
}

/* ----------------------------------------------------------
 setBacklight() is called from SpecialCommands(). It receives
 a backlight setting between 0 and 255. The backlight is set
 accordingly (via analogWrite()). Before exit the new backlight
 value is written to EEPROM.
 ----------------------------------------------------------*/
void setBacklight(uint8_t backlightSetting)
{
  analogWrite(BLPin, backlightSetting);
  EEPROM.write(LCD_BACKLIGHT_ADDRESS, backlightSetting);
}

/* ----------------------------------------------------------
 setBaudRate() is called from SpecialCommands(). It receives
 a baud rate setting balue that should be between 0 and 10.
 The baud rate is then set accordingly, and the new value is
 written to EEPROM. If the EEPROM value hasn't been written
 before (255), this function will default to 9600. If the value
 is out of bounds 10<baud<255, no action is taken.
 ----------------------------------------------------------*/
void setBaudRate(uint8_t baudSetting)
{
  // If EEPROM is unwritten (0xFF), set it to 9600 by default
  if (baudSetting == 255)
    baudSetting = 4;

  switch (baudSetting)
  {
    case 0:
      Serial.begin(300);
      break;
    case 1:
      Serial.begin(1200);
      break;
    case 2:
      Serial.begin(2400);
      break;
    case 3:
      Serial.begin(4800);
      break;
    case 4:
      Serial.begin(9600);
      break;
    case 5:
      Serial.begin(14400);
      break;
    case 6:
      Serial.begin(19200);
      break;
    case 7:
      Serial.begin(28800);
      break;
    case 8:
      Serial.begin(38400);
      break;
    case 9:
      Serial.begin(57600);
      break;
    case 10:
      Serial.begin(115200);
      break;
  }
  if ((baudSetting >= 0) && (baudSetting <= 10))
    EEPROM.write(BAUD_ADDRESS, baudSetting);
}




