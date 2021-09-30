#include "Stdafx.h"
#include "SIMD.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Bool failOnly = false;
IO::ConsoleWriter *console;

void UInt8x4Cmp(UInt8x4 u8x4, const Char *funcName, UInt8 val1, UInt8 val2, UInt8 val3, UInt8 val4)
{
	UInt8 buff[4];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreUInt8x4(buff, u8x4);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void UInt8x8Cmp(UInt8x8 u8x8, const Char *funcName, UInt8 val1, UInt8 val2, UInt8 val3, UInt8 val4, UInt8 val5, UInt8 val6, UInt8 val7, UInt8 val8)
{
	UInt8 buff[8];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreUInt8x8(buff, u8x8);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4 && buff[4] == val5 && buff[5] == val6 && buff[6] == val7 && buff[7] == val8)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void UInt8x16Cmp(UInt8x16 u8x16, const Char *funcName, UInt8 val1, UInt8 val2, UInt8 val3, UInt8 val4, UInt8 val5, UInt8 val6, UInt8 val7, UInt8 val8,
		UInt8 val9, UInt8 val10, UInt8 val11, UInt8 val12, UInt8 val13, UInt8 val14, UInt8 val15, UInt8 val16)
{
	UInt8 buff[16];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreUInt8x16(buff, u8x16);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4 && buff[4] == val5 && buff[5] == val6 && buff[6] == val7 && buff[7] == val8 &&
		buff[8] == val9 && buff[9] == val10 && buff[10] == val11 && buff[11] == val12 && buff[12] == val13 && buff[13] == val14 && buff[14] == val15 && buff[15] == val16)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void Int16x4Cmp(Int16x4 i16x4, const Char *funcName, Int16 val1, Int16 val2, Int16 val3, Int16 val4)
{
	Int16 buff[4];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreInt16x4(buff, i16x4);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void UInt16x4Cmp(UInt16x4 u16x4, const Char *funcName, UInt16 val1, UInt16 val2, UInt16 val3, UInt16 val4)
{
	UInt16 buff[4];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreUInt16x4(buff, u16x4);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void Int16x8Cmp(Int16x8 i16x8, const Char *funcName, Int16 val1, Int16 val2, Int16 val3, Int16 val4, Int16 val5, Int16 val6, Int16 val7, Int16 val8)
{
	Int16 buff[8];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreInt16x8(buff, i16x8);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4 && buff[4] == val5 && buff[5] == val6 && buff[6] == val7 && buff[7] == val8)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void UInt16x8Cmp(UInt16x8 u16x8, const Char *funcName, UInt16 val1, UInt16 val2, UInt16 val3, UInt16 val4, UInt16 val5, UInt16 val6, UInt16 val7, UInt16 val8)
{
	UInt16 buff[8];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreUInt16x8(buff, u16x8);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4 && buff[4] == val5 && buff[5] == val6 && buff[6] == val7 && buff[7] == val8)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void Int32x4Cmp(Int32x4 i32x4, const Char *funcName, Int32 val1, Int32 val2, Int32 val3, Int32 val4)
{
	Int32 buff[4];
	Text::StringBuilderUTF8 sb;
	Bool succ;
	PStoreInt32x4(buff, i32x4);
	if (buff[0] == val1 && buff[1] == val2 && buff[2] == val3 && buff[3] == val4)
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" success");
		succ = true;
	}
	else
	{
		sb.Append((const UTF8Char*)funcName);
		sb.Append((const UTF8Char*)" failed");
		succ = false;
	}
	if (!failOnly || !succ)
	{
		console->WriteLine(sb.ToString());
	}
}

void CmpResult(Bool result, const Char *funcName)
{
	if (!failOnly || !result)
	{
		Text::StringBuilderUTF8 sb;
		if (result)
		{
			sb.Append((const UTF8Char*)funcName);
			sb.Append((const UTF8Char*)" success");
		}
		else
		{
			sb.Append((const UTF8Char*)funcName);
			sb.Append((const UTF8Char*)" failed");
		}
		console->WriteLine(sb.ToString());
	}
}

void PrintValUInt8x4(UInt8x4 val)
{
	UInt8 buff[4];
	PStoreUInt8x4(buff, val);
	printf("Val[0] = %x\r\n", buff[0]);
	printf("Val[1] = %x\r\n", buff[1]);
	printf("Val[2] = %x\r\n", buff[2]);
	printf("Val[3] = %x\r\n", buff[3]);
}

void PrintValUInt8x8(UInt8x8 val)
{
	UInt8 buff[8];
	PStoreUInt8x8(buff, val);
	printf("Val[0] = %x\r\n", buff[0]);
	printf("Val[1] = %x\r\n", buff[1]);
	printf("Val[2] = %x\r\n", buff[2]);
	printf("Val[3] = %x\r\n", buff[3]);
	printf("Val[4] = %x\r\n", buff[4]);
	printf("Val[5] = %x\r\n", buff[5]);
	printf("Val[6] = %x\r\n", buff[6]);
	printf("Val[7] = %x\r\n", buff[7]);
}

void PrintValUInt8x16(UInt8x16 val)
{
	UInt8 buff[16];
	PStoreUInt8x16(buff, val);
	printf("Val[0] = %x\r\n", buff[0]);
	printf("Val[1] = %x\r\n", buff[1]);
	printf("Val[2] = %x\r\n", buff[2]);
	printf("Val[3] = %x\r\n", buff[3]);
	printf("Val[4] = %x\r\n", buff[4]);
	printf("Val[5] = %x\r\n", buff[5]);
	printf("Val[6] = %x\r\n", buff[6]);
	printf("Val[7] = %x\r\n", buff[7]);
	printf("Val[8] = %x\r\n", buff[8]);
	printf("Val[9] = %x\r\n", buff[9]);
	printf("Val[10] = %x\r\n", buff[10]);
	printf("Val[11] = %x\r\n", buff[11]);
	printf("Val[12] = %x\r\n", buff[12]);
	printf("Val[13] = %x\r\n", buff[13]);
	printf("Val[14] = %x\r\n", buff[14]);
	printf("Val[15] = %x\r\n", buff[15]);
}

void PrintValUInt16x4(UInt16x4 val)
{
	printf("Val[0] = %x\r\n", PEXTUW4(val, 0));
	printf("Val[1] = %x\r\n", PEXTUW4(val, 1));
	printf("Val[2] = %x\r\n", PEXTUW4(val, 2));
	printf("Val[3] = %x\r\n", PEXTUW4(val, 3));
}

void PrintValUInt16x8(UInt16x8 val)
{
	printf("Val[0] = %x\r\n", PEXTUW8(val, 0));
	printf("Val[1] = %x\r\n", PEXTUW8(val, 1));
	printf("Val[2] = %x\r\n", PEXTUW8(val, 2));
	printf("Val[3] = %x\r\n", PEXTUW8(val, 3));
	printf("Val[4] = %x\r\n", PEXTUW8(val, 4));
	printf("Val[5] = %x\r\n", PEXTUW8(val, 5));
	printf("Val[6] = %x\r\n", PEXTUW8(val, 6));
	printf("Val[7] = %x\r\n", PEXTUW8(val, 7));
}

void PrintValInt16x4(Int16x4 val)
{
	printf("Val[0] = %x\r\n", PEXTW4(val, 0));
	printf("Val[1] = %x\r\n", PEXTW4(val, 1));
	printf("Val[2] = %x\r\n", PEXTW4(val, 2));
	printf("Val[3] = %x\r\n", PEXTW4(val, 3));
}

void PrintValInt16x8(Int16x8 val)
{
	printf("Val[0] = %x\r\n", PEXTW8(val, 0));
	printf("Val[1] = %x\r\n", PEXTW8(val, 1));
	printf("Val[2] = %x\r\n", PEXTW8(val, 2));
	printf("Val[3] = %x\r\n", PEXTW8(val, 3));
	printf("Val[4] = %x\r\n", PEXTW8(val, 4));
	printf("Val[5] = %x\r\n", PEXTW8(val, 5));
	printf("Val[6] = %x\r\n", PEXTW8(val, 6));
	printf("Val[7] = %x\r\n", PEXTW8(val, 7));
}

void PrintValInt32x4(Int32x4 val)
{
	printf("Val[0] = %x\r\n", PEXTD4(val, 0));
	printf("Val[1] = %x\r\n", PEXTD4(val, 1));
	printf("Val[2] = %x\r\n", PEXTD4(val, 2));
	printf("Val[3] = %x\r\n", PEXTD4(val, 3));
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 *abuff = MemAllocA(UInt8, 32);
	abuff[0] = 0x00;
	abuff[1] = 0x11;
	abuff[2] = 0x22;
	abuff[3] = 0x33;
	abuff[4] = 0x44;
	abuff[5] = 0x55;
	abuff[6] = 0x66;
	abuff[7] = 0x77;
	abuff[8] = 0x88;
	abuff[9] = 0x99;
	abuff[10] = 0xAA;
	abuff[11] = 0xBB;
	abuff[12] = 0xCC;
	abuff[13] = 0xDD;
	abuff[14] = 0xEE;
	abuff[15] = 0xFF;
	abuff[16] = 0xFF;
	abuff[17] = 0xEE;
	abuff[18] = 0xDD;
	abuff[19] = 0xCC;
	abuff[20] = 0xBB;
	abuff[21] = 0xAA;
	abuff[22] = 0x99;
	abuff[23] = 0x88;
	abuff[24] = 0x77;
	abuff[25] = 0x66;
	abuff[26] = 0x55;
	abuff[27] = 0x44;
	abuff[28] = 0x33;
	abuff[29] = 0x22;
	abuff[30] = 0x11;
	abuff[31] = 0x00;
	NEW_CLASS(console, IO::ConsoleWriter());
	UInt8x4Cmp(PUInt8x4Clear(), "PUInt8x4Clear", 0, 0, 0, 0);
	UInt8x8Cmp(PUInt8x8Clear(), "PUInt8x8Clear", 0, 0, 0, 0, 0, 0, 0, 0);
	UInt8x16Cmp(PUInt8x16Clear(), "PUInt8x16Clear", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	Int16x4Cmp(PInt16x4Clear(), "PInt16x4Clear", 0, 0, 0, 0);
	UInt16x4Cmp(PUInt16x4Clear(), "PUInt16x4Clear", 0, 0, 0, 0);
	Int16x8Cmp(PInt16x8Clear(), "PInt16x8Clear", 0, 0, 0, 0, 0, 0, 0, 0);
	UInt16x8Cmp(PUInt16x8Clear(), "PUInt16x8Clear", 0, 0, 0, 0, 0, 0, 0, 0);
	Int32x4Cmp(PInt32x4Clear(), "PInt32x4Clear", 0, 0, 0, 0);
	UInt8x4Cmp(PUInt8x4SetA(129), "PUInt8x4SetA", 129, 129, 129, 129);
	UInt8x8Cmp(PUInt8x8SetA(129), "PUInt8x8SetA", 129, 129, 129, 129, 129, 129, 129, 129);
	UInt8x16Cmp(PUInt8x16SetA(129), "PUInt8x16SetA", 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129);
	Int16x4Cmp(PInt16x4SetA(-777), "PInt16x4SetA", -777, -777, -777, -777);
	UInt16x4Cmp(PUInt16x4SetA(0x8765), "PUInt16x4SetA", 0x8765, 0x8765, 0x8765, 0x8765);
	Int16x8Cmp(PInt16x8SetA(-777), "PInt16x8SetA", -777, -777, -777, -777, -777, -777, -777, -777);
	UInt16x8Cmp(PUInt16x8SetA(0x8765), "PUInt16x8SetA", 0x8765, 0x8765, 0x8765, 0x8765, 0x8765, 0x8765, 0x8765, 0x8765);
	Int32x4Cmp(PInt32x4SetA(-12345), "PInt32x4SetA", -12345, -12345, -12345, -12345);
	UInt8x4Cmp(PLoadUInt8x4(abuff), "PLoadUInt8x4", 0x00, 0x11, 0x22, 0x33);
	UInt8x8Cmp(PLoadUInt8x8(abuff), "PLoadUInt8x8", 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77);
	UInt8x16Cmp(PLoadUInt8x16(abuff), "PLoadUInt8x16", 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF);
	Int16x4Cmp(PLoadInt16x4(abuff), "PLoadInt16x4", 0x1100, 0x3322, 0x5544, 0x7766);
	UInt16x4Cmp(PLoadUInt16x4(abuff), "PLoadUInt16x4", 0x1100, 0x3322, 0x5544, 0x7766);
	Int16x8Cmp(PLoadInt16x8(abuff), "PLoadInt16x8", 0x1100, 0x3322, 0x5544, 0x7766, (Int16)0x9988, (Int16)0xBBAA, (Int16)0xDDCC, (Int16)0xFFEE);
	Int16x8Cmp(PLoadInt16x8A(abuff), "PLoadInt16x8A", 0x1100, 0x3322, 0x5544, 0x7766, (Int16)0x9988, (Int16)0xBBAA, (Int16)0xDDCC, (Int16)0xFFEE);
	UInt16x8Cmp(PLoadUInt16x8(abuff), "PLoadUInt16x8", 0x1100, 0x3322, 0x5544, 0x7766, 0x9988, 0xBBAA, 0xDDCC, 0xFFEE);
	UInt16x8Cmp(PLoadUInt16x8A(abuff), "PLoadUInt16x8A", 0x1100, 0x3322, 0x5544, 0x7766, 0x9988, 0xBBAA, 0xDDCC, 0xFFEE);
	Int32x4Cmp(PLoadInt32x4(abuff), "PLoadInt32x4", 0x33221100, 0x77665544, (Int32)0xBBAA9988, (Int32)0xFFEEDDCC);
	Int32x4Cmp(PLoadInt32x4A(abuff), "PLoadInt32x4A", 0x33221100, 0x77665544, (Int32)0xBBAA9988, (Int32)0xFFEEDDCC);
	Int16x8Cmp(PMLoadInt16x4(&abuff[0], &abuff[16]), "PMLoadInt16x4", (Int16)0xEEFF, (Int16)0xCCDD, (Int16)0xAABB, (Int16)0x8899, 0x1100, 0x3322, 0x5544, 0x7766);
	Int16x4Cmp(PCONVU16x4_I(PLoadUInt16x4(abuff)), "PCONVU16x4_I", 0x1100, 0x3322, 0x5544, 0x7766);
	UInt16x4Cmp(PCONVI16x4_U(PLoadInt16x4(abuff)), "PCONVI16x4_U", 0x1100, 0x3322, 0x5544, 0x7766);
	Int16x8Cmp(PCONVU16x8_I(PLoadUInt16x8(abuff)), "PCONVU16x8_I", 0x1100, 0x3322, 0x5544, 0x7766, (Int16)0x9988, (Int16)0xBBAA, (Int16)0xDDCC, (Int16)0xFFEE);
	UInt16x8Cmp(PCONVI16x8_U(PLoadInt16x8(abuff)), "PCONVI16x8_U", 0x1100, 0x3322, 0x5544, 0x7766, 0x9988, 0xBBAA, 0xDDCC, 0xFFEE);
	UInt16x4 u16x4 = PLoadUInt16x4(abuff);
	Int16x4 i16x4 = PLoadInt16x4(abuff);
	UInt16x8 u16x8 = PLoadUInt16x8(abuff);
	Int16x8 i16x8 = PLoadInt16x8(abuff);
	Int32x4 i32x4 = PLoadInt32x4(abuff);
	CmpResult(PEXTUW4(u16x4, 0) == 0x1100 && PEXTUW4(u16x4, 1) == 0x3322 && PEXTUW4(u16x4, 2) == 0x5544 && PEXTUW4(u16x4, 3) == 0x7766, "PEXTUW4");
	CmpResult(PEXTW4(i16x4, 0) == 0x1100 && PEXTW4(i16x4, 1) == 0x3322 && PEXTW4(i16x4, 2) == 0x5544 && PEXTW4(i16x4, 3) == 0x7766, "PEXTW4");
	CmpResult(PEXTUW8(u16x8, 0) == 0x1100 && PEXTUW8(u16x8, 1) == 0x3322 && PEXTUW8(u16x8, 2) == 0x5544 && PEXTUW8(u16x8, 3) == 0x7766 &&
		PEXTUW8(u16x8, 4) == 0x9988 && PEXTUW8(u16x8, 5) == 0xBBAA && PEXTUW8(u16x8, 6) == 0xDDCC && PEXTUW8(u16x8, 7) == 0xFFEE, "PEXTUW8");
	CmpResult(PEXTW8(i16x8, 0) == 0x1100 && PEXTW8(i16x8, 1) == 0x3322 && PEXTW8(i16x8, 2) == 0x5544 && PEXTW8(i16x8, 3) == 0x7766 &&
		PEXTW8(i16x8, 4) == (Int16)0x9988 && PEXTW8(i16x8, 5) == (Int16)0xBBAA && PEXTW8(i16x8, 6) == (Int16)0xDDCC && PEXTW8(i16x8, 7) == (Int16)0xFFEE, "PEXTW8");
	CmpResult(PEXTD4(i32x4, 0) == 0x33221100 && PEXTD4(i32x4, 1) == 0x77665544 && PEXTD4(i32x4, 2) == (Int32)0xBBAA9988 && PEXTD4(i32x4, 3) == (Int32)0xFFEEDDCC, "PEXTD4");
	u16x4 = PINSUW4(PINSUW4(u16x4, 1, 0x1234), 3, 0x4567);
	UInt16x4Cmp(PINSUW4(PINSUW4(u16x4, 1, 0x1234), 3, 0x4567), "PINSUW4", 0x1100, 0x1234, 0x5544, 0x4567);
	UInt8x8Cmp(PUNPCKUBB4(PLoadUInt8x4(abuff), PLoadUInt8x4(&abuff[16])), "PUNPCKUBB4", 0x00, 0xFF, 0x11, 0xEE, 0x22, 0xDD, 0x33, 0xCC);
	UInt16x4Cmp(PUNPCKUBW4(PLoadUInt8x4(abuff), PLoadUInt8x4(&abuff[16])), "PUNPCKUBW4", 0xFF00, 0xEE11, 0xDD22, 0xCC33);
	UInt16x8Cmp(PUNPCKLUBW8(PLoadUInt8x16(abuff), PLoadUInt8x16(&abuff[16])), "PUNPCKLUBW8", 0xFF00, 0xEE11, 0xDD22, 0xCC33, 0xBB44, 0xAA55, 0x9966, 0x8877);
	UInt16x8Cmp(PUNPCKHUBW8(PLoadUInt8x16(abuff), PLoadUInt8x16(&abuff[16])), "PUNPCKHUBW8", 0x7788, 0x6699, 0x55AA, 0x44BB, 0x33CC, 0x22DD, 0x11EE, 0x00FF);
	Int16x8Cmp(PUNPCKWW4(PLoadInt16x4(abuff), PLoadInt16x4(&abuff[16])), "PUNPCKWW4", (Int16)0x1100, (Int16)0xEEFF, (Int16)0x3322, (Int16)0xCCDD, (Int16)0x5544, (Int16)0xAABB, (Int16)0x7766, (Int16)0x8899);
	Int16x8Cmp(PUNPCKLWW8(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PUNPCKLWW8", (Int16)0x1100, (Int16)0xEEFF, (Int16)0x3322, (Int16)0xCCDD, (Int16)0x5544, (Int16)0xAABB, (Int16)0x7766, (Int16)0x8899);
	Int16x8Cmp(PUNPCKHWW8(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PUNPCKHWW8", (Int16)0x9988, (Int16)0x6677, (Int16)0xBBAA, (Int16)0x4455, (Int16)0xDDCC, (Int16)0x2233, (Int16)0xFFEE, (Int16)0x0011);
	Int32x4Cmp(PUNPCKLWD4(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PUNPCKLWD4", (Int32)0xEEFF1100, (Int32)0xCCDD3322, (Int32)0xAABB5544, (Int32)0x88997766);
	Int32x4Cmp(PUNPCKHWD4(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PUNPCKHWD4", (Int32)0x66779988, (Int32)0x4455BBAA, (Int32)0x2233DDCC, (Int32)0x0011FFEE);
	Int32x4Cmp(PUNPCKWD4(PLoadInt16x4(abuff), PLoadInt16x4(&abuff[16])), "PUNPCKWD4", (Int32)0xEEFF1100, (Int32)0xCCDD3322, (Int32)0xAABB5544, (Int32)0x88997766);
	Int16x8Cmp(PMergeW4(PLoadInt16x4(abuff), PLoadInt16x4(&abuff[16])), "PMergeW4", (Int16)0x1100, (Int16)0x3322, (Int16)0x5544, (Int16)0x7766, (Int16)0xEEFF, (Int16)0xCCDD, (Int16)0xAABB, (Int16)0x8899);
	Int16x8Cmp(PMergeLW4(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PMergeLW4", (Int16)0x1100, (Int16)0x3322, (Int16)0x5544, (Int16)0x7766, (Int16)0xEEFF, (Int16)0xCCDD, (Int16)0xAABB, (Int16)0x8899);
	Int16x8Cmp(PMergeHW4(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PMergeHW4", (Int16)0x9988, (Int16)0xBBAA, (Int16)0xDDCC, (Int16)0xFFEE, (Int16)0x6677, (Int16)0x4455, (Int16)0x2233, (Int16)0x0011);
	Int16x8Cmp(PMergeSARDW4(PLoadInt32x4(abuff), PLoadInt32x4(&abuff[16]), 16), "PMergeSARDW4 16", (Int16)0x3322, (Int16)0x7766, (Int16)0xBBAA, (Int16)0xFFEE, (Int16)0xCCDD, (Int16)0x8899, (Int16)0x4455, (Int16)0x0011);
	Int16x8Cmp(PMergeSARDW4(PLoadInt32x4(abuff), PLoadInt32x4(&abuff[16]), 15), "PMergeSARDW4 15", (Int16)0x6644, (Int16)0x7FFF, (Int16)0x8000, (Int16)0xFFDD, (Int16)0x99BB, (Int16)0x8000, (Int16)0x7FFF, (Int16)0x0022);
	Int16x4Cmp(PSALW4(PLoadInt16x4(&abuff[16]), 1), "PSALW4", (Int16)0xDDFE, (Int16)0x99BA, (Int16)0x5576, (Int16)0x1132);
	UInt16x4Cmp(PSHRW4(PLoadUInt16x4(&abuff[16]), 1), "PSHRW4", 0x777F, 0x666E, 0x555D, 0x444C);
	UInt16x8Cmp(PSHRW8(PLoadUInt16x8(&abuff[16]), 1), "PSHRW8", 0x777F, 0x666E, 0x555D, 0x444C, 0x333B, 0x222A, 0x1119, 0x0008);
	Int16x4Cmp(PSARW4(PLoadInt16x4(&abuff[16]), 1), "PSARW4", (Int16)0xF77F, (Int16)0xE66E, (Int16)0xD55D, (Int16)0xC44C);
	Int16x8Cmp(PSARW8(PLoadInt16x8(&abuff[16]), 1), "PSARW8", (Int16)0xF77F, (Int16)0xE66E, (Int16)0xD55D, (Int16)0xC44C, (Int16)0x333B, (Int16)0x222A, (Int16)0x1119, (Int16)0x0008);
	Int32x4Cmp(PSARD4(PLoadInt32x4(&abuff[16]), 1), "PSARD4", (Int32)0xE66EF77F, (Int32)0xC44CD55D, 0x222AB33B, 0x00089119);
	Int16x4Cmp(PSARSDW4(PLoadInt32x4(&abuff[16]), 15), "PSARSDW4", (Int16)0x99BB, (Int16)0x8000, (Int16)0x7FFF, (Int16)0x0022);
	Int16x8Cmp(PSARSDW8(PLoadInt32x4(abuff), PLoadInt32x4(&abuff[16]), 15), "PSARSDW8", (Int16)0x6644, (Int16)0x7FFF, (Int16)0x8000, (Int16)0xFFDD, (Int16)0x99BB, (Int16)0x8000, (Int16)0x7FFF, (Int16)0x0022);
	UInt8x4Cmp(PSHRADDWB4(PLoadUInt16x4(abuff), PLoadUInt16x4(&abuff[16]), 7), "PSHRADDWB4 7", 0xFF, 0xFF, 0xFF, 0xFF);
	UInt8x4Cmp(PSHRADDWB4(PLoadUInt16x4(abuff), PLoadUInt16x4(&abuff[16]), 8), "PSHRADDWB4 8", 0xFF, 0xFF, 0xFF, 0xFF);
	UInt8x4Cmp(PSHRADDWB4(PLoadUInt16x4(abuff), PLoadUInt16x4(&abuff[16]), 9), "PSHRADDWB4 9", 0x7F, 0x7F, 0x7F, 0x7F);
	UInt8x4Cmp(PADDUB4(PLoadUInt8x4(abuff), PLoadUInt8x4(abuff)), "PADDUB4", 0x00, 0x22, 0x44, 0x66);
	UInt8x8Cmp(PADDUB8(PLoadUInt8x8(abuff), PLoadUInt8x8(abuff)), "PADDUB8", 0x00, 0x22, 0x44, 0x66, 0x88, 0xAA, 0xCC, 0xEE);
	UInt8x16Cmp(PADDUB16(PLoadUInt8x16(abuff), PLoadUInt8x16(abuff)), "PADDUB16", 0x00, 0x22, 0x44, 0x66, 0x88, 0xAA, 0xCC, 0xEE, 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE);
	Int16x4Cmp(PADDW4(PLoadInt16x4(abuff), PLoadInt16x4(abuff)), "PADDW4", 0x2200, 0x6644, (Int16)0xAA88, (Int16)0xEECC);
	UInt16x4Cmp(PADDUW4(PLoadUInt16x4(abuff), PLoadUInt16x4(abuff)), "PADDUW4", 0x2200, 0x6644, 0xAA88, 0xEECC);
	Int16x8Cmp(PADDW8(PLoadInt16x8(abuff), PLoadInt16x8(abuff)), "PADDW8", 0x2200, 0x6644, (Int16)0xAA88, (Int16)0xEECC, 0x3310, 0x7754, (Int16)0xBB98, (Int16)0xFFDC);
	Int32x4Cmp(PADDD4(PLoadInt32x4(abuff), PLoadInt32x4(abuff)), "PADDD4", 0x66442200, (Int32)0xEECCAA88, 0x77553310, (Int32)0xFFDDBB98);
	UInt8x4Cmp(PADDSUWB4(PLoadInt16x4(&abuff[16]), PLoadInt16x4(&abuff[16])), "PADDSUWB4", 0x00, 0x00, 0xFF, 0xFF);
	Int16x4Cmp(PSADDW4(PLoadInt16x4(&abuff[16]), PLoadInt16x4(&abuff[16])), "PSADDW4", (Int16)0xDDFE, (Int16)0x99BA, (Int16)0x8000, (Int16)0x8000);
	UInt16x4Cmp(PSADDUW4(PLoadUInt16x4(&abuff[16]), PLoadUInt16x4(&abuff[16])), "PSADDUW4", 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF);
	Int16x8Cmp(PSADDW8(PLoadInt16x8(&abuff[16]), PLoadInt16x8(&abuff[16])), "PSADDW8", (Int16)0xDDFE, (Int16)0x99BA, (Int16)0x8000, (Int16)0x8000, 0x7FFF, 0x7FFF, 0x4466, 0x0022);
	Int16x8Cmp(PHSADDW8_4(PLoadInt16x8(&abuff[16]), PLoadInt16x8(&abuff[16])), "PHSADDW8_4", 0x5576, 0x1132, (Int16)0xCCEE, (Int16)0x88AA, 0x5576, 0x1132, (Int16)0xCCEE, (Int16)0x88AA);
	Int32x4Cmp(PMADDWD(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PMADDWD", (Int32)0xF4A82B5A, (Int32)0xABE8FCA2, (Int32)0xC4BF05AA, (Int32)0xFB6E4672);
	Int16x4Cmp(PSUBW4(PLoadInt16x4(abuff), PLoadInt16x4(&abuff[16])), "PSUBW4", 0x2201, 0x6645, (Int16)0xAA89, (Int16)0xEECD);
	Int32x4Cmp(PSUBD4(PLoadInt32x4(abuff), PLoadInt32x4(&abuff[16])), "PSUBD4", 0x66442201, (Int32)0xEECCAA89, 0x77553311, (Int32)0xFFDDBB99);
	Int16x4Cmp(PMULHW4(PLoadInt16x4(abuff), PLoadInt16x4(&abuff[16])), "PMULHW4", (Int16)0xFEDE, (Int16)0xF5C9, (Int16)0xE399, (Int16)0xC84F);
	Int16x8Cmp(PMULHW8(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PMULHW8", (Int16)0xFEDE, (Int16)0xF5C9, (Int16)0xE399, (Int16)0xC84F, (Int16)0xD6FC, (Int16)0xEDC2, (Int16)0xFB6E, (Int16)0xFFFF);
	UInt16x4Cmp(PMULUHW4(PLoadUInt16x4(abuff), PLoadUInt16x4(&abuff[16])), "PMULUHW4", 0x0FDE, 0x28EB, 0x38DD, 0x3FB5);
	UInt16x4Cmp(PMULULW4(PLoadUInt16x4(abuff), PLoadUInt16x4(&abuff[16])), "PMULULW4", 0xEF00, 0x3C5A, 0x70AC, 0x8BF6);
	Int16x4Cmp(PMULM2HW4(PLoadInt16x4(abuff), PLoadInt16x4(&abuff[16])), "PMULM2HW4", (Int16)0xFDBC, (Int16)0xEB92, (Int16)0xC732, (Int16)0x909E);
	Int16x8Cmp(PMULM2HW8(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PMULM2HW8", (Int16)0xFDBC, (Int16)0xEB92, (Int16)0xC732, (Int16)0x909E, (Int16)0xADF8, (Int16)0xDB84, (Int16)0xF6DC, (Int16)0xFFFE);
	Int16x8Cmp(PORW8(PLoadInt16x8(abuff), PLoadInt16x8(&abuff[16])), "PORW8", (Int16)0xFFFF, (Int16)0xFFFF, (Int16)0xFFFF, (Int16)0xFFFF, (Int16)0xFFFF, (Int16)0xFFFF, (Int16)0xFFFF, (Int16)0xFFFF);

/*
#define SI16ToI8x4(v1) _mm_packs_epi16(v1, v1)
#define SI16ToI8x8(v1) _mm_packs_epi16(v1, v1)
#define SI16ToI8x16(v1, v2) _mm_packs_epi16(v1, v2)
#define SU16ToU8x4(v1) _mm_packs_epi16(v1, v1)
#define SI32ToU8x8(v1, v2) _mm_packus_epi16(_mm_packs_epi32(v1, v2), v1)
#define SI32ToI16x4(v1) _mm_packs_epi32(v1, v1)
#define SI32ToI16x8(v1, v2) _mm_packs_epi32(v1, v2)
//#define PCONVI8x4_U(v) (v)
//#define PCONVI8x8_U(v) (v)
//#define PCONVI8x16_U(v) (v)
*/
	DEL_CLASS(console);
	MemFreeA(abuff);
	return 0;
}