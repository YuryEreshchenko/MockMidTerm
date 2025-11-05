const int RedLEDPin = 9;
const int GreenLEDPin = 10;
const int BlueLEDPin = 11;

const int pushButton = 2;

int buttonState = 0;

unsigned long pressingTime = 0;
const long longPressInterval = 1000;
bool buttonPressed = false; 

int ledCounter = 1;           // Tracks current color (1-4)

//Mode 0 : Change colors at 1 second intervals (always on)
//Mode 1 : Blink twice at 0.25 second intervals before changing color
int currentMode = 0;

unsigned long blinkTimer = 0;
bool blinkOn = false;

// Mode 0 variables
const int mode0Interval = 1000;  // 1 second

// Mode 1 variables
const int mode1BlinkInterval = 250;  // 0.25 seconds
int blinkCount = 0;

int RCurrentColor = 0;
int GCurrentColor = 0;
int BCurrentColor = 0;

String currentColor = "OFF";
bool directionColor = true;

void setup() {
  Serial.begin(9600);
  pinMode(RedLEDPin, OUTPUT);
  pinMode(GreenLEDPin, OUTPUT);
  pinMode(BlueLEDPin, OUTPUT);
  pinMode(pushButton, INPUT);
  
  // Initialize with first color
  updateColorValues();
  blinkTimer = millis();
  blinkOn = true;
  PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
}

void loop() {
  int clickType = checkButton();

  if (clickType == 1) {
    // Short press changes direction
    directionColor = !directionColor;
    Serial.println(directionColor ? "Direction: Forward" : "Direction: Backward");
  }
  else if (clickType == 2) {
    // Long press toggles between the two modes
    currentMode = !currentMode;
    Serial.print("Switched to Mode ");
    Serial.println(currentMode);
    
    // Reset variables when changing modes
    blinkTimer = millis();
    blinkOn = true;
    blinkCount = 0;
    
    // If switching to mode 0, turn LED on immediately
    if (currentMode == 0) {
      PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
    }
  }
  
  updateLEDColor(currentMode);
}

void updateLEDColor(int currentMode) {
  unsigned long currentTime = millis();
  
  // Mode 0: Change colors at 1 second intervals (LED always on)
  if (currentMode == 0) {
    if (currentTime - blinkTimer >= mode0Interval) {
      blinkTimer = currentTime;
      // Change to next color
      changeToNextColor();
      // Keep LED on with new color
      PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
    }
  }
  
  // Mode 1: Blink twice at 0.25 second intervals before changing color
  else if (currentMode == 1) {
    if (currentTime - blinkTimer >= mode1BlinkInterval) {
      blinkOn = !blinkOn;
      blinkTimer = currentTime;
      
      if (blinkOn) {
        blinkCount++;
        
        // If we just completed 2 blinks (after the 2nd OFF), change color
        if (blinkCount > 2) {
          changeToNextColor();
          blinkCount = 1;  // Reset to 1 since we're starting the first blink of new color
        }
        
        PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
      } else {
        PowerLEDControl(255, 255, 255);  // LED OFF
      }
    }
  }
}

void changeToNextColor() {
  if (directionColor) {
    ledCounter++;
    if (ledCounter > 4) {
      ledCounter = 1;
    }
  } else {
    ledCounter--;
    if (ledCounter < 1) {
      ledCounter = 4;
    }
  }
  
  updateColorValues();
  
  Serial.print("Changed to Color: ");
  Serial.println(currentColor);
}

void updateColorValues() {
  // RED
  if (ledCounter == 1) {
    RCurrentColor = 0;
    GCurrentColor = 255;
    BCurrentColor = 255;
    currentColor = "RED";
  }
  // YELLOW
  else if (ledCounter == 2) {
    RCurrentColor = 0;
    GCurrentColor = 0;
    BCurrentColor = 255;
    currentColor = "YELLOW";
  }
  // BLUE
  else if (ledCounter == 3) {
    RCurrentColor = 255;
    GCurrentColor = 255;
    BCurrentColor = 0;
    currentColor = "BLUE";
  }
  // PURPLE
  else if (ledCounter == 4) {
    RCurrentColor = 0;
    GCurrentColor = 255;
    BCurrentColor = 0;
    currentColor = "PURPLE";
  }
}

int checkButton() {
  int clickType = 0;
  buttonState = !digitalRead(pushButton);
  delay(5);

  if (buttonState == HIGH && !buttonPressed) {
    pressingTime = millis();
    buttonPressed = true;
  }
  if (buttonState == LOW && buttonPressed) {
    unsigned long currentTime = millis();
    if (currentTime - pressingTime < longPressInterval) {
      Serial.println("Short click");
      clickType = 1;
    }
    else {
      Serial.println("Long Press");
      clickType = 2;
    }
    buttonPressed = false;
  }

  return clickType;
}

void PowerLEDControl(int red, int green, int blue) {
  analogWrite(RedLEDPin, red);
  analogWrite(GreenLEDPin, green);
  analogWrite(BlueLEDPin, blue);
}