 /*
 * File:   Main.c
 * Author: Emilio Velasquez 18352
 *
 * Created on 15 de febrero de 2021, 08:12 AM
 */

//Slave_1
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
#include <stdint.h>
#include <stdio.h>
#include "spi.h"        //se llaman las librerias a utilizar

uint8_t Data_ADC = 0;
uint8_t Test = 0;      //se crean variables para guardar datos y testear

void setup(void);
void config_adc(void);
void ADC_Read(void);  // se crean los prototipos de las funciones

void __interrupt() isr(void) {

    if (PIR1bits.ADIF) {    //interrupcion para guardar dato del ADC
        PIR1bits.ADIF = 0;
        Data_ADC = ADRESH;  //Se guarda el dato del ADC
    }

    if (SSPIF == 1) {       //interrupcion para recibir dato del SPI master
        Test = SSPBUF;
        SSPIF = 0;
    }

}

void setup(void) {      //funcion de inicializacion y configuracion del PIC
    ANSEL = 0x01;
    ANSELH = 0x00;      //se habilita unicamente para un puerto analogo

    TRISA = 0x21;       // se establece como entrada RA5 y RA0 para el SS y Ponteciometro 
    TRISB = 0x00;
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 0;   //Se configura bits para SPI en puerto C

    PORTA = 0x00;
    PORTB = 0x00;       //Se limpian los puertos

    config_adc();       //se llama la funcion de configuracion del ADC

    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE); //se inicia y configura el SPI en Slave

    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1; //se habilitan variables para interrupciones globales
    PIE1bits.ADIE = 1;  //Se habilita interrupcion del ADC
    PIR1bits.ADIF = 0;  //Resetea bandera de conversion ADC

    PIE1bits.SSPIE = 1;
    PIR1bits.SSPIF = 0; //se habilitan las interrupciones para SPI
    return;
}

void main(void) {
    setup();        // se llama la funcion para iniciar el pic

    while (1) {
        ADC_Read();     //se llama la funcion para leer el adc
        SSPBUF = Data_ADC;  //Se envia dato a travez del SPI
        PORTB = Data_ADC;   //Para testeo se muestra el dato ADC en el puerto B
    }
}

void config_adc(void) {     //Funcion de configuracion del ADC
    ADCON0 = 0x41;          //Se configura el ADC, se enciende y selecciona canal 0
    ADCON1bits.ADFM = 0;    //Justifica a la izquierda
    ADCON1bits.VCFG0 = 0;   //Se apagan voltajes de referncia
    ADCON1bits.VCFG1 = 0;
    return;
}

void ADC_Read(void) {       //Funcion de lectua del ADC
    if (ADCON0bits.GO_DONE == 0) {  //cuando no esta completada la conversion
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");         //No se realiza nada mientras ocurre la conversion
        ADCON0bits.GO_DONE = 1; // iniciar cconversion 
    }
}

