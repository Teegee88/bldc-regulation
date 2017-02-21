#define PWM 11  // pin 6 doesnt provide the right frequency
#define DIR 8

void setup() {
  // put your setup code here, to run once:
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);

  // change pwm frequency to 31.3kHz
 // TCCR1B = TCCR1B & B11111000 | B00000001;
 // TCCR2B = TCCR2B & B11111000 | B00000001;
 TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(PWM, 127);  // reminder, values from 0 to 255 for the pwm
  digitalWrite(DIR, 1);
}
