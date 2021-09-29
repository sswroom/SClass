#include "Stdafx.h"
#include "SIMD.h"
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
/*
#define PUNPCKBB8(v1, v2) _mm_unpacklo_epi8(v1, v2)
#define PUNPCKUBB4(v1, v2) _mm_unpacklo_epi8(v1, v2)
#define PUNPCKUBW4(v1, v2) _mm_unpacklo_epi8(v1, v2)
#define PUNPCKUBW8(v1, v2) _mm_unpacklo_epi8(v1, v2)
#define PUNPCKLUBW8(v1, v2) _mm_unpacklo_epi8(v1, v2)
#define PUNPCKHUBW8(v1, v2) _mm_unpackhi_epi8(v1, v2)
#define PUNPCKWW4(v1, v2) _mm_unpacklo_epi16(v1, v2)
#define PUNPCKLWW8(v1, v2) _mm_unpacklo_epi16(v1, v2)
#define PUNPCKHWW8(v1, v2) _mm_unpackhi_epi16(v1, v2)
#define PUNPCKLWD4(v1, v2) _mm_unpacklo_epi16(v1, v2)
#define PUNPCKHWD4(v1, v2) _mm_unpackhi_epi16(v1, v2)
#define PUNPCKWD4(v1, v2) _mm_unpacklo_epi16(v1, v2)
#define PMergeW4(v1, v2) _mm_unpacklo_epi64(v1, v2)
#define PMergeLW4(v1, v2) _mm_unpacklo_epi64(v1, v2)
#define PMergeHW4(v1, v2) _mm_unpackhi_epi64(v1, v2)
#define PMergeSARDW4(v1, v2, cnt) _mm_packs_epi32(_mm_srai_epi32(v1, cnt), _mm_srai_epi32(v2, cnt))
#define PSALW4(v1, v2) _mm_slli_epi16(v1, v2)
#define PSHRW4(v1, v2) _mm_srli_epi16(v1, v2)
#define PSHRW8(v1, v2) _mm_srli_epi16(v1, v2)
#define PSARW4(v1, v2) _mm_srai_epi16(v1, v2)
#define PSARW8(v1, v2) _mm_srai_epi16(v1, v2)
#define PSARD4(v1, v2) _mm_srai_epi32(v1, v2)
#define PSARSDW4(v1, cnt) _mm_packs_epi32(_mm_srai_epi32(v1, cnt), v1)
#define PSARSDW8(v1, v2, cnt) _mm_packs_epi32(_mm_srai_epi32(v1, cnt), _mm_srai_epi32(v2, cnt))
#define PSHRADDWB4(v1, v2, cnt) _mm_packs_epi16(_mm_srli_epi16(_mm_adds_epu16(v1, v2), cnt), v2)
#define PADDUB4(v1, v2) _mm_add_epi8(v1, v2)
#define PADDUB8(v1, v2) _mm_add_epi8(v1, v2)
#define PADDUB16(v1, v2) _mm_add_epi8(v1, v2)
#define PADDW4(v1, v2) _mm_add_epi16(v1, v2)
#define PADDUW4(v1, v2) _mm_add_epi16(v1, v2)
#define PADDW8(v1, v2) _mm_add_epi16(v1, v2)
#define PADDD4(v1, v2) _mm_add_epi32(v1, v2)
#define PADDSUWB4(v1, v2) _mm_packs_epi16(_mm_add_epi16(v1, v2), v2)
#define PSADDW4(v1, v2) _mm_adds_epi16(v1, v2)
#define PSADDUW4(v1, v2) _mm_adds_epu16(v1, v2)
#define PSADDW8(v1, v2) _mm_adds_epi16(v1, v2)
#define PHSADDW8_4(v1, v2) _mm_adds_epi16(_mm_unpacklo_epi64(v1, v2), _mm_unpackhi_epi64(v1, v2))
#define PMADDWD(v1, v2) _mm_madd_epi16(v1, v2)
#define PSUBW4(v1, v2) _mm_sub_epi16(v1, v2)
#define PSUBD4(v1, v2) _mm_sub_epi32(v1, v2)
#define PMULHW4(v1, v2) _mm_mulhi_epi16(v1, v2)
#define PMULHW8(v1, v2) _mm_mulhi_epi16(v1, v2)
#define PMULUHW4(v1, v2) _mm_mulhi_epu16(v1, v2)
#define PMULULW4(v1, v2) _mm_mullo_epi16(v1, v2)
#define PMULM2HW4(v1, v2) _mm_slli_epi16(_mm_mulhi_epi16(v1, v2), 1)
#define PMULM2HW8(v1, v2) _mm_slli_epi16(_mm_mulhi_epi16(v1, v2), 1)
#define PMULLD4(v1, v2) _mm_mullo_epi16(v1, v2)
#define PORW8(v1, v2) _mm_or_si128(v1, v2)
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