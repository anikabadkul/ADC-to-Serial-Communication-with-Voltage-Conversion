#include <avr/io.h>
#include <util/delay.h>
#include "my_adc_lib.h"
#include "my_uart_lib.h"
#include "i2c.h"
#include "SSD1306.h"
#include <stdlib.h>

// Define the analog pin, the TMP36's Vout pin is connected to
int main(void) {
    unsigned int digitalValue; // Variable to store the digital value read from ADC
    char buffer1[6], buffer2[6]; // Buffers for converting temperature values to strings
    float TOO_HOT = 60.0; // Threshold for high temperature in Fahrenheit

    // Initialize UART, ADC, and OLED display
    uart_init();
    adc_init();
    OLED_Init();

    // Configure PORTC as input (for sensors) and PORTD5 as input with pull-up enabled
    DDRC = 0x00;
    PORTD = 1<<PORTD5;
    DDRD = 1<<DDD3; // Configure PORTD3 as output (for controlling an LED or another indicator)

    while (1) {
        // Read digital value from ADC
        digitalValue = get_adc();
        // Calculate temperature in Celsius from the digital value
        float tempC = ((V_REF*1000)/1023.0 * digitalValue - 500) / 10.0;
        // Convert Celsius to Fahrenheit
        float tempF = (tempC*1.8)+32;
        
        // Convert float to string with 1 decimal place precision
        dtostrf(tempF, 4, 1, buffer1);
        dtostrf(tempC, 4, 1, buffer2);

        // Check if button on PORTD5 is pressed (low)
        if ((PIND & (1 << PIND5)) == 0) {
            // Send temperature in Celsius to UART
            send_string("Temperature:  ");
            send_string(buffer2);
            send_string("C");
            uart_send(13); // Carriage return
            uart_send(10); // New line

            // Display temperature in Celsius on OLED
            OLED_GoToLine(1);
            OLED_DisplayString("Temperature");
            OLED_GoToLine(3);
            OLED_DisplayString(buffer2);
            OLED_DisplayString("C");
            _delay_ms(1000); // Delay to allow reading
        }
        else {
            // Send temperature in Fahrenheit to UART
            send_string("Temperature:  ");
            send_string(buffer1);
            send_string("F");
            uart_send(13); // Carriage return
            uart_send(10); // New line

            // Display temperature in Fahrenheit on OLED
            OLED_GoToLine(1);
            OLED_DisplayString("Temperature");
            OLED_GoToLine(3);
            OLED_DisplayString(buffer1);
            OLED_DisplayString("F");
            _delay_ms(1000); // Delay to allow reading
        }

        // Activate an external device if temperature is too high
        if(tempF > TOO_HOT){
            PORTD |= (1 << PORTD3); // Set PORTD3 high
        }
        else{
            PORTD &= ~(1 << PORTD3); // Set PORTD3 low
        }
    }
}
