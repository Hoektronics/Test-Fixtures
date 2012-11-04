#include <LiquidCrystal.h>

#define TEST_LED_PIN 11
#define PASS_LED_PIN 10
#define FAIL_LED_PIN  9

#define BUTTON_PIN    8

#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define ENCODER_INTERRUPT_A 0
#define ENCODER_INTERRUPT_B 1

#define LCD_RS_PIN    14
#define LCD_E_PIN     12
#define LCD_DB4_PIN    7
#define LCD_DB5_PIN    6
#define LCD_DB6_PIN    5
#define LCD_DB7_PIN    4

LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_DB4_PIN, LCD_DB5_PIN, LCD_DB6_PIN, LCD_DB7_PIN);

#define STEP_PIN      16
#define DIR_PIN       15
#define ENABLE_PIN    21
#define MS1_PIN       20
#define MS2_PIN       19
#define SLEEP_PIN     17
#define RESET_PIN     18

#define PPR 1600

void setup()
{  
  //init our lcd
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("HoekDrive17 Tester");
  lcd.setCursor(0,1);
  lcd.print("Version 1.0");
  
  //setup our LEDS
  pinMode(TEST_LED_PIN, OUTPUT);
  pinMode(PASS_LED_PIN, OUTPUT);
  pinMode(FAIL_LED_PIN, OUTPUT);

  //flash em
  fade_led(TEST_LED_PIN, 512);
  fade_led(PASS_LED_PIN, 512);
  fade_led(FAIL_LED_PIN, 512);

  //setup our button
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  
  //setup our encoder pins
  pinMode(ENCODER_A_PIN, INPUT);
  digitalWrite(ENCODER_A_PIN, HIGH);
  pinMode(ENCODER_B_PIN, INPUT);
  digitalWrite(ENCODER_B_PIN, HIGH);

  attachInterrupt(ENCODER_INTERRUPT_A, read_quadrature_a, CHANGE);
  attachInterrupt(ENCODER_INTERRUPT_B, read_quadrature_b, CHANGE);  
  
  //initialize our serial port
  Serial.begin(115200);
  Serial.println("HoekDrive17 Test Fixture v1.0");

  //initialize our stepper driver
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(SLEEP_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  
  digitalWrite(ENABLE_PIN, LOW); //default to disabled
  digitalWrite(SLEEP_PIN, LOW); //default to sleep mode
  digitalWrite(RESET_PIN, HIGH); //default to reset mode
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, LOW);
}

volatile int encoder_position = 0;

void read_quadrature_a()
{  
  // found a low-to-high on channel A
  if (digitalRead(ENCODER_A_PIN) == HIGH)
  {   
    // check channel B to see which way
    if (digitalRead(ENCODER_B_PIN) == LOW)
        encoder_position++;
    else
        encoder_position--;
  }
  // found a high-to-low on channel A
  else                                        
  {
    // check channel B to see which way
    if (digitalRead(ENCODER_B_PIN) == LOW)
        encoder_position--;
    else
        encoder_position++;
  }
}

void read_quadrature_b()
{  
  // found a low-to-high on channel B
  if (digitalRead(ENCODER_B_PIN) == HIGH)
  {   
    // check channel A to see which way
    if (digitalRead(ENCODER_A_PIN) == HIGH)
        encoder_position++;
    else
        encoder_position--;
  }
  // found a high-to-low on channel B
  else                                        
  {
    // check channel A to see which way
    if (digitalRead(ENCODER_A_PIN) == HIGH)
        encoder_position--;
    else
        encoder_position++;
  }
}

void output_test_status(String test, String status)
{
  //show our LCD info.
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEST: ");
  lcd.print(test);
  lcd.setCursor(0,1);
  lcd.print("STATUS: ");
  lcd.print(status);

  //output to serial.
  Serial.print("TEST: ");
  Serial.print(test);
  Serial.print(" STATUS: ");
  Serial.println(status);
}

void loop()
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PRESS BUTTON");
  lcd.setCursor(0,1);
  lcd.print("TO BEGIN TEST.");
  
  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);
    
  //setup our leds.
  digitalWrite(TEST_LED_PIN, HIGH);
  digitalWrite(PASS_LED_PIN, LOW);
  digitalWrite(FAIL_LED_PIN, LOW);
    
  //run all our tests.
  boolean pass = false;
  while (true)
  {
    pass = test_full_fwd();
    if (!pass)
      break;

    pass = test_full_rev();
    if (!pass)
      break;

    pass = test_half_fwd();
    if (!pass)
      break;

    pass = test_half_rev();
    if (!pass)
      break;

    pass = test_1_4_fwd();
    if (!pass)
      break;

    pass = test_1_4_rev();
    if (!pass)
      break;

    pass = test_1_16_fwd();
    if (!pass)
      break;

    pass = test_1_16_rev();
    if (!pass)
      break;
      
    //okay, we're done.
    break;
  }
  
  //turn off our driver
  digitalWrite(ENABLE_PIN, LOW); //default to disabled
  digitalWrite(SLEEP_PIN, LOW); //default to sleep mode
  digitalWrite(RESET_PIN, HIGH); //default to reset mode
  
  //setup our leds.
  digitalWrite(TEST_LED_PIN, LOW);
  digitalWrite(PASS_LED_PIN, pass);
  digitalWrite(FAIL_LED_PIN, !pass);
  
  //show the result.
  if (pass)
    output_test_status("ALL TESTS", "PASS");
  else
    output_test_status("ALL TESTS", "FAIL");
  
  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);
}

boolean measure_steps(int steps, int expected, int tolerance, int delay_time)
{
  //reset our position counter!
  encoder_position = 0;
  
  delay_time = 300000 / steps;
  
  //do our steps
  for (int i=0; i<steps; i++)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(delay_time);
    digitalWrite(STEP_PIN, LOW);
  }

  //log our results.
  Serial.print("EXPECTED: ");
  Serial.print(expected, DEC);
  Serial.print(" TOLERANCE: ");
  Serial.print(tolerance, DEC);
  Serial.print(" ACTUAL: ");
  Serial.print(encoder_position, DEC);
  Serial.println();
  
  //did it pass?
  return (encoder_position >= (expected - tolerance) && encoder_position <= (expected + tolerance));
}

boolean test_full_fwd()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
  
  output_test_status("FULL FWD", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(200, 1600, 10, 16))
  {
    output_test_status("FULL FWD", "PASS");
    return true;
  }
  else
  {
    output_test_status("FULL FWD", "FAIL");
    return false;
  } 
}

boolean test_full_rev()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  
  output_test_status("FULL REV", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(200, -1600, 10, 16))
  {
    output_test_status("FULL REV", "PASS");
    return true;
  }
  else
  {
    output_test_status("FULL REV", "FAIL");
    return false;
  }
}

boolean test_half_fwd()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS2_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);

  output_test_status("HALF FWD", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(400, 1600, 10, 8))
  {
    output_test_status("HALF FWD", "PASS");
    return true;
  }
  else
  {
    output_test_status("HALF FWD", "FAIL");
    return false;
  }
}

boolean test_half_rev()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS2_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  
  output_test_status("HALF REV", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(400, -1600, 10, 8))
  {
    output_test_status("HALF REV", "PASS");
    return true;
  }
  else
  {
    output_test_status("FULL REV", "FAIL");
    return false;
  }
}

boolean test_1_4_fwd()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, HIGH);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
  
  output_test_status("1/4  FWD", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(800, 1600, 10, 4))
  {
    output_test_status("1/4  FWD", "PASS");
    return true;
  }
  else
  {
    output_test_status("1/4  FWD", "FAIL");
    return false;
  } 
}

boolean test_1_4_rev()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, LOW);
  digitalWrite(MS2_PIN, HIGH);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  
  output_test_status("1/4  REV", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(800, -1600, 10, 4))
  {
    output_test_status("1/4  REV", "PASS");
    return true;
  }
  else
  {
    output_test_status("1/4  REV", "FAIL");
    return false;
  } 
}

boolean test_1_16_fwd()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS2_PIN, HIGH);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, HIGH);
  
  output_test_status("1/16 FWD", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(3200, 1600, 10, 1))
  {
    output_test_status("1/16 FWD", "PASS");
    return true;
  }
  else
  {
    output_test_status("1/16 FWD", "FAIL");
    return false;
  }
}

boolean test_1_16_rev()
{
  //configure all our pins.
  digitalWrite(SLEEP_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS2_PIN, HIGH);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  
  output_test_status("1/16 REV", "RUNNING");
  
  //okay, do our test.
  if (measure_steps(3200, -1600, 10, 1))
  {
    output_test_status("1/16 REV", "PASS");
    return true;
  }
  else
  {
    output_test_status("1/16 REV", "FAIL");
    return false;
  }
}

//simple function to blink the LED.
void fade_led(byte pin, int milliseconds)
{
  int top = milliseconds/2;

  for (int i=0; i<top; i++)
  {
    analogWrite(pin, map(i, 0, top, 0, 255));
    delay(1);
  }

  for (int i=top; i>0; i--)
  {
    analogWrite(pin, map(i, 0, top, 0, 255));
    delay(1);
  }

  analogWrite(pin, 0);
}
