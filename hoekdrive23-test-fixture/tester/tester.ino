
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

#define STEP_PIN      16
#define DIR_PIN       28  //these are wired wrong on the library module
#define ENABLE_PIN    21  //ditto.
#define RESET_PIN     18

#define M1_PIN        17
#define M2_PIN        19
#define M3_PIN        20

#define DCY1_PIN      22
#define DCY2_PIN      23
#define ALERT_PIN     25
#define DOWN_PIN      26

#define VCC_RELAY_PIN     24
#define VCC_VOLTAGE_PIN   2
#define VCC_CURRENT_PIN   1

#define VMOT_RELAY_PIN    27
#define VMOT_VOLTAGE_PIN  3
#define VMOT_CURRENT_PIN  0

#define VREF_DIGIPOT_CS_PIN      49
#define VREF_DIGIPOT_MEASURE_PIN 4

void setup()
{
  //various setup calls.
  testease_setup();
  encoder_setup();
  
  //initialize our stepper driver
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);
  pinMode(M2_PIN, OUTPUT);
  pinMode(M3_PIN, OUTPUT);
  pinMode(DCY1_PIN, OUTPUT);
  pinMode(DCY2_PIN, OUTPUT);
  pinMode(ALERT_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  
  digitalWrite(ALERT_PIN, HIGH);
  digitalWrite(DOWN_PIN, HIGH);
  
  //setup our basic pins.
  digitalWrite(ENABLE_PIN, LOW); //default to disabled
  digitalWrite(RESET_PIN, LOW); //start up right.
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);

  //default step mode.
  digitalWrite(M1_PIN, LOW);
  digitalWrite(M2_PIN, LOW);
  digitalWrite(M3_PIN, LOW);

  //normal 0% decay
  digitalWrite(DCY1_PIN, LOW);
  digitalWrite(DCY2_PIN, LOW);

  //our power relays  
  pinMode(VCC_RELAY_PIN, OUTPUT);
  pinMode(VMOT_RELAY_PIN, OUTPUT);
  digitalWrite(VCC_RELAY_PIN, LOW);
  digitalWrite(VMOT_RELAY_PIN, LOW);

  pinMode(VREF_DIGIPOT_CS_PIN, OUTPUT);
  digitalWrite(VREF_DIGIPOT_CS_PIN, HIGH);
}

void loop()
{
  testease_wait_for_start();
  
  //run_tests();

}

void run_tests()
{
  /*
  //turn on 5v power supply
  digitalWrite(VCC_RELAY_PIN, HIGH);

  lcd.clear();
  lcd.write("VCC ON.");
  
  delay(200);

  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);

  //turn on 5v power supply
  digitalWrite(VMOT_RELAY_PIN, HIGH);

  lcd.clear();
  lcd.write("VMOT ON.");
  
  delay(200);

  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);
*/    
  digitalWrite(VCC_RELAY_PIN, LOW);
  digitalWrite(VMOT_RELAY_PIN, LOW);
  
  delay(200);
  
  //run all our tests.
  boolean pass = false;

  while (true)
  {
    //turn on 5v power supply
    digitalWrite(VCC_RELAY_PIN, HIGH);

    //wait for relay to click.
    delay(100);

    //check idle voltage.
    pass = testease_measure_voltage(VCC_VOLTAGE_PIN, 5.00, 0.05, 100000.0, 100000.0);
    if (!pass)
      break;

    //check idle current.
    pass = testease_measure_current(VCC_CURRENT_PIN, -1.0, 0.40);
    if (!pass)
      break;

    //turn on VMOT power supply @ check idle current
    digitalWrite(VMOT_RELAY_PIN, HIGH);

    //wait for relay to click.
    delay(100);

    //check idle voltage.
    pass = testease_measure_voltage(VMOT_VOLTAGE_PIN, 12.0, 0.10, 100000.0, 10000.0);
    if (!pass)
      break;

    //check idle current.
    pass = testease_measure_current(VMOT_CURRENT_PIN, 0.00, 0.40);
    if (!pass)
      break;
      
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(ENABLE_PIN, HIGH);
      
    //delay(2000);

    //configure us to full step mode 
    //digitalWrite(RESET_PIN, LOW);
    //delay(1);
    //digitalWrite(RESET_PIN, HIGH);
    //configure_microsteps(2);
    //digitalWrite(ENABLE_PIN, HIGH);

/*    
    //measure all of our current settings.
    float current_settings[] = {4.0, 3.5, 3.0, 2.5, 2.0, 1.5, 1.0, 0.5};
    for (byte i=0; i<8; i++)
    {
      set_stepper_current(current_settings[i]);
      pass = testease_measure_current(VMOT_CURRENT_PIN, current_settings[i], 0.05);
      if (!pass)
        break;
    }
    if (!pass)
      break;
*/
    //test our step modes
    byte step_modes[] = {2, 8, 10, 16, 20, 32, 40, 64};
    for (byte i=0; i<8; i++)
    {
      pass = test_rotation(step_modes[i], true);
      //if (!pass)
      //  break;
      pass = test_rotation(step_modes[i], false);
      //if (!pass)
      //  break;
        
      //check idle voltage.
      //pass = testease_measure_voltage(VMOT_VOLTAGE_PIN, 12.0, 0.10, 100000.0, 10000.0);
      //if (!pass)
      //  break;
  
      //check idle current.
      //pass = testease_measure_current(VMOT_CURRENT_PIN, 0.00, 0.40);
      //if (!pass)
      //  break;
    }
    if (!pass)
      break;

    //okay, we're done.
    break;

  }

  //turn off our driver
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(M1_PIN, LOW);
  digitalWrite(M2_PIN, LOW);
  digitalWrite(M3_PIN, LOW);
  digitalWrite(DCY1_PIN, LOW);
  digitalWrite(DCY2_PIN, LOW);
      
  //turn off power supplies supply
  digitalWrite(VMOT_RELAY_PIN, LOW);
  digitalWrite(VCC_RELAY_PIN, LOW);
  
  //wait for test end.
  testease_handle_test_end(pass);
}

boolean test_rotation(byte microsteps, boolean dir)
{
  //set to 3.0 amps for the test
  set_stepper_current(3.0);

  //configure all our pins.
//  digitalWrite(RESET_PIN, HIGH);
  configure_microsteps(microsteps);
//  digitalWrite(ENABLE_PIN, HIGH);
  digitalWrite(DIR_PIN, dir);

  int steps = 200 * microsteps;
  int micro_delay = 2000 / microsteps;

  steps = steps * 2;
  
  Serial.print("steps: ");
  Serial.print(steps, DEC);
  Serial.print(" delay: ");
  Serial.print(micro_delay, DEC);
  Serial.println();

  //show our information
  lcd.clear();
  lcd.print("1/");
  lcd.print(microsteps, DEC);
  lcd.print(" STEP ");
  if (dir)
    lcd.print(" FWD: ");
  else
    lcd.print(" REF: ");

  Serial.print("1/");
  Serial.print(microsteps, DEC);
  Serial.print(" STEP ");
  if (dir)
    Serial.print("FORWARD MODE: ");
  else
    Serial.print("REVERSE MODE: ");
  
  //okay, do our test.
  if (measure_steps(steps, 1600, 10, micro_delay))
  {
    lcd.print("PASS");
    Serial.println("PASS");
    return true;
  }
  else
  {
    lcd.print("FAIL");
    Serial.println("FAIL");
    return false;
  }
}

void find_max_speed()
{
  int steps = 64*200;  // 1/64th step mode
  int expected = 1600; // our 400 PPR encoder
  int tolerance = 10;
  int delay_time = 20;
  
  boolean skipped = false;
  while (!skipped)
  {
     skipped = measure_steps(steps, expected, tolerance, delay_time 
     
     if (!skipped)
       delay_time--;
  }
  
  int microseconds = steps * delay_time;
  
  float rpm = (float)60000000 / (float)microseconds;
  Serial.print("Max speed: ");
  Serial.println(rpm, DEC);
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("RPM: ");
  lcd.print(rpm, DEC);
  
  //wait until we get a button press.
  while (digitalRead(BUTTON_PIN))
    delay(1);
}

boolean measure_steps(int steps, int expected, int tolerance, int delay_time)
{
  //reset our position counter!
  encoder_set_position(0);
  
  //do our steps
  for (int i=0; i<steps; i++)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(delay_time);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(10);
  }
  
  check_alert_ok();

  //log our results.
  Serial.print("EXPECTED: ");
  Serial.print(expected, DEC);
  Serial.print(" TOLERANCE: ");
  Serial.print(tolerance, DEC);
  Serial.print(" ACTUAL: ");
  Serial.print(encoder_get_position(), DEC);
  Serial.println();
  
  //did it pass?
  return (encoder_get_position() >= (expected - tolerance) && encoder_get_position() <= (expected + tolerance));
}

void configure_microsteps(byte ms)
{
  if (ms == 2)
  {
    digitalWrite(M1_PIN, LOW);
    digitalWrite(M2_PIN, LOW);
    digitalWrite(M3_PIN, LOW);
  }
  else if (ms == 8)
  {
    digitalWrite(M1_PIN, LOW);
    digitalWrite(M2_PIN, LOW);
    digitalWrite(M3_PIN, HIGH);    
  }
  else if (ms == 10)
  {
    digitalWrite(M1_PIN, LOW);
    digitalWrite(M2_PIN, HIGH);
    digitalWrite(M3_PIN, LOW);    
  }
  else if (ms == 16)
  {
    digitalWrite(M1_PIN, LOW);
    digitalWrite(M2_PIN, HIGH);
    digitalWrite(M3_PIN, HIGH);    
  }
  else if (ms == 20)
  {
    digitalWrite(M1_PIN, HIGH);
    digitalWrite(M2_PIN, LOW);
    digitalWrite(M3_PIN, LOW);    
  }
  else if (ms == 32)
  {
    digitalWrite(M1_PIN, HIGH);
    digitalWrite(M2_PIN, LOW);
    digitalWrite(M3_PIN, HIGH);    
  }
  else if (ms == 40)
  {
    digitalWrite(M1_PIN, HIGH);
    digitalWrite(M2_PIN, HIGH);
    digitalWrite(M3_PIN, LOW);    
  }
  else if (ms == 64)
  {
    digitalWrite(M1_PIN, HIGH);
    digitalWrite(M2_PIN, HIGH);
    digitalWrite(M3_PIN, HIGH);    
  }
  
  digitalWrite(RESET_PIN, LOW);
  delay(50);
  digitalWrite(RESET_PIN, HIGH);
  delay(1);
}

void set_stepper_current(float current)
{
  float reading = (float)analogRead(VREF_DIGIPOT_MEASURE_PIN);
  float vref = (5.0 / 1024.0) * reading;
  float actual = vref * .33333333 * (1/0.24);
  Serial.print("VREF Reading: ");
  Serial.print(reading, DEC);
  Serial.print(" VREF: ");
  Serial.print(vref, DEC);
  Serial.print(" CURRENT: ");
  Serial.print(actual);
  Serial.println();
}

boolean check_alert_ok()
{
  if (digitalRead(ALERT_PIN))
  {
     Serial.println("Temperature Alert PASS");
    return true; 
  }
  else
  {
    Serial.println("Temperature Alert FAIL.");
    return false;
  }
}

void testease_setup()
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
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Press start to");
  lcd.setCursor(0, 1);
  lcd.print("begin test.");
  
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


  //turn off leds
  digitalWrite(TEST_LED_PIN, LOW);
  digitalWrite(PASS_LED_PIN, LOW);
  digitalWrite(FAIL_LED_PIN, LOW);
    
  delay(250);
}

// samples the output from the acs712 current measurement chip.
// returns the current level as a float representing the amperage
// the acs712 comes in 3 flavors with different amp ratings: 5A, 20A, and 30A.
// this code is currently set up to work with the 5A chip.
// it is important to note, the output is linear and centered around 2.5V.
// sensitivity levels:
// 5A:  185mV / A
// 20A: 100mV / A
// 30A:  66mV / A

float read_acs712_current(byte pin)
{
  int reading = analogRead(pin) - 512;
  float voltage = (5.0 / 1024.0) * (float)reading;
  float amperage = voltage / 0.185;
  
  Serial.print("Analog: ");
  Serial.print(reading, DEC);
  Serial.print(" Voltage: ");
  Serial.print(voltage, DEC);
  Serial.print(" Amperage: ");
  Serial.print(amperage, DEC);
  Serial.println();
  
  return amperage;
}

boolean testease_measure_voltage(byte pin, float voltage, float tolerance, float dividertop, float dividerbottom)
{
  float ratio = dividerbottom/(dividertop+dividerbottom);
  //Serial.print("Ratio: ");
  //Serial.print(ratio, DEC);
  //Serial.println();

  int reading = analogRead(pin);
  float v = ((5.0 / 1024.0) * (float)reading)/ratio;
  
  Serial.print("Analog: ");
  Serial.print(reading, DEC);
  Serial.print(" Voltage: ");
  Serial.print(v, DEC);
  Serial.println();
  
  if (v < (voltage + voltage*tolerance) && v > (voltage - voltage*tolerance))
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean testease_measure_current(byte pin, float bottom, float top)
{
  float c = read_acs712_current(pin);
  
  if (c < top && c > bottom)
  {
    return true;
  }
  else
  {
    return false;
  }
}
