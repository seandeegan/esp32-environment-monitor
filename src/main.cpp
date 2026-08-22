#include <Arduino.h>
#include <TFT_eSPI.h>
//#include <TFT_Touch.h>
#include <SPI.h>
#include <DHT.h>


#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22 (AM2302)


DHT dht(DHTPIN, DHTTYPE);

TFT_eSPI my_lcd = TFT_eSPI();       // Invoke custom library

float celsiusToFahrenheit(float celsius) {
  return( celsius * 1.8 + 32);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  my_lcd.init();
  my_lcd.setRotation(0);

  my_lcd.fillScreen(TFT_BLACK);
  my_lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  my_lcd.setTextSize(2);


 // my_lcd.setCursor(50, 100);
  //my_lcd.print("Hello, World!");

    //my_lcd.setCursor(0, 0);
  //my_lcd.print("loc test");



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
  my_lcd.print(celsiusToFahrenheit(temperature));
  my_lcd.print("F");

  delay(2000);
}

