#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Manage/AddressResolver.h"
#include "Manage/DasmARM64.h"
#include "Manage/ThreadContextARM64.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#if defined(_MSC_VER)
#pragma warning( disable : 4311 4312)
#endif

/*
Cond table
0000	EQ
0001	NE
0010	CS
0011	CC
0100	MI
0101	PL
0110	VS
0111	VC
1000	HI
1001	LS
1010	GE
1011	LT
1100	GT
1101	LE
1110	-
*/

Bool __stdcall DasmARM64_IsEndFunc(const UTF8Char *funcName, UOSInt nameLen)
{
	if (Text::StrIndexOfC(funcName, nameLen, UTF8STRC("(exit+0)")) != INVALID_INDEX)
	{
		return true;
	}
	return false;
}

UTF8Char *DasmARM64_ParseReg64(Manage::DasmARM64::DasmARM64_Sess* sess, UTF8Char *regName, Int32 regNo, UInt64 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = &sess->regs.X0;
		return Text::StrConcatC(regName, UTF8STRC("x0"));
	case 1:
		*regPtr = &sess->regs.X1;
		return Text::StrConcatC(regName, UTF8STRC("x1"));
	case 2:
		*regPtr = &sess->regs.X2;
		return Text::StrConcatC(regName, UTF8STRC("x2"));
	case 3:
		*regPtr = &sess->regs.X3;
		return Text::StrConcatC(regName, UTF8STRC("x3"));
	case 4:
		*regPtr = &sess->regs.X4;
		return Text::StrConcatC(regName, UTF8STRC("x4"));
	case 5:
		*regPtr = &sess->regs.X5;
		return Text::StrConcatC(regName, UTF8STRC("x5"));
	case 6:
		*regPtr = &sess->regs.X6;
		return Text::StrConcatC(regName, UTF8STRC("x6"));
	case 7:
		*regPtr = &sess->regs.X7;
		return Text::StrConcatC(regName, UTF8STRC("x7"));
	case 8:
		*regPtr = &sess->regs.X8;
		return Text::StrConcatC(regName, UTF8STRC("x8"));
	case 9:
		*regPtr = &sess->regs.X9;
		return Text::StrConcatC(regName, UTF8STRC("x9"));
	case 10:
		*regPtr = &sess->regs.X10;
		return Text::StrConcatC(regName, UTF8STRC("x10"));
	case 11:
		*regPtr = &sess->regs.X11;
		return Text::StrConcatC(regName, UTF8STRC("x11"));
	case 12:
		*regPtr = &sess->regs.X12;
		return Text::StrConcatC(regName, UTF8STRC("x12"));
	case 13:
		*regPtr = &sess->regs.X13;
		return Text::StrConcatC(regName, UTF8STRC("x13"));
	case 14:
		*regPtr = &sess->regs.X14;
		return Text::StrConcatC(regName, UTF8STRC("x14"));
	case 15:
		*regPtr = &sess->regs.X15;
		return Text::StrConcatC(regName, UTF8STRC("x15"));
	default:
		*regName = 0;
		return regName;
	}
}

UInt32 DasmARM64_ParseImmA32(UInt8 rot, UInt8 val)
{
	if (rot == 0)
		return val;
	rot = rot << 1;
	if (rot < 8)
	{
		return (val >> rot) | (((UInt32)val) << (32 - rot));
	}
	else
	{
		return  ((UInt32)val) << (32 - rot);;
	}
}

UInt64 DasmARM64_ParseImmShift(Manage::DasmARM64::DasmARM64_Sess *sess, UInt64 oriVal)
{
	Int32 t = sess->codeBuff[0] & 0x60 >> 5;
	Int32 v = ((sess->codeBuff[1] & 0xf) << 1) | ((sess->codeBuff[0] & 0x80) >> 7);
	if (t == 0)
	{
		if (v == 0)
		{
			return oriVal;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSL #"));
			sess->sbuff = Text::StrInt32(sess->sbuff, v);
			return oriVal << v;
		}
	}
	else if (t ==1)
	{
		if (v == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSR #32"));
			return 0;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSR #"));
			sess->sbuff = Text::StrInt32(sess->sbuff, v);
			return oriVal >> v;
		}
	}
	else if (t == 2)
	{
		if (v == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #32"));
			if (((Int32)oriVal) < 0)
				return (UInt64)-1;
			else
				return 0;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #"));
			sess->sbuff = Text::StrInt32(sess->sbuff, v);
			return (UInt64)(((Int32)oriVal) >> v);
		}
	}
	else if (t == 3)
	{
		if (v == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", RRX"));
			return 0;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ROR #"));
			sess->sbuff = Text::StrInt32(sess->sbuff, v);
			return (oriVal >> v) | (oriVal << (32 - v));
		}
	}
	return oriVal;
}

Bool __stdcall DasmARM64_00(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_01(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_02(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_03(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_04(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_05(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_06(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_07(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_08(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_09(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_0A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("beq 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_0B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bleq 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_0C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_0D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_0E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_0F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_10(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_11(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_12(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_13(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_14(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_15(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_16(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_17(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_18(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_19(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_1A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bne 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_1B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blne 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_1C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_1D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_1E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_1F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_20(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_21(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_22(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_23(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_24(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_25(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_26(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_27(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_28(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_29(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_2A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bcs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_2B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blcs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_2C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_2D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_2E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_2F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_30(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_31(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_32(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_33(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_34(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_35(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_36(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_37(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_38(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_39(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_3A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bcc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_3B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blcc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_3C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_3D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_3E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_3F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_40(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_41(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_42(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_43(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_44(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_45(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_46(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_47(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_48(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_49(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_4A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bmi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_4B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blmi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_4C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_4D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_4E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_4F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_50(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_51(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_52(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_53(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_54(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_55(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_56(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_57(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_58(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_59(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_5A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bpl 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_5B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blpl 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_5C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_5D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_5E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_5F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_60(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_61(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_62(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_63(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_64(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_65(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_66(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_67(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_68(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_69(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_6A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bvs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_6B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blvs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_6C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_6D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_6E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_6F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_70(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_71(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_72(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_73(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_74(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_75(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_76(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_77(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_78(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_79(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_7A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bvc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_7B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blvc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_7C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_7D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_7E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_7F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_80(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_81(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_82(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrs;
	UInt64 *regPtrd;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andhi "));
		return false;
	case 0x20:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("eorhi "));
		return false;
	case 0x40:
		if (sess->codeBuff[2] == 0x2f)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adrhi "));
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subhi "));
		}
		return false;
	case 0x60:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsbhi "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x70:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsbshi "));
		return false;
	case 0x80:
		if (sess->codeBuff[2] == 0x2f)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adrhi "));
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addhi "));
		}
		return false;
	case 0xa0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adchi "));
		return false;
	case 0xc0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbchi "));
		return false;
	case 0xe0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rschi "));
		return false;
	}
	return false;
}

Bool __stdcall DasmARM64_83(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrd;
	Int32 val;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0xe0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mvnhi "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		val = (Int32)DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrInt32(sess->sbuff, val);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
/*		*regPtrd = val;
		if (regPtrd == 15)
		{
			sess->endType = Manage::DasmARM64::ET_JMP;
			sess->retAddr = val;
		}*/
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_84(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_85(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrs;
	UInt64 *regPtrd;
	Int32 addr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrhi "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		addr = ((sess->codeBuff[1] & 0xf) << 8) | (sess->codeBuff[0]);
		if (addr != 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #+"));
			sess->sbuff = Text::StrInt32(sess->sbuff, addr);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
/*		sess->memReader->ReadMemory(addr + *regPtrs, (UInt8*)regPtrd, 4);
		if (regPtrd == 15)
		{
			sess->endType = Manage::DasmARM64::ET_JMP;
			sess->retAddr = *regPtrd;
		}*/
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_86(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_87(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrs;
	UInt64 *regPtrs2;
	UInt64 *regPtrd;
	Int64 addr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x80:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("strhi "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[2] & 0xf), &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", +"));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[0] & 0xf), &regPtrs2);
		addr = (Int64)DasmARM64_ParseImmShift(sess, *regPtrs2);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
		return true;
	case 0x90:
		if (sess->codeBuff[0] & 0x10)
		{

		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrhi "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[2] & 0xf), &regPtrs);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", +"));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[0] & 0xf), &regPtrs2);
			addr = (Int64)DasmARM64_ParseImmShift(sess, *regPtrs2);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
	/*		*regPtrd = val;
			if (regPtrd == 15)
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
				sess->retAddr = val;
			}*/
			return true;
		}
		break;
	}
	return false;
}

Bool __stdcall DasmARM64_88(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_89(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_8A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bhi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_8B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blhi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_8C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_8D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_8E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_8F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_90(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_91(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_92(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_93(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_94(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_95(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_96(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_97(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_98(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_99(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_9A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bls 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_9B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blls 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_9C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_9D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_9E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_9F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_A9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_AA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bge 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_AB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blge 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_AC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_AD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_AE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_AF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_B9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_BA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_BB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bllt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_BC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_BD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_BE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_BF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_C9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_CA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bgt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_CB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blgt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_CC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_CD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_CE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_CF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_DA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ble 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_DB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blle 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_DC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_DD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_DE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_DF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrd;
	UInt64 *regPtrs1;
	UInt64 *regPtrs2;
	Int64 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x80:
		if (sess->codeBuff[0] & 0x10)
		{
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs1);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs2);
			imm = (Int64)DasmARM64_ParseImmShift(sess, *regPtrs2);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			*regPtrd = *regPtrs1 + (UInt64)imm;
			if ((sess->codeBuff[1] & 0xf0) == 0xf0)
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
		break;
	case 0xC0:
		if ((sess->codeBuff[0] & 0xF0) == 0xB0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("strh "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs1);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("], #"));
			imm = ((sess->codeBuff[0] & 0xf) | ((sess->codeBuff[1] & 0xf) << 4));
			sess->sbuff = Text::StrInt64(sess->sbuff, imm);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			*regPtrs1 += (UInt64)imm;
			if ((sess->codeBuff[1] & 0xf0) == 0xf0)
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
		break;
	}
	return false;
}

Bool __stdcall DasmARM64_E1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrd;
	UInt64 *regPtrs;
	UInt64 *regPtrs2;
	Int64 imm;
	UInt8 t;
	UTF8Char *sptr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x20:
		if ((sess->codeBuff[0] & 0x80) == 0)
		{
			if ((sess->codeBuff[0] & 0x70) == 0x30)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blx "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[0] & 0xf), &regPtrs);

				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" (0x"));
				sess->sbuff = Text::StrHexVal64(sess->sbuff, *regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
				if (sess->addrResol)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
					sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, *regPtrs);
					if (sess->sbuff == 0)
					{
						sess->sbuff = sptr;
					}
					if (DasmARM64_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
					{
						sess->endType = Manage::DasmARM64::ET_EXIT;
						sess->retAddr = *regPtrs;
					}
				}
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
		break;
	case 0x50:
		if ((sess->codeBuff[0] & 0x90) == 0x10)
		{
//05 00 50 E1
		}
		else
		{
			imm = ((sess->codeBuff[1] & 0xf) << 1) | (sess->codeBuff[0] >> 7);
			t = (sess->codeBuff[0] & 0x60) >> 5;
			if (t == 0)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSL #"));
					sess->sbuff = Text::StrInt64(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else if (t == 1)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSR #32"));
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSR #"));
					sess->sbuff = Text::StrInt64(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else if (t == 2)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #32"));
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #"));
					sess->sbuff = Text::StrInt64(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else if (t == 3)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", RRX"));
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ROR #"));
					sess->sbuff = Text::StrInt64(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
		}
		break;
	case 0x90:
		if ((sess->codeBuff[0] & 0xF0) == 0xB0)
		{
			if ((sess->codeBuff[2] & 0xF0) == 0x90)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrh "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs2);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
//				*regPtrd = sess->memReader->ReadMemUInt16(*regPtrs + *regPtrs2);
				return true;
			}
		}
		break;
	case 0xa0:
		if (sess->codeBuff[0] & 0x10)
		{
		}
		else
		{
			imm = ((sess->codeBuff[1] & 0xf) << 1) | (sess->codeBuff[0] >> 7);
			t = (sess->codeBuff[0] & 0x60) >> 5;
			if (t == 0)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt64(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs << imm;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
			}
			else if (t == 1)
			{
				if (imm == 0)
				{
					imm = 32;
				}
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsr "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt64(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = *regPtrs >> imm;
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM64::ET_FUNC_RET;
					sess->retAddr = sess->regs.PC;
				}
				return true;
			}
			else if (t == 2)
			{
				if (imm == 0)
				{
					imm = 32;
				}
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("asr "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt64(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = (UInt64)(Int64)(((Int32)*regPtrs) >> imm);
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM64::ET_FUNC_RET;
					sess->retAddr = sess->regs.PC;
				}
				return true;
			}
			else if (t == 3)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rrx "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs >> 1;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt64(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = (*regPtrs >> imm) | (*regPtrs << (32 - imm));
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
			}
		}
		break;
	}
	return false;
}

Bool __stdcall DasmARM64_E2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrd;
	UInt64 *regPtrs;
	Int32 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x40:
		imm = DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs - imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0x50:
		imm = DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subs "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs - imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0x80:
		imm = DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs + imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
/*	case 0xa0:
		if (sess->codeBuff[0] & 0x10)
		{
		}
		else
		{
			imm = ((sess->codeBuff[1] & 0xf) << 1) | (sess->codeBuff[0] >> 7);
			t = (sess->codeBuff[0] & 0x60) >> 5;
			if (t == 0)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs << imm;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
			}
			else if (t == 1)
			{
				if (imm == 0)
				{
					imm = 32;
				}
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsr "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt32(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = *regPtrs >> imm;
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM64::ET_FUNC_RET;
					sess->retAddr = sess->regs.PC;
				}
				return true;
			}
			else if (t == 2)
			{
				if (imm == 0)
				{
					imm = 32;
				}
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("asr "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt32(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = ((Int32)*regPtrs) >> imm;
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM64::ET_FUNC_RET;
					sess->retAddr = sess->regs.PC;
				}
				return true;
			}
			else if (t == 3)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rrx "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs >> 1;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = (*regPtrs >> imm) | (*regPtrs << (32 - imm));
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM64::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
			}
		}
		break;*/
	case 0xb0:
		imm = DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adcs "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs + imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_E3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtr;
	Int32 val;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		return false;
	case 0x10:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("tst "));
		return false;
	case 0x20:
		if (sess->codeBuff[2] == 0x20)
		{
			switch (sess->codeBuff[0])
			{
			case 0:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("nop"));
				return true;
			case 1:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("yeild"));
				return true;
			case 2:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("wfe"));
				return true;
			case 3:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("wfi"));
				return true;
			case 4:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sev"));
				return true;
			case 5:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sevl"));
				return true;
			default:
				if ((sess->codeBuff[0] & 0xf0) == 0xf0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dbg #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, sess->codeBuff[0] & 0xf);
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("msr "));
		}
		return false;
	case 0x30:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("teq "));
		return false;
	case 0x40:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movt "));
		return false;
	case 0x50:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x60:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("msr "));
		return false;
	case 0x70:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmn "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x80:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("orr "));
		return false;
	case 0x90:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("orr "));
		return false;
	case 0xa0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		val = DasmARM64_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrInt32(sess->sbuff, val);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtr = val;
		if ((sess->codeBuff[2] & 0xf) == 15)
		{
			sess->endType = Manage::DasmARM64::ET_JMP;
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0xb0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		return false;
	case 0xc0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bic "));
		return false;
	case 0xd0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bic "));
		return false;
	case 0xe0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mvn "));
		return false;
	case 0xf0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mvn "));
		return false;
	}
	return false;
}

Bool __stdcall DasmARM64_E4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrd;
	UInt64 *regPtrs;
	Int32 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		imm = ((sess->codeBuff[1] & 0xf) << 8) | sess->codeBuff[0];
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldr "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("], #+"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->memReader->ReadMemory(*regPtrs, (UInt8*)regPtrd, 4);
		*regPtrs += imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14 || (sess->codeBuff[2] & 0xf) == 13)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_E5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtrd;
	UInt64 *regPtrs;
	Int32 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		imm = ((sess->codeBuff[1] & 0xf) << 8) | sess->codeBuff[0];
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldr "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		if (imm != 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #+"));
			sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
		sess->memReader->ReadMemory((UInt64)(Int64)imm + *regPtrs, (UInt8*)regPtrd, 4);
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14 || (sess->codeBuff[2] & 0xf) == 13)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM64::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0xC0:
		imm = ((sess->codeBuff[1] & 0xf) << 8) | sess->codeBuff[0];
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("strb "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		if (true)//imm != 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #+"));
			sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_E6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt64 *regPtr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		{
			Bool found = false;
			UInt8 v;
			UInt8 regNo;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrfm "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", {"));
			v = 1;
			regNo = 0;
			while (v)
			{
				if (sess->codeBuff[0] & v)
				{
					if (found)
					{
						sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					}
					else
					{
						found = true;
					}
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, regNo, &regPtr);
					sess->memReader->ReadMemory(sess->regs.SP, (UInt8*)regPtr, 4);
					sess->regs.SP += 4;
				}
				v = v << 1;
				regNo++;
			}
			v = 1;
			while (v)
			{
				if (sess->codeBuff[1] & v)
				{
					if (found)
					{
						sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					}
					else
					{
						found = true;
					}
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, regNo, &regPtr);
					sess->memReader->ReadMemory(sess->regs.SP, (UInt8*)regPtr, 4);
					sess->regs.SP += 4;
				}
				v = v << 1;
				regNo++;
			}
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("}\r\n"));
			if (sess->codeBuff[1] & 0x80)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
	case 0xb0:
		if (sess->codeBuff[2] == 0xbd)
		{
			Bool found = false;
			UInt8 v;
			UInt8 regNo;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
			v = 1;
			regNo = 0;
			while (v)
			{
				if (sess->codeBuff[0] & v)
				{
					if (found)
					{
						sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					}
					else
					{
						found = true;
					}
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, regNo, &regPtr);
					sess->memReader->ReadMemory(sess->regs.SP, (UInt8*)regPtr, 4);
					sess->regs.SP += 4;
				}
				v = v << 1;
				regNo++;
			}
			v = 1;
			while (v)
			{
				if (sess->codeBuff[1] & v)
				{
					if (found)
					{
						sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					}
					else
					{
						found = true;
					}
					sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, regNo, &regPtr);
					sess->memReader->ReadMemory(sess->regs.SP, (UInt8*)regPtr, 4);
					sess->regs.SP += 4;
				}
				v = v << 1;
				regNo++;
			}
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			if (sess->codeBuff[1] & 0x80)
			{
				sess->endType = Manage::DasmARM64::ET_FUNC_RET;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
		break;
	}
	return false;
}

Bool __stdcall DasmARM64_E9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_EA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("b 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));

	sess->regs.PC = (UInt64)((Int64)sess->regs.PC + addr + 4);
	sess->jmpAddrs->Add(sess->regs.PC);
	sess->endType = Manage::DasmARM64::ET_JMP;
	sess->retAddr = sess->regs.PC;
	return true;
}

Bool __stdcall DasmARM64_EB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	Int32 addr;
	UTF8Char *sptr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bl 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, (UInt64)((Int64)sess->regs.PC + addr + 4));
		if (sess->sbuff == 0)
		{
			sess->sbuff = sptr;
		}
		if (DasmARM64_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
		{
			sess->endType = Manage::DasmARM64::ET_EXIT;
			sess->retAddr = (UInt64)((Int64)sess->regs.PC + addr + 4);
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add((UInt64)((Int64)sess->regs.PC + addr + 4));
	return true;
}

Bool __stdcall DasmARM64_EC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_ED(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_EE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_EF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_F9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_FA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_FB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_FC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_FD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_FE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_FF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T00(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T01(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T02(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T03(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T04(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T05(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T06(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T07(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T08(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T09(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T0A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T0B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T0C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T0D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T0E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T0F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T10(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T11(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T12(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T13(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T14(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T15(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T16(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T17(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T18(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T19(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T1A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T1B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T1C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T1D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T1E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T1F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T20(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T21(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T22(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T23(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T24(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T25(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T26(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T27(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T28(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T29(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T2A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T2B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T2C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T2D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T2E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T2F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T30(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T31(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T32(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T33(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T34(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T35(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T36(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T37(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T38(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T39(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T3A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T3B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T3C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T3D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T3E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T3F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T40(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T41(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T42(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T43(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T44(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T45(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T46(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T47(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T48(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T49(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T4A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T4B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T4C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T4D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T4E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T4F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T50(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T51(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T52(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T53(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T54(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T55(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T56(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T57(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T58(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T59(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T5A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T5B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T5C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T5D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T5E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T5F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T60(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T61(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T62(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T63(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T64(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T65(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T66(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T67(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T68(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T69(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T6A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T6B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T6C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T6D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T6E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T6F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T70(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T71(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T72(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T73(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T74(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T75(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T76(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T77(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T78(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T79(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T7A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T7B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T7C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T7D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T7E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T7F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T80(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T81(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T82(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T83(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T84(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T85(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T86(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T87(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T88(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T89(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T8A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T8B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T8C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T8D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T8E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T8F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T90(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T91(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T92(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T93(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T94(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T95(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T96(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T97(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T98(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T99(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T9A(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T9B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T9C(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T9D(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T9E(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_T9F(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TA9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TAA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TAB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TAC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TAD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TAE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TAF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TB9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TBA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TBB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TBC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TBD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TBE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TBF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TC9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TCA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TCB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TCC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TCD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TCE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TCF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TD9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TDA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TDB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TDC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TDD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TDE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TDF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TE9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TEA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TEB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TEC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TED(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TEE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TEF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF0(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF7(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF8(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TF9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TFA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TFB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TFC(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TFD(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TFE(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_TFF(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Manage::DasmARM64::DasmARM64()
{
	DasmARM64_Code *codes;
	codes = this->codes = MemAlloc(DasmARM64_Code, 256);
	codes[0x00] = DasmARM64_00;
	codes[0x01] = DasmARM64_01;
	codes[0x02] = DasmARM64_02;
	codes[0x03] = DasmARM64_03;
	codes[0x04] = DasmARM64_04;
	codes[0x05] = DasmARM64_05;
	codes[0x06] = DasmARM64_06;
	codes[0x07] = DasmARM64_07;
	codes[0x08] = DasmARM64_08;
	codes[0x09] = DasmARM64_09;
	codes[0x0A] = DasmARM64_0A;
	codes[0x0B] = DasmARM64_0B;
	codes[0x0C] = DasmARM64_0C;
	codes[0x0D] = DasmARM64_0D;
	codes[0x0E] = DasmARM64_0E;
	codes[0x0F] = DasmARM64_0F;
	codes[0x10] = DasmARM64_10;
	codes[0x11] = DasmARM64_11;
	codes[0x12] = DasmARM64_12;
	codes[0x13] = DasmARM64_13;
	codes[0x14] = DasmARM64_14;
	codes[0x15] = DasmARM64_15;
	codes[0x16] = DasmARM64_16;
	codes[0x17] = DasmARM64_17;
	codes[0x18] = DasmARM64_18;
	codes[0x19] = DasmARM64_19;
	codes[0x1A] = DasmARM64_1A;
	codes[0x1B] = DasmARM64_1B;
	codes[0x1C] = DasmARM64_1C;
	codes[0x1D] = DasmARM64_1D;
	codes[0x1E] = DasmARM64_1E;
	codes[0x1F] = DasmARM64_1F;
	codes[0x20] = DasmARM64_20;
	codes[0x21] = DasmARM64_21;
	codes[0x22] = DasmARM64_22;
	codes[0x23] = DasmARM64_23;
	codes[0x24] = DasmARM64_24;
	codes[0x25] = DasmARM64_25;
	codes[0x26] = DasmARM64_26;
	codes[0x27] = DasmARM64_27;
	codes[0x28] = DasmARM64_28;
	codes[0x29] = DasmARM64_29;
	codes[0x2A] = DasmARM64_2A;
	codes[0x2B] = DasmARM64_2B;
	codes[0x2C] = DasmARM64_2C;
	codes[0x2D] = DasmARM64_2D;
	codes[0x2E] = DasmARM64_2E;
	codes[0x2F] = DasmARM64_2F;
	codes[0x30] = DasmARM64_30;
	codes[0x31] = DasmARM64_31;
	codes[0x32] = DasmARM64_32;
	codes[0x33] = DasmARM64_33;
	codes[0x34] = DasmARM64_34;
	codes[0x35] = DasmARM64_35;
	codes[0x36] = DasmARM64_36;
	codes[0x37] = DasmARM64_37;
	codes[0x38] = DasmARM64_38;
	codes[0x39] = DasmARM64_39;
	codes[0x3A] = DasmARM64_3A;
	codes[0x3B] = DasmARM64_3B;
	codes[0x3C] = DasmARM64_3C;
	codes[0x3D] = DasmARM64_3D;
	codes[0x3E] = DasmARM64_3E;
	codes[0x3F] = DasmARM64_3F;
	codes[0x40] = DasmARM64_40;
	codes[0x41] = DasmARM64_41;
	codes[0x42] = DasmARM64_42;
	codes[0x43] = DasmARM64_43;
	codes[0x44] = DasmARM64_44;
	codes[0x45] = DasmARM64_45;
	codes[0x46] = DasmARM64_46;
	codes[0x47] = DasmARM64_47;
	codes[0x48] = DasmARM64_48;
	codes[0x49] = DasmARM64_49;
	codes[0x4A] = DasmARM64_4A;
	codes[0x4B] = DasmARM64_4B;
	codes[0x4C] = DasmARM64_4C;
	codes[0x4D] = DasmARM64_4D;
	codes[0x4E] = DasmARM64_4E;
	codes[0x4F] = DasmARM64_4F;
	codes[0x50] = DasmARM64_50;
	codes[0x51] = DasmARM64_51;
	codes[0x52] = DasmARM64_52;
	codes[0x53] = DasmARM64_53;
	codes[0x54] = DasmARM64_54;
	codes[0x55] = DasmARM64_55;
	codes[0x56] = DasmARM64_56;
	codes[0x57] = DasmARM64_57;
	codes[0x58] = DasmARM64_58;
	codes[0x59] = DasmARM64_59;
	codes[0x5A] = DasmARM64_5A;
	codes[0x5B] = DasmARM64_5B;
	codes[0x5C] = DasmARM64_5C;
	codes[0x5D] = DasmARM64_5D;
	codes[0x5E] = DasmARM64_5E;
	codes[0x5F] = DasmARM64_5F;
	codes[0x60] = DasmARM64_60;
	codes[0x61] = DasmARM64_61;
	codes[0x62] = DasmARM64_62;
	codes[0x63] = DasmARM64_63;
	codes[0x64] = DasmARM64_64;
	codes[0x65] = DasmARM64_65;
	codes[0x66] = DasmARM64_66;
	codes[0x67] = DasmARM64_67;
	codes[0x68] = DasmARM64_68;
	codes[0x69] = DasmARM64_69;
	codes[0x6A] = DasmARM64_6A;
	codes[0x6B] = DasmARM64_6B;
	codes[0x6C] = DasmARM64_6C;
	codes[0x6D] = DasmARM64_6D;
	codes[0x6E] = DasmARM64_6E;
	codes[0x6F] = DasmARM64_6F;
	codes[0x70] = DasmARM64_70;
	codes[0x71] = DasmARM64_71;
	codes[0x72] = DasmARM64_72;
	codes[0x73] = DasmARM64_73;
	codes[0x74] = DasmARM64_74;
	codes[0x75] = DasmARM64_75;
	codes[0x76] = DasmARM64_76;
	codes[0x77] = DasmARM64_77;
	codes[0x78] = DasmARM64_78;
	codes[0x79] = DasmARM64_79;
	codes[0x7A] = DasmARM64_7A;
	codes[0x7B] = DasmARM64_7B;
	codes[0x7C] = DasmARM64_7C;
	codes[0x7D] = DasmARM64_7D;
	codes[0x7E] = DasmARM64_7E;
	codes[0x7F] = DasmARM64_7F;
	codes[0x80] = DasmARM64_80;
	codes[0x81] = DasmARM64_81;
	codes[0x82] = DasmARM64_82;
	codes[0x83] = DasmARM64_83;
	codes[0x84] = DasmARM64_84;
	codes[0x85] = DasmARM64_85;
	codes[0x86] = DasmARM64_86;
	codes[0x87] = DasmARM64_87;
	codes[0x88] = DasmARM64_88;
	codes[0x89] = DasmARM64_89;
	codes[0x8A] = DasmARM64_8A;
	codes[0x8B] = DasmARM64_8B;
	codes[0x8C] = DasmARM64_8C;
	codes[0x8D] = DasmARM64_8D;
	codes[0x8E] = DasmARM64_8E;
	codes[0x8F] = DasmARM64_8F;
	codes[0x90] = DasmARM64_90;
	codes[0x91] = DasmARM64_91;
	codes[0x92] = DasmARM64_92;
	codes[0x93] = DasmARM64_93;
	codes[0x94] = DasmARM64_94;
	codes[0x95] = DasmARM64_95;
	codes[0x96] = DasmARM64_96;
	codes[0x97] = DasmARM64_97;
	codes[0x98] = DasmARM64_98;
	codes[0x99] = DasmARM64_99;
	codes[0x9A] = DasmARM64_9A;
	codes[0x9B] = DasmARM64_9B;
	codes[0x9C] = DasmARM64_9C;
	codes[0x9D] = DasmARM64_9D;
	codes[0x9E] = DasmARM64_9E;
	codes[0x9F] = DasmARM64_9F;
	codes[0xA0] = DasmARM64_A0;
	codes[0xA1] = DasmARM64_A1;
	codes[0xA2] = DasmARM64_A2;
	codes[0xA3] = DasmARM64_A3;
	codes[0xA4] = DasmARM64_A4;
	codes[0xA5] = DasmARM64_A5;
	codes[0xA6] = DasmARM64_A6;
	codes[0xA7] = DasmARM64_A7;
	codes[0xA8] = DasmARM64_A8;
	codes[0xA9] = DasmARM64_A9;
	codes[0xAA] = DasmARM64_AA;
	codes[0xAB] = DasmARM64_AB;
	codes[0xAC] = DasmARM64_AC;
	codes[0xAD] = DasmARM64_AD;
	codes[0xAE] = DasmARM64_AE;
	codes[0xAF] = DasmARM64_AF;
	codes[0xB0] = DasmARM64_B0;
	codes[0xB1] = DasmARM64_B1;
	codes[0xB2] = DasmARM64_B2;
	codes[0xB3] = DasmARM64_B3;
	codes[0xB4] = DasmARM64_B4;
	codes[0xB5] = DasmARM64_B5;
	codes[0xB6] = DasmARM64_B6;
	codes[0xB7] = DasmARM64_B7;
	codes[0xB8] = DasmARM64_B8;
	codes[0xB9] = DasmARM64_B9;
	codes[0xBA] = DasmARM64_BA;
	codes[0xBB] = DasmARM64_BB;
	codes[0xBC] = DasmARM64_BC;
	codes[0xBD] = DasmARM64_BD;
	codes[0xBE] = DasmARM64_BE;
	codes[0xBF] = DasmARM64_BF;
	codes[0xC0] = DasmARM64_C0;
	codes[0xC1] = DasmARM64_C1;
	codes[0xC2] = DasmARM64_C2;
	codes[0xC3] = DasmARM64_C3;
	codes[0xC4] = DasmARM64_C4;
	codes[0xC5] = DasmARM64_C5;
	codes[0xC6] = DasmARM64_C6;
	codes[0xC7] = DasmARM64_C7;
	codes[0xC8] = DasmARM64_C8;
	codes[0xC9] = DasmARM64_C9;
	codes[0xCA] = DasmARM64_CA;
	codes[0xCB] = DasmARM64_CB;
	codes[0xCC] = DasmARM64_CC;
	codes[0xCD] = DasmARM64_CD;
	codes[0xCE] = DasmARM64_CE;
	codes[0xCF] = DasmARM64_CF;
	codes[0xD0] = DasmARM64_D0;
	codes[0xD1] = DasmARM64_D1;
	codes[0xD2] = DasmARM64_D2;
	codes[0xD3] = DasmARM64_D3;
	codes[0xD4] = DasmARM64_D4;
	codes[0xD5] = DasmARM64_D5;
	codes[0xD6] = DasmARM64_D6;
	codes[0xD7] = DasmARM64_D7;
	codes[0xD8] = DasmARM64_D8;
	codes[0xD9] = DasmARM64_D9;
	codes[0xDA] = DasmARM64_DA;
	codes[0xDB] = DasmARM64_DB;
	codes[0xDC] = DasmARM64_DC;
	codes[0xDD] = DasmARM64_DD;
	codes[0xDE] = DasmARM64_DE;
	codes[0xDF] = DasmARM64_DF;
	codes[0xE0] = DasmARM64_E0;
	codes[0xE1] = DasmARM64_E1;
	codes[0xE2] = DasmARM64_E2;
	codes[0xE3] = DasmARM64_E3;
	codes[0xE4] = DasmARM64_E4;
	codes[0xE5] = DasmARM64_E5;
	codes[0xE6] = DasmARM64_E6;
	codes[0xE7] = DasmARM64_E7;
	codes[0xE8] = DasmARM64_E8;
	codes[0xE9] = DasmARM64_E9;
	codes[0xEA] = DasmARM64_EA;
	codes[0xEB] = DasmARM64_EB;
	codes[0xEC] = DasmARM64_EC;
	codes[0xED] = DasmARM64_ED;
	codes[0xEE] = DasmARM64_EE;
	codes[0xEF] = DasmARM64_EF;
	codes[0xF0] = DasmARM64_F0;
	codes[0xF1] = DasmARM64_F1;
	codes[0xF2] = DasmARM64_F2;
	codes[0xF3] = DasmARM64_F3;
	codes[0xF4] = DasmARM64_F4;
	codes[0xF5] = DasmARM64_F5;
	codes[0xF6] = DasmARM64_F6;
	codes[0xF7] = DasmARM64_F7;
	codes[0xF8] = DasmARM64_F8;
	codes[0xF9] = DasmARM64_F9;
	codes[0xFA] = DasmARM64_FA;
	codes[0xFB] = DasmARM64_FB;
	codes[0xFC] = DasmARM64_FC;
	codes[0xFD] = DasmARM64_FD;
	codes[0xFE] = DasmARM64_FE;
	codes[0xFF] = DasmARM64_FF;

	codes = this->codesT = MemAlloc(DasmARM64_Code, 256);
	codes[0x00] = DasmARM64_T00;
	codes[0x01] = DasmARM64_T01;
	codes[0x02] = DasmARM64_T02;
	codes[0x03] = DasmARM64_T03;
	codes[0x04] = DasmARM64_T04;
	codes[0x05] = DasmARM64_T05;
	codes[0x06] = DasmARM64_T06;
	codes[0x07] = DasmARM64_T07;
	codes[0x08] = DasmARM64_T08;
	codes[0x09] = DasmARM64_T09;
	codes[0x0A] = DasmARM64_T0A;
	codes[0x0B] = DasmARM64_T0B;
	codes[0x0C] = DasmARM64_T0C;
	codes[0x0D] = DasmARM64_T0D;
	codes[0x0E] = DasmARM64_T0E;
	codes[0x0F] = DasmARM64_T0F;
	codes[0x10] = DasmARM64_T10;
	codes[0x11] = DasmARM64_T11;
	codes[0x12] = DasmARM64_T12;
	codes[0x13] = DasmARM64_T13;
	codes[0x14] = DasmARM64_T14;
	codes[0x15] = DasmARM64_T15;
	codes[0x16] = DasmARM64_T16;
	codes[0x17] = DasmARM64_T17;
	codes[0x18] = DasmARM64_T18;
	codes[0x19] = DasmARM64_T19;
	codes[0x1A] = DasmARM64_T1A;
	codes[0x1B] = DasmARM64_T1B;
	codes[0x1C] = DasmARM64_T1C;
	codes[0x1D] = DasmARM64_T1D;
	codes[0x1E] = DasmARM64_T1E;
	codes[0x1F] = DasmARM64_T1F;
	codes[0x20] = DasmARM64_T20;
	codes[0x21] = DasmARM64_T21;
	codes[0x22] = DasmARM64_T22;
	codes[0x23] = DasmARM64_T23;
	codes[0x24] = DasmARM64_T24;
	codes[0x25] = DasmARM64_T25;
	codes[0x26] = DasmARM64_T26;
	codes[0x27] = DasmARM64_T27;
	codes[0x28] = DasmARM64_T28;
	codes[0x29] = DasmARM64_T29;
	codes[0x2A] = DasmARM64_T2A;
	codes[0x2B] = DasmARM64_T2B;
	codes[0x2C] = DasmARM64_T2C;
	codes[0x2D] = DasmARM64_T2D;
	codes[0x2E] = DasmARM64_T2E;
	codes[0x2F] = DasmARM64_T2F;
	codes[0x30] = DasmARM64_T30;
	codes[0x31] = DasmARM64_T31;
	codes[0x32] = DasmARM64_T32;
	codes[0x33] = DasmARM64_T33;
	codes[0x34] = DasmARM64_T34;
	codes[0x35] = DasmARM64_T35;
	codes[0x36] = DasmARM64_T36;
	codes[0x37] = DasmARM64_T37;
	codes[0x38] = DasmARM64_T38;
	codes[0x39] = DasmARM64_T39;
	codes[0x3A] = DasmARM64_T3A;
	codes[0x3B] = DasmARM64_T3B;
	codes[0x3C] = DasmARM64_T3C;
	codes[0x3D] = DasmARM64_T3D;
	codes[0x3E] = DasmARM64_T3E;
	codes[0x3F] = DasmARM64_T3F;
	codes[0x40] = DasmARM64_T40;
	codes[0x41] = DasmARM64_T41;
	codes[0x42] = DasmARM64_T42;
	codes[0x43] = DasmARM64_T43;
	codes[0x44] = DasmARM64_T44;
	codes[0x45] = DasmARM64_T45;
	codes[0x46] = DasmARM64_T46;
	codes[0x47] = DasmARM64_T47;
	codes[0x48] = DasmARM64_T48;
	codes[0x49] = DasmARM64_T49;
	codes[0x4A] = DasmARM64_T4A;
	codes[0x4B] = DasmARM64_T4B;
	codes[0x4C] = DasmARM64_T4C;
	codes[0x4D] = DasmARM64_T4D;
	codes[0x4E] = DasmARM64_T4E;
	codes[0x4F] = DasmARM64_T4F;
	codes[0x50] = DasmARM64_T50;
	codes[0x51] = DasmARM64_T51;
	codes[0x52] = DasmARM64_T52;
	codes[0x53] = DasmARM64_T53;
	codes[0x54] = DasmARM64_T54;
	codes[0x55] = DasmARM64_T55;
	codes[0x56] = DasmARM64_T56;
	codes[0x57] = DasmARM64_T57;
	codes[0x58] = DasmARM64_T58;
	codes[0x59] = DasmARM64_T59;
	codes[0x5A] = DasmARM64_T5A;
	codes[0x5B] = DasmARM64_T5B;
	codes[0x5C] = DasmARM64_T5C;
	codes[0x5D] = DasmARM64_T5D;
	codes[0x5E] = DasmARM64_T5E;
	codes[0x5F] = DasmARM64_T5F;
	codes[0x60] = DasmARM64_T60;
	codes[0x61] = DasmARM64_T61;
	codes[0x62] = DasmARM64_T62;
	codes[0x63] = DasmARM64_T63;
	codes[0x64] = DasmARM64_T64;
	codes[0x65] = DasmARM64_T65;
	codes[0x66] = DasmARM64_T66;
	codes[0x67] = DasmARM64_T67;
	codes[0x68] = DasmARM64_T68;
	codes[0x69] = DasmARM64_T69;
	codes[0x6A] = DasmARM64_T6A;
	codes[0x6B] = DasmARM64_T6B;
	codes[0x6C] = DasmARM64_T6C;
	codes[0x6D] = DasmARM64_T6D;
	codes[0x6E] = DasmARM64_T6E;
	codes[0x6F] = DasmARM64_T6F;
	codes[0x70] = DasmARM64_T70;
	codes[0x71] = DasmARM64_T71;
	codes[0x72] = DasmARM64_T72;
	codes[0x73] = DasmARM64_T73;
	codes[0x74] = DasmARM64_T74;
	codes[0x75] = DasmARM64_T75;
	codes[0x76] = DasmARM64_T76;
	codes[0x77] = DasmARM64_T77;
	codes[0x78] = DasmARM64_T78;
	codes[0x79] = DasmARM64_T79;
	codes[0x7A] = DasmARM64_T7A;
	codes[0x7B] = DasmARM64_T7B;
	codes[0x7C] = DasmARM64_T7C;
	codes[0x7D] = DasmARM64_T7D;
	codes[0x7E] = DasmARM64_T7E;
	codes[0x7F] = DasmARM64_T7F;
	codes[0x80] = DasmARM64_T80;
	codes[0x81] = DasmARM64_T81;
	codes[0x82] = DasmARM64_T82;
	codes[0x83] = DasmARM64_T83;
	codes[0x84] = DasmARM64_T84;
	codes[0x85] = DasmARM64_T85;
	codes[0x86] = DasmARM64_T86;
	codes[0x87] = DasmARM64_T87;
	codes[0x88] = DasmARM64_T88;
	codes[0x89] = DasmARM64_T89;
	codes[0x8A] = DasmARM64_T8A;
	codes[0x8B] = DasmARM64_T8B;
	codes[0x8C] = DasmARM64_T8C;
	codes[0x8D] = DasmARM64_T8D;
	codes[0x8E] = DasmARM64_T8E;
	codes[0x8F] = DasmARM64_T8F;
	codes[0x90] = DasmARM64_T90;
	codes[0x91] = DasmARM64_T91;
	codes[0x92] = DasmARM64_T92;
	codes[0x93] = DasmARM64_T93;
	codes[0x94] = DasmARM64_T94;
	codes[0x95] = DasmARM64_T95;
	codes[0x96] = DasmARM64_T96;
	codes[0x97] = DasmARM64_T97;
	codes[0x98] = DasmARM64_T98;
	codes[0x99] = DasmARM64_T99;
	codes[0x9A] = DasmARM64_T9A;
	codes[0x9B] = DasmARM64_T9B;
	codes[0x9C] = DasmARM64_T9C;
	codes[0x9D] = DasmARM64_T9D;
	codes[0x9E] = DasmARM64_T9E;
	codes[0x9F] = DasmARM64_T9F;
	codes[0xA0] = DasmARM64_TA0;
	codes[0xA1] = DasmARM64_TA1;
	codes[0xA2] = DasmARM64_TA2;
	codes[0xA3] = DasmARM64_TA3;
	codes[0xA4] = DasmARM64_TA4;
	codes[0xA5] = DasmARM64_TA5;
	codes[0xA6] = DasmARM64_TA6;
	codes[0xA7] = DasmARM64_TA7;
	codes[0xA8] = DasmARM64_TA8;
	codes[0xA9] = DasmARM64_TA9;
	codes[0xAA] = DasmARM64_TAA;
	codes[0xAB] = DasmARM64_TAB;
	codes[0xAC] = DasmARM64_TAC;
	codes[0xAD] = DasmARM64_TAD;
	codes[0xAE] = DasmARM64_TAE;
	codes[0xAF] = DasmARM64_TAF;
	codes[0xB0] = DasmARM64_TB0;
	codes[0xB1] = DasmARM64_TB1;
	codes[0xB2] = DasmARM64_TB2;
	codes[0xB3] = DasmARM64_TB3;
	codes[0xB4] = DasmARM64_TB4;
	codes[0xB5] = DasmARM64_TB5;
	codes[0xB6] = DasmARM64_TB6;
	codes[0xB7] = DasmARM64_TB7;
	codes[0xB8] = DasmARM64_TB8;
	codes[0xB9] = DasmARM64_TB9;
	codes[0xBA] = DasmARM64_TBA;
	codes[0xBB] = DasmARM64_TBB;
	codes[0xBC] = DasmARM64_TBC;
	codes[0xBD] = DasmARM64_TBD;
	codes[0xBE] = DasmARM64_TBE;
	codes[0xBF] = DasmARM64_TBF;
	codes[0xC0] = DasmARM64_TC0;
	codes[0xC1] = DasmARM64_TC1;
	codes[0xC2] = DasmARM64_TC2;
	codes[0xC3] = DasmARM64_TC3;
	codes[0xC4] = DasmARM64_TC4;
	codes[0xC5] = DasmARM64_TC5;
	codes[0xC6] = DasmARM64_TC6;
	codes[0xC7] = DasmARM64_TC7;
	codes[0xC8] = DasmARM64_TC8;
	codes[0xC9] = DasmARM64_TC9;
	codes[0xCA] = DasmARM64_TCA;
	codes[0xCB] = DasmARM64_TCB;
	codes[0xCC] = DasmARM64_TCC;
	codes[0xCD] = DasmARM64_TCD;
	codes[0xCE] = DasmARM64_TCE;
	codes[0xCF] = DasmARM64_TCF;
	codes[0xD0] = DasmARM64_TD0;
	codes[0xD1] = DasmARM64_TD1;
	codes[0xD2] = DasmARM64_TD2;
	codes[0xD3] = DasmARM64_TD3;
	codes[0xD4] = DasmARM64_TD4;
	codes[0xD5] = DasmARM64_TD5;
	codes[0xD6] = DasmARM64_TD6;
	codes[0xD7] = DasmARM64_TD7;
	codes[0xD8] = DasmARM64_TD8;
	codes[0xD9] = DasmARM64_TD9;
	codes[0xDA] = DasmARM64_TDA;
	codes[0xDB] = DasmARM64_TDB;
	codes[0xDC] = DasmARM64_TDC;
	codes[0xDD] = DasmARM64_TDD;
	codes[0xDE] = DasmARM64_TDE;
	codes[0xDF] = DasmARM64_TDF;
	codes[0xE0] = DasmARM64_TE0;
	codes[0xE1] = DasmARM64_TE1;
	codes[0xE2] = DasmARM64_TE2;
	codes[0xE3] = DasmARM64_TE3;
	codes[0xE4] = DasmARM64_TE4;
	codes[0xE5] = DasmARM64_TE5;
	codes[0xE6] = DasmARM64_TE6;
	codes[0xE7] = DasmARM64_TE7;
	codes[0xE8] = DasmARM64_TE8;
	codes[0xE9] = DasmARM64_TE9;
	codes[0xEA] = DasmARM64_TEA;
	codes[0xEB] = DasmARM64_TEB;
	codes[0xEC] = DasmARM64_TEC;
	codes[0xED] = DasmARM64_TED;
	codes[0xEE] = DasmARM64_TEE;
	codes[0xEF] = DasmARM64_TEF;
	codes[0xF0] = DasmARM64_TF0;
	codes[0xF1] = DasmARM64_TF1;
	codes[0xF2] = DasmARM64_TF2;
	codes[0xF3] = DasmARM64_TF3;
	codes[0xF4] = DasmARM64_TF4;
	codes[0xF5] = DasmARM64_TF5;
	codes[0xF6] = DasmARM64_TF6;
	codes[0xF7] = DasmARM64_TF7;
	codes[0xF8] = DasmARM64_TF8;
	codes[0xF9] = DasmARM64_TF9;
	codes[0xFA] = DasmARM64_TFA;
	codes[0xFB] = DasmARM64_TFB;
	codes[0xFC] = DasmARM64_TFC;
	codes[0xFD] = DasmARM64_TFD;
	codes[0xFE] = DasmARM64_TFE;
	codes[0xFF] = DasmARM64_TFF;
}

Manage::DasmARM64::~DasmARM64()
{
	MemFree(this->codes);
	MemFree(this->codesT);
}

Text::CString Manage::DasmARM64::GetHeader(Bool fullRegs)
{
	if (fullRegs)
	{
		return CSTR(" SP       LR       PC       CPSR     R0       R1       R2       R3       R4       R5       R6       R7       R8       R9       R10      FP       IP       Code");
	}
	else
	{
		return CSTR(" SP       LR       PC       CPSR     Code");
	}
}

Bool Manage::DasmARM64::Disasm64(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt64 *currInst, UInt64 *currStack, UInt64 *currFrame, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs)
{
	UTF8Char sbuff[512];
	UInt8 buff[16];
	UInt64 oriPC;
	DasmARM64_Sess sess;
	Text::StringBuilderUTF8 *outStr;
	UOSInt initJmpCnt = jmpAddrs->GetCount();
	NEW_CLASS(outStr, Text::StringBuilderUTF8());
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	MemCopyNO(&sess.regs, regs, sizeof(Manage::DasmARM64::DasmARM64_Regs));
	sess.regs.PC = *currInst;
	sess.regs.SP = *currStack;
	sess.regs.LR = *currFrame;
	sess.code = buff;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmARM64::ET_NOT_END;
	sess.thisStatus = 0;
	sess.codeHdlrs = (void**)this->codes;
	sess.codeTHdlrs = (void**)this->codesT;
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	sess.codeBuff = buff;
	*blockStart = sess.regs.PC;

	while (true)
	{
		Bool ret;

		outStr->ClearStr();
		outStr->AppendHex64(sess.regs.SP);
		outStr->AppendC(UTF8STRC(" "));
		outStr->AppendHex64(sess.regs.LR);
		outStr->AppendC(UTF8STRC(" "));
		outStr->AppendHex64(sess.regs.PC);
		outStr->AppendC(UTF8STRC(" "));
		if (fullRegs)
		{
			outStr->AppendHex64(sess.regs.X0);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X1);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X2);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X3);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X4);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X5);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X6);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X7);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X8);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X9);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X10);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X11);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X12);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X13);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X14);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X15);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X16);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X17);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X18);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X19);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X20);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X21);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X22);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X23);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X24);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X25);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X26);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X27);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.X28);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.FP);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendC(UTF8STRC(" "));
		}
		sess.sbuff = sbuff;
		oriPC = sess.regs.PC;
		if (sess.memReader->ReadMemory(oriPC, buff, 2) != 2)
		{
			ret = false;
		}
		else
		{
			ret = this->codesT[buff[1]](&sess);
		}
		if (!ret)
		{
			UOSInt buffSize;
			outStr->AppendC(UTF8STRC("Unknown opcode "));
			buffSize = sess.memReader->ReadMemory(oriPC, buff, 16);
			if (buffSize > 0)
			{
				outStr->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
			}
			outStr->AppendC(UTF8STRC("\r\n"));
			writer->WriteStrC(outStr->ToString(), outStr->GetLength());
			DEL_CLASS(outStr);
			return false;
		}
		outStr->AppendSlow(sbuff);
		writer->WriteStrC(outStr->ToString(), outStr->GetLength());
		if (sess.endType == Manage::DasmARM64::ET_JMP && (UInt32)sess.retAddr >= *blockStart && (UInt32)sess.retAddr <= sess.regs.PC)
		{
			UOSInt i;
			UInt64 minAddr = (UInt64)(Int64)-1;
			UInt64 jmpAddr;
			i = jmpAddrs->GetCount();
			while (i-- > initJmpCnt)
			{
				jmpAddr = jmpAddrs->GetItem(i);
				if (jmpAddr >= sess.regs.PC && jmpAddr < minAddr)
				{
					minAddr = jmpAddr;
				}
			}
			if (minAddr - sess.regs.PC > 0x1000)
			{
				*currInst = sess.retAddr;
				*currStack = sess.regs.SP;
				*currFrame = sess.regs.LR;
				*blockEnd = sess.regs.PC;
				DEL_CLASS(outStr);
				MemCopyNO(regs, &sess.regs, sizeof(Manage::DasmARM64::DasmARM64_Regs));
				return false;
			}
			sess.regs.PC = minAddr;
			sess.endType = Manage::DasmARM64::ET_NOT_END;
		}
		else if (sess.endType != Manage::DasmARM64::ET_NOT_END)
		{
			*currInst = sess.retAddr;
			*currStack = sess.regs.SP;
			*currFrame = sess.regs.LR;
			*blockEnd = sess.regs.PC;
			DEL_CLASS(outStr);
			MemCopyNO(regs, &sess.regs, sizeof(Manage::DasmARM64::DasmARM64_Regs));
			return sess.endType != Manage::DasmARM64::ET_EXIT;
		}
//		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = 0;
	}
}

Manage::Dasm::Dasm_Regs *Manage::DasmARM64::CreateRegs()
{
	Manage::DasmARM64::DasmARM64_Regs *regs = MemAlloc(Manage::DasmARM64::DasmARM64_Regs, 1);
	return regs;
}

void Manage::DasmARM64::FreeRegs(Dasm_Regs *regs)
{
	MemFree(regs);
}

Manage::DasmARM64::DasmARM64_Sess *Manage::DasmARM64::CreateSess(Manage::DasmARM64::DasmARM64_Regs *regs, UInt8 *code, UInt16 codeSegm)
{
	Manage::DasmARM64::DasmARM64_Sess *sess = MemAlloc(Manage::DasmARM64::DasmARM64_Sess, 1);
	sess->code = code;
	sess->codeSegm = codeSegm;
	sess->codeHdlrs = (void**)this->codes;
	//sess->code0fHdlrs = (void**)this->codes0f;
	NEW_CLASS(sess->callAddrs, Data::ArrayListUInt64());
	NEW_CLASS(sess->jmpAddrs, Data::ArrayListUInt64());
	MemCopyNO(&sess->regs, regs, sizeof(Manage::DasmARM64::DasmARM64_Regs));
	return sess;
}

void Manage::DasmARM64::DeleteSess(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	if (sess)
	{
		DEL_CLASS(sess->callAddrs);
		DEL_CLASS(sess->jmpAddrs);
		MemFree(sess);
	}
}

Bool Manage::DasmARM64::DasmNext(Manage::DasmARM64::DasmARM64_Sess *sess, UTF8Char *buff, OSInt *outBuffSize)
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
