# TiltDrive

**A wireless RC car you drive by tilting your hand.**

Tilt your hand forward and the car drives forward. Tilt left and it turns left. Hold your hand flat and it stops. No joystick, no buttons.

![TiltDrive car and glove](images/tiltdrive.jpg)

> 🎥 **Demo video:** [Watch TiltDrive in action](#) <!-- replace # with your video link -->

---

## How it works

TiltDrive is two separate devices that talk over a 2.4 GHz radio link:

```
GLOVE:  MPU6050 (tilt) ──I2C──▶ Arduino Nano ──SPI──▶ nRF24L01
                                                         │
                                                   2.4 GHz radio
                                                         ▼
CAR:    Motors ◀── L298N motor driver ◀── Arduino Uno ◀──SPI── nRF24L01
```

1. **Sense:** The MPU6050 accelerometer on the glove measures which way gravity points, giving the hand's forward/back and left/right tilt.
2. **Send:** The Arduino Nano packs the tilt values into a small data packet and transmits it over the nRF24L01 radio about 20 times per second.
3. **Receive:** The Arduino Uno on the car reads each packet and decides which direction to drive.
4. **Drive:** The Uno sets the L298N motor driver's direction pins, which switch power from the battery to the motors.
5. **Failsafe:** If the car hasn't heard from the glove for 0.3 seconds, it stops automatically.

| Hand position | Car action |
|---|---|
| Tilt forward | Drive forward |
| Tilt back | Reverse |
| Tilt left | Turn left (spin in place) |
| Tilt right | Turn right (spin in place) |
| Flat | Stop |
| Signal lost | Stop (failsafe) |

---

## Hardware

### Glove (transmitter)
| Part | Purpose |
|---|---|
| Arduino Nano | Reads the sensor and sends data |
| MPU6050 (GY-521) | Accelerometer/gyroscope that measures hand tilt |
| nRF24L01+ PA+LNA | 2.4 GHz radio transmitter |
| 400-point breadboard | Holds the electronics on the glove |
| 9V battery | Powers the glove (disconnected at the snap to turn off) |
| Glove | Mounting base |

### Car (receiver)
| Part | Purpose |
|---|---|
| Arduino Uno R3 | Receives data and controls the motors |
| nRF24L01+ PA+LNA | 2.4 GHz radio receiver |
| L298N motor driver | H-bridge that controls motor direction and handles motor current |
| 2× TT gear motors + 2WD chassis | Drivetrain |
| 2× 18650 Li-ion cells (7.4V) | Powers the motors and the Uno |
| 2-cell 18650 holder with switch | Battery mount and on/off |

---

## Wiring

### Glove: Arduino Nano

| MPU6050 | Nano |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

| nRF24L01 | Nano |
|---|---|
| VCC | **3V3** (not 5V) |
| GND | GND |
| CE | D9 |
| CSN | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| IRQ | not connected |

| 9V battery | Nano |
|---|---|
| + (red) | VIN |
| − (black) | GND |

### Car: Arduino Uno + L298N

| Battery / L298N | Connection |
|---|---|
| Battery + | L298N 12V terminal |
| Battery − | L298N GND terminal |
| L298N 12V terminal | Uno VIN (powers the Uno from the battery) |
| L298N GND terminal | Uno GND (shared ground) |
| L298N OUT1 / OUT2 | Motor A |
| L298N OUT3 / OUT4 | Motor B |
| ENA / ENB | Jumper caps left on (full speed) |

| L298N | Uno |
|---|---|
| IN1 | D7 |
| IN2 | D4 |
| IN3 | D3 |
| IN4 | D2 |

| nRF24L01 | Uno |
|---|---|
| VCC | **3.3V** (not 5V) |
| GND | GND |
| CE | D9 |
| CSN | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| IRQ | not connected |

> ⚠️ The nRF24L01 runs on **3.3V only**. Connecting it to 5V can damage it.

---

## Software

**Libraries:**
- [RF24 by TMRh20](https://github.com/nRF24/RF24) (install through the Arduino IDE Library Manager)
- `SPI` and `Wire` (built into the Arduino IDE)

**Code:**
- [`glove/glove_final/glove_final.ino`](glove/glove_final/glove_final.ino): upload to the Nano
- [`car/car_final/car_final.ino`](car/car_final/car_final.ino): upload to the Uno

**Upload settings for the Nano:** Board → Arduino Nano, Processor → ATmega328P (Old Bootloader).

### Tuning
- **Sensitivity:** change `TILT` in the car code (lower = more sensitive).
- **Directions reversed:** swap `forward()`/`backward()` or `turnLeft()`/`turnRight()` in the car's `loop()`.
- **Forward tilt turns instead of driving:** the sensor is mounted rotated 90°; swap `data.x` and `data.y` in the car's `loop()`.

---

## Challenges and what I learned

- **Battery short on first power-up.** The battery pack smoked briefly during my first test. I shut it off immediately, traced the wiring, and learned to double-check polarity and power connections before switching on lithium cells.
- **Motors spinning in opposite directions.** One motor was wired in reverse. Swapping its two wires on the L298N fixed it, and it made me understand how an H-bridge controls direction.
- **Getting the radios to connect.** The nRF24L01 wouldn't connect at first. Debugging it meant learning how SPI works (SCK, MOSI, MISO, CSN) and why those pins are fixed in hardware.
- **Reading the motion sensor.** I read the MPU6050 directly over I2C (no sensor library), which meant learning about device addresses and registers.
- **Back-heavy chassis.** The battery pack is the heaviest part, so I rearranged the layout to move it closer to the center of the car.
- **Running wirelessly.** I powered the Uno from the car battery through VIN and gave the glove its own 9V supply so neither side needs a computer.

---

## Future upgrades

- [ ] **Proportional speed:** more tilt = faster, using PWM on the L298N's ENA/ENB pins
- [ ] **Flex sensors** on the fingers for extra commands
- [ ] **Lights** that react to driving direction
- [ ] Smoother turning (one wheel slower instead of spinning in place)
- [ ] A more permanent, soldered glove board

---

Built by **Arshdeep Gabri** · [LinkedIn](https://www.linkedin.com/in/arshtheparsh)
