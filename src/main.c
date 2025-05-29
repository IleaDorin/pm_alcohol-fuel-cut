#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define THRESHOLD 750 // ya drunk from here on...

#define MQ3_ANALOG_CHANNEL 1 // A1
#define LED_PIN_RED PD3
#define LED_PIN_GREEN PD6
#define BUZZER_PIN PD4
#define RELAY_PIN PD7
#define BUTTON_PIN PD5

volatile uint8_t buzzer_muted = 0;

void UART_init(unsigned int ubrr)
{
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Send a character over UART
void UART_transmit(char data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

// Send string over UART
void UART_print(const char *str)
{
    while (*str)
    {
        UART_transmit(*str++);
    }
}

// Send number as ASCII
// inspo from:
// https://stackoverflow.com/questions/75956769/transmitting-number-as-number-through-uart-with-stm32
void UART_print_number(uint16_t num)
{
    char buffer[10];
    itoa(num, buffer, 10);
    UART_print(buffer);
    UART_print("\r\n");
}

// ADC init to read from ADC1 (A1) (from lab tbh)
void ADC_init()
{
    ADMUX = 0;
    ADMUX |= (1 << MUX0);
    ADMUX |= (1 << REFS0);

    ADCSRA = 0;
    ADCSRA |= (7 << ADPS0);
    ADCSRA |= (1 << ADEN);
}

// Read from A1
uint16_t ADC_read()
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;
    return ADC;
}

// external interrupt for buzzer mute
void interrupt_init()
{
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);

    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT21);
    sei();
}

// ISR for pin change
ISR(PCINT2_vect)
{
    if (!(PIND & (1 << BUTTON_PIN)))
    { // button pressed
        buzzer_muted = 1;
        UART_print(">>> Buzzer muted by interrupt!\r\n");
    }
}

int main(void)
{
    DDRD |= (1 << LED_PIN_RED) | (1 << BUZZER_PIN) | (1 << RELAY_PIN) | (1 << LED_PIN_GREEN);
    PORTD |= (1 << RELAY_PIN);

    UART_init(103);
    ADC_init();
    interrupt_init();

    while (1)
    {
        uint16_t value = ADC_read();
        UART_print("ADC = ");
        UART_print_number(value);

        if (value > THRESHOLD)
        {
            UART_print(">>> Alcohol detected!\r\n");
            PORTD |= (1 << LED_PIN_RED);
            PORTD &= ~(1 << RELAY_PIN);
            PORTD &= ~(1 << LED_PIN_GREEN);

            if (!buzzer_muted)
            {
                PORTD |= (1 << BUZZER_PIN);
            }
            else
            {
                PORTD &= ~(1 << BUZZER_PIN);
            }
        }
        else
        {
            PORTD &= ~(1 << LED_PIN_RED);
            PORTD &= ~(1 << BUZZER_PIN);
            PORTD |= (1 << RELAY_PIN);
            PORTD |= (1 << LED_PIN_GREEN);
            buzzer_muted = 0; // resetting when no alcohol is detected
        }

        _delay_ms(500);
    }
}
