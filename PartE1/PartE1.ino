
//Setup Arduino pins
const int RedLEDPin = 9;
const int GreenLEDPin = 10;
const int BlueLEDPin = 11;

const int pushButton = 2;
int buttonState = 0;
int ledCounter = 1;

bool buttonPressed = false;

String currentColor = "OFF";

bool ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

bool directionColor = true;


// the setup function runs once when you press reset or power the board
void setup() {

  Serial.begin(9600);

  pinMode(RedLEDPin, OUTPUT);
  pinMode(GreenLEDPin, OUTPUT);
  pinMode(BlueLEDPin, OUTPUT);

  pinMode(pushButton, INPUT);
}




// the loop function runs over and over again forever
void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    }

    else {
      ledState = LOW;

      if (directionColor == true) {
        ledCounter++;

        if (ledCounter == 5) {
          ledCounter = 1;
        }
      }

      else {
        ledCounter--;

        if (ledCounter == 0) {
          ledCounter = 4;
        }
      }
    }
  }
  Serial.print("Current Color : ");
  Serial.println(currentColor);

  buttonState = digitalRead(pushButton);
  //Inverse the state becuase module gives the opposite
  if (buttonState == LOW) {
    buttonState = HIGH;
  }

  else if (buttonState == HIGH) {
    buttonState = LOW;
  }

  if (buttonState == HIGH && !buttonPressed) {

    directionColor = !directionColor;

    buttonPressed = true;
    delay(5);  //Debounce
  }

  else if (buttonState == LOW && buttonPressed) {
    buttonPressed = false;
    delay(5);  //Debounce
  }


if (!buttonPressed){
  //RED
  if (ledCounter == 1) {
    if (ledState == HIGH) {
      PowerLEDControl(0,255,255);
    } 
    else {
    PowerLEDControl(255,255,255);
    }
    currentColor = "RED";
  }

  //YELLOW
  else if (ledCounter == 2) {

    if (ledState == HIGH) {
    PowerLEDControl(0,0,255);
    }
    else {
    PowerLEDControl(255,255,255);
    }
    currentColor = "YELLOW";
  }

  //BLUE
  else if (ledCounter == 3) {

    if (ledState == HIGH) {
    PowerLEDControl(255,255,0);
    }

    else {
    PowerLEDControl(255,255,255);
    }
    currentColor = "BLUE";
  }

  //PURPLE
  else if (ledCounter == 4) {
    if (ledState == HIGH) {
    PowerLEDControl(0,255,0);
    }

    else {
    PowerLEDControl(255,255,255);
    }

    currentColor = "PURPLE";
  }
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
