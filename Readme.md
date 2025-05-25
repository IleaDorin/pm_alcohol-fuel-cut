# Alcohol-Triggered Fuel Cut – Firmware

This firmware runs on an Arduino Uno and is part of a system that detects alcohol concentration using an MQ-3 sensor. When the measured level exceeds a defined threshold, the system activates a relay (cutting power to the fuel pump), turns on an LED, and sounds a buzzer. A separate button allows the user to mute the buzzer while keeping the other outputs active.

The implementation is written in C using direct register access, without relying on Arduino libraries.

## Lab concepts used

### GPIO
Digital outputs are used to control the LED, buzzer, and relay using `PORTD` and `DDRD`. A digital input with pull-up is used for the mute button.

### USART (Serial Monitor)
Serial communication is initialized and handled using `UBRR0`, `UCSR0x`, and `UDR0`. Sensor values and system messages are printed to the Serial Monitor over USB at 9600 baud.

### ADC
The analog signal from the MQ-3 sensor is read on pin A1 using the internal ADC. Configuration is done using `ADMUX`, `ADCSRA`, and `ADC`.

### Interrupts
A pin change interrupt (`PCINT21`, PD5) is used to detect button presses. The ISR sets a flag that disables the buzzer while the alarm is active.

## Pin configuration

| Pin  | Function         |
|------|------------------|
| A1   | MQ-3 analog output |
| D3   | LED              |
| D4   | Buzzer           |
| D5   | Mute button      |
| D7   | Relay control    |
