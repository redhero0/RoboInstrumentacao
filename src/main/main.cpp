#include <Arduino.h>
#include <Ultrasonic.h>


#define ECHO 13
#define TRIG 12

/*
------------------  
|  HC-SR04 (1)   |   
------------------  
|  VCC   |  +5V  | 
|  GND   |  GND  |   
|  TRIG  |  13   |  
|  ECHO  |  12   |   

*/
#define TRIG_PIN 9 // Pin connected to the ultrasonic sensor's TRIG
#define ECHO_PIN 8 // Pin connected to the ultrasonic sensor's ECHO

// objects for the hc-sr04
Ultrasonic ultrasonic(TRIG, ECHO); 

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN A1     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

#include <AFMotor.h>
#include <Arduino.h>

AF_DCMotor left_motor(4);
AF_DCMotor right_motor(3);

void setup() {
  Serial.begin(115200);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void displayInfo(float temp, float humidity, int distance)
{
  // Clear display
  display.clearDisplay();

  // Display temperature and humidity side by side
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("T: ");
  display.print(temp);
  display.print(" ");
  display.cp437(true);
  display.write(167); // ° symbol
  display.print("C");

  display.setCursor(70, 0); // Move to the right for humidity
  display.print("H: ");
  display.print(humidity);
  display.print("%");

  // Display distance below
  display.setCursor(0, 20);
  display.print("D: ");
  display.print(distance);
  display.print(" cm");

  display.display();
}

void motorControl (int left_speed, int right_speed, int duration)
{
  // Turn on motor
  left_motor.setSpeed(left_speed);
  right_motor.setSpeed(right_speed);
	left_motor.run(FORWARD);
  right_motor.run(FORWARD);
  delay(duration);
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}

void loop() {
  // Read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int d = ultrasonic.read(CM);

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  displayInfo(t, h, d);
  motorControl(200, 200, 500);
}
