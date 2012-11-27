#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 3
#define ENCODER_INTERRUPT_A 0
#define ENCODER_INTERRUPT_B 1
#define PPR 1600

volatile int encoder_position = 0;

void encoder_setup()
{
  //setup our encoder pins
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  attachInterrupt(ENCODER_INTERRUPT_A, read_quadrature_a, CHANGE);
  attachInterrupt(ENCODER_INTERRUPT_B, read_quadrature_b, CHANGE);
}

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
    if (digitalRead(ENCODER_A_PIN) == LOW)
        encoder_position++;
    else
        encoder_position--;
  }
  // found a high-to-low on channel B
  else                                        
  {
    // check channel A to see which way
    if (digitalRead(ENCODER_A_PIN) == LOW)
        encoder_position--;
    else
        encoder_position++;
  }
}