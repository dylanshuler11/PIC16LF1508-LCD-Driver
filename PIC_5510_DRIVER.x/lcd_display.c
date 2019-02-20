/*
 * File:   newmain.c
 * Author: shuler18
 *
 * Created on February 7, 2019, 3:39 PM
 */

// PIC16LF1508 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF        // Watchdog Timer Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover Mode (Internal/External Switchover Mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define _XTAL_FREQ 31000 //500kHz, default
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include "lcd_display.h"
#include "SPI_comms.h"
#include "nokiaFont.h"

void main(void) {
    OSCCONbits.IRCF=0x0; //start lf 31khz clk
    SCS1 = 1; //using internal oscillator block
    ANSELBbits.ANSB5=0; //set reset pin to be digital
    TRISBbits.TRISB5 = 0; //set reset pin to be output
    RST=1; //make sure reset is high at the start of execution
    
    //init MSSP and LCD
    SPI_init_master();
    init_5510();
    set_xy(0,0);
    fill_lcd(0);
    
    //fun stuff:
    while(1) {
        fill_lcd(0x0F); //put some crap on screen, should be half of each col
        __delay_ms(500);
        fill_lcd(0x0);
    }
}

/*
 * function to set cursor to specific row and write data to it
 */
void set_pixel(char x, char y, char data)
{
    set_x(x);
    set_y(y);
    SPI_write_data(data);
}

/*
 * Function to send LCD commands to LCD via SPI bus
 */
void sendLCDcmd(char cmd)
{
    DC = 0;
    SPI_write_data(cmd);
}


void sendLCDdata(char data)
{
    int i;
    for(i = 0; i < 5; i++)
    {
        //SPI_write_data(font[(data) - (0x20)[i]]);
    }
}

/*
 *function to send init commands to the lcd
 */
void init_5510(void)
{
    //reset LCD on startup. Takes t(ms)>100
    __delay_ms(100); //likely not needed, but ensures the negative edge is seen
    RST = 0;
    __delay_ms(500);
    RST = 1;
    
    //lcd init commands
    sendLCDcmd(0x21); //extended cmd's follow
    sendLCDcmd(0xC0); //set Vop aka contrast, this was the best value found
    sendLCDcmd(0x07); //set tempature coef
    sendLCDcmd(0x13); //set LCD Bias to 1:65 may be wrong/unneeded
    sendLCDcmd(0x20); //must send 0x20 before modding display control mode
    sendLCDcmd(0x0C); //normal display mode
}

/*
 *function to fill lcd screen with a single value
 * 
 * int wb - 0=white ff = black
 */
void fill_lcd(char wb)
{
    int i, j;
    DC = 1; //sending data
    for(i=0; i<6; i++)
    {
        for(j=0; j<84; j++)
        {
            SPI_write_data(wb);
        }
    }
}

/*
 * function to set the position of both X and Y cursors
 */
void set_xy(char x, char y)
{
    sendLCDcmd(x | 0x80);
    sendLCDcmd(y | 0x40);
}

/*
 * function to set the position of LCD's X cursor
 */
void set_x(char x)
{
    sendLCDcmd(x | 0x80);
}

/*
 * function to set the position LCD's Y cursor
 */
void set_y(char y)
{
    sendLCDcmd(y | 0x40);
}