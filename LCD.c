#include "LCD.h"

#define _Byte2Ascii(Value) (Value = Value + '0')

#if (LCD_Size == 801)
#define __LCD_Rows 1
#define __LCD_Columns 8
#define __LCD_LineStart_1 0x00
#elif (LCD_Size == 802)
#define __LCD_Rows 2
#define __LCD_Columns 8
#define __LCD_LineStart_1 0x00
#define __LCD_LineStart_2 0x40
#elif (LCD_Size == 1601)
#define __LCD_Rows 1
#define __LCD_Columns 16
#define __LCD_LineStart_1 0x00
#if (LCD_Type == B)
#define __LCD_LINESTART_1B 0x40
#endif
#elif (LCD_Size == 1602)
#define __LCD_Rows 2
#define __LCD_Columns 16
#define __LCD_LineStart_1 0x00
#define __LCD_LineStart_2 0x40
#elif (LCD_Size == 1604)
#define __LCD_Rows 4
#define __LCD_Columns 16
#define __LCD_LineStart_1 0x00
#define __LCD_LineStart_2 0x40
#define __LCD_LineStart_3 0x10
#define __LCD_LineStart_4 0x50
#elif (LCD_Size == 2001)
#define __LCD_Rows 1
#define __LCD_Columns 20
#define __LCD_LineStart_1 0x00
#elif (LCD_Size == 2002)
#define __LCD_Rows 2
#define __LCD_Columns 20
#define __LCD_LineStart_1 0x00
#define __LCD_LineStart_2 0x40
#elif (LCD_Size == 2004)
#define __LCD_Rows 4
#define __LCD_Columns 20
#define __LCD_LineStart_1 0x00
#define __LCD_LineStart_2 0x40
#define __LCD_LineStart_3 0x14
#define __LCD_LineStart_4 0x54
#elif (LCD_Size == 4001)
#define __LCD_Rows 1
#define __LCD_Columns 40
#define __LCD_LineStart_1 0x00
#elif (LCD_Size == 4002)
#define __LCD_Rows 2
#define __LCD_Columns 40
#define __LCD_LineStart_1 0x00
#define __LCD_LineStart_2 0x40
#endif

static void LCD_SendCommandHigh(uint8_t Command);
static void LCD_Send(uint8_t Data);
static uint8_t LCD_Read();
static inline void Pulse_En();
static void Int2bcd(int32_t Value, char *BCD);

void LCD_Setup()
{
	PinMode(LCD_D4, Output);
	PinMode(LCD_D5, Output);
	PinMode(LCD_D6, Output);
	PinMode(LCD_D7, Output);
	PinMode(LCD_RS, Output);
	PinMode(LCD_RW, Output);
	PinMode(LCD_EN, Output);

	DigitalWrite(LCD_D4, Low);
	DigitalWrite(LCD_D5, Low);
	DigitalWrite(LCD_D6, Low);
	DigitalWrite(LCD_D7, Low);
	DigitalWrite(LCD_RS, Low);
	DigitalWrite(LCD_RW, Low);
	DigitalWrite(LCD_EN, Low);

	_delay_ms(__LCD_Delay_1);
	LCD_SendCommandHigh(__LCD_CMD_FunctionSet | __LCD_CMD_8BitMode);
	_delay_ms(__LCD_Delay_2);
	LCD_SendCommandHigh(__LCD_CMD_FunctionSet | __LCD_CMD_8BitMode);
	_delay_ms(__LCD_Delay_3);
	LCD_SendCommandHigh(__LCD_CMD_FunctionSet | __LCD_CMD_8BitMode);
	_delay_ms(__LCD_Delay_4);
	LCD_SendCommandHigh(__LCD_CMD_FunctionSet | __LCD_CMD_4BitMode);
	LCD_SendCommand(__LCD_CMD_FunctionSet | __LCD_CMD_4BitMode | __LCD_CMD_2Line | __LCD_CMD_5x8Dots);
	LCD_SendCommand(__LCD_CMD_DisplayControl | __LCD_CMD_DisplayOn | __LCD_CMD_CursorOff | __LCD_CMD_BlinkOff);
	LCD_SendCommand(__LCD_CMD_EntryModeSet | __LCD_CMD_EntryIncrement | __LCD_CMD_EntryNoShift);
	LCD_SendCommand(__LCD_CMD_ClearDisplay);
}

void LCD_SendCommand(uint8_t Command)
{
	LCD_WaitBusy();

	DigitalWrite(LCD_RS, Low);
	LCD_Send(Command);
}

void LCD_SendData(char c)
{
	LCD_WaitBusy();

	DigitalWrite(LCD_RS, High);
	LCD_Send((uint8_t)(c));
}

void LCD_WaitBusy()
{
	uint8_t busy = 0;
	PinMode(LCD_D4, Input);
	PinMode(LCD_D5, Input);
	PinMode(LCD_D6, Input);
	PinMode(LCD_D7, Input);
	DigitalWrite(LCD_RS, Low);
	DigitalWrite(LCD_RW, High);

	do
	{
		DigitalWrite(LCD_EN, High);
		_delay_us(__LCD_Pulse_us);
		busy &= ~(1 << __LCD_BusyFlag);
		busy |= (DigitalRead(LCD_D7) << __LCD_BusyFlag);
		DigitalWrite(LCD_EN, Low);
		Pulse_En();
	} while (BitCheck(busy, __LCD_BusyFlag));

	PinMode(LCD_D4, Output);
	PinMode(LCD_D5, Output);
	PinMode(LCD_D6, Output);
	PinMode(LCD_D7, Output);
	DigitalWrite(LCD_RW, Low);
}

void LCD_BuildChar(char *Data, uint8_t Position)
{
	if (Position < 0)
	return;
	if (Position >= 8)
	return;

	Point_t p = LCD_GetP();
	uint8_t i;
	LCD_SendCommand(__LCD_CMD_SetCGRAMAddress | (Position << 3));

	for (i = 0; i < 8; i++)
	LCD_SendData(Data[i]);

	LCD_GotoXY(p.X, p.Y);
}

void LCD_BuildChar_P(const char *Data, uint8_t Position)
{
	if (Position < 0)
	return;
	if (Position >= 8)
	return;

	Point_t p = LCD_GetP();
	uint8_t i;

	LCD_SendCommand(__LCD_CMD_SetCGRAMAddress | (Position << 3));

	for (i = 0; i < 8; i++)
	LCD_SendData(pgm_read_byte(Data[i]));

	LCD_GotoXY(p.X, p.Y);
}

void LCD_Clear()
{
	LCD_SendCommand(__LCD_CMD_ClearDisplay);
}

void LCD_ClearLine(uint8_t Line)
{
	uint8_t i = 0;

	LCD_GotoXY(0, Line);
	while (i <= __LCD_Columns)
	{
		LCD_SendData(' ');
		i++;
	}
}

void LCD_GotoXY(uint8_t X, uint8_t Y)

{
	if ((X < __LCD_Columns) && (Y < __LCD_Rows))
	{
		uint8_t addr = 0;
		switch (Y)
		{
			#if ((defined(__LCD_LineStart_4)) || (defined(__LCD_LineStart_3)) || (defined(__LCD_LineStart_2)) || (defined(__LCD_LineStart_1)))
			case (0):
			addr = __LCD_LineStart_1;
			#if ((LCD_Size == 1601) && (LCD_Type == B))
			if (X >= (__LCD_Columns >> 1))
			{
				X -= __LCD_Columns >> 1;
				addr = __LCD_LINESTART_1B;
			}
			#endif
			break;
			#endif
			#if ((defined(__LCD_LineStart_4)) || (defined(__LCD_LineStart_3)) || (defined(__LCD_LineStart_2)))
			case (1):
			addr = __LCD_LineStart_2;
			break;
			#endif
			#if ((defined(__LCD_LineStart_4)) || (defined(__LCD_LineStart_3)))
			case (2):
			addr = __LCD_LineStart_3;
			break;
			#endif
			#if (defined(__LCD_LineStart_4))
			case (3):
			addr = __LCD_LineStart_4;
			break;
			#endif
		}
		addr = __LCD_CMD_SetDDRAMAddress | (addr | X);
		LCD_SendCommand(addr);
	}
}

Point_t LCD_GetP()
{
	Point_t p;

	p.X = LCD_Read();
	p.Y = 0;
	#if (__LCD_Rows == 1)
	#if ((LCD_Size == 1601) && (LCD_Type == B))
	if (p.X >= __LCD_LINESTART_1B)
	p.X = p.X - __LCD_LINESTART_1B + (__LCD_Columns >> 1);
	#endif
	#elif (__LCD_Rows == 2)
	if (p.X >= __LCD_LineStart_2)
	{
		p.X -= __LCD_LineStart_2;
		p.Y = 1;
	}
	#elif (__LCD_Rows == 3)
	if (p.X >= __LCD_LineStart_2)
	{
		p.X -= __LCD_LineStart_2;
		p.Y = 1;
	}
	else if (p.X >= __LCD_LineStart_3)
	{
		p.X -= __LCD_LineStart_3;
		p.Y = 2;
	}
	#elif (__LCD_Rows == 4)
	if (p.X >= __LCD_LineStart_4)
	{
		p.X -= __LCD_LineStart_4;
		p.Y = 3;
	}
	else if (p.X >= __LCD_LineStart_2)
	{
		p.X -= __LCD_LineStart_2;
		p.Y = 1;
	}
	else if (p.X >= __LCD_LineStart_3)
	{
		p.X -= __LCD_LineStart_3;
		p.Y = 2;
	}
	#endif

	return p;
}

uint8_t LCD_GetX()
{
	return LCD_GetP().X;
}

uint8_t LCD_GetY()
{
	return LCD_GetP().Y;
}

void LCD_PrintChar(char Character)
{
	LCD_SendData(Character);
}

void LCD_PrintString(char *Text)
{
	while (*Text)
	LCD_SendData(*Text++);
}

void LCD_PrintString_P(const char *Text)
{
	char r = pgm_read_byte(Text++);
	while (r)
	{
		LCD_SendData(r);
		r = pgm_read_byte(Text++);
	}
}

void LCD_PrintInteger(int32_t Value)
{
	if (Value == 0)
	{
		LCD_PrintChar('0');
	}
	else if ((Value > INT32_MIN) && (Value <= INT32_MAX))
	{
		char arr[12] = {'\0'};
		Int2bcd(Value, arr);

		LCD_PrintString(arr);
	}
}

void LCD_PrintDouble(double Value, uint32_t Tens)
{
	if (Value == 0)
	{
		LCD_PrintChar('0');
		LCD_PrintChar('.');
		LCD_PrintChar('0');
	}
	else if ((Value >= (-2147483647)) && (Value < 2147483648))
	{
		if (Value < 0)
		{
			Value = -Value;
			LCD_PrintChar('-');
		}

		LCD_PrintInteger(Value);
		LCD_PrintChar('.');
		LCD_PrintInteger((Value - (uint32_t)(Value)) * Tens);
	}
}

static void LCD_SendCommandHigh(uint8_t Data)
{
	DigitalWrite(LCD_RS, Low);

	DigitalWrite(LCD_D4, BitCheck(Data, 4));
	DigitalWrite(LCD_D5, BitCheck(Data, 5));
	DigitalWrite(LCD_D6, BitCheck(Data, 6));
	DigitalWrite(LCD_D7, BitCheck(Data, 7));
	Pulse_En();
}

static void LCD_Send(uint8_t Data)
{
	DigitalWrite(LCD_D4, BitCheck(Data, 4));
	DigitalWrite(LCD_D5, BitCheck(Data, 5));
	DigitalWrite(LCD_D6, BitCheck(Data, 6));
	DigitalWrite(LCD_D7, BitCheck(Data, 7));
	Pulse_En();
	DigitalWrite(LCD_D4, BitCheck(Data, 0));
	DigitalWrite(LCD_D5, BitCheck(Data, 1));
	DigitalWrite(LCD_D6, BitCheck(Data, 2));
	DigitalWrite(LCD_D7, BitCheck(Data, 3));
	Pulse_En();
}

static uint8_t LCD_Read()
{
	uint8_t status = 0;

	LCD_WaitBusy();

	PinMode(LCD_D4, Input);
	PinMode(LCD_D5, Input);
	PinMode(LCD_D6, Input);
	PinMode(LCD_D7, Input);
	DigitalWrite(LCD_RS, Low);
	DigitalWrite(LCD_RW, High);
	DigitalWrite(LCD_EN, High);
	_delay_us(__LCD_Pulse_us);
	status |= DigitalRead(LCD_D4) << 4;
	status |= DigitalRead(LCD_D5) << 5;
	status |= DigitalRead(LCD_D6) << 6;
	DigitalWrite(LCD_EN, Low);
	DigitalWrite(LCD_EN, High);
	_delay_us(__LCD_Pulse_us);
	status |= DigitalRead(LCD_D4);
	status |= DigitalRead(LCD_D5) << 1;
	status |= DigitalRead(LCD_D6) << 2;
	status |= DigitalRead(LCD_D7) << 3;
	DigitalWrite(LCD_EN, Low);

	PinMode(LCD_D4, Output);
	PinMode(LCD_D5, Output);
	PinMode(LCD_D6, Output);
	PinMode(LCD_D7, Output);
	DigitalWrite(LCD_RW, Low);
	return status;
}

static inline void Pulse_En()
{
	DigitalWrite(LCD_EN, High);
	_delay_us(__LCD_Pulse_us);
	DigitalWrite(LCD_EN, Low);
}

static void Int2bcd(int32_t Value, char BCD[])
{
	uint8_t isNegative = 0;

	BCD[0] = BCD[1] = BCD[2] =
	BCD[3] = BCD[4] = BCD[5] =
	BCD[6] = BCD[7] = BCD[8] =
	BCD[9] = BCD[10] = '0';

	if (Value < 0)
	{
		isNegative = 1;
		Value = -Value;
	}

	while (Value > 1000000000)
	{
		Value -= 1000000000;
		BCD[1]++;
	}

	while (Value >= 100000000)
	{
		Value -= 100000000;
		BCD[2]++;
	}

	while (Value >= 10000000)
	{
		Value -= 10000000;
		BCD[3]++;
	}

	while (Value >= 1000000)
	{
		Value -= 1000000;
		BCD[4]++;
	}

	while (Value >= 100000)
	{
		Value -= 100000;
		BCD[5]++;
	}

	while (Value >= 10000)
	{
		Value -= 10000;
		BCD[6]++;
	}

	while (Value >= 1000)
	{
		Value -= 1000;
		BCD[7]++;
	}

	while (Value >= 100)
	{
		Value -= 100;
		BCD[8]++;
	}

	while (Value >= 10)
	{
		Value -= 10;
		BCD[9]++;
	}

	while (Value >= 1)
	{
		Value -= 1;
		BCD[10]++;
	}
	uint8_t i = 0;
	while (BCD[i] == '0')
	i++;
	if (isNegative)
	{
		i--;
		BCD[i] = '-';
	}
	uint8_t end = 10 - i;
	uint8_t offset = i;
	i = 0;
	while (i <= end)
	{
		BCD[i] = BCD[i + offset];
		i++;
	}
	BCD[i] = '\0';
}