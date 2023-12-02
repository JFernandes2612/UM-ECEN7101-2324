#include <AccelStepper.h>
#define MotorInterfaceType 4

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 

#define OLED_MOSI   8
#define OLED_CLK   9
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(4,2, A5); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#define PIN_FAN 10

#define PIN_BUTTON 7

int button_pressed = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define PIN_RED 3
#define PIN_GREEN 5
#define PIN_BLUE 6
int counter = 0;
int total_counter = 0;
int numColors = 255;

char datestring[20];

void setup() {
  Serial.begin(9600);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Rtc.Begin();

  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //Rtc.SetDateTime(compiled);

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_FAN, OUTPUT);
  analogWrite(PIN_FAN, 255);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(3.0); 
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.println("Close");
  getDateTime();
  display.setTextSize(1.0);
  display.println(datestring);
  display.display();
}
  
void loop() {
  if (button_pressed)
  {
    float colorNumber = counter > numColors ? counter - numColors: counter;
    
    float hue = (colorNumber / float(numColors)) * 360;
    long color = HSBtoRGB(hue, 1, 1); 
    
    int red = color >> 16 & 255;
    int green = color >> 8 & 255;
    int blue = color & 255;

    setColor(red, green, blue);
    
    counter = (counter + 3) % (numColors * 2);
    total_counter++;

    if (total_counter >= numColors)
    {
      analogWrite(PIN_FAN, 255);
      button_pressed = 0;
    }

    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(3.0);
    display.println("Opened");
    getDateTime();
    display.setTextSize(1.0);
    display.println(datestring);
    display.display();
  } else {
    if (digitalRead(A3))
      setColor(255, 255, 255);
    else
      setColor(0, 0, 0);

    if (digitalRead(PIN_BUTTON))
    {
      button_pressed = 1;
      total_counter = 0;
      counter = 0;
      analogWrite(PIN_FAN, 0);
    }

    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(3.0);
    display.println("Closed");
    getDateTime();
    display.setTextSize(1.0);
    display.println(datestring);
    display.display();

    delay(10);
  }
}

void setColor(unsigned char red, unsigned char green, unsigned char blue) 
{        
    analogWrite(PIN_RED, red);
    analogWrite(PIN_GREEN, green);
    analogWrite(PIN_BLUE, blue);
} 

long HSBtoRGB(float _hue, float _sat, float _brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    
    if (_sat == 0.0) {
        red = _brightness;
        green = _brightness;
        blue = _brightness;
    } else {
        if (_hue == 360.0) {
            _hue = 0;
        }

        int slice = _hue / 60.0;
        float hue_frac = (_hue / 60.0) - slice;

        float aa = _brightness * (1.0 - _sat);
        float bb = _brightness * (1.0 - _sat * hue_frac);
        float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
        
        switch(slice) {
            case 0:
                red = _brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = _brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = _brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = _brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = _brightness;
                break;
            case 5:
                red = _brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    long ired = red * 255.0;
    long igreen = green * 255.0;
    long iblue = blue * 255.0;
    
    return long((ired << 16) | (igreen << 8) | (iblue));
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void getDateTime()
{
  RtcDateTime dt = Rtc.GetDateTime();

  snprintf_P(datestring, 
          countof(datestring),
          PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
          dt.Day(),
          dt.Month(),
          dt.Year(),
          dt.Hour(),
          dt.Minute(),
          dt.Second() );
}