#include <SoftwareSerial.h>
#include <SPI.h>
 
SoftwareSerial rs232(2, 3); //232_TX,232_RX

const int inputButtons[] = {A0, A1, A2};
const int ledPins[] = {4, 5, 6};

int readButton = -1;
float readVolume = 0.0;
int readGain = 1;

int currentSource = 0;

int currentGain = 1;
int mutePin = 9;
int slaveSelectPin = 10;

void setup() {
  // Setup serial communication for debugging.
  Serial.begin(9600);
  
  // Setup RS232.
  rs232.begin(9600);
  
  // Setup SPI.
  pinMode(slaveSelectPin, OUTPUT);
  pinMode(mutePin, OUTPUT);
  SPI.begin();
  
  // Setup pins for input from buttons.
  for (int i = 0; i < 3; i++) {
    pinMode(inputButtons[i], INPUT);
  }
  
  // Setup pins for output to LEDs.
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  
  // Reset.
  setCurrentLED(currentSource);
  setCurrentVideo(currentSource);
  setVolume(1);
  digitalWrite(mutePin, HIGH);
}

/**
 * Return index of button being pressed.
*/
int readButtons() {
  int buttonState = -1;
  
  // Iterate over all buttons.
  for (int i = 0; i < 3; i++) {
    buttonState = digitalRead(inputButtons[i]);
    
    // Check if button is pressed.
    if (buttonState == HIGH) {
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
 * Set volume on PGA2310 through SPI.
*/
void setVolume(int gain) {
  digitalWrite(slaveSelectPin, LOW);
  SPI.transfer(gain);
  SPI.transfer(gain);
  digitalWrite(slaveSelectPin, HIGH);
}

/**
 * Read voltage of analog input knob.
*/
float readVolumeVoltage() {
  // Read the value from analog pin A5.
  int sensorValue = analogRead(A5);
  
  // Return analog sensor reading (0-1023).
  return sensorValue;
}

void loop() {
  // Read volume knob voltage and map to 8 bit values.
  readVolume = readVolumeVoltage();
  readGain = map(readVolume, 0, 1023, 1, 255);
  
  // Only set volume if gain has changed.
  if (readGain != currentGain) {
    currentGain = readGain;
    setVolume(currentGain);
  }
  
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
