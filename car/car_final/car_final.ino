/*
  TiltDrive - Car (Receiver)
  Board: Arduino Uno

  Receives hand-tilt data from the glove over an nRF24L01 radio and
  drives two DC motors through an L298N H-bridge motor driver.
  Includes a failsafe that stops the car if the signal is lost.

  Wiring:
    L298N    IN1 -> D7    IN2 -> D4    IN3 -> D3    IN4 -> D2
             ENA / ENB jumpers left on (full speed)
    nRF24L01 VCC -> 3.3V  GND -> GND   CE  -> D9    CSN -> D10
             SCK -> D13   MOSI -> D11  MISO -> D12  IRQ -> (unused)

  Library: RF24 by TMRh20 (install via Library Manager)
*/

#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);                  // CE, CSN
const byte address[6] = "00001";    // must match the glove

// Motor driver direction pins
const int IN1 = 7, IN2 = 4;         // motor A
const int IN3 = 3, IN4 = 2;         // motor B

const int TILT = 6000;              // tilt needed to move (lower = more sensitive)
const unsigned long TIMEOUT = 300;  // ms without signal before stopping

struct Data {
  int16_t x;
  int16_t y;
};
Data data;
unsigned long lastSignal = 0;

// ---- Movement helpers ----
void setMotors(int a, int b, int c, int d) {
  digitalWrite(IN1, a); digitalWrite(IN2, b);
  digitalWrite(IN3, c); digitalWrite(IN4, d);
}
void forward()   { setMotors(HIGH, LOW,  HIGH, LOW);  }
void backward()  { setMotors(LOW,  HIGH, LOW,  HIGH); }
void turnLeft()  { setMotors(LOW,  HIGH, HIGH, LOW);  }  // spin in place
void turnRight() { setMotors(HIGH, LOW,  LOW,  HIGH); }
void stopCar()   { setMotors(LOW,  LOW,  LOW,  LOW);  }

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopCar();

  // Radio setup: this side LISTENS
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    lastSignal = millis();

    if      (data.x >  TILT) forward();
    else if (data.x < -TILT) backward();
    else if (data.y >  TILT) turnRight();
    else if (data.y < -TILT) turnLeft();
    else                     stopCar();     // hand flat = stop
  }

  // Failsafe: no signal recently -> stop
  if (millis() - lastSignal > TIMEOUT) stopCar();
}
