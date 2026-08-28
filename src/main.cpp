#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <SPI.h>
#include <DHT.h>

// pin definitions
#define DHTPIN 27     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22 (AM2302)

#define TOUCH_DOUT  39
#define TOUCH_DIN   32 
#define TOUCH_CS    33
#define TOUCH_CLK   25



  // Hardware objects
DHT dht(DHTPIN, DHTTYPE);

TFT_eSPI lcdDisplay;       // Invoke lcd library

TFT_Touch touch(
  TOUCH_CS, 
  TOUCH_CLK, 
  TOUCH_DIN, 
  TOUCH_DOUT
);

TFT_eSPI_Button btn; // invoke button helper 


// sensor data 
float humidity;
float temperature;

// timing
unsigned long previousSensorTime= 0;
const unsigned long sensorInterval = 2000;

unsigned long lastButtonPressTime = 0;
const unsigned long buttonInterval = 350;



// celsius conversion
float celsiusToFahrenheit(float celsius) {
  return( celsius * 1.8 + 32);
}



// sensor reading state
bool isFahrenheit = true; // Flag to indicate if the temperature is in Fahrenheit


void readSensor(){
   float newHumidity = dht.readHumidity();
  float newTemperature = dht.readTemperature();

  // sensor failure check
  if (isnan(newTemperature) || isnan(newHumidity)){
    Serial.println("Sensor Reading Failed, check humidity or temp value");
  } else {
    humidity = newHumidity;
    temperature = newTemperature;
  }
}
  
 int tlabelWidth;
 

void updateDisplay(){
  // Temperature
  lcdDisplay.setCursor(tlabelWidth + 10, 20);
  



    if (isFahrenheit) {
    lcdDisplay.print(celsiusToFahrenheit(temperature));
   
    lcdDisplay.print("F");
  } else {
    lcdDisplay.print(temperature);
   
    lcdDisplay.print("C");
  }


  // Humidity
  lcdDisplay.setCursor(tlabelWidth + 10, 40);

  lcdDisplay.print(humidity);

  lcdDisplay.print("%");

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();


// init screen and set rot
  lcdDisplay.init();
  lcdDisplay.setRotation(0);

  // screen color black
  lcdDisplay.fillScreen(TFT_BLACK);
 

  touch.setCal(3800, 500, 230, 3670, 240, 320, 0); // Set calibration values for the touch screen

  // initialize button with settings and draw
  btn.initButton(&lcdDisplay, 50, 100, 100, 40, TFT_WHITE, TFT_BLUE, TFT_WHITE, "F/C", 2);
  btn.drawButton();


  // text color and size
   lcdDisplay.setTextColor(TFT_GREEN, TFT_BLACK);
  lcdDisplay.setTextSize(2);


  int testWidth = lcdDisplay.width();
  int testHeight = lcdDisplay.height();

  Serial.print("Width: ");
  Serial.println(testWidth);
  Serial.print("Height: ");
  Serial.println(testHeight);


  lcdDisplay.setCursor(0, 20);
  
   lcdDisplay.print("Temperature: ");
  
  lcdDisplay.setCursor(0, 40);
  
     lcdDisplay.print("Humidity: ");

  

 
 tlabelWidth = lcdDisplay.textWidth("Temperature: ");
  

}

void loop() {
  // put your main code here, to run repeatedly:
  
  // Store current time in milliseconds.
  unsigned long currentTime = millis();

if ( currentTime - previousSensorTime >= sensorInterval) {


  previousSensorTime = currentTime;

  readSensor();

}

updateDisplay();



    if (touch.Pressed()){
      int x = touch.X();
      int y = touch.Y();


      btn.press(btn.contains(x, y));
    } else {
      btn.press(false);
    }

      if (btn.justPressed()){
        if (currentTime - lastButtonPressTime >= buttonInterval){
        lastButtonPressTime = currentTime;

        isFahrenheit = !isFahrenheit;
      }
    }
  }



