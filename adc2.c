// BURST MODE OPERATION 

#include <lpc17xx.h>
#include <stdio.h>

#define RS_CTRL 0x08000000 // P0.27
#define EN_CTRL 0x10000000 // P0.28
#define DT_CTRL 0x07800000 // P0.23 to P0.26 data lines

void lcd_init(void);
void lcd_puts(unsigned char *);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_data(void);
void lcd_com(void);

void adc_init(void);
void adc_read(unsigned int* channel, unsigned int* result);

extern unsigned long int temp1, temp2;
unsigned long int temp1 = 0, temp2 = 0;

int main(void) {
    unsigned int adc_channel, adc_value;
    char buffer[16];

    SystemInit();
    SystemCoreClockUpdate();
    lcd_init();
    adc_init();
    
    clr_disp(); // Clear the display before writing new content
    
    while (1) {
        adc_read(&adc_channel, &adc_value); // Read ADC channel and value
        sprintf(buffer, "Ch: %d ADC: %d", adc_channel, adc_value); // Convert ADC value to string
        clr_disp(); // Clear display
        lcd_puts((unsigned char*)buffer); // Display ADC value on LCD
        delay_lcd(1000000); // Delay for display
    }
}

// LCD initialization function
void lcd_init() {
    LPC_PINCON->PINSEL3 &= 0xFC003FFF; // P0.23 to P0.28 as GPIO
    LPC_GPIO0->FIODIR |= DT_CTRL;      // Set P0.23 to P0.26 as output
    LPC_GPIO0->FIODIR |= RS_CTRL;      // Set P0.27 as output
    LPC_GPIO0->FIODIR |= EN_CTRL;      // Set P0.28 as output
    clr_disp();
    delay_lcd(3200);

    temp1 = 0x28; // Function set: 4-bit, 2 lines, 5x8 font
    lcd_com();
    delay_lcd(30000);

    temp1 = 0x0C; // Display ON, cursor OFF, blink OFF
    lcd_com();
    delay_lcd(800);

    temp1 = 0x06; // Entry mode set
    lcd_com();
    delay_lcd(800);
}

// Send command to LCD
void lcd_com(void) {
    temp2 = temp1 & 0xF0; // High nibble
    temp2 = temp2 << 19;  // Move to data lines position
    LPC_GPIO0->FIOCLR = DT_CTRL; // Clear data lines
    LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data lines
    LPC_GPIO0->FIOCLR = RS_CTRL; // Clear bit RS (command mode)
    LPC_GPIO0->FIOSET = EN_CTRL; // EN = 1
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL; // EN = 0

    temp2 = temp1 & 0x0F; // Low nibble
    temp2 = temp2 << 23;  // Move to data lines position
    LPC_GPIO0->FIOCLR = DT_CTRL; // Clear data lines
    LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data lines
    LPC_GPIO0->FIOCLR = RS_CTRL; // Clear bit RS (command mode)
    LPC_GPIO0->FIOSET = EN_CTRL; // EN = 1
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL; // EN = 0
}

// Send data to LCD
void lcd_data(void) {
    temp2 = temp1 & 0xF0; // High nibble
    temp2 = temp2 << 19;  // Move to data lines position
    LPC_GPIO0->FIOCLR = DT_CTRL; // Clear data lines
    LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data lines
    LPC_GPIO0->FIOSET = RS_CTRL; // Set RS to 1 (data mode)
    LPC_GPIO0->FIOSET = EN_CTRL; // EN = 1
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL; // EN = 0

    temp2 = temp1 & 0x0F; // Low nibble
    temp2 = temp2 << 23;  // Move to data lines position
    LPC_GPIO0->FIOCLR = DT_CTRL; // Clear data lines
    LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data lines
    LPC_GPIO0->FIOSET = RS_CTRL; // Set RS to 1 (data mode)
    LPC_GPIO0->FIOSET = EN_CTRL; // EN = 1
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL; // EN = 0
}

// Clear display
void clr_disp(void) {
    temp1 = 0x01; // Clear display command
    lcd_com();
    delay_lcd(1000000);
}

// Delay function
void delay_lcd(unsigned int r1) {
    unsigned int r;
    for (r = 0; r < r1; r++);
}

// ADC initialization for burst mode
void adc_init(void) {
    // Step 1: Configure the GPIO pin for ADC function using the PINSEL register
    LPC_PINCON->PINSEL1 |= (1 << 14) | (1 << 16); // P0.23 as AD0.0, P0.24 as AD0.1

    // Step 2: Enable the Clock to ADC module
    LPC_SC->PCONP |= (1 << 12); // Power up ADC

    // Step 3: Configure the ADC for burst mode and select channels
    LPC_ADC->ADCR = (1 << 0) |   // Select channel AD0.0
                    (1 << 1) |   // Select channel AD0.1
                    (4 << 8) |   // ADC clock is 25 MHz / (4 + 1) = 5 MHz
                    (1 << 16) |  // Enable burst mode
                    (1 << 21);   // Enable ADC
}

// ADC read function in burst mode
void adc_read(unsigned int* channel, unsigned int* result) {
    // In burst mode, the ADC continuously updates the result register.
    // Read the 12-bit A/D value from ADGDR (corresponding to the last converted channel)
    unsigned long valueADGDR = LPC_ADC->ADGDR;
    *channel = (valueADGDR >> 24) & 0x07; // Extract channel number
    *result = (valueADGDR >> 4) & 0xFFF;  // Extract conversion result
}
