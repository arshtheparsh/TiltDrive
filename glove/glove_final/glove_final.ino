/*
  TiltDrive - Glove (Transmitter)
  Board: Arduino Nano

  Reads hand tilt from an MPU6050 accelerometer and sends it to the car
  over an nRF24L01 2.4 GHz radio, 20 times per second.

  Wiring:
    MPU6050  VCC -> 5V    GND -> GND    SDA -> A4    SCL -> A5
    nRF24L01 VCC -> 3V3   GND -> GND    CE  -> D9    CSN -> D10
             SCK -> D13   MOSI -> D11   MISO -> D12  IRQ -> (unused)

  Library: RF24 by TMRh20 (install via Library Manager)
*/

#include <SPI.h>
#include <RF24.h>
#include <Wire.h>

RF24 radio(9, 10);                  // CE, CSN
const byte address[6] = "00001";    // must match the car

const uint8_t MPU_ADDR = 0x68;      // MPU6050 I2C address

// Data packet sent to the car
struct Data {
  int16_t x;   // forward / backward tilt
  int16_t y;   // left / right tilt
};
Data data;

void setup() {
  Serial.begin(9600);

  // Wake up the MPU6050 (it starts in sleep mode)
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);                 // power management register
  Wire.write(0);                    // 0 = wake up
  Wire.endTransmission();

  // Radio setup: this side SENDS
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  // Read accelerometer X, Y, Z (6 bytes starting at register 0x3B)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)6);

  data.x = Wire.read() << 8 | Wire.read();
  data.y = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read();         // Z axis not needed

  radio.write(&data, sizeof(data));

  // Debug output (Serial Monitor @ 9600 baud)
  Serial.print("X: "); Serial.print(data.x);
  Serial.print("  Y: "); Serial.println(data.y);

  delay(50);                        // ~20 updates per second
}
