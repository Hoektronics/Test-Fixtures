#define STEP_PIN      
#define DIR_PIN       
#define ENABLE_PIN    
#define RESET_PIN     

#define M1_PIN        
#define M2_PIN        
#define M3_PIN        

#define DCY1_PIN      
#define DCY2_PIN      
#define ALERT_PIN     
#define DOWN_PIN      

#define VCC_RELAY_PIN
#define VCC_VOLTAGE_PIN
#define VCC_CURRENT_PIN

#define VMOT_RELAY_PIN
#define VMOT_VOLTAGE_PIN
#define VMOT_CURRENT_PIN

#define VREF_DIGIPOT_CS_PIN
#define VREF_DIGIPOT_MEASURE_PIN

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
  
  //setup our basic pins.
  digitalWrite(ENABLE_PIN, LOW); //default to disabled
  digitalWrite(RESET_PIN, HIGH); //default to not reset
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);

  //normal 0% decay
  digitalWrite(DCY1_PIN, LOW);
  digitalWrite(DCY2_PIN, LOW);
}

void loop()
{
  testease_wait_for_start();

  //turn on 5v power supply
  digitalWrite(VCC_RELAY_PIN, HIGH);
    
  //run all our tests.
  boolean pass = false;

  while (true)
  {
    //check idle voltage.
    pass = testease_measure_voltage(VCC_VOLTAGE_PIN, 5.00, 0.05);
    if (!pass)
      break;

    //check idle current.
    pass = testease_measure_current(VCC_CURRENT_PIN, 0.00, 0.05);
    if (!pass)
      break;

    //turn on VMOT power supply @ check idle current
    digitalWrite(VMOT_RELAY_PIN, HIGH);

    //check idle voltage.
    pass = testease_measure_voltage(VMOT_VOLTAGE_PIN, 24.0, 0.05);
    if (!pass)
      break;

    //check idle current.
    pass = testease_measure_current(VMOT_CURRENT_PIN, 0.0, 0.05);
    if (!pass)
      break;

    //configure us to full step mode 
    digitalWrite(RESET_PIN, HIGH);
    delay(1);
    digitalWrite(RESET_PIN, LOW);
    configure_microsteps(microsteps);
    digitalWrite(ENABLE_PIN, HIGH);
    
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

    //test our step modes
    byte step_modes[] = {2, 8, 10, 16, 20, 32, 40, 64};
    for (byte i=0; i<8; i++)
    {
      pass = test_rotation(step_modes[i], true);
      if (!pass)
        break;
      pass = test_rotation(step_modes[i]), false);
      if (!pass)
        break;
    }
    if (!pass)
      break;
      
    //okay, we're done.
    break;
  }

  //turn off our driver
  digitalWrite(RESET_PIN, LOW); //reset our board
  digitalWrite(RESET_PIN, HIGH); //reset our board
  digitalWrite(ENABLE_PIN, LOW); //default to disabled
    
  //turn off power supplies supply
  digitalWrite(VMOT_RELAY_PIN, LOW);
  digitalWrite(VCC_RELAY_PIN, LOW);
  
  //wait for test end.
  testease_handle_test_end()
}

boolean measure_steps(int steps, int expected, int tolerance, int delay_time)
{
  //reset our position counter!
  encoder_position = 0;
  
  //do our steps
  for (int i=0; i<steps; i++)
  {
    digitalWrite(STEP_PIN, HIGH);
    delay(delay_time);
    digitalWrite(STEP_PIN, LOW);
    delay(1);
  }

  //log our results.
  Serial.print("EXPECTED: ");
  Serial.print(expected, DEC);
  Serial.print("TOLERANCE: ");
  Serial.print(tolerance, DEC);
  Serial.print("ACTUAL: ");
  Serial.print(encoder_position, DEC);
  Serial.println();
  
  //did it pass?
  return (encoder_position >= (expected - tolerance) && encoder_position <= (expected + tolerance));
}

boolean test_rotation(byte microsteps, boolean direction)
{
  //set to 3.0 amps for the test
  set_stepper_current(3.0);

  //configure all our pins.
  digitalWrite(RESET_PIN, HIGH);
  configure_microsteps(microsteps);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, direction);

  int steps = 200 * microsteps;
  int micro_delay = 16000 / steps;

  //show our information
  lcd.print("1/");
  lcd.print(microsteps, DEC);
  lcd.print(" STEP ");
  if (direction)
    lcd.print(" FWD: ");
  else
    lcd.print(" REF: ");

  Serial.print("1/");
  Serial.print(microsteps, DEC);
  Serial.print(" STEP ");
  if (direction)
    Serial.print("FORWARD MODE: ");
  else
    Serial.print("REVERSE MODE: ");
  
  //okay, do our test.
  if (measure_steps(steps, 1600, 10, micro_delay))
  {
    lcd.print("PASS")
    Serial.println("PASS")
    return true;
  }
  else
  {
    lcd.print("FAIL")
    Serial.println("FAIL")
    return false;
  }  
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
}