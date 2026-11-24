#include <LiquidCrystal.h>
// Display
LiquidCrystal lcd_1(12, 11, 5, 4, 3, 2);

// Loopstartup
const char text[] = "System initialisiert ";
const int textLen = sizeof(text) - 1;
int pos = 0;
unsigned long loopstartuptime = 0;
bool startup = true;

// Startup Elapse
unsigned long startuptime = 0;

// Button pin
const int button = 7;

// Resistor pins
const int widerstand1 = A1;
const int widerstand2 = A2;


unsigned long currentwiderstand1 = 2;
unsigned long currentwiderstand2 = 3;

void setup()
{
  lcd_1.begin(16, 2);
  Serial.begin(9600);
  Serial.println("System initialisiert");
}

void loop()
{
  if (startup){disablestartuploop();}
}

void disablestartuploop(){
  unsigned long elapsedstartup = millis() - startuptime;
  if (currentwiderstand1 == 0 | currentwiderstand2 == 0) {
    loopstartup();
  } else {
    startup = false;
  }
}

void loopstartup()
{
  unsigned long loopelapsedstartup = millis() - loopstartuptime;
  if (loopelapsedstartup > 500) {
    lcd_1.setCursor(0, 0);
  	for (int i = 0; i < 16; i++) {
      lcd_1.print(text[(pos + i) % textLen]);
    }
    pos = (pos + 1) % textLen;
    loopstartuptime = millis();
  }
}