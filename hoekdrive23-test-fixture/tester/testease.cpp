#include <LiquidCrystal.h>

#define TESTEASE_VERSION "Hoeken TestEase v1.0"
#ifndef TESTEASE_SPEED
  #define TESTEASE_SPEED 115200
#endif

//the PWM-enabled pins to control each LED
#define TEST_LED_PIN 11
#define PASS_LED_PIN 10
#define FAIL_LED_PIN  9

//this is our button to start the test
#define BUTTON_PIN    8

//pins to control our lcd screen
#define LCD_RS_PIN    14
#define LCD_E_PIN     12
#define LCD_DB4_PIN    7
#define LCD_DB5_PIN    6
#define LCD_DB6_PIN    5
#define LCD_DB7_PIN    4

//create our lcd object for later use.
LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);

void setup_testease()
{
  //setup our LEDS
  pinMode(TEST_LED_PIN, OUTPUT);
  pinMode(PASS_LED_PIN, OUTPUT);
  pinMode(FAIL_LED_PIN, OUTPUT);
  digitalWrite(TEST_LED_PIN, LOW);
  digitalWrite(PASS_LED_PIN, LOW);
  digitalWrite(FAIL_LED_PIN, LOW);

  //setup our button
  pinMode(BUTTON_PIN, INPUT); 
  
  //initialize our serial port
  Serial.begin(TESTEASE_SPEED);
  Serial.println(TESTEASE_VERSION);
}

void testease_wait_for_start()
{
  lcd.print("Press start\nto begin test.");
  
  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);
    
  //let em know.
  Serial.println("NEW TEST START.");
  
  //setup our leds.
  digitalWrite(TEST_LED_PIN, HIGH);
  digitalWrite(PASS_LED_PIN, LOW);
  digitalWrite(FAIL_LED_PIN, LOW);  
}

void testease_handle_test_end(bool pass)
{
  //setup our leds.
  digitalWrite(TEST_LED_PIN, LOW);
  digitalWrite(PASS_LED_PIN, pass);
  digitalWrite(FAIL_LED_PIN, !pass);
  
  //show the result.
  if (pass)
    Serial.println("BOARD PASSED.");
  else
    Serial.println("BOARD FAILED.");
  
  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);
}

