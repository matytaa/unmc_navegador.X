/*********************************************************************
 *
 *                Unimic 01 Version 1.1   10 September 2015
 *
 ***********************************************************************
 * FileName:        unmc_lcd_216.c
 * Dependencies:
 * Processor:       PIC18F26J50
 * Compiler:        XC8 1.34
 * Company:         Microchip Technology, Inc.
 *
 ***********************************************************************/
#include <unmc_lcd_216.h>


const int8 LCD_INIT_STRING[4] =
{
  0x20 | (lcd_type << 2), // Func set: 4-bit, 2 lines, 5x8 dots
  0x0C, // Display on
  1, // Clear display
  6 // Increment cursor
};
//-------------------------------------

void lcd_send_nibble(int8 nibble){

	DATA_PIN_4=!!(nibble & 0x01);
	DATA_PIN_5=!!(nibble & 0x02);
	DATA_PIN_6=!!(nibble & 0x04);
	DATA_PIN_7=!!(nibble & 0x08);

	Nop();
	EPIN=1;
	Nop();Nop();
	EPIN=0;
}
#ifdef USE_RW
int8 lcd_read_nibble(void){
	union{
		int8 retval_byte;
		struct{
			int8 retval_0:1;
			int8 retval_1:1;
			int8 retval_2:1;
			int8 retval_3:1;
		};
	}retval;

	retval.retval_byte = 0;

	EPIN=1;
	Nop();

	retval.retval_0 = READ_PIN_4;
	retval.retval_1 = READ_PIN_5;
	retval.retval_2 = READ_PIN_6;
	retval.retval_3 = READ_PIN_7;

	EPIN=0;

	return(retval.retval_byte);
}
#endif

#ifdef USE_RW
int8 lcd_read_byte(void){
	int8 low;
	int8 high;

	TRIS_DATA_PIN_4=1;
	TRIS_DATA_PIN_5=1;
	TRIS_DATA_PIN_6=1;
	TRIS_DATA_PIN_7=1;

	RWPIN=1;
	Nop();

	high = lcd_read_nibble();

	low = lcd_read_nibble();

	return( (high<<4) | low);
}
#endif

void lcd_send_byte(int8 address, int8 n){

	RSPIN=0;

#ifdef USE_RW
	while((lcd_read_byte()&0x80)==0x80);
#else
	_delay_100us();
#endif

	TRIS_DATA_PIN_4=0;
	TRIS_DATA_PIN_5=0;
	TRIS_DATA_PIN_6=0;
	TRIS_DATA_PIN_7=0;

	if(address)
		RSPIN=1;
	else
		RSPIN=0;

	Nop();

#ifdef USE_RW
	RWPIN=0;
	Nop();
#endif

	lcd_send_nibble(n >> 4);
	lcd_send_nibble(n & 0xf);
}

//----------------------------

void lcd_init(void){
	int8 i;

	TRISRS=0;
	TRISEN=0;
#ifdef USE_RW
	TRISRW=0;
#endif

	RSPIN=0;
	EPIN=0;
#ifdef USE_RW
	RWPIN=0;
#endif

	TRIS_DATA_PIN_4=0;
	TRIS_DATA_PIN_5=0;
	TRIS_DATA_PIN_6=0;
	TRIS_DATA_PIN_7=0;

	_delay_5ms();//15ms
	_delay_5ms();
	_delay_5ms();

	for(i=0 ;i < 3; i++){
		lcd_send_nibble(0x03);
		_delay_5ms();//5ms
  	}

	lcd_send_nibble(0x02);

	for(i=0; i < sizeof(LCD_INIT_STRING); i++){
		lcd_send_byte(0, LCD_INIT_STRING[i]);
	#ifndef USE_RW
		_delay_5ms();
	#endif
	}
}
//----------------------------
void lcd_gotoxy(int8 x, int8 y){
	int8 address;

	if(y != 1)
		address = lcd_line_two;
	else
		address=0;

	address += x-1;
	lcd_send_byte(0, 0x80 | address);
}

//-----------------------------

void lcd_putc(int8 c){
	switch(c){
		case '\f':
			lcd_send_byte(0,1);
		break;
		case '\n':
			lcd_gotoxy(1,2);
		break;
		case '\b':
      		lcd_send_byte(0,0x10);
		break;
		default:
			lcd_send_byte(1,c);
		break;
  }
}

void lcd_comand(int8 d){
	lcd_send_byte(0,d);
}

//------------------------------
#ifdef USE_RW
int8 lcd_getc(int8 x, int8 y){
	int8 value;

	lcd_gotoxy(x,y);

	while((lcd_read_byte()&0x80)==0x80);

	RSPIN=1;
	value = lcd_read_byte();
	RSPIN=0;

	return(value);
}
#endif

void lcd_setcursor_vb(int8 visible, int8 blink) {
	lcd_send_byte(0, 0xC|(visible<<1)|blink);
}

void lcd_puts(char *ptr){

	while(*ptr){
		lcd_putc(*ptr++);
	}
}

void lcd_putrs(const char *ptr){
	
	while(*ptr){
		lcd_putc(*ptr++);
	}
}
