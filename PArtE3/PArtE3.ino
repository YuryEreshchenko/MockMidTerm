const int RedLEDPin = 9;
const int GreenLEDPin = 10;
const int BlueLEDPin = 11;

const int pushButton = 2;

int buttonState = 0;

unsigned long pressingTime = 0;
const long longPressInterval = 1000;
const long whiteFadeActivation = 3000;  // 3 seconds for white fade mode
bool buttonPressed = false; 

int ledCounter = 1;           // Tracks current color (1-4)

//Mode 0 : Change colors at 1 second intervals (always on)
//Mode 1 : Blink twice at 0.25 second intervals before changing color
//Mode 2 : White fading effect (activated by holding button for 3+ seconds)
int currentMode = 0;
int previousMode = 0;  // Store the mode before entering white fade
bool inWhiteFadeMode = false;

unsigned long blinkTimer = 0;
bool blinkOn = false;

// Mode 0 variables
const int mode0Interval = 1000;  // 1 second

// Mode 1 variables
const int mode1BlinkInterval = 250;  // 0.25 seconds
int blinkCount = 0;

// White fade mode variables
unsigned long fadeTimer = 0;
const int fadeInterval = 15;  // Update every 15ms for smooth fading
int fadeBrightness = 0;
int fadeDirection = 1;        // 1 for increasing, -1 for decreasing
const int fadeStep = 5;       // How much to change brightness each step

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
    // Long press (1-3 seconds) toggles between mode 0 and mode 1
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
  else if (clickType == 3) {
    // Very long press (3+ seconds) - enter white fade mode
    Serial.println("Entering White Fade Mode");
    inWhiteFadeMode = true;
    previousMode = currentMode;  // Save current mode
    fadeTimer = millis();
    fadeBrightness = 0;
    fadeDirection = 1;
  }
  else if (clickType == 4) {
    // Button released from white fade mode - return to previous mode
    Serial.println("Exiting White Fade Mode");
    inWhiteFadeMode = false;
    currentMode = previousMode;  // Restore previous mode
    blinkTimer = millis();
    
    // Restore LED state based on mode
    if (currentMode == 0) {
      PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
    }
  }
  
  if (inWhiteFadeMode) {
    updateWhiteFade();
  } else {
    updateLEDColor(currentMode);
  }
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

void updateWhiteFade() {
  unsigned long currentTime = millis();
  
  if (currentTime - fadeTimer >= fadeInterval) {
    fadeBrightness += fadeDirection * fadeStep;
    
    // Reverse direction at the limits
    if (fadeBrightness >= 255) {
      fadeBrightness = 255;
      fadeDirection = -1;
    }
    else if (fadeBrightness <= 0) {
      fadeBrightness = 0;
      fadeDirection = 1;
    }
    
    fadeTimer = currentTime;
    
    // Create white color by fading all channels together
    // White = all channels at same level (inverse PWM: 255 = off, 0 = full on)
    int whiteLevel = 255 - fadeBrightness;  // Invert for common anode
    PowerLEDControl(whiteLevel, whiteLevel, whiteLevel);
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
  
  // Check if button is still held down and we're past 3 seconds
  if (buttonState == HIGH && buttonPressed && !inWhiteFadeMode) {
    unsigned long currentTime = millis();
    if (currentTime - pressingTime >= whiteFadeActivation) {
      clickType = 3;  // Enter white fade mode
    }
  }
  
  if (buttonState == LOW && buttonPressed) {
    unsigned long currentTime = millis();
    unsigned long pressDuration = currentTime - pressingTime;
    
    // If releasing from white fade mode
    if (inWhiteFadeMode) {
      clickType = 4;  // Exit white fade mode
    }
    // If press was less than 1 second - short click
    else if (pressDuration < longPressInterval) {
      Serial.println("Short click");
      clickType = 1;
    }
    // If press was between 1-3 seconds - long press
    else if (pressDuration < whiteFadeActivation) {
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