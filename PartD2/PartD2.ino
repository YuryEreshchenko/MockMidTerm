const int RedLEDPin = 9;
const int GreenLEDPin = 10;
const int BlueLEDPin = 11;

const int pushButton = 2;

int buttonState = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(RedLEDPin, OUTPUT);
  pinMode(GreenLEDPin, OUTPUT);
  pinMode(BlueLEDPin, OUTPUT);
  pinMode(pushButton, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  buttonState = !digitalRead(pushButton);

  Serial.println(buttonState);
  delay(1);

  if (buttonState == HIGH){
    PowerLEDControl(0,0,0);
  }
  else {
    PowerLEDControl(255,255,255);
  }

}

void PowerLEDControl(int red, int green, int blue) {
  analogWrite(RedLEDPin, red);
  analogWrite(GreenLEDPin, green);
  analogWrite(BlueLEDPin, blue);
}