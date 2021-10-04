#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Text/MyStringFloat.h"
#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[64];
	UInt16 iVal;
	Double dVal;
	iVal = 0x0001;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:0.000000059604645\r\n");

	iVal = 0x03ff;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:0.000060975552\r\n");

	iVal = 0x0400;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:0.00006103515625\r\n");

	iVal = 0x7bff;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:65504\r\n");

	iVal = 0x3bff;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:0.99951172\r\n");

	iVal = 0x3c00;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:1\r\n");

	iVal = 0x3c01;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:1.00097656\r\n");

	iVal = 0x3555;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:0.33325195\r\n");

	iVal = 0xc000;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:-2\r\n");

	iVal = 0x0000;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:0\r\n");

	iVal = 0x8000;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:-0\r\n");

	iVal = 0x7c00;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:infinity\r\n");

	iVal = 0xfc00;
	dVal = ReadNFloat16((UInt8*)&iVal);
	Text::StrDouble(sbuff, dVal);
	printf("ReadVal: %s\r\n", sbuff);
	printf("Expected:-infinity\r\n");
	return 0;
}

