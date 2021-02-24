 /*
 * File:   Main.c
 * Author: Emilio Velasquez 18352
 *
 * Created on 16 de febrero de 2021, 10:43 AM
 */

//Master
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
#include <pic16f887.h>
#include "lcd.h"
#include "spi.h"               // llamamos las librerias a utilizar

uint8_t Test1 = 0;             // variables para la interrupcion
uint8_t slave_select = 0;      // selector del slave
uint8_t SPI_slave_1 = 0;
uint8_t SPI_slave_2 = 0;
uint8_t SPI_slave_3 = 0;      // variables para recibir datos del SPI
char slave_1[6];
char slave_2[6];
char slave_3[6];             
char str[20];                //arrays para guardar caracteres de datos de pantalla

void setup(void);           
void init_UART(void);       //se crea los prototipos de las funciones

void Text_Uart(char *text)  //Funcion para textos donde se envian caracter por caracter al uart
{
  for(int i=0;text[i]!='\0';i++){
    TXREG = (text[i]);
  __delay_ms(5);}
}

void setup(void) {          //funcion de inicializacion y configuracion del PIC
    ANSEL = 0x00;
    ANSELH = 0x00;         //se deja los puertos como digitales

    TRISA = 0x00;
    TRISB = 0x00;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 1;
    TRISCbits.TRISC5 = 0;  //bits para el SPI en Master
    TRISD = 0x00;
    TRISE = 0x00;       // se establecen bits de entradas y salidas 

    PORTA = 0x00;
    PORTB = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;       // se limpian los puertos

    lcd_init();         //se inicia y configura la pantalla
    lcd_cmd(0x0c);      //se apaga el cursor

    init_UART();        //se inicia y configura el UART

    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);    //se inicia y configura el SPI en Master
    PIE1bits.SSPIE = 1;
    PIR1bits.SSPIF = 0; //se habilitan las interrupciones para SPI
    return;
}

void __interrupt() isr(void) {

    if (PIR1bits.SSPIF == 1) {  //cuando se activa la bandera SSPIF pasara a recoger el valor obtenido del SPI y lo almacena
        Test1 = SSPBUF;
        SSPIF = 0;              //apaga manualmente la bandera
    }   
}

void main(void) {
    setup();                    // se llama la funcion para iniciar el pic

    while (1) {                 //loop del main
        lcd_clear();            //se limpia pantalla para hacer refresh de datos

        if (slave_select == 2) { // en los ifs se tiene para habilitar cada slave y de esta manera evitar que los datos se solapen o se reciban mal
            PORTAbits.RA0 = 0;
            PORTAbits.RA1 = 1;
            PORTAbits.RA2 = 1;   // se enciende el primer slave y se apagan los otros dos
            SSPBUF = 0xFF;      // enviamos datos al slave para iniciar el clock
            __delay_ms(10);
            SPI_slave_1 = SSPBUF; //se recibe dato del buffer a la variable
            PORTAbits.RA0 = 1;
            PORTAbits.RA1 = 1;
            PORTAbits.RA2 = 1;  //se apagan todos los slaves
            slave_select = 0;   //se reinicia el contador
        }

        if (slave_select == 1) { //if para el segundo slave
            PORTAbits.RA0 = 1;  
            PORTAbits.RA1 = 0;
            PORTAbits.RA2 = 1;  // se enciende el segundo slave y se apagan los otros dos
            SSPBUF = 0x0F;      // enviamos datos al slave para iniciar el clock
            __delay_ms(10);
            SPI_slave_2 = SSPBUF; //se recibe dato del buffer a la variable
            PORTAbits.RA0 = 1;
            PORTAbits.RA1 = 1;
            PORTAbits.RA2 = 1;  //se apagan todos los slaves
            slave_select++;     //se suma uno al contador 
        }

        if (slave_select == 0) { //if para tercer slave
            PORTAbits.RA0 = 1;
            PORTAbits.RA1 = 1;
            PORTAbits.RA2 = 0;  //se enciende tercer slave y se apagan los otros dos
            SSPBUF = 0xFC;      //enviamos datos al slave para iniciar el clock
            __delay_ms(10);
            SPI_slave_3 = SSPBUF; //se recibe dato del buffer a la variable
            PORTAbits.RA0 = 1;
            PORTAbits.RA1 = 1;
            PORTAbits.RA2 = 1;  //se apagan todos los slaves
            slave_select++;     //se suma uno al contador
        }

        lcd_write_string("Pot  Cont  Temp");    //se envia el texto a la lcd de los datos a mostrar
        lcd_move_cursor(1, 0);
        sprintf(slave_1, "%d", SPI_slave_1);    //se hace un sprintf de los datos del primer slave, potenciometro
        lcd_write_string(slave_1);              //se esciben los datos del primer slave en la pantalla
        lcd_move_cursor(1, 6);
        sprintf(slave_2, "%d", SPI_slave_2);    //se hace un sprintf de los datos del segundo slave, contador
        lcd_write_string(slave_2);              //se esciben los datos del segundo slave en la pantalla
        lcd_move_cursor(1, 11);
        sprintf(slave_3, "%d", SPI_slave_3);    //se hace un sprintf de los datos del tercer slave, temperatura
        lcd_write_string(slave_3);              //se esciben los datos del tercer slave en la pantalla
  
        Text_Uart("Pot  Cont  Temp");           //por medio de la funcion texto a uart escribimos la descripcion de las variables en UART
        TXREG = (0x0d);                         //hacemos un salto de linea

        TXREG = (slave_1[0]);                   //escribimos primer, segundo y tercer bit del dato del potenciometro en UART
        __delay_ms(5);
        TXREG = (slave_1[1]);
        __delay_ms(5);
        TXREG = (slave_1[2]);
        __delay_ms(5);  
        Text_Uart("   ");                       // se realiza un espacio para separar

        TXREG = (slave_2[0]);                  //escribimos primer, segundo y tercer bit del dato del contador en UART
        __delay_ms(5);
        TXREG = (slave_2[1]);
        __delay_ms(5);
        TXREG = (slave_2[2]);
        __delay_ms(5);
        Text_Uart("    ");                      // se realiza un espacio para separar

        TXREG = (slave_3[0]);                   //escribimos primer, segundo y tercer bit del dato de la temperatura en UART
        __delay_ms(5);
        TXREG = (slave_3[1]);
        __delay_ms(5);
        TXREG = (slave_3[2]);
        __delay_ms(5);
        TXREG = (' ');  
        TXREG = (0x0d);                         // se realiza un espacio para separar y un salto de linea
    }
}

void init_UART(void) {                          // Funcion para iniciar y configurar UART
    SPBRG = 25;
    BRGH = 1;
    SYNC = 0;
    SPEN = 1;
    TRISC7 = 1;
    TRISC6 = 0;
    TXSTAbits.TX9 = 0;
    CREN = 1;
    TXEN = 1;
}



