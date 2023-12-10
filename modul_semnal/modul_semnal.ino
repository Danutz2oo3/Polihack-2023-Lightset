#include <RH_ASK.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

RH_ASK driver;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();


bool is_blinking = false;

const int leftRelayPin = 3;  // Change this to the digital pin connected to the left relay
const int rightRelayPin = 4; 
const int stopRelayPin = 5;
const int frontRelayPin = 6; // Change this to the digital pin connected to the right relay

void setup() {
  Serial.begin(9600); // Debugging only
  if (!driver.init())
    Serial.println("RF initialization failed");

  pinMode(leftRelayPin, OUTPUT);
  pinMode(rightRelayPin, OUTPUT);
  pinMode(stopRelayPin, OUTPUT);
  pinMode(frontRelayPin, OUTPUT);
   if(!accel.begin())
   {
      Serial.println("No valid sensor found");
      while(1);
   }
   Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);
}

void loop() {
  uint8_t buf[12];
  uint8_t buflen = sizeof(buf);
  if(is_blinking == false){
      if (driver.recv(buf, &buflen)) // Non-blocking
      {
        // Message with a good checksum received, dump it.
        buf[buflen] = '\0'; // Null-terminate the received data
        Serial.print("Message: ");
        Serial.println((char*)buf);

        // Activate relays based on the received direction
        if (strcmp((char*)buf, "LEFT") == 0) {
          blinkRelay(leftRelayPin);
        } else if (strcmp((char*)buf, "RIGHT") == 0) {
          blinkRelay(rightRelayPin);
        } else if (strcmp((char*)buf, "PRESSED") == 0){
          blinkBothRelay(rightRelayPin, leftRelayPin);
        }
      }
    }

    sensors_event_t event; 
    accel.getEvent(&event);
    float x = event.acceleration.x;
    float y = event.acceleration.y;
    float z = event.acceleration.z;
    Serial.print("X: "); Serial.print(x); Serial.print("");
    Serial.print("Y: "); Serial.print(y); Serial.print("");
    Serial.print("Z: "); Serial.print(z); Serial.print("");
    if(x > 9 || x < -9 || y > 9 || y < -9){
      blinkAllRelay(rightRelayPin, leftRelayPin, stopRelayPin, frontRelayPin);
    }
    Serial.println();

    delay(100);
}

void blinkRelay(int relayPin) {
  is_blinking = true;
  for (int i = 0; i < 4; ++i) {
    digitalWrite(relayPin, HIGH);  // Activate the relay
    delay(500);  // Wait for 0.5 seconds (adjust as needed)
    digitalWrite(relayPin, LOW);   // Deactivate the relay
    delay(500);  // Wait for 0.5 seconds (adjust as needed)
  }
  is_blinking = false;
}
void blinkBothRelay(int relayPin1, int relayPin2) {
  is_blinking = true;
  for (int i = 0; i < 7; ++i) {
    digitalWrite(relayPin1, HIGH);  // Activate the relay
    digitalWrite(relayPin2, HIGH);  // Activate the relay

    delay(500);  // Wait for 0.5 seconds (adjust as needed)
    digitalWrite(relayPin1, LOW);   // Deactivate the relay
    digitalWrite(relayPin2, LOW);   // Deactivate the relay

    delay(500);  // Wait for 0.5 seconds (adjust as needed)
  }
  is_blinking = false;
}

void blinkAllRelay(int relayPin1, int relayPin2, int relayPin3, int relayPin4) {
  is_blinking = true;
  for (int i = 0; i < 7; ++i) {
    digitalWrite(relayPin1, HIGH);  // Activate the relay
    digitalWrite(relayPin2, HIGH); 
    digitalWrite(relayPin3, 
    );
    digitalWrite(relayPin4, HIGH); // Activate the relay

    delay(500);  // Wait for 0.5 seconds (adjust as needed)
    digitalWrite(relayPin1, LOW);   // Deactivate the relay
    digitalWrite(relayPin2, LOW); 
    digitalWrite(relayPin3, LOW); 
    digitalWrite(relayPin4, LOW);   // Deactivate the relay

    delay(500);  // Wait for 0.5 seconds (adjust as needed)
  }
  is_blinking = false;
}