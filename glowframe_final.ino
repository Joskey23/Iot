#include <Adafruit_APDS9960.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#define BLYNK_TEMPLATE_ID "TMPL6Nwo-ZJFe"
#define BLYNK_TEMPLATE_NAME "GLOWFLOW"
#include <BlynkSimpleEsp32.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define INT_PIN 25
#define C4 261.63
#define D4 293.66
#define E4 329.63
#define G4 392.00
#define WHOLE 1000
#define HALF (WHOLE / 2)
#define QUARTER (WHOLE / 4)
#define buzzer 23

#define WIFI_SSID "CyberiaB152_2.4Ghz"
#define WIFI_PASSWORD "B152@2002"
#define BLYNK_TEMPLATE_ID "TMPL6nKeHjJFM"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "fvpDOSEAbIfE6D3pxilF6X4-fT983OFO"
#define APP_DEBUG  //#define BLYNK_DEBUG

#define LED_PIN 16
#define LED_COUNT 60
#define BRIGHTNESS 50

Adafruit_APDS9960 apds;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
char auth[] = "eSaU9onK7L9NqYqzpv-63FuGL-Ct9ovd";
//char ssid[] = "JojoTocatoca";
//char pass[] = "Amongus2373";
char ssid[] = "CyberiaB152_2.4Ghz";
char pass[] = "B152@2002";

uint32_t color = 0;
int distance;
bool hibernate = true;
bool GestureON= false;
const int HIBERNATION_DISTANCE = 200;  // Distance threshold for hibernation in millimeters
unsigned long lastGestureTime = 0;
const unsigned long gestureInterval = 10000; // 10 seconds
bool ledState = false; // LED strip state
bool done_interaction=false;
void setup() {
  Serial.begin(115200);
  delay(100);
  Blynk.begin(auth, ssid, pass);
    pinMode(INT_PIN, INPUT);
  pinMode(buzzer, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  // Initialize NeoPixel strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  if(!apds.begin()){
    Serial.println("failed to initialize device! Please check your wiring.");
  }
  else Serial.println("Device initialized!");

  
  // Enable proximity engine and interrupts
  apds.enableProximity(true);
  //apds.enableGesture(true);
    // Optionally set the interrupt threshold
  //apds.setProximityInterruptThreshold(50, 100, 1);
  //apds.enableProximityInterrupt();
  //apds.setProximityInterruptThreshold(0, 175);

  //enable the proximity interrupt
  //apds.enableProximityInterrupt();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  delay(2000);
  display.setTextColor(WHITE);
  enterHibernateMode();
  // Attach the interrupt pin to an ISR
  //attachInterrupt(digitalPinToInterrupt(INT_PIN), proximityInterrupt, FALLING);
}

void loop() {
  //distance = apds.readProximity();
  //uint8_t gesture = apds.readGesture();
    // Read color data
  done_interaction=false;
  Blynkrun();
  //start main program
  OLED_display(2, 0, "Rise and  Shine!");
  play_tone1();

  while(!done_interaction){
  if(!hibernate){
  OLED_display(2, 0, "How are you?");
  }
  
  delay(2000);//
    Blynkrun();
if (!GestureON){
  done_interaction=false;
    // If no gesture was detected, check for proximity
    apds.enableGesture(false);
  // Read proximity data
  uint8_t proximity_data = apds.readProximity();
  Serial.println(proximity_data);
  // Handle proximity data
  if (proximity_data > 50) {
    exitHibernateMode();
    GestureON=true;
  } 
  if (proximity_data <= 10) {
    enterHibernateMode();
  }

        }

// Check if a gesture has occurred
if(GestureON){
  Serial.println("Enter gesture interaction mode");
  delay(2000);
  lastGestureTime = millis();
  apds.enableProximity(true);
  apds.enableGesture(true);
  while(GestureON){

  uint8_t gesture = apds.readGesture();

  if (gesture==APDS9960_DOWN|gesture==APDS9960_UP|gesture==APDS9960_LEFT|gesture==APDS9960_RIGHT)
      {
        
      Serial.println("in the loop, need more accurate gesture");
      handGesture(gesture);
      lastGestureTime = millis();
      GestureON = true;
    
      }

  if (millis() - lastGestureTime > gestureInterval) {
      // It's been more than 10 seconds since the last gesture
      Serial.println("abort");
      GestureON = false;
      apds.enableGesture(false);
      done_interaction=true;
      }
  Blynk.run();
  ledSet();
    }
}

  }
OLED_display(2,0,"Thanks for using me!");
play_tone2();
delay(1000);



    // Additional low-power tasks or sleep mode here 
}
void Blynkrun(){
    Blynk.run();
    if (ledState) {
      apds.enableProximity(false);
    while(ledState){
      
      NeoAmbience();
      Blynk.run();  
      ledSet();
    }
    
  } 
  else {
    // Turn off NeoPixel strip
    strip.clear();
    strip.show();
    apds.enableProximity(true);
  }
  
}
void handGesture(int gesture) {

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);


if(gesture==APDS9960_UP){
      display.print("^ UP");
      Serial.println("^ UP");
      color = strip.Color(255, 0, 255);  // Magenta color
      ledSet();
      tone(buzzer, D4, HALF);
      delay(HALF);
      noTone(buzzer);
}
if(gesture==APDS9960_DOWN){
      display.print("v DOWN");
      Serial.println("v DOWN");
      color = strip.Color(255, 255, 255);  // White
      ledSet();
      tone(buzzer, C4, HALF);
      delay(HALF);
      noTone(buzzer);
}
if(gesture==APDS9960_RIGHT){
      display.print("> RIGHT");
      Serial.println("> RIGHT");
      color = strip.Color(255, 180, 0);  // Orange
      ledSet();
      tone(buzzer, E4, HALF);
      delay(HALF);
      noTone(buzzer);
}
if(gesture==APDS9960_LEFT){
      display.print("< LEFT");
      Serial.println("< LEFT");
      color = strip.Color(0, 255, 0);  // Green
      ledSet();
      tone(buzzer, G4, HALF);
      delay(HALF);
      noTone(buzzer);
}



}

void NeoAmbience(){
    // Enable color sensing
  
  apds.enableColor(true);
  uint16_t r, g, b, c;
    while(!apds.colorDataReady()){
    delay(5);
  }
  apds.getColorData(&r, &g, &b, &c);
  Serial.print("c:");
  Serial.println(c);
  // Calculate ambient light intensity
  uint16_t ambient_light = c;

  // Map the ambient light intensity to NeoPixel brightness (0-255)
  uint8_t neo_brightness = map(ambient_light, 0, 90, 255, 0);
  Serial.print("neo_brightness:");
  Serial.println(neo_brightness);

  if (c >= 0 && c <= 5) {
    // Maximum brightness
    neo_brightness = 255;
  } else if (c > 5 && c <= 20) {
    // Dim by half
    neo_brightness = 128;
  } else {
    // Turn off
    neo_brightness = 0;
  }
    // Set the brightness of NeoPixel strip
  strip.setBrightness(neo_brightness);
      // Update the strip to set new brightness
    strip.show();
    apds.enableColor(false);
}
void enterHibernateMode() {
  hibernate = true;
  if (!hibernate) return;
  
  // Turn off NeoPixel strip
  strip.clear();
  strip.show();
  
  // Clear OLED display
  display.clearDisplay();
  display.display();
  
  //hibernate = true;
}
void exitHibernateMode() {
    
  hibernate = false;
  if (hibernate) return;
  
  // Turn on NeoPixel strip
  strip.fill(strip.Color(255,255,255)); // White color for example
  strip.show();
  
  // Turn on OLED display with some text
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Proximity Detected!"));
  display.display();
  
  //hibernate = false;
}

void ledSet() {
  // Set the entire strip to the chosen color
  strip.fill(color, 0, strip.numPixels());
  strip.show();
  display.display();
}

void OLED_display(int textsize, int y, String text) {
  display.clearDisplay();
  display.setTextSize(textsize);
  display.setCursor(0, y);
  display.print(text);
  display.display();
}

void play_tone1() {
  tone(buzzer, C4, QUARTER);
  delay(QUARTER);

  tone(buzzer, E4, QUARTER);
  delay(QUARTER);

  tone(buzzer, G4, HALF);
  delay(HALF);
  noTone(buzzer);
}

void play_tone2() {
  tone(buzzer, G4, QUARTER);
  delay(QUARTER);

  tone(buzzer, E4, QUARTER);
  delay(QUARTER);

  tone(buzzer, C4, HALF);
  delay(HALF);
  noTone(buzzer);
}

BLYNK_WRITE(V1) {
Serial.println("Change to red-green");
color = strip.Color(0, 0, 255);  // red-green
}
BLYNK_WRITE(V2) {
Serial.println("Change to red");
color = strip.Color(255, 255, 255);  // red
}
BLYNK_WRITE(V3) {
Serial.println("Change to orange-green");
color = strip.Color(255, 180, 0);  // orange-green
}
BLYNK_WRITE(V4) {
Serial.println("Change to mixed");
color = strip.Color(120, 0, 0);  // mixed
}
BLYNK_WRITE(V5) {
  ledState = param.asInt(); // Get value as integer
}
