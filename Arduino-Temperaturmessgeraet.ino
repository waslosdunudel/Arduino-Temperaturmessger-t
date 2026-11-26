#include <LiquidCrystal.h>
#include <math.h>
// Display
LiquidCrystal lcd_1(12, 11, 5, 4, 3, 2);

// Loopstartup
const char text[] = "System initialisiert ";
const int textLen = sizeof(text) - 1;
int pos = 0;
unsigned long loopstartuptime = 0;
unsigned long displayratetime = 0;
int Displayzustand = 0;

// Startup Elapse
unsigned long startuptime = 0;

// Temperaturmessung mit NTC im Spannungsteiler
// Schaltung: +5 V -- Vorwiderstand 10 kOhm -- A0 -- NTC -- GND
const int SENSOR_PIN1 = A0;
const int SENSOR_PIN2 = A1;
const long R_VOR = 10000;              // Vorwiderstand in Ohm
const long R_NENN = 10000;             // NTC-Widerstand bei 25 °C in Ohm
const int B_WERT = 3950;               // B-Wert des NTC laut Datenblatt
const double KELVIN_OFFSET = 273.15;   // 0 °C in Kelvin
const double T_NENN = KELVIN_OFFSET + 25.0; // Nenntemperatur in Kelvin
const int button = 7;

void disablestartuploop(int bitwertNTC1, int bitwertNTC2);
double calctemperature(int bitwertNTC);
void displayerroronscreen(int errorcode);

void setup()
{

  lcd_1.begin(16, 2);
  Serial.begin(9600);
  
  pinMode(start_taster, button)
}

void loop()
{
  mainloop();
  display();
}

void mainloop()
{
  int bitwertNTC1 = analogRead(SENSOR_PIN1);
  int bitwertNTC2 = analogRead(SENSOR_PIN2);
  if (startup) {
    disablestartuploop(bitwertNTC1, bitwertNTC2);
  }
  double tCelsius1 = calctemperature(bitwertNTC1);
  double tCelsius2 = calctemperature(bitwertNTC2);
  Serial.println("Sensormessung 1:");
  Serial.print("Analog: ");
  Serial.println(bitwertNTC1);
  Serial.print("Temperatur: ");
  Serial.print(tCelsius1);
  Serial.println(" °C");
  Serial.println("Sensormessung 2:");
  Serial.print("Analog: ");
  Serial.println(bitwertNTC2);
  Serial.print("Temperatur: ");
  Serial.print(tCelsius2);
  Serial.println(" °C");
}

void display() {
  switch (displayzustand) {
    case 0: // Display aus und keine Daten mehr sammeln
      if (start_taster) {
        displayzustand = 1;
        Serial.println("System initialisiert");
        unsigned long zustand1_elapsed = millis() - zustand1_starttime;
      }
      break;
    case 1: // Display geht an und Initialisiert das Programm
      if (zustand1_elapsed < 2000) {
        startup();
      } else {
        displayzustand = 2;
        zustand2_starttime = millis();
      }
      break
  }
}

void startup()
{
  unsigned long loopelapsedstartup = millis() - loopstartuptime;
  if (loopelapsedstartup > 200) {
    lcd_1.setCursor(0, 0);
  	for (int i = 0; i < 16; i++) {
      lcd_1.print(text[(pos + i) % textLen]);
    }
    pos = (pos + 1) % textLen;
    loopstartuptime = millis();
  }
}

double calctemperature(int bitwertNTC) {
  if (bitwertNTC < 5 || bitwertNTC > 1018) {
    Serial.println("Sensorfehler: Schaltung pruefen");
    displayerroronscreen(1);
    return NAN;
  } else {
    
    // Widerstand des NTC berechnen
    double verhaeltnis = (double)bitwertNTC / 1024.0;
    double widerstandNTC = R_VOR * (verhaeltnis / (1.0 - verhaeltnis));

    // Temperatur mit der Beta-Formel berechnen
    double tKelvin = 1.0 / ((1.0 / T_NENN) + (1.0 / B_WERT) * log(widerstandNTC / R_NENN));
    double tCelsius = tKelvin - KELVIN_OFFSET;

    return tCelsius;
  }
}

void displaycurrenttemperatureonscreen(double tCelsius1, double tCelsius2) {
  unsigned long displayrate = millis() - displayratetime;
  if (displayrate > refreshrate){
  lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp1: ");
  lcd_1.print(tCelsius1, 2);
  lcd_1.print(" C");
  lcd_1.setCursor(0, 1);
  lcd_1.print("Temp2: ");
  lcd_1.print(tCelsius2, 2);
  lcd_1.print(" C");
    displayratetime = millis();
  }
  
}
void displayerroronscreen(int errorcode) {
  switch (errorcode) {
    case 1:
      Serial.println("Sensorfehler: Schaltung pruefen");
      lcd_1.clear();
      lcd_1.setCursor(0, 0);
      lcd_1.print("Sensorfehler " + String(errorcode));
      lcd_1.setCursor(0, 1);
      lcd_1.print("Schaltung pruefen");
      error = true;
      break;
    default:
      Serial.println("Unbekannter Fehlercode");
      break;
  }
}
