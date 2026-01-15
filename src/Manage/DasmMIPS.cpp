#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/DasmMIPS.h"
#include "Manage/ThreadContextMIPS.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#ifndef __GCC__
#pragma warning( disable : 4311 4312)
#endif

UnsafeArrayOpt<UTF8Char> DasmMIPS_ParseReg8(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("$zero"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("$at"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("$v0"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("$v1"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("$a0"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("$a1"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("$a2"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("$a3"));
	case 8:
		return Text::StrConcatC(regName, UTF8STRC("$t0"));
	case 9:
		return Text::StrConcatC(regName, UTF8STRC("$t1"));
	case 10:
		return Text::StrConcatC(regName, UTF8STRC("$t2"));
	case 11:
		return Text::StrConcatC(regName, UTF8STRC("$t3"));
	case 12:
		return Text::StrConcatC(regName, UTF8STRC("$t4"));
	case 13:
		return Text::StrConcatC(regName, UTF8STRC("$t5"));
	case 14:
		return Text::StrConcatC(regName, UTF8STRC("$t6"));
	case 15:
		return Text::StrConcatC(regName, UTF8STRC("$t7"));
	case 16:
		return Text::StrConcatC(regName, UTF8STRC("$s0"));
	case 17:
		return Text::StrConcatC(regName, UTF8STRC("$s1"));
	case 18:
		return Text::StrConcatC(regName, UTF8STRC("$s2"));
	case 19:
		return Text::StrConcatC(regName, UTF8STRC("$s3"));
	case 20:
		return Text::StrConcatC(regName, UTF8STRC("$s4"));
	case 21:
		return Text::StrConcatC(regName, UTF8STRC("$s5"));
	case 22:
		return Text::StrConcatC(regName, UTF8STRC("$s6"));
	case 23:
		return Text::StrConcatC(regName, UTF8STRC("$s7"));
	case 24:
		return Text::StrConcatC(regName, UTF8STRC("$t8"));
	case 25:
		return Text::StrConcatC(regName, UTF8STRC("$t9"));
	case 26:
		return Text::StrConcatC(regName, UTF8STRC("$k0"));
	case 27:
		return Text::StrConcatC(regName, UTF8STRC("$k1"));
	case 28:
		return Text::StrConcatC(regName, UTF8STRC("$gp"));
	case 29:
		return Text::StrConcatC(regName, UTF8STRC("$sp"));
	case 30:
		return Text::StrConcatC(regName, UTF8STRC("$fp"));
	case 31:
		return Text::StrConcatC(regName, UTF8STRC("$ra"));
	}
	return 0;
}

Bool DasmMIPS_00(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_01(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_02(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_03(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_04(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_05(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_06(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_07(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_08(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_09(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_10(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_11(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_12(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_13(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_14(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_15(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_16(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_17(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_18(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_19(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_1A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_1B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_1C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_1D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_1E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_1F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_20(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_21(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_22(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_23(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_24(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_25(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_26(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_27(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_28(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_29(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_2A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_2B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_2C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_2D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_2E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_2F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_30(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_31(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_32(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_33(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_34(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_35(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_36(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_37(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_38(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_39(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_3A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_3B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_3C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_3D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_3E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_3F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_00(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_01(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_02(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_03(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_04(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_05(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_06(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_07(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_08(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_09(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_0A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_0B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_0C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_0D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_0E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_0F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_10(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_11(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_12(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_13(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_14(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_15(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_16(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_17(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_18(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_19(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_1A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_1B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_1C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_1D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_1E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_1F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_20(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_21(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_22(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_23(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_24(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_25(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_26(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_27(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_28(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_29(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_2A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_2B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_2C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_2D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_2E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_2F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_30(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_31(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_32(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_33(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_34(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_35(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_36(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_37(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_38(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_39(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_3A(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_3B(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_3C(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_3D(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_3E(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Bool DasmMIPS_0_3F(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	return false;
}

Manage::DasmMIPS::DasmMIPS()
{
	DasmMIPS_Code *codes;
	codes = this->codes = MemAlloc(DasmMIPS_Code, 256);
	codes[0x00] = DasmMIPS_00;
	codes[0x01] = DasmMIPS_01;
	codes[0x02] = DasmMIPS_02;
	codes[0x03] = DasmMIPS_03;
	codes[0x04] = DasmMIPS_04;
	codes[0x05] = DasmMIPS_05;
	codes[0x06] = DasmMIPS_06;
	codes[0x07] = DasmMIPS_07;
	codes[0x08] = DasmMIPS_08;
	codes[0x09] = DasmMIPS_09;
	codes[0x0A] = DasmMIPS_0A;
	codes[0x0B] = DasmMIPS_0B;
	codes[0x0C] = DasmMIPS_0C;
	codes[0x0D] = DasmMIPS_0D;
	codes[0x0E] = DasmMIPS_0E;
	codes[0x0F] = DasmMIPS_0F;
	codes[0x10] = DasmMIPS_10;
	codes[0x11] = DasmMIPS_11;
	codes[0x12] = DasmMIPS_12;
	codes[0x13] = DasmMIPS_13;
	codes[0x14] = DasmMIPS_14;
	codes[0x15] = DasmMIPS_15;
	codes[0x16] = DasmMIPS_16;
	codes[0x17] = DasmMIPS_17;
	codes[0x18] = DasmMIPS_18;
	codes[0x19] = DasmMIPS_19;
	codes[0x1A] = DasmMIPS_1A;
	codes[0x1B] = DasmMIPS_1B;
	codes[0x1C] = DasmMIPS_1C;
	codes[0x1D] = DasmMIPS_1D;
	codes[0x1E] = DasmMIPS_1E;
	codes[0x1F] = DasmMIPS_1F;
	codes[0x20] = DasmMIPS_20;
	codes[0x21] = DasmMIPS_21;
	codes[0x22] = DasmMIPS_22;
	codes[0x23] = DasmMIPS_23;
	codes[0x24] = DasmMIPS_24;
	codes[0x25] = DasmMIPS_25;
	codes[0x26] = DasmMIPS_26;
	codes[0x27] = DasmMIPS_27;
	codes[0x28] = DasmMIPS_28;
	codes[0x29] = DasmMIPS_29;
	codes[0x2A] = DasmMIPS_2A;
	codes[0x2B] = DasmMIPS_2B;
	codes[0x2C] = DasmMIPS_2C;
	codes[0x2D] = DasmMIPS_2D;
	codes[0x2E] = DasmMIPS_2E;
	codes[0x2F] = DasmMIPS_2F;
	codes[0x30] = DasmMIPS_30;
	codes[0x31] = DasmMIPS_31;
	codes[0x32] = DasmMIPS_32;
	codes[0x33] = DasmMIPS_33;
	codes[0x34] = DasmMIPS_34;
	codes[0x35] = DasmMIPS_35;
	codes[0x36] = DasmMIPS_36;
	codes[0x37] = DasmMIPS_37;
	codes[0x38] = DasmMIPS_38;
	codes[0x39] = DasmMIPS_39;
	codes[0x3A] = DasmMIPS_3A;
	codes[0x3B] = DasmMIPS_3B;
	codes[0x3C] = DasmMIPS_3C;
	codes[0x3D] = DasmMIPS_3D;
	codes[0x3E] = DasmMIPS_3E;
	codes[0x3F] = DasmMIPS_3F;

	codes = this->codes_0 = MemAlloc(DasmMIPS_Code, 256);
	codes[0x00] = DasmMIPS_0_00;
	codes[0x01] = DasmMIPS_0_01;
	codes[0x02] = DasmMIPS_0_02;
	codes[0x03] = DasmMIPS_0_03;
	codes[0x04] = DasmMIPS_0_04;
	codes[0x05] = DasmMIPS_0_05;
	codes[0x06] = DasmMIPS_0_06;
	codes[0x07] = DasmMIPS_0_07;
	codes[0x08] = DasmMIPS_0_08;
	codes[0x09] = DasmMIPS_0_09;
	codes[0x0A] = DasmMIPS_0_0A;
	codes[0x0B] = DasmMIPS_0_0B;
	codes[0x0C] = DasmMIPS_0_0C;
	codes[0x0D] = DasmMIPS_0_0D;
	codes[0x0E] = DasmMIPS_0_0E;
	codes[0x0F] = DasmMIPS_0_0F;
	codes[0x10] = DasmMIPS_0_10;
	codes[0x11] = DasmMIPS_0_11;
	codes[0x12] = DasmMIPS_0_12;
	codes[0x13] = DasmMIPS_0_13;
	codes[0x14] = DasmMIPS_0_14;
	codes[0x15] = DasmMIPS_0_15;
	codes[0x16] = DasmMIPS_0_16;
	codes[0x17] = DasmMIPS_0_17;
	codes[0x18] = DasmMIPS_0_18;
	codes[0x19] = DasmMIPS_0_19;
	codes[0x1A] = DasmMIPS_0_1A;
	codes[0x1B] = DasmMIPS_0_1B;
	codes[0x1C] = DasmMIPS_0_1C;
	codes[0x1D] = DasmMIPS_0_1D;
	codes[0x1E] = DasmMIPS_0_1E;
	codes[0x1F] = DasmMIPS_0_1F;
	codes[0x20] = DasmMIPS_0_20;
	codes[0x21] = DasmMIPS_0_21;
	codes[0x22] = DasmMIPS_0_22;
	codes[0x23] = DasmMIPS_0_23;
	codes[0x24] = DasmMIPS_0_24;
	codes[0x25] = DasmMIPS_0_25;
	codes[0x26] = DasmMIPS_0_26;
	codes[0x27] = DasmMIPS_0_27;
	codes[0x28] = DasmMIPS_0_28;
	codes[0x29] = DasmMIPS_0_29;
	codes[0x2A] = DasmMIPS_0_2A;
	codes[0x2B] = DasmMIPS_0_2B;
	codes[0x2C] = DasmMIPS_0_2C;
	codes[0x2D] = DasmMIPS_0_2D;
	codes[0x2E] = DasmMIPS_0_2E;
	codes[0x2F] = DasmMIPS_0_2F;
	codes[0x30] = DasmMIPS_0_30;
	codes[0x31] = DasmMIPS_0_31;
	codes[0x32] = DasmMIPS_0_32;
	codes[0x33] = DasmMIPS_0_33;
	codes[0x34] = DasmMIPS_0_34;
	codes[0x35] = DasmMIPS_0_35;
	codes[0x36] = DasmMIPS_0_36;
	codes[0x37] = DasmMIPS_0_37;
	codes[0x38] = DasmMIPS_0_38;
	codes[0x39] = DasmMIPS_0_39;
	codes[0x3A] = DasmMIPS_0_3A;
	codes[0x3B] = DasmMIPS_0_3B;
	codes[0x3C] = DasmMIPS_0_3C;
	codes[0x3D] = DasmMIPS_0_3D;
	codes[0x3E] = DasmMIPS_0_3E;
	codes[0x3F] = DasmMIPS_0_3F;
}

Manage::DasmMIPS::~DasmMIPS()
{
	MemFree(this->codes);
	MemFree(this->codes_0);
}

Text::CStringNN Manage::DasmMIPS::GetHeader(Bool fullRegs) const
{
	if (fullRegs)
	{
		return CSTR(" $SP      $FP      $PC      R0       R1       R2       R3       R4       R5       R6       R7       R8       R9       R10      FP       IP       Code");
	}
	else
	{
		return CSTR(" $SP      $FP      $PC      Code");
	}
}

Bool Manage::DasmMIPS::Disasm32(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs)
{
	UTF8Char sbuff[512];
	UInt8 buff[16];
	DasmMIPS_Sess sess;
	Text::StringBuilderUTF8 outStr;
	IntOS initJmpCnt = jmpAddrs->GetCount();
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	MemCopyNO(&sess.regs, regs.Ptr(), sizeof(DasmMIPS_Regs));
	sess.regs.pc = *currInst;
	sess.regs.sp = *currStack;
	sess.regs.fp = *currFrame;
	sess.code = buff;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmMIPS::ET_NOT_END;
	sess.thisStatus = 0;
	sess.codeHdlrs = (void**)this->codes;
	sess.code_0Hdlrs = (void**)this->codes_0;
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	*blockStart = (Int32)sess.regs.pc;

	while (true)
	{
		Bool ret;

		outStr.ClearStr();
		outStr.AppendHex32((UInt32)sess.regs.sp);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32((UInt32)sess.regs.fp);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32((UInt32)sess.regs.pc);
		outStr.AppendC(UTF8STRC(" "));
		if (fullRegs)
		{
			IntOS i;
			i = 0;
			while (i < 29)
			{
				outStr.AppendHex32((UInt32)sess.regs.regs[i]);
				outStr.AppendC(UTF8STRC(" "));
				i++;
			}
			outStr.AppendHex32((UInt32)sess.regs.ra);
			outStr.AppendC(UTF8STRC(" "));
		}
		sess.sbuff = sbuff;
		if (sess.memReader->ReadMemory(sess.regs.pc, buff, 4) != 4)
		{
			ret = false;
		}
		else
		{
			ret = this->codes[buff[0] >> 2](sess);
		}
		if (!ret)
		{
			IntOS buffSize;
			outStr.AppendC(UTF8STRC("Unknown opcode "));
			buffSize = sess.memReader->ReadMemory(sess.regs.pc, buff, 16);
			if (buffSize > 0)
			{
				outStr.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
			}
			outStr.AppendC(UTF8STRC("\r\n"));
			writer->Write(outStr.ToCString());
			return false;
		}
		outStr.AppendSlow(sbuff);
		writer->Write(outStr.ToCString());
		if (sess.endType == Manage::DasmMIPS::ET_JMP && (UInt32)sess.retAddr >= *blockStart && (UInt32)sess.retAddr <= sess.regs.pc)
		{
			IntOS i;
			UInt32 minAddr = 0xffffffff;
			UInt32 jmpAddr;
			i = jmpAddrs->GetCount();
			while (i-- > initJmpCnt)
			{
				jmpAddr = jmpAddrs->GetItem(i);
				if (jmpAddr >= sess.regs.pc && jmpAddr < minAddr)
				{
					minAddr = jmpAddr;
				}
			}
			if (minAddr - sess.regs.pc > 0x1000)
			{
				*currInst = sess.retAddr;
				*currStack = (Int32)sess.regs.sp;
				*currFrame = (Int32)sess.regs.fp;
				*blockEnd = (Int32)sess.regs.pc;
				return false;
			}
			sess.regs.pc = minAddr;
			sess.endType = Manage::DasmMIPS::ET_NOT_END;
		}
		else if (sess.endType != Manage::DasmMIPS::ET_NOT_END)
		{
			*currInst = sess.retAddr;
			*currStack = (Int32)sess.regs.sp;
			*currFrame = (Int32)sess.regs.fp;
			*blockEnd = (Int32)sess.regs.pc;
			return sess.endType != Manage::DasmMIPS::ET_EXIT;
		}
//		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = 0;
	}
}

NN<Manage::Dasm::Dasm_Regs> Manage::DasmMIPS::CreateRegs() const
{
	return MemAllocNN(Manage::DasmMIPS::DasmMIPS_Regs);
}

void Manage::DasmMIPS::FreeRegs(NN<Dasm_Regs> regs) const
{
	MemFreeNN(regs);
}

NN<Manage::DasmMIPS::DasmMIPS_Sess> Manage::DasmMIPS::CreateSess(NN<Manage::DasmMIPS::DasmMIPS_Regs> regs, UInt8 *code, UInt16 codeSegm)
{
	NN<Manage::DasmMIPS::DasmMIPS_Sess> sess = MemAllocNN(Manage::DasmMIPS::DasmMIPS_Sess);
	sess->code = code;
	sess->codeSegm = codeSegm;
	sess->codeHdlrs = (void**)this->codes;
	//sess->code0fHdlrs = (void**)this->codes0f;
	NEW_CLASS(sess->callAddrs, Data::ArrayListUInt32());
	NEW_CLASS(sess->jmpAddrs, Data::ArrayListUInt32());
	MemCopyNO(&sess->regs, regs.Ptr(), sizeof(Manage::DasmMIPS::DasmMIPS_Regs));
	return sess;
}

void Manage::DasmMIPS::DeleteSess(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess)
{
	DEL_CLASS(sess->callAddrs);
	DEL_CLASS(sess->jmpAddrs);
	MemFreeNN(sess);
}

Bool Manage::DasmMIPS::DasmNext(NN<Manage::DasmMIPS::DasmMIPS_Sess> sess, UTF8Char *buff, IntOS *outBuffSize)
{
/*	*buff = 0;
	if (outBuffSize)
	{
		*outBuffSize = 0;
	}
	sess->outSPtr = buff;
	sess->thisStatus = 0;
	sess->endStatus = 0;
	if (sess->regs.CS != sess->codeSegm)
	{
		sess->code = &sess->code[(((Int32)sess->regs.CS) - ((Int32)sess->codeSegm)) << 4];
		sess->codeSegm = sess->regs.CS;
//		return false;
	}
	Bool isSucc = this->codes[sess->code[sess->regs.IP]](sess);
	*outBuffSize = sess->outSPtr - buff;
	return isSucc;*/
	return false;
} //True = succ
