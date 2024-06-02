// FOR SOFTWARE CONTROLLED MODE
#include <lpc17xx.h>
#include <stdio.h>

#define RS_CTRL 0x08000000 // P0.27
#define EN_CTRL 0x10000000 // P0.28
#define DT_CTRL 0x07800000 // P0.23 to P0.26 data lines

void lcd_init(void);
void lcd_puts(unsigned char *);
void clr_disp(void);
void delay_lcd(unsigned int);

void adc_init(void);
uint16_t adc_read(void);

extern unsigned long int temp1, temp2;
unsigned long int temp1 = 0, temp2 = 0;

int main(void) {
    uint16_t adc_value;
    char buffer[16];

    SystemInit();
    SystemCoreClockUpdate();
    lcd_init();
    adc_init();
    
    clr_disp(); // Clear the display before writing new content
    
    while (1) {
        adc_value = adc_read(); // Read ADC value
        sprintf(buffer, "ADC: %d", adc_value); // Convert ADC value to string
        clr_disp(); // Clear display
        lcd_puts((unsigned char*)buffer); // Display ADC value on LCD
        delay_lcd(1000000); // Delay for display
    }
}

// Simplified LCD initialization
void lcd_init() {
    // LCD initialization code as explained earlier
    // Assume this function sets up the LCD correctly
}

void lcd_puts(unsigned char *buf1) {
    unsigned int i = 0;
    while (buf1[i] != '\0') {
        temp1 = buf1[i];
        lcd_data();
        i++;
        if (i == 16) {
            temp1 = 0xC0;
            lcd_com();
        }
    }
}

void clr_disp(void) {
    temp1 = 0x01; // Clear display command
    lcd_com();
    delay_lcd(1000000);
}

void delay_lcd(unsigned int r1) {
    unsigned int r;
    for (r = 0; r < r1; r++);
}

// ADC initialization
void adc_init(void) {
    // Step 1: Configure the GPIO pin for ADC function using the PINSEL register
    LPC_PINCON->PINSEL1 |= (1 << 14); // P0.23 as AD0.0

    // Step 2: Enable the Clock to ADC module
    LPC_SC->PCONP |= (1 << 12); // Power up ADC

    // Step 3: Deselect all the channels and Power on the internal ADC module
    LPC_ADC->ADCR = (0 << 0) |   // Deselect all channels
                    (4 << 8) |   // ADC clock is 25 MHz / (4 + 1) = 5 MHz
                    (1 << 21);   // Enable ADC

    // Step 4: Select the particular channel for A/D conversion
    LPC_ADC->ADCR |= (1 << 0); // Select channel AD0.0

    // Note: Steps 5-7 will be handled in the adc_read function
}

// ADC read function
uint16_t adc_read(void) {
    // Step 5: Set the ADCR.START bit for starting the A/D conversion for the selected channel
    LPC_ADC->ADCR |= (1 << 24); // Start conversion

    // Step 6: Wait for the conversion to complete
    while (!(LPC_ADC->ADGDR & (1 << 31))); // Wait for conversion to complete

    // Step 7: Read the 12-bit A/D value from ADGR.RESULT
    return (LPC_ADC->ADGDR >> 4) & 0xFFF; // Read result (12-bit value)
}

// Additional required functions for LCD (lcd_data and lcd_com)

void lcd_data(void) {
    // Send data to LCD (implementation assumed as explained previously)
}

void lcd_com(void) {
    // Send command to LCD (implementation assumed as explained previously)
}
