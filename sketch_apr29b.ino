#include <Servo.h> //includes the servo library
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Adafruit_TCS34725.h"

LiquidCrystal_I2C lcd_main(0x27, 20, 4); // Main 20x4 LCD
LiquidCrystal_I2C lcd_S1(0x24, 16, 2);   // LCD for S1
LiquidCrystal_I2C lcd_S2(0x23, 16, 2);   // LCD for S2
LiquidCrystal_I2C lcd_S3(0x25, 16, 2);   // LCD for S3
LiquidCrystal_I2C lcd_S4(0x26, 16, 2);   // LCD for S4
LiquidCrystal_I2C lcd_c(0x22, 16, 2);

Servo myservo;

#define ir_enter 2
#define ir_back  4

#define ir_car1 5
#define ir_car2 6
#define ir_car3 7
#define ir_car4 8
#define redpin 9
#define greenpin 10
#define bluepin 11

#define commonAnode true


byte gammatable[256];




int S1 = 0, S2 = 0, S3 = 0, S4 = 0;
int slot = 4;  
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
void setup() {
  Serial.begin(9600);

  pinMode(ir_car1, INPUT);
  pinMode(ir_car2, INPUT);
  pinMode(ir_car3, INPUT);
  pinMode(ir_car4, INPUT);

  pinMode(ir_enter, INPUT);
  pinMode(ir_back, INPUT);
  
  myservo.attach(3);
  myservo.write(90);
  
  // Initialize the main 20x4 LCD
  lcd_main.init();
  lcd_main.backlight();
  lcd_main.begin(20, 4);  
  lcd_main.setCursor(0, 1);
  lcd_main.print("    Car  parking  ");
  lcd_main.setCursor(0, 2);
  lcd_main.print("       System     ");
  delay(2000);
  lcd_main.clear();   

  // Initialize the additional 16x2 LCDs
  lcd_S1.init();
  lcd_S1.backlight();
  lcd_S1.begin(16, 2);
  lcd_S1.setCursor(0, 0);
  lcd_S1.print("S1 duration: ");
  lcd_S1.setCursor(0, 1);
  lcd_S1.print("S1 Price: ");

  lcd_S2.init();
  lcd_S2.backlight();
  lcd_S2.begin(16, 2);
  lcd_S2.setCursor(0, 0);
  lcd_S2.print("S2 duration: ");
  lcd_S2.setCursor(0, 1);
  lcd_S2.print("S2 Price: ");

  lcd_S3.init();
  lcd_S3.backlight();
  lcd_S3.begin(16, 2);
  lcd_S3.setCursor(0, 0);
  lcd_S3.print("S3 duration: ");
  lcd_S3.setCursor(0, 1);
  lcd_S3.print("S3(EV) Price:");

  lcd_S4.init();
  lcd_S4.backlight();
  lcd_S4.begin(16, 2);
  lcd_S4.setCursor(0, 0);
  lcd_S4.print("S4 duration: ");
  lcd_S4.setCursor(0, 1);
  lcd_S4.print("S4(EV) Price:");

  

  lcd_c.init();
  lcd_c.backlight();
  lcd_c.begin(16, 2);


 

  tcs.begin(); // Initialize color sensor

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  for (int i = 0; i < 256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
  }

  Read_Sensor();

  int total = S1 + S2 + S3 + S4;
  slot = 4 - total; // Update the slot count based on the available parking spaces
}

void loop() {
  Read_Sensor();

  int total = S1 + S2 + S3 + S4;
  slot = 4 - total; // Update the slot count based on the available parking spaces

  lcd_main.setCursor(0, 0);
  lcd_main.print("   Have Slot: "); 
  lcd_main.print(slot);
  lcd_main.print("    ");  

  lcd_main.setCursor(0, 1);
  if (S1 == 1) {
    lcd_main.print("S1:Fill ");
  } else {
    lcd_main.print("S1:Empty");
  }

  lcd_main.setCursor(10, 1);
  if (S2 == 1) {
    lcd_main.print("S2:Fill ");
  } else {
    lcd_main.print("S2:Empty");
  }

  lcd_main.setCursor(0, 2);
  if (S3 == 1) {
    lcd_main.print("S3:Fill ");
  } else {
    lcd_main.print("S3:Empty");
  }

  lcd_main.setCursor(10, 2);
  if (S4 == 1) {
    lcd_main.print("S4:Fill ");
  } else {
    lcd_main.print("S4:Empty");
  }

  if (digitalRead (ir_enter) == LOW) {
    myservo.write(180); // Open the servo
  }

  if (digitalRead (ir_back) == LOW) {
    myservo.write(90); // Close the servo
  }

  delay(100); // Add a small delay for stability
}

void Read_Sensor() {
  S1 = 0, S2 = 0, S3 = 0, S4 = 0;

  if (digitalRead(ir_car1) == 0) {
    S1 = 1;
  }
  if (digitalRead(ir_car2) == 0) {
    S2 = 1;
  }
  if (digitalRead(ir_car3) == 0) {
    S3 = 1;
  }
  if (digitalRead(ir_car4) == 0) {
    S4 = 1;
  }

  static bool prevS1 = LOW;
  static unsigned long prevTime = 0;
  
  if (S1 != prevS1) {
    if (S1 == HIGH) {
      prevTime = millis();
    } else {
      unsigned long duration = (millis() - prevTime) / 1000;
      lcd_S1.setCursor(13, 0); // Set cursor position on the additional LCD for S1
      lcd_S1.print(duration);
      lcd_S1.print(" s     ");
      lcd_S1.setCursor(9, 1); // Set cursor position on the additional LCD for S1
      
      lcd_S1.print(" Rs ");
      lcd_S1.print(duration*10);
      delay(duration+3000);
      lcd_S1.setCursor(13, 0); 
      lcd_S1.print("     "); 
      lcd_S1.setCursor(9, 1); 
      lcd_S1.print("      ");
    }
    prevS1 = S1;
  }

  static bool prevS2 = LOW;
  static unsigned long prevTimeS2 = 0;
  
  if (S2 != prevS2) {
    if (S2 == HIGH) {
      prevTimeS2 = millis();
    } else {
      unsigned long durationS2 = (millis() - prevTimeS2) / 1000;
      lcd_S2.setCursor(13, 0); // Set cursor position on the additional LCD for S2
      lcd_S2.print(durationS2);
      lcd_S2.print(" s     ");
      lcd_S2.setCursor(9, 1); // Set cursor position on the additional LCD for S1
      
      lcd_S2.print(" Rs ");
      lcd_S2.print(durationS2*10);
      delay(durationS2+3000);
      lcd_S2.setCursor(13, 0); 
      lcd_S2.print("     "); 
      lcd_S2.setCursor(9, 1); 
      lcd_S2.print("      ");
    }
    prevS2 = S2;
  }

  static bool prevS3 = LOW;
  static unsigned long prevTimeS3 = 0;
  
  if (S3 != prevS3) {
    if (S3 == HIGH) {
      prevTimeS3 = millis();
    } else {
      unsigned long durationS3 = (millis() - prevTimeS3) / 1000;
      lcd_S3.setCursor(13, 0); // Set cursor position on the additional LCD for S3
      lcd_S3.print(durationS3);
      lcd_S3.print(" s     ");
      lcd_S3.setCursor(9, 1); // Set cursor position on the additional LCD for S1
      
      lcd_S3.print(" Rs ");
      lcd_S3.print(durationS3*5);
      delay(durationS3+3000);
      lcd_S3.setCursor(13, 0); 
      lcd_S3.print("     "); 
      lcd_S3.setCursor(9, 1); 
      lcd_S3.print("      ");
    }
    prevS3 = S3;
  }

  static bool prevS4 = LOW;
  static unsigned long prevTimeS4 = 0;
  
  if (S4 != prevS4) {
    if (S4 == HIGH) {
      prevTimeS4 = millis();
    } else {
      unsigned long durationS4 = (millis() - prevTimeS4) / 1000;
      lcd_S4.setCursor(13, 0); // Set cursor position on the additional LCD for S4
      lcd_S4.print(durationS4);
      lcd_S4.print("s     ");
     
      lcd_S4.setCursor(9, 1); // Set cursor position on the additional LCD for S1
      
      lcd_S4.print(" Rs ");
      lcd_S4.print(durationS4*5);
      delay(durationS4+3000);
      lcd_S4.setCursor(13, 0); 
      lcd_S4.print("     "); 
      lcd_S4.setCursor(9, 1); 
      lcd_S4.print("      ");
    }
    prevS4 = S4;
  }

  
 float red, green, blue;
  
  // Read color at the entrance
  tcs.setInterrupt(false); // turn on LED
  delay(60); // takes 50ms to read
  tcs.getRGB(&red, &green, &blue);
  tcs.setInterrupt(true); // turn off LED
  
  lcd_c.clear();
  

  if (red >= 100 && red > green && red > blue) {
    lcd_c.setCursor(6, 0);
    lcd_c.print("E V"); // RED
    lcd_c.setCursor(1, 1);
    lcd_c.print("Not Detected");
  } else if (green >= 100 && green > red && green > blue) {
    lcd_c.setCursor(0, 0);
    lcd_c.print("Electric Vehicle"); // GREEN
    lcd_c.setCursor(0, 1);
    lcd_c.print("Use Slot S3 & S4");
  } else if (blue >= 100 && blue > red && blue > green) {
    lcd_c.setCursor(6, 0);
    lcd_c.print("E V");  // BLUE
    lcd_c.setCursor(1, 1);
    lcd_c.print("Not Detected");
  } else {
    lcd_c.setCursor(6, 0);
    lcd_c.print("E V"); // UNKNOWN
    lcd_c.setCursor(1, 1);
    lcd_c.print("Not Detected");
  }
}
