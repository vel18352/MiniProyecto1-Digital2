 /*
 * File:   Main.c
 * Author: Emilio Velasquez 18352
 *
 * Created on 15 de febrero de 2021, 08:14 AM
 */

//Slave2
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define _XTAL_FREQ   4000000


// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "spi.h"
#include <stdint.h>    //se llaman las librerias a utilizar

uint8_t Test = 0;     //se crean variables para guardar datos y testear

void setup(void);     // se crean los prototipos de las funciones

void __interrupt() isr(void) { //interrupcion para recibir dato del SPI master
    if (SSPIF == 1) {   
        Test = SSPBUF;
        SSPIF = 0;
    }
}

void main(void) {
    setup();          // se llama la funcion para iniciar el pic
    while (1) {
        SSPBUF = PORTB; //se envia datos a travez del SPI
        if (PORTEbits.RE0 == 1) {   //Si el primer boton es presionado
            PORTB++;                //Incrementa puerto B
            __delay_ms(275);        //delay para evitar rebote
        } else if (PORTEbits.RE1 == 1) {    //si el segundo boton es presionado
            PORTB--;                //Decrementa puerto B
            __delay_ms(275);        //delay para evitar rebote
        }   
    }
}

void setup(void) {   //funcion de inicializacion y configuracion del PIC
    ANSEL = 0x00;
    ANSELH = 0x00;      //se establecen puertos como digitales

    TRISA = 0x20;       //se establece RA5 como input para SS 
    TRISB = 0x00;           
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 0;   //Se configura bits para SPI en puerto C
    TRISE = 0x03;       //se establece como input las primeras dos entradas del puerto E

    PORTA = 0x00;
    PORTB = 0x00;
    PORTE = 0x00;       //se limpian puertos
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE); //se inicia y configura el SPI en Slave
}

