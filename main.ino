#include <SoftwareSerial.h>
 
SoftwareSerial rs232(11, 12); //232_TX,232_RX

const int inputButtons[] = {A0, A1, A2};
const int ledPins[] = {8, 9, 10};

int readButton = -1;
float readVolume = 0.0;

int currentSource = 0;

void setup() {
  // Setup serial communication for debugging.
  Serial.begin(9600);
  
  // Setup RS232.
  rs232.begin(9600);
  
  // Setup pins for output to LEDs.
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  
  // Reset.
  setCurrentLED(currentSource);
  setCurrentVideo(currentSource);
}

/**
 * Return index of button being pressed.
*/
int readButtons() {
  int buttonState = -1;
  
  // Iterate over all buttons.
  for (int i = 0; i < 3; i++) {
    buttonState = analogRead(inputButtons[i]);
    
    // Check if button is pressed.
    if (buttonState > 1000) {
      // Return index of button.
      return i;
    }
  }
  
  // Return -1 if no pressed button.
  return -1;
}

/**
 * Toggle LEDs to supplied source.
*/
void setCurrentLED(int currentLED) {
  // Iterate over all LEDs.
  for (int i = 0; i < 3; i++) {
    // Turn LED on if it's the current source, otherwise off.
    digitalWrite(ledPins[i], (i == currentLED ? HIGH : LOW));
  }
}

/**
 * Toggle video switch to supplied source.
*/
void setCurrentVideo(int currentVideo) {
  switch (currentVideo) {
    case 0:
      rs232.println("x1AVx1");
      break;
    case 1:
      rs232.println("x2AVx1");
      break;
    case 2:
      rs232.println("x3AVx1");
      break;
  }
}

/**
 * Read voltage of analog input knob.
*/
float readVolumeVoltage() {
  // Read the value from analog pin A0.
  int sensorValue = analogRead(A5);
  
  // Convert and return analog sensor reading (0-1023) to a voltage (0-5V).
  return sensorValue * (5.0 / 1023.0);
}

void loop() {
  // Read volume knob voltage.
  readVolume = readVolumeVoltage();
  Serial.println(readVolume);
  
  // Read buttons.
  readButton = readButtons();
  
  // Only proceed if a button is pressed and it's not the current source.
  if (readButton != -1 && readButton != currentSource) {
    currentSource = readButton;
    
    // Turn on LED for current source.
    setCurrentLED(currentSource);
    
    // Send command to AV switch.
    setCurrentVideo(currentSource);
  }
  
  delay(100);
}
