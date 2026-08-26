#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <SPI.h>
#include <DHT.h>


#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22 (AM2302)

#define TOUCH_DOUT  39
#define TOUCH_DIN   32 
#define TOUCH_CS    33
#define TOUCH_CLK   25

TFT_Touch touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT);


DHT dht(DHTPIN, DHTTYPE);

TFT_eSPI my_lcd;       // Invoke custom library
TFT_eSPI_Button btn; // invoke button helper 


float celsiusToFahrenheit(float celsius) {
  return( celsius * 1.8 + 32);
}

bool isFahrenheit = true; // Flag to indicate if the temperature is in Fahrenheit

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();



  my_lcd.init();
  my_lcd.setRotation(0);

  my_lcd.fillScreen(TFT_BLACK);
  my_lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  my_lcd.setTextSize(2);

  touch.setCal(3800, 500, 230, 3670, 240, 320, 0); // Set calibration values for the touch screen

  btn.initButton(&my_lcd, 50, 100, 100, 40, TFT_WHITE, TFT_BLUE, TFT_WHITE, "Click", 2);
  btn.drawButton();


 // my_lcd.setCursor(50, 100);
  //my_lcd.print("Hello, World!");

  



  int testWidth = my_lcd.width();
  int testHeight = my_lcd.height();

  Serial.print("Width: ");
  Serial.println(testWidth);
  Serial.print("Height: ");
  Serial.println(testHeight);
}

void loop() {
  // put your main code here, to run repeatedly:
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  my_lcd.setCursor(0, 20);
  my_lcd.print("Humidity: ");
  my_lcd.print(humidity);
  my_lcd.print("%");
  
  my_lcd.setCursor(0, 40);
  my_lcd.print("Temperature: ");
  
  if (isFahrenheit) {
    my_lcd.print(celsiusToFahrenheit(temperature));
    my_lcd.print("F");
  } else {
    my_lcd.print(temperature);
    my_lcd.print("C");
  }

    if (touch.Pressed()){
      Serial.println("Touch detected!");
      Serial.print(touch.X());
      Serial.print(", ");
      Serial.println(touch.Y());

    }

  delay(2000); // Delay for 2 seconds before the next reading
}

