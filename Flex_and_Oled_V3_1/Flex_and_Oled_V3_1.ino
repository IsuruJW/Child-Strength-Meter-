const int flexPin1 = A0;        // Pin connected to voltage divider output
const int flexPin2 = A1;        // Pin connected to voltage divider output
const int flexPin3 = A2;        // Pin connected to voltage divider output
const byte interruptPin = 2;    // For the reset switch

volatile float prev_avgAngle = 0;            // A global variable to store the average resistance

// Change following constants according to your project's design

const float VCC = 5;                    // voltage at Ardunio 5V line
const float R_DIV = 47000.0;            // resistor used to create a voltage divider
const long flatResistance = 10000;       // resistance when flat
const long bendResistance = 16000;      // resistance at 90 deg
int SP_delay = 10;                      // The delay within serial print commands.
int AD_delay = 1000;                    // The time delay of the welcome screen adafruit logo

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Serial.begin(9600);
  pinMode(flexPin1, INPUT);
  pinMode(flexPin2, INPUT);
  pinMode(flexPin3, INPUT);
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), resetbtn, CHANGE);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(AD_delay);
  display.clearDisplay();
}

void loop()
{
  // Read the ADC, and calculate voltage and resistance from it
  int ADCflex1 = analogRead(flexPin1);
  float Vflex1 = ADCflex1 * VCC / 1023.0;
  float Rflex1 = R_DIV * (VCC / Vflex1 - 1.0);

  int ADCflex2 = analogRead(flexPin2);
  float Vflex2 = ADCflex2 * VCC / 1023.0;
  float Rflex2 = R_DIV * (VCC / Vflex2 - 1.0);
  
  int ADCflex3 = analogRead(flexPin3);
  float Vflex3 = ADCflex3 * VCC / 1023.0;
  float Rflex3 = R_DIV * (VCC / Vflex3 - 1.0);

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle1 = map(Rflex1, flatResistance, bendResistance, 0, 100.0);
  angle1 = constrain(angle1,0,100);

  float angle2 = map(Rflex2, flatResistance, bendResistance, 0, 100.0);
  angle2 = constrain(angle2,0,100);
 
  float angle3 = map(Rflex3, flatResistance, bendResistance, 0, 100.0);
  angle3 = constrain(angle3,0,100);
  
   // Average Angle
  float avgAngle = (angle1 + angle2 + angle3) / 3;
  int display_val = int(avgAngle);    // remove the decimal points
  
  String k;             // to adjust the cursor
  if(display_val==100){
    k = "  ";
  }
  else{
    k = "   ";
  }

  if (avgAngle >= prev_avgAngle) { // Display the AVG strength and holds it
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print(" Strength  ");
    display.println(k + String(display_val));
    display.display();
    prev_avgAngle = avgAngle;
    delay(100);
  }
}

void resetbtn() {
  prev_avgAngle = 0;
}
