#include <lpc17xx.h>

#define RS_CTRL 0x08000000 // P0.27
#define EN_CTRL 0x10000000 // P0.28
#define DT_CTRL 0x07800000 // P0.23 to P0.26 data lines

void lcd_init(void);
void wr_cn(void);
void clr_disp(void);
void delay_lcd(unsigned int);
void lcd_com(void);
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void lcd_puts(unsigned char *);

extern unsigned long int temp1, temp2;
unsigned long int temp1 = 0, temp2 = 0;

int main(void) {
    unsigned char Msg3[11] = {"MIT"};
    unsigned char Msg4[12] = {"ICT"};

    SystemInit();
    SystemCoreClockUpdate();
    lcd_init();
    temp1 = 0x80;
    lcd_com();
    delay_lcd(800000);
    lcd_puts(&Msg3[0]);
    temp1 = 0xC0;
    lcd_com();
    delay_lcd(800000);
    lcd_puts(&Msg4[0]);
}

// LCD initialization
void lcd_init() {
    /* Ports initialized as GPIO */
    LPC_PINCON->PINSEL3 &= 0xFC003FFF; // P0.23 to P0.28 as GPIO
    /* Setting the directions as output */
    LPC_GPIO0->FIODIR |= DT_CTRL;
    LPC_GPIO0->FIODIR |= RS_CTRL;
    LPC_GPIO0->FIODIR |= EN_CTRL;
    clear_ports();
    delay_lcd(3200);

    // Initialization sequence for LCD
    temp2 = (0x30 << 19);
    wr_cn();
    delay_lcd(30000);
    temp2 = (0x30 << 19);
    wr_cn();
    delay_lcd(30000);
    temp2 = (0x30 << 19);
    wr_cn();
    delay_lcd(30000);
    temp2 = (0x20 << 19);
    wr_cn();
    delay_lcd(30000);

    // Function set: 4-bit mode, 2 lines, 5x8 dots
    temp1 = 0x28;
    lcd_com();
    delay_lcd(30000);

    // Display on, cursor off, blink off
    temp1 = 0x0C;
    lcd_com();
    delay_lcd(800);

    // Entry mode set: increment mode, no shift
    temp1 = 0x06;
    lcd_com();
    delay_lcd(800);

    // Clear display
    temp1 = 0x01;
    lcd_com();
    delay_lcd(1000000);

    // Set DDRAM address to 0x80
    temp1 = 0x80;
    lcd_com();
    delay_lcd(800);
    return;
}

void lcd_com(void) {
    temp2 = temp1 & 0xF0; // High nibble
    temp2 = temp2 << 19;  // Move to data lines position
    wr_cn();
    temp2 = temp1 & 0x0F; // Low nibble
    temp2 = temp2 << 23;  // Move to data lines position
    wr_cn();
    delay_lcd(1000000);
    return;
}

// Command nibble output routine
void wr_cn(void) { // Write command reg
    clear_ports();
    LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data lines
    LPC_GPIO0->FIOCLR = RS_CTRL; // Clear bit RS
    LPC_GPIO0->FIOSET = EN_CTRL; // EN = 1
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL; // EN = 0
    return;
}

// Data output routine which also outputs high nibble first and lower nibble next
void lcd_data(void) {
    temp2 = temp1 & 0xF0;
    temp2 = temp2 << 19;
    wr_dn();
    temp2 = temp1 & 0x0F;
    temp2 = temp2 << 23;
    wr_dn();
    delay_lcd(1000000);
    return;
}

// Data nibble output routine
void wr_dn(void) {
    clear_ports();
    LPC_GPIO0->FIOPIN = temp2; // Assign the value to the data lines
    LPC_GPIO0->FIOSET = RS_CTRL; // Set bit RS
    LPC_GPIO0->FIOSET = EN_CTRL; // EN = 1
    delay_lcd(25);
    LPC_GPIO0->FIOCLR = EN_CTRL; // EN = 0
    return;
}

void delay_lcd(unsigned int r1) {
    unsigned int r;
    for(r = 0; r < r1; r++);
    return;
}

void clr_disp(void) {
    temp1 = 0x01;
    lcd_com();
    delay_lcd(1000000);
    return;
}

void clear_ports(void) {
    /* Clearing the lines at power on */
    LPC_GPIO0->FIOCLR = DT_CTRL; // Clearing data lines
    LPC_GPIO0->FIOCLR = RS_CTRL; // Clearing RS line
    LPC_GPIO0->FIOCLR = EN_CTRL; // Clearing Enable line
    return;
}

void lcd_puts(unsigned char *buf1) {
    unsigned int i = 0;
    while(buf1[i] != '\0') {
        temp1 = buf1[i];
        lcd_data();
        i++;
        if(i == 16) {
            temp1 = 0xC0;
            lcd_com();
        }
    }
    return;
}
