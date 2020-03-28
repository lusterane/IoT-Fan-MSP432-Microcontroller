// input variables
int fanSpeed = 100;
int buttonOneState = 0;
int buttonTwoState = 0;

// set pin numbers
const int pwmPin = P3_2; // pwm pin
const int tachPin = P6_1; // tach pin
const int buttonOne = 73;     // the number of the pushbutton pin
const int buttonTwo = 74;     // the number of the pushbutton pin
const int ledYellow = 78;
const int ledBlue =  77;      // the number of the LED pin
const int ledRed =  75;      // the number of the LED pin

void setup() {
  Serial.begin(115200);

  pinMode(pwmPin, INPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT); // blue
  pinMode(buttonOne, INPUT_PULLUP); // PUSH 1
  pinMode(buttonTwo, INPUT_PULLUP); // PUSH 2

}
int boop = 0;
void loop() {
  // put your main code here, to run repeatedly: 
  buttonOneState = digitalRead(buttonOne);
  buttonTwoState = digitalRead(buttonTwo);

  if(buttonOneState == LOW && fanSpeed > 0){ // LOW MEANS PRESSED
    fanSpeed -= 51;
    digitalWrite(ledRed, HIGH);
    delay(300);
    digitalWrite(ledRed, LOW);
  }
  if(buttonTwoState == LOW && fanSpeed < 255) {
    fanSpeed += 51;
    digitalWrite(ledBlue, HIGH);
    delay(300);
    digitalWrite(ledBlue, LOW);
  }
  
  analogWrite(pwmPin, fanSpeed); // 0-255
  /*
  Serial.print("(pwmPin) PWM: ");
  Serial.print(fanSpeed, DEC);
  Serial.print("% | ");
*/
  //Serial.println((String)"Tach: " + pulseIn(tachPin, HIGH));
  Serial.println(pulseIn(tachPin, HIGH));
  
}
