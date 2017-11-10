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
#include <stdlib.h>
#include <unmc_lcd_216.h>
#include <unmc_rtcc_01.h>
#include <unmc_config_01.h>
#include <unmc_inout_03.h>

#define DERECHA         0
#define IZQUIERDA       1

int vMinutos[10] = NULL;
int vSegundos[10] = NULL;
int vMilesimas[10] = NULL;
int nroVuelta = 0;
int cursorVuelta = 0;

int ultimoRecorrido = -1;
int estadoAnteriorCentro = 0;
int cambioEstadoAbajo = 0;
int estadoCentro = 0;
int numeroVuelta = 0;
int fila = 2;
int columna = 9;
int milesima = 0;
int flajReloj = 0;
int unMinuto = 0;
int unSegundo = 0;
int unaMilesima = 0;

int unMinutoParcial = 0;
int unSegundoParcial = 0;
int unaMilesimaParcial = 0;

int segundosTotales = 0;
int segundosParciales = 0;
int segundosDisplay = 0;
int milesimasDisplay = 0;

int mejorVuelta = 0;
int primeraVuelta = 0;
int minutoVuelta = 0;
int segundoVuelta = 0;
int milesimaVuelta = 0;

int sumaMinutosVueltas = 0;
int sumaSegundosVueltas = 0;
int sumaMilesimasVueltas = 0;

void mostrarReloj();
void cambiarEstadoCentro();
void caratulaCronometro();
void mostrarCaratula();
void clickIniciar();
void inicializarCronometro();
void cronometrando();
void deteniendoCronometro();
void tiempoParcial();
void verificarNroVuelta();
void almacenarTiempoParcial();
void continuarCronometrando();
void almacenarTiempo();
void mostrarMejorVuelta();

void CargoVueltas(int p_nrovuelta, int p_minuto, int p_segundo, int p_milesima);

void vueltasAlmacenadasAdelante();
void vueltasAlmacenadasAtras();
void mostrarVuelta();
void debug();

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/// Funcion Caratula
/// Display presentation day hour
/// variable lecture diasem, anio, dia, hora, etc
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[

void mostrarReloj(){
    lcd_comand(0b00001100);             //Enciende display sin cursor y sin blink  
    sprintf(buffer2,"%01u:%02u.%03u",minuto,segundo,milesima);
    lcd_gotoxy(1,2);
    lcd_putrs(buffer2);
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
        /* idem a otras 3 anteriores
        TRISAbits.TRISA0=1;
        TRISBbits.TRISB0=0;
        TRISCbits.TRISC0=0;
        */
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
    
    void cambiarEstadoCentro(){
        if (estadoCentro==0){
            estadoCentro = 1;
        }else{
            estadoCentro = 0;
        }
    }
    
    void caratulaCronometro()
    {
        lcd_comand(0b00001100);             //Enciende display sin cursor y sin blink  
        lcd_gotoxy(1,1);        
        lcd_putrs("CRONOMETRAR MI");
        
        lcd_gotoxy(1,2);        
        lcd_putrs("TIEMPO DE VUELTA");
    }
    
    void mostrarCaratula(){
        if(estadoCentro == 0){
            caratulaCronometro();
            estadoCentro++;
        }
    }
    
    void clickIniciar(){
        if (switch_Center == 0 && estadoCentro == 1){
            estadoCentro++;
            __delay_ms(40);
            lcd_comand(0b00000001);
            __delay_ms(90);
            lcd_gotoxy(1,1);      
            lcd_putrs("CLICK INICIAR");
            __delay_ms(90);
        }
    }
    
    void inicializarCronometro(){
        if (switch_Center == 0 && estadoCentro == 2)
        {
            estadoCentro++;
            __delay_ms(40);
            lcd_comand(0b00000001);
            __delay_ms(90);
            lcd_gotoxy(1,1);      
            lcd_putrs("CRONOMETRANDO");
            hora = 0;
            minuto = 0;
            segundo = 0;
            milesima = 0;
            Write_RTC();
            __delay_ms(90);//add
            flajReloj = 1;
        }
    }
    
    void cronometrando(){
        if (flajReloj == 1 && estadoCentro == 3){
            milesima = milesima + 9;
            Read_RTC();
            mostrarReloj(); 
            
            if (milesima == 999){
                milesima = 0;
            }
        }
    }
    
    void deteniendoCronometro(){
        if (estadoCentro == 3 && switch_Center == 0){
            __delay_ms(98);
            estadoCentro = 4;
            __delay_ms(98);
            flajReloj = 0;
            unMinuto = minuto;
            unSegundo = segundo;
            unaMilesima = milesima;
            //almacenarTiempoParcial();
        }
    }
    
    void tiempoParcial(){
        if(estadoCentro == 4){
            mostrarReloj();
        }
    }

    void almacenarTiempoParcial(){
        verificarNroVuelta();
        if(nroVuelta==0){
            unMinutoParcial = unMinuto;
            unSegundoParcial = unSegundo;
            unaMilesimaParcial = unaMilesima;
        }else{
            for (int i = 0; i < nroVuelta; i++){
                sumaMinutosVueltas = sumaMinutosVueltas + vMinutos[i];
                sumaSegundosVueltas = sumaSegundosVueltas + vSegundos[i];
                sumaMilesimasVueltas = sumaMilesimasVueltas + vMilesimas[i];
            }            
            
            unMinutoParcial = unMinuto - sumaMinutosVueltas;
            unSegundoParcial = unSegundo - sumaSegundosVueltas;
            unaMilesimaParcial = unaMilesima - sumaMilesimasVueltas;
           
        }
        CargoVueltas(nroVuelta, unMinutoParcial, unSegundoParcial, unaMilesimaParcial);
        nroVuelta++;
    }
    
    void verificarNroVuelta(){
        if (nroVuelta > 9){
                nroVuelta = 0;
        }
        sumaMinutosVueltas = 0;
        sumaSegundosVueltas = 0;
        sumaMilesimasVueltas = 0;
    }
        
    void continuarCronometrando(){
        if (estadoCentro == 4 && switch_Center == 0){
            __delay_ms(98);
            estadoCentro = 3;
            __delay_ms(98);
            flajReloj = 1;
            almacenarTiempoParcial();
            lcd_gotoxy(1,1);      
            lcd_putrs("CRONOMETRANDO");
            minuto = unMinuto;
            segundo = unSegundo;
            milesima = unaMilesima;
            Write_RTC();
        }
    }
    
    void almacenarTiempo(){
        if (switch_Down == 0 && estadoCentro == 4){
            verificarNroVuelta();
            CargoVueltas(nroVuelta, unMinuto, unSegundo, unaMilesima);
            mejorVuelta = 0;
            if (primeraVuelta == 0){
                minutoVuelta = unMinuto;
                segundoVuelta = unSegundo;
                milesimaVuelta = unaMilesima;
                primeraVuelta = 1;
                segundosTotales = 60*minutoVuelta + segundoVuelta;
                
            }
            
            segundosParciales = unMinuto*60 + unSegundo;
            if ((segundosParciales < segundosTotales)||((segundosParciales == segundosTotales)&&(unaMilesima < milesimaVuelta))){
                mostrarMejorVuelta();
                segundosTotales = segundosParciales;

                minutoVuelta = unMinuto;
                segundoVuelta = unSegundo;
                milesimaVuelta = unaMilesima;
            }

            
            if (mejorVuelta == 0){
                lcd_comand(0b00000001);
                __delay_ms(90);
                lcd_gotoxy(1,1);      
                lcd_putrs("TU TIEMPO");
            }
            
            mostrarReloj();
            estadoCentro = 2;
            nroVuelta++;
            //misVueltas = CargoNodoVueltas(misVueltas, 1, minutoVuelta, segundoVuelta, milesimaVuelta);
            //aca llamada a struct y retorna struct
        }
    }
    
    void mostrarMejorVuelta(){
        lcd_comand(0b00000001);
        __delay_ms(45);
        lcd_gotoxy(1,1);      
        lcd_putrs("MEJOR VUELTA");
        mejorVuelta = 1;
        segundosDisplay = segundosTotales - segundosParciales;
        milesimasDisplay =  milesimaVuelta - unaMilesima;
        if (milesimasDisplay < 0) {
            milesimasDisplay = 1000 + milesimasDisplay;
            segundosDisplay--;
        }

        sprintf(buffer2,"-%02u.%03u",segundosDisplay,milesimasDisplay);
        lcd_gotoxy(10,2);
        lcd_putrs(buffer2); 
    }
    
    void CargoVueltas(int p_nrovuelta, int p_minuto, int p_segundo, int p_milesima){
        vMinutos[p_nrovuelta] = p_minuto;
        vSegundos[p_nrovuelta] = p_segundo;
        vMilesimas[p_nrovuelta] = p_milesima;
    }    

    void vueltasAlmacenadasAdelante(){
        if (switch_Right == 0){
            __delay_ms(98);
            if (ultimoRecorrido == IZQUIERDA){
                cursorVuelta= cursorVuelta + 2;
            }
            
            if(cursorVuelta > 9){
                cursorVuelta = 0;
            }
            mostrarVuelta();
            cursorVuelta++;
            ultimoRecorrido = DERECHA;
        }
    }
    
    void vueltasAlmacenadasAtras(){
        if (switch_Left == 0){
            __delay_ms(98);
            if (ultimoRecorrido == DERECHA){
                cursorVuelta= cursorVuelta - 2;
            }
            if(cursorVuelta < 0){
                cursorVuelta = 9;
            }
            mostrarVuelta();
            cursorVuelta--;
            ultimoRecorrido = IZQUIERDA;
        }
    }
    
    void mostrarVuelta(){
        minuto = vMinutos[cursorVuelta];
        segundo = vSegundos[cursorVuelta];
        milesima = vMilesimas[cursorVuelta];

        lcd_comand(0b00000001);
        __delay_ms(90);
        sprintf(buffer2,"VUELTA NRO %01u", cursorVuelta+1);
        lcd_gotoxy(1,1);      
        lcd_putrs(buffer2);
        mostrarReloj();
    }
    
    void debug(){
        while(1){
            lcd_gotoxy(1,1);
            sprintf(buffer2, "segundos %03u", sumaMinutosVueltas);
            lcd_putrs(buffer2);
        }
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
    
    while(1){
        
        mostrarCaratula();
        clickIniciar();
        inicializarCronometro();
        cronometrando();
        deteniendoCronometro();
        tiempoParcial();
        continuarCronometrando();
        almacenarTiempo();        
        vueltasAlmacenadasAdelante();
        vueltasAlmacenadasAtras();
    }
    return 0;
}
    