/*********************************************************************
 *
 *                Unimic 01 Version 1.1   10 September 2015
 *
 ***********************************************************************
 * FileName:        main.c
 * Dependencies:
 * Processor:       PIC18F26J50
 * Compiler:        XC8 1.34
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ***********************************************************************/
#include <xc.h>
#include <delays.h>
#include <stdio.h>
#include <unmc_lcd_216.h>
#include <unmc_rtcc_01.h>
#include <unmc_config_01.h>
#include <unmc_inout_03.h>

void dia_sem(int);
void show_menu();
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/// Funcion Caratula
/// Display presentation day hour
/// variable lecture diasem, anio, dia, hora, etc
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[

    void caratula(void)
    {
        lcd_comand(0b00001100);             //Enciende display sin cursor y sin blink  
        lcd_gotoxy(1,1);        
        //lcd_putrs("UNIMIC");
        dia_sem(diasem);
        sprintf(buffer2,"%02u/%02u/%02u",dia,mes,anio);
        lcd_gotoxy(9,1);
        lcd_putrs(buffer2);
        sprintf(buffer2,"%02u:%02u:%02u",hora,minuto,segundo);
        lcd_gotoxy(1,2);
        lcd_putrs(buffer2);
     b=b++;
        /**
        if(b>6)
           {
            if(b==11){b=0;}
            lcd_gotoxy(1,1);
            lcd_putrs("      ");
           }*/
    }

    void dia_sem(int e){
        if(e==0){
            lcd_putrs("DOMINGO");
        }if(e==1){
            lcd_putrs("LUNES");
        }if(e==2){
            lcd_putrs("MARTES");
        }if(e==3){
            lcd_putrs("MIERCOLES");
        }if(e==4){
            lcd_putrs("JUEVES");
        }if(e==5){
            lcd_putrs("VIERNES");
        }if(e==6){
            lcd_putrs("SABADO");
        }
    }
    
    void show_menu(){
        lcd_comand(0b00000001);
        __delay_ms(9);
        lcd_gotoxy(1,1);
        lcd_putrs("Settear hora");
        lcd_gotoxy(1,2);
        lcd_putrs("Pedir cafe");
    }
    
    void settear_hora(){
        lcd_comand(0b00000001);
        __delay_ms(9);
        lcd_gotoxy(1,1);
        lcd_putrs("Ing dd/mm/aaaa");
        lcd_gotoxy(1,2);
        sprintf(buffer2,"%02u/%02u/%02u",dia,mes,anio);
        lcd_putrs(buffer2);
    }
    
    void print_clk(int col, int fila){
        //dia_sem(diasem);
        sprintf(buffer2,"%02u/%02u/%02u",dia,mes,anio);
        lcd_gotoxy(col,fila);
        lcd_putrs(buffer2);
        sprintf(buffer2,"%02u:%02u:%02u",hora,minuto,segundo);
    }
    
    void mover_puntero(){
        if(switch_Right == 0){
            while(switch_Right == 1){__delay_ms(10);};
            lcd_comand(0b00000100);
            __delay_ms(40);
        }
    }
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/// Funcion Setup
///
///
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[

    void Setup(void)
    {
    OSCTUNEbits.INTSRC=1;       //setea el oscilador de 32768 para el RTC
    OSCTUNEbits.PLLEN=0;        //desactiva PLL
    OSCCONbits.IRCF0=1;         //selecciona el clock en 8MHz
    OSCCONbits.IRCF1=1;
    OSCCONbits.IRCF2=1;
    OSCCONbits.SCS0=0;          //oscilator INTRC
    OSCCONbits.SCS1=0;
    TRISA = 0b11111111;
    TRISB = 0;
    TRISC = 0b00000111;
    TRISAbits.TRISA0=1;
    TRISBbits.TRISB0=0;
    TRISCbits.TRISC0=0;
    ANCON0=0b11111111;          // Config AN7 to AN0 Digital Ports
    //ANCON0=0b11111111;          // Config AN11 Analog Port
    ADCON0=0b00111111;          // Control AN11 Analog Port
    ADCON1=0b11111111;          // Config Analog Port
    RTCCFGbits.RTCEN=1;
    RTCCFGbits.RTCWREN=1;
    T1CONbits.T1OSCEN=1;
    Write_RTC();
    lcd_init();
    lcd_comand(0b00001100);     //Display=on / Cursor=off / Blink=off
    LED_2_Off;
    LED_3_Off;
    }
/*------------------------------------------------------------------------------
********************************************************************************
Funcion main
Funcion principal del programa
********************************************************************************
--------------------------------------------------------------------------------*/
int main(void)
{
Setup();
while(1)
    {
    caratula();
    Read_RTC();

    if(switch_Center == 0){
        lcd_comand(0b00000001);
        __delay_ms(90);
        lcd_gotoxy(1,1);
        lcd_putrs("Seleccion");
        
        __delay_ms(90);
        while(switch_Center == 1){__delay_ms(10);};
        
        if(switch_Center == 0){
            show_menu();         
            __delay_ms(90);
            while(switch_Center == 1){__delay_ms(10);};

            if(switch_Center == 0){
                settear_hora();
                __delay_ms(90);
                while(switch_Center == 1){__delay_ms(10);
                
                    if(switch_Up == 0){
                        while(switch_Up == 0){__delay_ms(10);};
                        dia++;
                        if(dia==32){
                            dia=0;
                        }
                        Write_RTC();
                        print_clk(1,2);
                    }else if(switch_Down == 0){
                        while(switch_Down == 0){__delay_ms(10);};
                        dia--;
                        if(dia<0){
                            dia = 31;
                        }
                        Write_RTC();
                        print_clk(1,2);
                    }else if(switch_Right == 0){
                        lcd_gotoxy(3,1);
                        __delay_ms(90);
                        if(switch_Up == 0){
                            while(switch_Up == 0){__delay_ms(10);};
                            mes++;
                            Write_RTC();
                            print_clk(3,2);
                        }else if(switch_Down == 0){
                            while(switch_Down == 0){__delay_ms(10);};
                            mes--;
                            Write_RTC();
                            print_clk(3,2);
                        }
                    }
                };
                
            }
        }
    }
    
    if(switch1 == 0)
    {
    LED_2_Toggle;
    }
    else LED_2_Off;

    if(switch2 == 0)
    {
    LED_3_On;
    }
    else LED_3_Off;
   
    __delay_ms(98);             // 98ms retardo maximo para esta funcion

    }
return 0;
}
