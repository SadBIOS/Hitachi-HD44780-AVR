#include <LCD.h>
#include <util/delay.h>

int main(void) {
	_delay_ms(100);
	LCD_Setup();
	_delay_ms(200);
	LCD_GotoXY(0,0);
	LCD_PrintString("Are Mama?");
	LCD_GotoXY(0,1);
	LCD_PrintString("Tumi Dekhi");
	LCD_GotoXY(0,2);
	LCD_PrintString("Display");
	LCD_GotoXY(0,3);
	LCD_PrintString("Chalaiteso :)");
	while (1) {}
	return 0;
}