#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Manage/AddressResolver.h"
#include "Manage/DasmARM.h"
#include "Manage/ThreadContextARM.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#if !defined(__GNUC__)
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

Bool DasmARM_IsEndFunc(UnsafeArray<const UTF8Char> funcName, UOSInt nameLen)
{
	if (Text::StrIndexOfC(funcName, nameLen, UTF8STRC("(exit+0)")) != INVALID_INDEX)
	{
		return true;
	}
	return false;
}

UnsafeArray<UTF8Char> DasmARM_ParseReg32(NN<Manage::DasmARM::DasmARM_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo, UInt32 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = &sess->regs.R0;
		return Text::StrConcatC(regName, UTF8STRC("r0"));
	case 1:
		*regPtr = &sess->regs.R1;
		return Text::StrConcatC(regName, UTF8STRC("r1"));
	case 2:
		*regPtr = &sess->regs.R2;
		return Text::StrConcatC(regName, UTF8STRC("r2"));
	case 3:
		*regPtr = &sess->regs.R3;
		return Text::StrConcatC(regName, UTF8STRC("r3"));
	case 4:
		*regPtr = &sess->regs.R4;
		return Text::StrConcatC(regName, UTF8STRC("r4"));
	case 5:
		*regPtr = &sess->regs.R5;
		return Text::StrConcatC(regName, UTF8STRC("r5"));
	case 6:
		*regPtr = &sess->regs.R6;
		return Text::StrConcatC(regName, UTF8STRC("r6"));
	case 7:
		*regPtr = &sess->regs.R7;
		return Text::StrConcatC(regName, UTF8STRC("r7"));
	case 8:
		*regPtr = &sess->regs.R8;
		return Text::StrConcatC(regName, UTF8STRC("r8"));
	case 9:
		*regPtr = &sess->regs.R9;
		return Text::StrConcatC(regName, UTF8STRC("r9"));
	case 10:
		*regPtr = &sess->regs.R10;
		return Text::StrConcatC(regName, UTF8STRC("r10"));
	case 11:
		*regPtr = &sess->regs.FP;
		return Text::StrConcatC(regName, UTF8STRC("r11"));
	case 12:
		*regPtr = &sess->regs.IP;
		return Text::StrConcatC(regName, UTF8STRC("r12"));
	case 13:
		*regPtr = &sess->regs.SP;
		return Text::StrConcatC(regName, UTF8STRC("sp"));
	case 14:
		*regPtr = &sess->regs.LR;
		return Text::StrConcatC(regName, UTF8STRC("lr"));
	case 15:
		*regPtr = &sess->regs.PC;
		return Text::StrConcatC(regName, UTF8STRC("pc"));
	default:
		*regName = 0;
		return regName;
	}
}

UInt32 DasmARM_ParseImmA32(UInt8 rot, UInt8 val)
{
	if (rot == 0)
		return val;
	rot = (UInt8)(rot << 1);
	if (rot < 8)
	{
		return (val >> rot) | (((UInt32)val) << (32 - rot));
	}
	else
	{
		return  ((UInt32)val) << (32 - rot);;
	}
}

UInt32 DasmARM_ParseImmShift(NN<Manage::DasmARM::DasmARM_Sess> sess, UInt32 oriVal)
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
				return (UInt32)-1;
			else
				return 0;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #"));
			sess->sbuff = Text::StrInt32(sess->sbuff, v);
			return (UInt32)((Int32)oriVal) >> v;
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

Bool DasmARM_00(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_01(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_02(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_03(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_04(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_05(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_06(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_07(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_08(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_09(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_0A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("beq 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_0B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bleq 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_0C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_0D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_0E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_0F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_10(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_11(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_12(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_13(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_14(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_15(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_16(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_17(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_18(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_19(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_1A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bne 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_1B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blne 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_1C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_1D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_1E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_1F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_20(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_21(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_22(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_23(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_24(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_25(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_26(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_27(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_28(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_29(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_2A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bcs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_2B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blcs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_2C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_2D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_2E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_2F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_30(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_31(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_32(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_33(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_34(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_35(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_36(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_37(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_38(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_39(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_3A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bcc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_3B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blcc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_3C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_3D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_3E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_3F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_40(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_41(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_42(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_43(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_44(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_45(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_46(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_47(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_48(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_49(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_4A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bmi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_4B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blmi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_4C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_4D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_4E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_4F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_50(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_51(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_52(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_53(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_54(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_55(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_56(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_57(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_58(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_59(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_5A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bpl 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_5B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blpl 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_5C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_5D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_5E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_5F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_60(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_61(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_62(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_63(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_64(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_65(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_66(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_67(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_68(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_69(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_6A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bvs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_6B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blvs 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_6C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_6D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_6E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_6F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_70(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_71(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_72(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_73(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_74(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_75(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_76(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_77(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_78(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_79(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_7A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bvc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_7B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blvc 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_7C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_7D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_7E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_7F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_80(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_81(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_82(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrs;
	UInt32 *regPtrd;
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
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]));
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

Bool DasmARM_83(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrd;
	Int32 val;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0xe0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mvnhi "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		val = (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrInt32(sess->sbuff, val);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
/*		*regPtrd = val;
		if (regPtrd == 15)
		{
			sess->endType = Manage::DasmARM::ET_JMP;
			sess->retAddr = val;
		}*/
		return true;
	}
	return false;
}

Bool DasmARM_84(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_85(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrs;
	UInt32 *regPtrd;
	Int32 addr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrhi "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
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
			sess->endType = Manage::DasmARM::ET_JMP;
			sess->retAddr = *regPtrd;
		}*/
		return true;
	}
	return false;
}

Bool DasmARM_86(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_87(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrs;
	UInt32 *regPtrs2;
	UInt32 *regPtrd;
	UInt32 addr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x80:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("strhi "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[2] & 0xf), &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", +"));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[0] & 0xf), &regPtrs2);
		addr = DasmARM_ParseImmShift(sess, *regPtrs2);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
		return true;
	case 0x90:
		if (sess->codeBuff[0] & 0x10)
		{

		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrhi "));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[2] & 0xf), &regPtrs);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", +"));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[0] & 0xf), &regPtrs2);
			addr = DasmARM_ParseImmShift(sess, *regPtrs2);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
	/*		*regPtrd = val;
			if (regPtrd == 15)
			{
				sess->endType = Manage::DasmARM::ET_JMP;
				sess->retAddr = val;
			}*/
			return true;
		}
		break;
	}
	return false;
}

Bool DasmARM_88(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_89(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_8A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bhi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_8B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blhi 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_8C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_8D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_8E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_8F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_90(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_91(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_92(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_93(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_94(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_95(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_96(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_97(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_98(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_99(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_9A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bls 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_9B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blls 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_9C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_9D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_9E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_9F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_A9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_AA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bge 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_AB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blge 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_AC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_AD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_AE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_AF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_B9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_BA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_BB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bllt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_BC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_BD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_BE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_BF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_C9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_CA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bgt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_CB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blgt 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_CC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_CD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_CE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_CF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_D9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_DA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ble 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_DB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blle 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_DC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_DD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_DE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_DF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_E0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrd;
	UInt32 *regPtrs1;
	UInt32 *regPtrs2;
	Int32 imm;
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
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs1);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs2);
			imm = (Int32)DasmARM_ParseImmShift(sess, *regPtrs2);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			*regPtrd = *regPtrs1 + (UInt32)imm;
			if ((sess->codeBuff[1] & 0xf0) == 0xf0)
			{
				sess->endType = Manage::DasmARM::ET_JMP;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
		break;
	case 0xC0:
		if ((sess->codeBuff[0] & 0xF0) == 0xB0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("strh "));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs1);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("], #"));
			imm = ((sess->codeBuff[0] & 0xf) | ((sess->codeBuff[1] & 0xf) << 4));
			sess->sbuff = Text::StrUInt16(sess->sbuff, (UInt16)imm);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			*regPtrs1 += (UInt32)imm;
			if ((sess->codeBuff[1] & 0xf0) == 0xf0)
			{
				sess->endType = Manage::DasmARM::ET_JMP;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
		break;
	}
	return false;
}

Bool DasmARM_E1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	UInt32 *regPtrd;
	UInt32 *regPtrs;
	UInt32 *regPtrs2;
	Int32 imm;
	UInt8 t;
	UnsafeArray<UTF8Char> sptr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x20:
		if ((sess->codeBuff[0] & 0x80) == 0)
		{
			if ((sess->codeBuff[0] & 0x70) == 0x30)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blx "));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[0] & 0xf), &regPtrs);

				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" (0x"));
				sess->sbuff = Text::StrHexVal32(sess->sbuff, *regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
				if (sess->addrResol.SetTo(addrResol))
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
					sess->sbuff = addrResol->ResolveName(sptr = sess->sbuff, *regPtrs).Or(sess->sbuff);
					if (DasmARM_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
					{
						sess->endType = Manage::DasmARM::ET_EXIT;
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSL #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else if (t == 1)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSR #32"));
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", LSR #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else if (t == 2)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #32"));
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ASR #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else if (t == 3)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", RRX"));
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ROR #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
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
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs2);
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs << imm;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
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
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt32(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = *regPtrs >> imm;
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM::ET_FUNC_RET;
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
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt32(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = (UInt32)(((Int32)*regPtrs) >> imm);
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM::ET_FUNC_RET;
					sess->retAddr = sess->regs.PC;
				}
				return true;
			}
			else if (t == 3)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rrx "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs >> 1;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = (*regPtrs >> imm) | (*regPtrs << (32 - imm));
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
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

Bool DasmARM_E2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrd;
	UInt32 *regPtrs;
	Int32 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x40:
		imm = (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs - (UInt32)imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0x50:
		imm = (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subs "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs - (UInt32)imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0x80:
		imm = (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs + (UInt32)imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM::ET_JMP;
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs << imm;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
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
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt32(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = *regPtrs >> imm;
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM::ET_FUNC_RET;
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
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
				sess->sbuff = Text::StrInt32(sess->sbuff, imm);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				*regPtrd = ((Int32)*regPtrs) >> imm;
				if ((sess->codeBuff[2] & 0xf) == 15)
				{
					sess->endType = Manage::DasmARM::ET_FUNC_RET;
					sess->retAddr = sess->regs.PC;
				}
				return true;
			}
			else if (t == 3)
			{
				if (imm == 0)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rrx "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = *regPtrs >> 1;
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[0] & 0xf, &regPtrs);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
					sess->sbuff = Text::StrInt32(sess->sbuff, imm);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					*regPtrd = (*regPtrs >> imm) | (*regPtrs << (32 - imm));
					if ((sess->codeBuff[2] & 0xf) == 15)
					{
						sess->endType = Manage::DasmARM::ET_FUNC_RET;
						sess->retAddr = sess->regs.PC;
					}
					return true;
				}
			}
		}
		break;*/
	case 0xb0:
		imm = (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adcs "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtrd = *regPtrs + (UInt32)imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	}
	return false;
}

Bool DasmARM_E3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtr;
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
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x60:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("msr "));
		return false;
	case 0x70:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmn "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]));
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
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #"));
		val = (Int32)DasmARM_ParseImmA32(sess->codeBuff[1] & 0xf, sess->codeBuff[0]);
		sess->sbuff = Text::StrInt32(sess->sbuff, val);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		*regPtr = (UInt32)val;
		if ((sess->codeBuff[2] & 0xf) == 15)
		{
			sess->endType = Manage::DasmARM::ET_JMP;
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

Bool DasmARM_E4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrd;
	UInt32 *regPtrs;
	Int32 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		imm = ((sess->codeBuff[1] & 0xf) << 8) | sess->codeBuff[0];
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldr "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("], #+"));
		sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->memReader->ReadMemory(*regPtrs, (UInt8*)regPtrd, 4);
		*regPtrs += (UInt32)imm;
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14 || (sess->codeBuff[2] & 0xf) == 13)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	}
	return false;
}

Bool DasmARM_E5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtrd;
	UInt32 *regPtrs;
	Int32 imm;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		imm = ((sess->codeBuff[1] & 0xf) << 8) | sess->codeBuff[0];
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldr "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
		if (imm != 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", #+"));
			sess->sbuff = Text::StrInt32(sess->sbuff, imm);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
		sess->memReader->ReadMemory((UInt32)imm + *regPtrs, (UInt8*)regPtrd, 4);
		if ((sess->codeBuff[1] & 0xf0) == 0xf0)
		{
			if ((sess->codeBuff[2] & 0xf) == 14 || (sess->codeBuff[2] & 0xf) == 13)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
			}
			else
			{
				sess->endType = Manage::DasmARM::ET_JMP;
			}
			sess->retAddr = sess->regs.PC;
		}
		return true;
	case 0xC0:
		imm = ((sess->codeBuff[1] & 0xf) << 8) | sess->codeBuff[0];
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("strb "));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, (sess->codeBuff[1] & 0xf0) >> 4, &regPtrd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtrs);
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

Bool DasmARM_E6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_E7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_E8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	UInt32 *regPtr;
	sess->regs.PC += 4;
	switch (sess->codeBuff[2] & 0xf0)
	{
	case 0x90:
		{
			Bool found = false;
			UInt8 v;
			UInt8 regNo;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldrfm "));
			sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, sess->codeBuff[2] & 0xf, &regPtr);
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, regNo, &regPtr);
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, regNo, &regPtr);
					sess->memReader->ReadMemory(sess->regs.SP, (UInt8*)regPtr, 4);
					sess->regs.SP += 4;
				}
				v = v << 1;
				regNo++;
			}
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("}\r\n"));
			if (sess->codeBuff[1] & 0x80)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, regNo, &regPtr);
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
					sess->sbuff = DasmARM_ParseReg32(sess, sess->sbuff, regNo, &regPtr);
					sess->memReader->ReadMemory(sess->regs.SP, (UInt8*)regPtr, 4);
					sess->regs.SP += 4;
				}
				v = v << 1;
				regNo++;
			}
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			if (sess->codeBuff[1] & 0x80)
			{
				sess->endType = Manage::DasmARM::ET_FUNC_RET;
				sess->retAddr = sess->regs.PC;
			}
			return true;
		}
		break;
	}
	return false;
}

Bool DasmARM_E9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_EA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("b 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));

	sess->regs.PC += (UInt32)addr + 4;
	sess->jmpAddrs->Add(sess->regs.PC);
	sess->endType = Manage::DasmARM::ET_JMP;
	sess->retAddr = sess->regs.PC;
	return true;
}

Bool DasmARM_EB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	NN<Manage::AddressResolver> addrResol;
	Int32 addr;
	UnsafeArray<UTF8Char> sptr;
	sess->regs.PC += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bl 0x"));
	addr = ReadInt24(sess->codeBuff) << 2;
	sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->regs.PC + (UInt32)addr + 4);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ("));
	if (addr > 0)
	{
		*sess->sbuff++ = '+';
	}
	sess->sbuff = Text::StrInt32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = addrResol->ResolveName(sptr = sess->sbuff, sess->regs.PC + (UInt32)addr + 4).Or(sess->sbuff);
		if (DasmARM_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
		{
			sess->endType = Manage::DasmARM::ET_EXIT;
			sess->retAddr = sess->regs.PC + (UInt32)addr + 4;
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(sess->regs.PC + (UInt32)addr + 4);
	return true;
}

Bool DasmARM_EC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_ED(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_EE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_EF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_F9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_FA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_FB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_FC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_FD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_FE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_FF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T00(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T01(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T02(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T03(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T04(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T05(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T06(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T07(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T08(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T09(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T0A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T0B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T0C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T0D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T0E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T0F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T10(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T11(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T12(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T13(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T14(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T15(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T16(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T17(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T18(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T19(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T1A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T1B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T1C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T1D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T1E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T1F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T20(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T21(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T22(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T23(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T24(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T25(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T26(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T27(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T28(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T29(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T2A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T2B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T2C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T2D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T2E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T2F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T30(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T31(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T32(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T33(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T34(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T35(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T36(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T37(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T38(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T39(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T3A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T3B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T3C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T3D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T3E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T3F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T40(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T41(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T42(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T43(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T44(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T45(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T46(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T47(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T48(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T49(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T4A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T4B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T4C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T4D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T4E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T4F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T50(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T51(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T52(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T53(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T54(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T55(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T56(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T57(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T58(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T59(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T5A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T5B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T5C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T5D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T5E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T5F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T60(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T61(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T62(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T63(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T64(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T65(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T66(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T67(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T68(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T69(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T6A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T6B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T6C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T6D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T6E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T6F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T70(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T71(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T72(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T73(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T74(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T75(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T76(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T77(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T78(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T79(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T7A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T7B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T7C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T7D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T7E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T7F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T80(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T81(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T82(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T83(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T84(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T85(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T86(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T87(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T88(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T89(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T8A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T8B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T8C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T8D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T8E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T8F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T90(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T91(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T92(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T93(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T94(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T95(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T96(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T97(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T98(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T99(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T9A(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T9B(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T9C(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T9D(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T9E(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_T9F(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TA9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TAA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TAB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TAC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TAD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TAE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TAF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TB9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TBA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TBB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TBC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TBD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TBE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TBF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TC9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TCA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TCB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TCC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TCD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TCE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TCF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TD9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TDA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TDB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TDC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TDD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TDE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TDF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TE9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TEA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TEB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TEC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TED(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TEE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TEF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF0(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF1(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF2(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF3(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF4(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF5(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF6(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF7(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF8(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TF9(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TFA(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TFB(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TFC(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TFD(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TFE(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Bool DasmARM_TFF(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	return false;
}

Manage::DasmARM::DasmARM()
{
	DasmARM_Code *codes;
	codes = this->codes = MemAlloc(DasmARM_Code, 256);
	codes[0x00] = DasmARM_00;
	codes[0x01] = DasmARM_01;
	codes[0x02] = DasmARM_02;
	codes[0x03] = DasmARM_03;
	codes[0x04] = DasmARM_04;
	codes[0x05] = DasmARM_05;
	codes[0x06] = DasmARM_06;
	codes[0x07] = DasmARM_07;
	codes[0x08] = DasmARM_08;
	codes[0x09] = DasmARM_09;
	codes[0x0A] = DasmARM_0A;
	codes[0x0B] = DasmARM_0B;
	codes[0x0C] = DasmARM_0C;
	codes[0x0D] = DasmARM_0D;
	codes[0x0E] = DasmARM_0E;
	codes[0x0F] = DasmARM_0F;
	codes[0x10] = DasmARM_10;
	codes[0x11] = DasmARM_11;
	codes[0x12] = DasmARM_12;
	codes[0x13] = DasmARM_13;
	codes[0x14] = DasmARM_14;
	codes[0x15] = DasmARM_15;
	codes[0x16] = DasmARM_16;
	codes[0x17] = DasmARM_17;
	codes[0x18] = DasmARM_18;
	codes[0x19] = DasmARM_19;
	codes[0x1A] = DasmARM_1A;
	codes[0x1B] = DasmARM_1B;
	codes[0x1C] = DasmARM_1C;
	codes[0x1D] = DasmARM_1D;
	codes[0x1E] = DasmARM_1E;
	codes[0x1F] = DasmARM_1F;
	codes[0x20] = DasmARM_20;
	codes[0x21] = DasmARM_21;
	codes[0x22] = DasmARM_22;
	codes[0x23] = DasmARM_23;
	codes[0x24] = DasmARM_24;
	codes[0x25] = DasmARM_25;
	codes[0x26] = DasmARM_26;
	codes[0x27] = DasmARM_27;
	codes[0x28] = DasmARM_28;
	codes[0x29] = DasmARM_29;
	codes[0x2A] = DasmARM_2A;
	codes[0x2B] = DasmARM_2B;
	codes[0x2C] = DasmARM_2C;
	codes[0x2D] = DasmARM_2D;
	codes[0x2E] = DasmARM_2E;
	codes[0x2F] = DasmARM_2F;
	codes[0x30] = DasmARM_30;
	codes[0x31] = DasmARM_31;
	codes[0x32] = DasmARM_32;
	codes[0x33] = DasmARM_33;
	codes[0x34] = DasmARM_34;
	codes[0x35] = DasmARM_35;
	codes[0x36] = DasmARM_36;
	codes[0x37] = DasmARM_37;
	codes[0x38] = DasmARM_38;
	codes[0x39] = DasmARM_39;
	codes[0x3A] = DasmARM_3A;
	codes[0x3B] = DasmARM_3B;
	codes[0x3C] = DasmARM_3C;
	codes[0x3D] = DasmARM_3D;
	codes[0x3E] = DasmARM_3E;
	codes[0x3F] = DasmARM_3F;
	codes[0x40] = DasmARM_40;
	codes[0x41] = DasmARM_41;
	codes[0x42] = DasmARM_42;
	codes[0x43] = DasmARM_43;
	codes[0x44] = DasmARM_44;
	codes[0x45] = DasmARM_45;
	codes[0x46] = DasmARM_46;
	codes[0x47] = DasmARM_47;
	codes[0x48] = DasmARM_48;
	codes[0x49] = DasmARM_49;
	codes[0x4A] = DasmARM_4A;
	codes[0x4B] = DasmARM_4B;
	codes[0x4C] = DasmARM_4C;
	codes[0x4D] = DasmARM_4D;
	codes[0x4E] = DasmARM_4E;
	codes[0x4F] = DasmARM_4F;
	codes[0x50] = DasmARM_50;
	codes[0x51] = DasmARM_51;
	codes[0x52] = DasmARM_52;
	codes[0x53] = DasmARM_53;
	codes[0x54] = DasmARM_54;
	codes[0x55] = DasmARM_55;
	codes[0x56] = DasmARM_56;
	codes[0x57] = DasmARM_57;
	codes[0x58] = DasmARM_58;
	codes[0x59] = DasmARM_59;
	codes[0x5A] = DasmARM_5A;
	codes[0x5B] = DasmARM_5B;
	codes[0x5C] = DasmARM_5C;
	codes[0x5D] = DasmARM_5D;
	codes[0x5E] = DasmARM_5E;
	codes[0x5F] = DasmARM_5F;
	codes[0x60] = DasmARM_60;
	codes[0x61] = DasmARM_61;
	codes[0x62] = DasmARM_62;
	codes[0x63] = DasmARM_63;
	codes[0x64] = DasmARM_64;
	codes[0x65] = DasmARM_65;
	codes[0x66] = DasmARM_66;
	codes[0x67] = DasmARM_67;
	codes[0x68] = DasmARM_68;
	codes[0x69] = DasmARM_69;
	codes[0x6A] = DasmARM_6A;
	codes[0x6B] = DasmARM_6B;
	codes[0x6C] = DasmARM_6C;
	codes[0x6D] = DasmARM_6D;
	codes[0x6E] = DasmARM_6E;
	codes[0x6F] = DasmARM_6F;
	codes[0x70] = DasmARM_70;
	codes[0x71] = DasmARM_71;
	codes[0x72] = DasmARM_72;
	codes[0x73] = DasmARM_73;
	codes[0x74] = DasmARM_74;
	codes[0x75] = DasmARM_75;
	codes[0x76] = DasmARM_76;
	codes[0x77] = DasmARM_77;
	codes[0x78] = DasmARM_78;
	codes[0x79] = DasmARM_79;
	codes[0x7A] = DasmARM_7A;
	codes[0x7B] = DasmARM_7B;
	codes[0x7C] = DasmARM_7C;
	codes[0x7D] = DasmARM_7D;
	codes[0x7E] = DasmARM_7E;
	codes[0x7F] = DasmARM_7F;
	codes[0x80] = DasmARM_80;
	codes[0x81] = DasmARM_81;
	codes[0x82] = DasmARM_82;
	codes[0x83] = DasmARM_83;
	codes[0x84] = DasmARM_84;
	codes[0x85] = DasmARM_85;
	codes[0x86] = DasmARM_86;
	codes[0x87] = DasmARM_87;
	codes[0x88] = DasmARM_88;
	codes[0x89] = DasmARM_89;
	codes[0x8A] = DasmARM_8A;
	codes[0x8B] = DasmARM_8B;
	codes[0x8C] = DasmARM_8C;
	codes[0x8D] = DasmARM_8D;
	codes[0x8E] = DasmARM_8E;
	codes[0x8F] = DasmARM_8F;
	codes[0x90] = DasmARM_90;
	codes[0x91] = DasmARM_91;
	codes[0x92] = DasmARM_92;
	codes[0x93] = DasmARM_93;
	codes[0x94] = DasmARM_94;
	codes[0x95] = DasmARM_95;
	codes[0x96] = DasmARM_96;
	codes[0x97] = DasmARM_97;
	codes[0x98] = DasmARM_98;
	codes[0x99] = DasmARM_99;
	codes[0x9A] = DasmARM_9A;
	codes[0x9B] = DasmARM_9B;
	codes[0x9C] = DasmARM_9C;
	codes[0x9D] = DasmARM_9D;
	codes[0x9E] = DasmARM_9E;
	codes[0x9F] = DasmARM_9F;
	codes[0xA0] = DasmARM_A0;
	codes[0xA1] = DasmARM_A1;
	codes[0xA2] = DasmARM_A2;
	codes[0xA3] = DasmARM_A3;
	codes[0xA4] = DasmARM_A4;
	codes[0xA5] = DasmARM_A5;
	codes[0xA6] = DasmARM_A6;
	codes[0xA7] = DasmARM_A7;
	codes[0xA8] = DasmARM_A8;
	codes[0xA9] = DasmARM_A9;
	codes[0xAA] = DasmARM_AA;
	codes[0xAB] = DasmARM_AB;
	codes[0xAC] = DasmARM_AC;
	codes[0xAD] = DasmARM_AD;
	codes[0xAE] = DasmARM_AE;
	codes[0xAF] = DasmARM_AF;
	codes[0xB0] = DasmARM_B0;
	codes[0xB1] = DasmARM_B1;
	codes[0xB2] = DasmARM_B2;
	codes[0xB3] = DasmARM_B3;
	codes[0xB4] = DasmARM_B4;
	codes[0xB5] = DasmARM_B5;
	codes[0xB6] = DasmARM_B6;
	codes[0xB7] = DasmARM_B7;
	codes[0xB8] = DasmARM_B8;
	codes[0xB9] = DasmARM_B9;
	codes[0xBA] = DasmARM_BA;
	codes[0xBB] = DasmARM_BB;
	codes[0xBC] = DasmARM_BC;
	codes[0xBD] = DasmARM_BD;
	codes[0xBE] = DasmARM_BE;
	codes[0xBF] = DasmARM_BF;
	codes[0xC0] = DasmARM_C0;
	codes[0xC1] = DasmARM_C1;
	codes[0xC2] = DasmARM_C2;
	codes[0xC3] = DasmARM_C3;
	codes[0xC4] = DasmARM_C4;
	codes[0xC5] = DasmARM_C5;
	codes[0xC6] = DasmARM_C6;
	codes[0xC7] = DasmARM_C7;
	codes[0xC8] = DasmARM_C8;
	codes[0xC9] = DasmARM_C9;
	codes[0xCA] = DasmARM_CA;
	codes[0xCB] = DasmARM_CB;
	codes[0xCC] = DasmARM_CC;
	codes[0xCD] = DasmARM_CD;
	codes[0xCE] = DasmARM_CE;
	codes[0xCF] = DasmARM_CF;
	codes[0xD0] = DasmARM_D0;
	codes[0xD1] = DasmARM_D1;
	codes[0xD2] = DasmARM_D2;
	codes[0xD3] = DasmARM_D3;
	codes[0xD4] = DasmARM_D4;
	codes[0xD5] = DasmARM_D5;
	codes[0xD6] = DasmARM_D6;
	codes[0xD7] = DasmARM_D7;
	codes[0xD8] = DasmARM_D8;
	codes[0xD9] = DasmARM_D9;
	codes[0xDA] = DasmARM_DA;
	codes[0xDB] = DasmARM_DB;
	codes[0xDC] = DasmARM_DC;
	codes[0xDD] = DasmARM_DD;
	codes[0xDE] = DasmARM_DE;
	codes[0xDF] = DasmARM_DF;
	codes[0xE0] = DasmARM_E0;
	codes[0xE1] = DasmARM_E1;
	codes[0xE2] = DasmARM_E2;
	codes[0xE3] = DasmARM_E3;
	codes[0xE4] = DasmARM_E4;
	codes[0xE5] = DasmARM_E5;
	codes[0xE6] = DasmARM_E6;
	codes[0xE7] = DasmARM_E7;
	codes[0xE8] = DasmARM_E8;
	codes[0xE9] = DasmARM_E9;
	codes[0xEA] = DasmARM_EA;
	codes[0xEB] = DasmARM_EB;
	codes[0xEC] = DasmARM_EC;
	codes[0xED] = DasmARM_ED;
	codes[0xEE] = DasmARM_EE;
	codes[0xEF] = DasmARM_EF;
	codes[0xF0] = DasmARM_F0;
	codes[0xF1] = DasmARM_F1;
	codes[0xF2] = DasmARM_F2;
	codes[0xF3] = DasmARM_F3;
	codes[0xF4] = DasmARM_F4;
	codes[0xF5] = DasmARM_F5;
	codes[0xF6] = DasmARM_F6;
	codes[0xF7] = DasmARM_F7;
	codes[0xF8] = DasmARM_F8;
	codes[0xF9] = DasmARM_F9;
	codes[0xFA] = DasmARM_FA;
	codes[0xFB] = DasmARM_FB;
	codes[0xFC] = DasmARM_FC;
	codes[0xFD] = DasmARM_FD;
	codes[0xFE] = DasmARM_FE;
	codes[0xFF] = DasmARM_FF;

	codes = this->codesT = MemAlloc(DasmARM_Code, 256);
	codes[0x00] = DasmARM_T00;
	codes[0x01] = DasmARM_T01;
	codes[0x02] = DasmARM_T02;
	codes[0x03] = DasmARM_T03;
	codes[0x04] = DasmARM_T04;
	codes[0x05] = DasmARM_T05;
	codes[0x06] = DasmARM_T06;
	codes[0x07] = DasmARM_T07;
	codes[0x08] = DasmARM_T08;
	codes[0x09] = DasmARM_T09;
	codes[0x0A] = DasmARM_T0A;
	codes[0x0B] = DasmARM_T0B;
	codes[0x0C] = DasmARM_T0C;
	codes[0x0D] = DasmARM_T0D;
	codes[0x0E] = DasmARM_T0E;
	codes[0x0F] = DasmARM_T0F;
	codes[0x10] = DasmARM_T10;
	codes[0x11] = DasmARM_T11;
	codes[0x12] = DasmARM_T12;
	codes[0x13] = DasmARM_T13;
	codes[0x14] = DasmARM_T14;
	codes[0x15] = DasmARM_T15;
	codes[0x16] = DasmARM_T16;
	codes[0x17] = DasmARM_T17;
	codes[0x18] = DasmARM_T18;
	codes[0x19] = DasmARM_T19;
	codes[0x1A] = DasmARM_T1A;
	codes[0x1B] = DasmARM_T1B;
	codes[0x1C] = DasmARM_T1C;
	codes[0x1D] = DasmARM_T1D;
	codes[0x1E] = DasmARM_T1E;
	codes[0x1F] = DasmARM_T1F;
	codes[0x20] = DasmARM_T20;
	codes[0x21] = DasmARM_T21;
	codes[0x22] = DasmARM_T22;
	codes[0x23] = DasmARM_T23;
	codes[0x24] = DasmARM_T24;
	codes[0x25] = DasmARM_T25;
	codes[0x26] = DasmARM_T26;
	codes[0x27] = DasmARM_T27;
	codes[0x28] = DasmARM_T28;
	codes[0x29] = DasmARM_T29;
	codes[0x2A] = DasmARM_T2A;
	codes[0x2B] = DasmARM_T2B;
	codes[0x2C] = DasmARM_T2C;
	codes[0x2D] = DasmARM_T2D;
	codes[0x2E] = DasmARM_T2E;
	codes[0x2F] = DasmARM_T2F;
	codes[0x30] = DasmARM_T30;
	codes[0x31] = DasmARM_T31;
	codes[0x32] = DasmARM_T32;
	codes[0x33] = DasmARM_T33;
	codes[0x34] = DasmARM_T34;
	codes[0x35] = DasmARM_T35;
	codes[0x36] = DasmARM_T36;
	codes[0x37] = DasmARM_T37;
	codes[0x38] = DasmARM_T38;
	codes[0x39] = DasmARM_T39;
	codes[0x3A] = DasmARM_T3A;
	codes[0x3B] = DasmARM_T3B;
	codes[0x3C] = DasmARM_T3C;
	codes[0x3D] = DasmARM_T3D;
	codes[0x3E] = DasmARM_T3E;
	codes[0x3F] = DasmARM_T3F;
	codes[0x40] = DasmARM_T40;
	codes[0x41] = DasmARM_T41;
	codes[0x42] = DasmARM_T42;
	codes[0x43] = DasmARM_T43;
	codes[0x44] = DasmARM_T44;
	codes[0x45] = DasmARM_T45;
	codes[0x46] = DasmARM_T46;
	codes[0x47] = DasmARM_T47;
	codes[0x48] = DasmARM_T48;
	codes[0x49] = DasmARM_T49;
	codes[0x4A] = DasmARM_T4A;
	codes[0x4B] = DasmARM_T4B;
	codes[0x4C] = DasmARM_T4C;
	codes[0x4D] = DasmARM_T4D;
	codes[0x4E] = DasmARM_T4E;
	codes[0x4F] = DasmARM_T4F;
	codes[0x50] = DasmARM_T50;
	codes[0x51] = DasmARM_T51;
	codes[0x52] = DasmARM_T52;
	codes[0x53] = DasmARM_T53;
	codes[0x54] = DasmARM_T54;
	codes[0x55] = DasmARM_T55;
	codes[0x56] = DasmARM_T56;
	codes[0x57] = DasmARM_T57;
	codes[0x58] = DasmARM_T58;
	codes[0x59] = DasmARM_T59;
	codes[0x5A] = DasmARM_T5A;
	codes[0x5B] = DasmARM_T5B;
	codes[0x5C] = DasmARM_T5C;
	codes[0x5D] = DasmARM_T5D;
	codes[0x5E] = DasmARM_T5E;
	codes[0x5F] = DasmARM_T5F;
	codes[0x60] = DasmARM_T60;
	codes[0x61] = DasmARM_T61;
	codes[0x62] = DasmARM_T62;
	codes[0x63] = DasmARM_T63;
	codes[0x64] = DasmARM_T64;
	codes[0x65] = DasmARM_T65;
	codes[0x66] = DasmARM_T66;
	codes[0x67] = DasmARM_T67;
	codes[0x68] = DasmARM_T68;
	codes[0x69] = DasmARM_T69;
	codes[0x6A] = DasmARM_T6A;
	codes[0x6B] = DasmARM_T6B;
	codes[0x6C] = DasmARM_T6C;
	codes[0x6D] = DasmARM_T6D;
	codes[0x6E] = DasmARM_T6E;
	codes[0x6F] = DasmARM_T6F;
	codes[0x70] = DasmARM_T70;
	codes[0x71] = DasmARM_T71;
	codes[0x72] = DasmARM_T72;
	codes[0x73] = DasmARM_T73;
	codes[0x74] = DasmARM_T74;
	codes[0x75] = DasmARM_T75;
	codes[0x76] = DasmARM_T76;
	codes[0x77] = DasmARM_T77;
	codes[0x78] = DasmARM_T78;
	codes[0x79] = DasmARM_T79;
	codes[0x7A] = DasmARM_T7A;
	codes[0x7B] = DasmARM_T7B;
	codes[0x7C] = DasmARM_T7C;
	codes[0x7D] = DasmARM_T7D;
	codes[0x7E] = DasmARM_T7E;
	codes[0x7F] = DasmARM_T7F;
	codes[0x80] = DasmARM_T80;
	codes[0x81] = DasmARM_T81;
	codes[0x82] = DasmARM_T82;
	codes[0x83] = DasmARM_T83;
	codes[0x84] = DasmARM_T84;
	codes[0x85] = DasmARM_T85;
	codes[0x86] = DasmARM_T86;
	codes[0x87] = DasmARM_T87;
	codes[0x88] = DasmARM_T88;
	codes[0x89] = DasmARM_T89;
	codes[0x8A] = DasmARM_T8A;
	codes[0x8B] = DasmARM_T8B;
	codes[0x8C] = DasmARM_T8C;
	codes[0x8D] = DasmARM_T8D;
	codes[0x8E] = DasmARM_T8E;
	codes[0x8F] = DasmARM_T8F;
	codes[0x90] = DasmARM_T90;
	codes[0x91] = DasmARM_T91;
	codes[0x92] = DasmARM_T92;
	codes[0x93] = DasmARM_T93;
	codes[0x94] = DasmARM_T94;
	codes[0x95] = DasmARM_T95;
	codes[0x96] = DasmARM_T96;
	codes[0x97] = DasmARM_T97;
	codes[0x98] = DasmARM_T98;
	codes[0x99] = DasmARM_T99;
	codes[0x9A] = DasmARM_T9A;
	codes[0x9B] = DasmARM_T9B;
	codes[0x9C] = DasmARM_T9C;
	codes[0x9D] = DasmARM_T9D;
	codes[0x9E] = DasmARM_T9E;
	codes[0x9F] = DasmARM_T9F;
	codes[0xA0] = DasmARM_TA0;
	codes[0xA1] = DasmARM_TA1;
	codes[0xA2] = DasmARM_TA2;
	codes[0xA3] = DasmARM_TA3;
	codes[0xA4] = DasmARM_TA4;
	codes[0xA5] = DasmARM_TA5;
	codes[0xA6] = DasmARM_TA6;
	codes[0xA7] = DasmARM_TA7;
	codes[0xA8] = DasmARM_TA8;
	codes[0xA9] = DasmARM_TA9;
	codes[0xAA] = DasmARM_TAA;
	codes[0xAB] = DasmARM_TAB;
	codes[0xAC] = DasmARM_TAC;
	codes[0xAD] = DasmARM_TAD;
	codes[0xAE] = DasmARM_TAE;
	codes[0xAF] = DasmARM_TAF;
	codes[0xB0] = DasmARM_TB0;
	codes[0xB1] = DasmARM_TB1;
	codes[0xB2] = DasmARM_TB2;
	codes[0xB3] = DasmARM_TB3;
	codes[0xB4] = DasmARM_TB4;
	codes[0xB5] = DasmARM_TB5;
	codes[0xB6] = DasmARM_TB6;
	codes[0xB7] = DasmARM_TB7;
	codes[0xB8] = DasmARM_TB8;
	codes[0xB9] = DasmARM_TB9;
	codes[0xBA] = DasmARM_TBA;
	codes[0xBB] = DasmARM_TBB;
	codes[0xBC] = DasmARM_TBC;
	codes[0xBD] = DasmARM_TBD;
	codes[0xBE] = DasmARM_TBE;
	codes[0xBF] = DasmARM_TBF;
	codes[0xC0] = DasmARM_TC0;
	codes[0xC1] = DasmARM_TC1;
	codes[0xC2] = DasmARM_TC2;
	codes[0xC3] = DasmARM_TC3;
	codes[0xC4] = DasmARM_TC4;
	codes[0xC5] = DasmARM_TC5;
	codes[0xC6] = DasmARM_TC6;
	codes[0xC7] = DasmARM_TC7;
	codes[0xC8] = DasmARM_TC8;
	codes[0xC9] = DasmARM_TC9;
	codes[0xCA] = DasmARM_TCA;
	codes[0xCB] = DasmARM_TCB;
	codes[0xCC] = DasmARM_TCC;
	codes[0xCD] = DasmARM_TCD;
	codes[0xCE] = DasmARM_TCE;
	codes[0xCF] = DasmARM_TCF;
	codes[0xD0] = DasmARM_TD0;
	codes[0xD1] = DasmARM_TD1;
	codes[0xD2] = DasmARM_TD2;
	codes[0xD3] = DasmARM_TD3;
	codes[0xD4] = DasmARM_TD4;
	codes[0xD5] = DasmARM_TD5;
	codes[0xD6] = DasmARM_TD6;
	codes[0xD7] = DasmARM_TD7;
	codes[0xD8] = DasmARM_TD8;
	codes[0xD9] = DasmARM_TD9;
	codes[0xDA] = DasmARM_TDA;
	codes[0xDB] = DasmARM_TDB;
	codes[0xDC] = DasmARM_TDC;
	codes[0xDD] = DasmARM_TDD;
	codes[0xDE] = DasmARM_TDE;
	codes[0xDF] = DasmARM_TDF;
	codes[0xE0] = DasmARM_TE0;
	codes[0xE1] = DasmARM_TE1;
	codes[0xE2] = DasmARM_TE2;
	codes[0xE3] = DasmARM_TE3;
	codes[0xE4] = DasmARM_TE4;
	codes[0xE5] = DasmARM_TE5;
	codes[0xE6] = DasmARM_TE6;
	codes[0xE7] = DasmARM_TE7;
	codes[0xE8] = DasmARM_TE8;
	codes[0xE9] = DasmARM_TE9;
	codes[0xEA] = DasmARM_TEA;
	codes[0xEB] = DasmARM_TEB;
	codes[0xEC] = DasmARM_TEC;
	codes[0xED] = DasmARM_TED;
	codes[0xEE] = DasmARM_TEE;
	codes[0xEF] = DasmARM_TEF;
	codes[0xF0] = DasmARM_TF0;
	codes[0xF1] = DasmARM_TF1;
	codes[0xF2] = DasmARM_TF2;
	codes[0xF3] = DasmARM_TF3;
	codes[0xF4] = DasmARM_TF4;
	codes[0xF5] = DasmARM_TF5;
	codes[0xF6] = DasmARM_TF6;
	codes[0xF7] = DasmARM_TF7;
	codes[0xF8] = DasmARM_TF8;
	codes[0xF9] = DasmARM_TF9;
	codes[0xFA] = DasmARM_TFA;
	codes[0xFB] = DasmARM_TFB;
	codes[0xFC] = DasmARM_TFC;
	codes[0xFD] = DasmARM_TFD;
	codes[0xFE] = DasmARM_TFE;
	codes[0xFF] = DasmARM_TFF;
}

Manage::DasmARM::~DasmARM()
{
	MemFree(this->codes);
	MemFree(this->codesT);
}

Text::CStringNN Manage::DasmARM::GetHeader(Bool fullRegs) const
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

Bool Manage::DasmARM::Disasm32(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs)
{
	UTF8Char sbuff[512];
	UInt8 buff[16];
	UInt32 oriPC;
	DasmARM_Sess sess;
	Text::StringBuilderUTF8 outStr;
	UOSInt initJmpCnt = jmpAddrs->GetCount();
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	MemCopyNO(&sess.regs, regs.Ptr(), sizeof(Manage::DasmARM::DasmARM_Regs));
	sess.regs.PC = *currInst;
	sess.regs.SP = *currStack;
	sess.regs.LR = *currFrame;
	sess.code = buff;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmARM::ET_NOT_END;
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

		outStr.ClearStr();
		outStr.AppendHex32(sess.regs.SP);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32(sess.regs.LR);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32(sess.regs.PC);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32(sess.regs.CPSR);
		outStr.AppendC(UTF8STRC(" "));
		if (fullRegs)
		{
			outStr.AppendHex32(sess.regs.R0);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R1);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R2);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R3);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R4);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R5);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R6);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R7);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R8);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R9);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.R10);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.FP);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.IP);
			outStr.AppendC(UTF8STRC(" "));
		}
		sess.sbuff = sbuff;
		oriPC = sess.regs.PC;
		if (sess.regs.CPSR & 0x20)
		{
			if (sess.memReader->ReadMemory(oriPC, buff, 2) != 2)
			{
				ret = false;
			}
			else
			{
				ret = this->codesT[buff[1]](sess);
			}
		}
		else
		{
			if (sess.memReader->ReadMemory(oriPC, buff, 4) != 4)
			{
				ret = false;
			}
			else
			{
				ret = this->codes[buff[3]](sess);
			}
		}
		if (!ret)
		{
			UOSInt buffSize;
			outStr.AppendC(UTF8STRC("Unknown opcode "));
			buffSize = sess.memReader->ReadMemory(oriPC, buff, 16);
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
		if (sess.endType == Manage::DasmARM::ET_JMP && (UInt32)sess.retAddr >= *blockStart && (UInt32)sess.retAddr <= sess.regs.PC)
		{
			UOSInt i;
			UInt32 minAddr = 0xffffffff;
			UInt32 jmpAddr;
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
				MemCopyNO(regs.Ptr(), &sess.regs, sizeof(Manage::DasmARM::DasmARM_Regs));
				return false;
			}
			sess.regs.PC = minAddr;
			sess.endType = Manage::DasmARM::ET_NOT_END;
		}
		else if (sess.endType != Manage::DasmARM::ET_NOT_END)
		{
			*currInst = sess.retAddr;
			*currStack = sess.regs.SP;
			*currFrame = sess.regs.LR;
			*blockEnd = sess.regs.PC;
			MemCopyNO(regs.Ptr(), &sess.regs, sizeof(Manage::DasmARM::DasmARM_Regs));
			return sess.endType != Manage::DasmARM::ET_EXIT;
		}
//		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = 0;
	}
}

NN<Manage::Dasm::Dasm_Regs> Manage::DasmARM::CreateRegs() const
{
	return MemAllocNN(Manage::DasmARM::DasmARM_Regs);
}

void Manage::DasmARM::FreeRegs(NN<Dasm_Regs> regs) const
{
	MemFreeNN(regs);
}

NN<Manage::DasmARM::DasmARM_Sess> Manage::DasmARM::CreateSess(NN<Manage::DasmARM::DasmARM_Regs> regs, UInt8 *code, UInt16 codeSegm)
{
	NN<Manage::DasmARM::DasmARM_Sess> sess = MemAllocNN(Manage::DasmARM::DasmARM_Sess);
	sess->code = code;
	sess->codeSegm = codeSegm;
	sess->codeHdlrs = (void**)this->codes;
	//sess->code0fHdlrs = (void**)this->codes0f;
	NEW_CLASS(sess->callAddrs, Data::ArrayListUInt32());
	NEW_CLASS(sess->jmpAddrs, Data::ArrayListUInt32());
	MemCopyNO(&sess->regs, regs.Ptr(), sizeof(Manage::DasmARM::DasmARM_Regs));
	return sess;
}

void Manage::DasmARM::DeleteSess(NN<Manage::DasmARM::DasmARM_Sess> sess)
{
	DEL_CLASS(sess->callAddrs);
	DEL_CLASS(sess->jmpAddrs);
	MemFreeNN(sess);
}

Bool Manage::DasmARM::DasmNext(NN<Manage::DasmARM::DasmARM_Sess> sess, UTF8Char *buff, UOSInt *outBuffSize)
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
