#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver;

# define VRX_PIN  A0
# define VRY_PIN A1
#define BUTTON_PIN   8  // Arduino pin connected to SW  pin


# define LEFT_THRESHOLD  400
# define RIGHT_THRESHOLD 800

const char* DIRECTION_LEFT = "LEFT";
const char* DIRECTION_RIGHT = "RIGHT";
const char* BUTTON_PRESSED = "PRESSED";

unsigned long lastSendTime = 0;
const unsigned long disableDuration = 2000;  // Disable duration in milliseconds (2 seconds)

void setup() {
  Serial.begin(9600);
  if (!driver.init())
    Serial.println("RF initialization failed");
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  unsigned long currentTime = millis();
  int xValue = analogRead(VRX_PIN);
  int yValue = analogRead(VRY_PIN);
  int buttonState = digitalRead(BUTTON_PIN);
  //Serial.println(xValue);
  //Serial.println(yValue);
  //Serial.println(buttonState);
  const char* direction = nullptr;

  if (xValue < LEFT_THRESHOLD)
    direction = DIRECTION_LEFT;
  else if (xValue > RIGHT_THRESHOLD)
    direction = DIRECTION_RIGHT;

  // Send data over RF if there is an active direction and enough time has passed since the last send
  if (direction != nullptr && (currentTime - lastSendTime > disableDuration)) {
    Serial.print("Sending direction: ");
    Serial.println(direction);
    driver.send((uint8_t*)direction, strlen(direction));
    driver.waitPacketSent();

    // Update the last send time
    lastSendTime = currentTime;
  }
  if (buttonState == LOW && (currentTime - lastSendTime > disableDuration)) {
    Serial.println("Button Pressed!");
    driver.send((uint8_t*)BUTTON_PRESSED, strlen(BUTTON_PRESSED));
    driver.waitPacketSent();

    // Update the last send time
    lastSendTime = currentTime;
  }

  delay(10); // Adding a small delay for stability
}
