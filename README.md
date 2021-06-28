# 1-DoF motion simulator for dynamic oscillation

An open-source, desktop-size, highly dynamic, 1-DoF linear motion simulator for you to reliabily generate and measure linear motion for your expermentation. All components can be sourced easily.

## What you can do with it
You can mount your sample on the simulator and applied any linear motion you program to it. This setup is optimized for low payload and high acceleration, which is hard to find in commercially available solutions.

![title](/images/demo_4.5Hz.gif)

video demo (Amplitude: 10 mm, Frequency: 4.5 Hz)

## Bill of materials
- [Raspberry Pi](https://www.raspberrypi.org/) 3B/4B
- Micro controller [Teensy 4.0](https://www.pjrc.com/store/teensy40.html)
- AMS rotary encoder [AS5045 development board](https://ams.com/as5045adapterboard/)
- [1602 LCD module](https://joy-it.net/de/products/SBC-LCD16x2)
- Stepper motor [103H7823-1740](https://uk.rs-online.com/web/p/dc-motors/8787673/?sra=pstk) or equivalent
- Motor driver [G201X](https://www.geckodrive.com/g201x-digital-step-drive.html)
- MISUMI precision ball screws [KUHC1205-340-100](https://uk.misumi-ec.com/vona2/detail/110300077030/?PNSearch=KUHC1205-340-100&HissuCode=KUHC1205-340-100&searchFlow=suggest2products&Keyword=KUHC1205-340-100)
- Emergency stop button
- DC power supply 50-60V

<!-- ## Mechanics
TBD
(Explosion view of everything) -->

## Electronics
Teensy 4.0 is chosen for it real-time performance in high speed (600 Mhz). It ensures timing critical tasks are not missed, like sending precise pulses for the stepper motor and display synchronization information on the LCD screen.

The raspberry pi is used to log sensor data, in this case the encoder data. Its program is designed in a modular style and additional sensors can be easily added. (The raspberry pi could have done the motor control task by teensy only if the motor speed is low.)

### Wiring
| Teensy - realtime controller | Raspberry Pi - high speed data logging |
| --- | --- |
| ![title](/images/wiring_teensy.png) | ![title](/images/wiring_RPi.png) |

### Program
The programs for the Teensy4.0 and raspberry pi are in the folder `/electronics`. It offers
- Customizable sinusoidal oscillation: amplitude, frequency, duration, interval, repetitions, LCD display text.
- Manual control to move the platform.
- Encoder reading in 2 kHz (can be set to other sampling frequencies)


<!-- compile:
`make`

execute:
`sudo ./encoder`

data logging:
rawData -->


<!-- ## References
If this project is helpful for your project, we kindly ask you to cite us: -->
