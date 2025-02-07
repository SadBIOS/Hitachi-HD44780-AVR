#ifndef LCD_H
#define LCD_H
#include <inttypes.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define LCD_Size 2004
#define LCD_Type A
#define LCD_D4 A, 0
#define LCD_D5 A, 1
#define LCD_D6 A, 2
#define LCD_D7 A, 3
#define LCD_RS A, 4
#define LCD_RW A, 5
#define LCD_EN A, 6

#define __LCD_Pulse_us 1
#define __LCD_Delay_1 20
#define __LCD_Delay_2 10
#define __LCD_Delay_3 1
#define __LCD_Delay_4 1

#define __LCD_CMD_ClearDisplay 0x01
#define __LCD_CMD_ReturnHome 0x02
#define __LCD_CMD_EntryModeSet 0x04
#define __LCD_CMD_DisplayControl 0x08
#define __LCD_CMD_CursorShift 0x10
#define __LCD_CMD_FunctionSet 0x20
#define __LCD_CMD_SetCGRAMAddress 0x40
#define __LCD_CMD_SetDDRAMAddress 0x80

#define __LCD_CMD_EntryIncrement 0x02
#define __LCD_CMD_EntryDecrement 0x00
#define __LCD_CMD_EntryShift 0x01
#define __LCD_CMD_EntryNoShift 0x00

#define __LCD_CMD_DisplayOn 0x04
#define __LCD_CMD_DisplayOff 0x00
#define __LCD_CMD_CursonOn 0x02
#define __LCD_CMD_CursorOff 0x00
#define __LCD_CMD_BlinkOn 0x01
#define __LCD_CMD_BlinkOff 0x00

#define __LCD_CMD_DisplayMove 0x08
#define __LCD_CMD_CursorMove 0x00
#define __LCD_CMD_MoveRight 0x04
#define __LCD_CMD_MoveLeft 0x00

#define __LCD_CMD_8BitMode 0x10
#define __LCD_CMD_4BitMode 0x00
#define __LCD_CMD_2Line 0x08
#define __LCD_CMD_1Line 0x00
#define __LCD_CMD_5x10Dots 0x04
#define __LCD_CMD_5x8Dots 0x00

#define __LCD_BusyFlag 7

#define PinMode(x, y) (y ? _SET(DDR, x) : _CLEAR(DDR, x))
#define DigitalWrite(x, y) (y ? _SET(PORT, x) : _CLEAR(PORT, x))
#define DigitalRead(x) (_GET(PIN, x))
#define PinModeToggle(x) (_TOGGLE(DDR, x))
#define DigitalLevelToggle(x) (_TOGGLE(PORT, x))

#define BitSet(x, y) (x |= (1UL << y))
#define BitClear(x, y) (x &= (~(1UL << y)))
#define BitToggle(x, y) (x ^= (1UL << y))
#define BitCheck(x, y) (x & (1UL << y) ? 1 : 0)

#define PORT(port) (_PORT(port))
#define DDR(port) (_DDR(port))
#define PIN(port) (_PIN(port))

#define _PORT(port) (PORT##port)
#define _DDR(port) (DDR##port)
#define _PIN(port) (PIN##port)

#define _SET(type, port, bit) (BitSet((type##port), bit))
#define _CLEAR(type, port, bit) (BitClear((type##port), bit))
#define _TOGGLE(type, port, bit) (BitToggle((type##port), bit))
#define _GET(type, port, bit) (BitCheck((type##port), bit))

#define Input 0
#define Output !Input
#define Low 0
#define High !Low
#define False 0
#define True !False

typedef struct
{
	uint8_t X, Y;
} Point_t;

void LCD_Setup();
void LCD_SendCommand(uint8_t Command);
void LCD_SendData(char Character);
void LCD_WaitBusy();
void LCD_BuildChar(char *Data, uint8_t Position);
void LCD_BuildChar_P(const char *Data, uint8_t Position);

void LCD_Clear();
void LCD_ClearLine(uint8_t Line);
void LCD_GotoXY(uint8_t X, uint8_t Y);
Point_t LCD_GetP();
uint8_t LCD_GetX();
uint8_t LCD_GetY();

void LCD_PrintChar(char Character);
void LCD_PrintString(char *Text);
void LCD_PrintString_P(const char *Text);
void LCD_PrintInteger(int32_t Value);
void LCD_PrintDouble(double Value, uint32_t Tens);

#endif
