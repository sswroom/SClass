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
https://developer.arm.com/documentation/ddi0602/2022-12/Base-Instructions/TBNZ--Test-bit-and-Branch-if-Nonzero-?lang=en

*/

Int32 DasmARM64_ExtractSigned(UInt32 val, UOSInt startBit, UOSInt bitCnt)
{
	Int32 ival = (Int32)(val >> startBit);
	Int32 mask = 1 << (bitCnt - 1);
	if (ival & mask)
	{
		return -mask + (ival & (mask - 1));
	}
	else
	{
		return ival & (mask - 1);
	}
}

Bool __stdcall DasmARM64_IsEndFunc(const UTF8Char *funcName, UOSInt nameLen)
{
	if (Text::StrEndsWithC(funcName, nameLen, UTF8STRC("ExitThread")))
	{
		return true;
	}
	else if (Text::StrEndsWithC(funcName, nameLen, UTF8STRC("ExitProcess")))
	{
		return true;
	}
	else if (Text::StrEndsWithC(funcName, nameLen, UTF8STRC("RtlExitUserThread")))
	{
		return true;
	}
	else if (Text::StrIndexOfC(funcName, nameLen, UTF8STRC("(exit+0)")) != INVALID_INDEX)
	{
		return true;
	}
	return false;
}

UTF8Char *DasmARM64_ParseReg64(Manage::DasmARM64::DasmARM64_Sess* sess, UTF8Char *regName, UInt32 regNo, UInt64 **regPtr)
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
	case 16:
		*regPtr = &sess->regs.X16;
		return Text::StrConcatC(regName, UTF8STRC("x16"));
	case 17:
		*regPtr = &sess->regs.X17;
		return Text::StrConcatC(regName, UTF8STRC("x17"));
	case 18:
		*regPtr = &sess->regs.X18;
		return Text::StrConcatC(regName, UTF8STRC("x18"));
	case 19:
		*regPtr = &sess->regs.X19;
		return Text::StrConcatC(regName, UTF8STRC("x19"));
	case 20:
		*regPtr = &sess->regs.X20;
		return Text::StrConcatC(regName, UTF8STRC("x20"));
	case 21:
		*regPtr = &sess->regs.X21;
		return Text::StrConcatC(regName, UTF8STRC("x21"));
	case 22:
		*regPtr = &sess->regs.X22;
		return Text::StrConcatC(regName, UTF8STRC("x22"));
	case 23:
		*regPtr = &sess->regs.X23;
		return Text::StrConcatC(regName, UTF8STRC("x23"));
	case 24:
		*regPtr = &sess->regs.X24;
		return Text::StrConcatC(regName, UTF8STRC("x24"));
	case 25:
		*regPtr = &sess->regs.X25;
		return Text::StrConcatC(regName, UTF8STRC("x25"));
	case 26:
		*regPtr = &sess->regs.X26;
		return Text::StrConcatC(regName, UTF8STRC("x26"));
	case 27:
		*regPtr = &sess->regs.X27;
		return Text::StrConcatC(regName, UTF8STRC("x27"));
	case 28:
		*regPtr = &sess->regs.X28;
		return Text::StrConcatC(regName, UTF8STRC("x28"));
	case 29:
		*regPtr = &sess->regs.X29;
		return Text::StrConcatC(regName, UTF8STRC("fp"));
	case 30:
		*regPtr = &sess->regs.LR;
		return Text::StrConcatC(regName, UTF8STRC("lr"));
	case 31:
		*regPtr = &sess->regs.SP;
		return Text::StrConcatC(regName, UTF8STRC("sp"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char* DasmARM64_ParseReg32(Manage::DasmARM64::DasmARM64_Sess* sess, UTF8Char* regName, UInt32 regNo, UInt32** regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = (UInt32*)& sess->regs.X0;
		return Text::StrConcatC(regName, UTF8STRC("w0"));
	case 1:
		*regPtr = (UInt32*)&sess->regs.X1;
		return Text::StrConcatC(regName, UTF8STRC("w1"));
	case 2:
		*regPtr = (UInt32*)&sess->regs.X2;
		return Text::StrConcatC(regName, UTF8STRC("w2"));
	case 3:
		*regPtr = (UInt32*)&sess->regs.X3;
		return Text::StrConcatC(regName, UTF8STRC("w3"));
	case 4:
		*regPtr = (UInt32*)&sess->regs.X4;
		return Text::StrConcatC(regName, UTF8STRC("w4"));
	case 5:
		*regPtr = (UInt32*)&sess->regs.X5;
		return Text::StrConcatC(regName, UTF8STRC("w5"));
	case 6:
		*regPtr = (UInt32*)&sess->regs.X6;
		return Text::StrConcatC(regName, UTF8STRC("w6"));
	case 7:
		*regPtr = (UInt32*)&sess->regs.X7;
		return Text::StrConcatC(regName, UTF8STRC("w7"));
	case 8:
		*regPtr = (UInt32*)&sess->regs.X8;
		return Text::StrConcatC(regName, UTF8STRC("w8"));
	case 9:
		*regPtr = (UInt32*)&sess->regs.X9;
		return Text::StrConcatC(regName, UTF8STRC("w9"));
	case 10:
		*regPtr = (UInt32*)&sess->regs.X10;
		return Text::StrConcatC(regName, UTF8STRC("w10"));
	case 11:
		*regPtr = (UInt32*)&sess->regs.X11;
		return Text::StrConcatC(regName, UTF8STRC("w11"));
	case 12:
		*regPtr = (UInt32*)&sess->regs.X12;
		return Text::StrConcatC(regName, UTF8STRC("w12"));
	case 13:
		*regPtr = (UInt32*)&sess->regs.X13;
		return Text::StrConcatC(regName, UTF8STRC("w13"));
	case 14:
		*regPtr = (UInt32*)&sess->regs.X14;
		return Text::StrConcatC(regName, UTF8STRC("w14"));
	case 15:
		*regPtr = (UInt32*)&sess->regs.X15;
		return Text::StrConcatC(regName, UTF8STRC("w15"));
	case 16:
		*regPtr = (UInt32*)&sess->regs.X16;
		return Text::StrConcatC(regName, UTF8STRC("w16"));
	case 17:
		*regPtr = (UInt32*)&sess->regs.X17;
		return Text::StrConcatC(regName, UTF8STRC("w17"));
	case 18:
		*regPtr = (UInt32*)&sess->regs.X18;
		return Text::StrConcatC(regName, UTF8STRC("w18"));
	case 19:
		*regPtr = (UInt32*)&sess->regs.X19;
		return Text::StrConcatC(regName, UTF8STRC("w19"));
	case 20:
		*regPtr = (UInt32*)&sess->regs.X20;
		return Text::StrConcatC(regName, UTF8STRC("w20"));
	case 21:
		*regPtr = (UInt32*)&sess->regs.X21;
		return Text::StrConcatC(regName, UTF8STRC("w21"));
	case 22:
		*regPtr = (UInt32*)&sess->regs.X22;
		return Text::StrConcatC(regName, UTF8STRC("w22"));
	case 23:
		*regPtr = (UInt32*)&sess->regs.X23;
		return Text::StrConcatC(regName, UTF8STRC("w23"));
	case 24:
		*regPtr = (UInt32*)&sess->regs.X24;
		return Text::StrConcatC(regName, UTF8STRC("w24"));
	case 25:
		*regPtr = (UInt32*)&sess->regs.X25;
		return Text::StrConcatC(regName, UTF8STRC("w25"));
	case 26:
		*regPtr = (UInt32*)&sess->regs.X26;
		return Text::StrConcatC(regName, UTF8STRC("w26"));
	case 27:
		*regPtr = (UInt32*)&sess->regs.X27;
		return Text::StrConcatC(regName, UTF8STRC("w27"));
	case 28:
		*regPtr = (UInt32*)&sess->regs.X28;
		return Text::StrConcatC(regName, UTF8STRC("w28"));
	case 29:
		*regPtr = (UInt32*)&sess->regs.X29;
		return Text::StrConcatC(regName, UTF8STRC("fp"));
	case 30:
		*regPtr = (UInt32*)&sess->regs.LR;
		return Text::StrConcatC(regName, UTF8STRC("lr"));
	case 31:
		*regPtr = (UInt32*)&sess->regs.SP;
		return Text::StrConcatC(regName, UTF8STRC("sp"));
	default:
		*regName = 0;
		return regName;
	}
}

UInt32 DasmARM64_ParseImmA32(UInt8 rot, UInt8 val)
{
	if (rot == 0)
		return val;
	rot = (UInt8)(rot << 1);
	if (rot < 8)
	{
		return (UInt32)(val >> rot) | (((UInt32)val) << (32 - rot));
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
	return false;
}

Bool __stdcall DasmARM64_0B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	Int32 imm26 = DasmARM64_ExtractSigned(code, 0, 26);
	UInt64 addr = (UInt64)((Int64)sess->regs.PC + (imm26 * 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("b "));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	if (sess->addrResol)
	{
		UTF8Char* sptr;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, addr);
		if (sess->sbuff == 0)
		{
			sess->sbuff = sptr;
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC = addr;
	sess->jmpAddrs->Add(sess->regs.PC);
	sess->endType = Manage::DasmARM64::ET_JMP;
	sess->retAddr = sess->regs.PC;
	UOSInt i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		UInt64 addr = sess->jmpAddrs->GetItem(i);
		if (addr >= sess->regs.PC && addr <= sess->regs.PC + 16)
		{
			sess->endType = Manage::DasmARM64::ET_NOT_END;
			break;
		}
	}
	return true;
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
	return false;
}

Bool __stdcall DasmARM64_1B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFE0FFE0) == 0x2A0003E0)
	{
		UInt32 rm = (code >> 16) & 0x1F;
		UInt32 rd = code & 0x1F;
		UInt32 *wd;
		UInt32 *wm;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rm, &wm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*wd = *wm;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_2B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	UInt32 rt = code & 0x1F;
	Int32 imm19 = (Int32)((code >> 5) & 0x7FFFF);
	UInt32* wt;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cbz "));
	sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rt, &wt);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + (imm19 * 4)));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	return true;
}

Bool __stdcall DasmARM64_35(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	UInt32 rt = code & 0x1F;
	Int32 imm19 = (Int32)((code >> 5) & 0x7FFFF);
	UInt32* wt;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cbnz "));
	sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rt, &wt);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + (imm19 * 4)));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	return true;
}

Bool __stdcall DasmARM64_36(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_37(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	UInt32 rt = code & 0x1F;
	UInt32 imm14 = (code >> 5) & 0x3FFF;
	UInt32 b40 = (code >> 19) & 0x1F;
	UInt64* xt;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("tbnz "));
	sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
	sess->sbuff = Text::StrHexVal32V(sess->sbuff, b40);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, sess->regs.PC + (imm14 * 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	return true;
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
	return false;
}

Bool __stdcall DasmARM64_3B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	return false;
}

Bool __stdcall DasmARM64_4B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFF800000) == 0x52800000)
	{
		UInt32 rd = code & 0x1F;
		UInt32 imm16 = (code >> 5) & 0xFFFF;
		UInt32 hw = (code >> 21) & 3;
		UInt32* wd;
		if (hw != 0)
		{
			imm16 = imm16 << 16;
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
		sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm16);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*wd = imm16;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_53(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFC00000) == 0x53000000)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imms = (code >> 10) & 0x3F;
		UInt32 immr = (code >> 16) & 0x3F;
		UInt32* wd;
		UInt32* wn;
		if (immr == 0 && imms == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("uxtb "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			*wd = *wn & 0xff;
		}
		else if (immr == 0 && imms == 15)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("uxth "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			*wd = *wn & 0xffff;
		}
		else if (imms == 31)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsr "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#"));
			sess->sbuff = Text::StrUInt32(sess->sbuff, immr);
			*wd = *wn >> immr;
		}
		else if (imms + 1 == immr)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#"));
			sess->sbuff = Text::StrUInt32(sess->sbuff, 31 - imms);
			*wd = *wn << (31 - imms);
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ubfm "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#"));
			sess->sbuff = Text::StrUInt32(sess->sbuff, immr);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#"));
			sess->sbuff = Text::StrUInt32(sess->sbuff, imms);
			//////////////////////////////////////////////
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_54(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0x10) == 0)
	{
		UInt32 cond = code & 0xF;
		UInt32 imm19 = (code >> 5) & 0x7FFFF;
		Int32 ofst;
		if (imm19 & 0x40000)
		{
			ofst = -0x40000 + (Int32)(imm19 & 0x3FFFF);
		}
		else
		{
			ofst = (Int32)imm19;
		}
		switch (cond)
		{
		case 0:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("beq "));
			break;
		case 1:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bne "));
			break;
		case 2:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bcs "));
			break;
		case 3:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bcc "));
			break;
		case 4:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bmi "));
			break;
		case 5:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bpl "));
			break;
		case 6:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bvs "));
			break;
		case 7:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bvc "));
			break;
		case 8:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bhi "));
			break;
		case 9:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bls "));
			break;
		case 10:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bge "));
			break;
		case 11:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blt "));
			break;
		case 12:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bgt "));
			break;
		case 13:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ble "));
			break;
		case 14:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bal "));
			break;
		case 15:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bnv "));
			break;
		}
		UInt64 addr = (UInt64)((Int64)sess->regs.PC + (ofst * 4));
		sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
		if (sess->addrResol)
		{
			UTF8Char* sptr;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
			sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, addr);
			if (sess->sbuff == 0)
			{
				sess->sbuff = sptr;
			}
			if (DasmARM64_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
			{
				sess->endType = Manage::DasmARM64::ET_EXIT;
				sess->retAddr = sess->regs.PC;
			}
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		sess->jmpAddrs->Add(addr);
		return true;
	}
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFFFFC00) == 0x5AC02000)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 *wd;
		UInt32 *wn;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("abs "));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if (*wn & 0x80000000)
		{
			*wd = (~*wn) + 1;
		}
		else
		{
			*wd = *wn;
		}
		sess->regs.PC += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_5B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	return false;
}

Bool __stdcall DasmARM64_6B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0x800000) == 0)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xFFF;
		UInt32 *wd;
		UInt32 *wn;
		if (code & 0x400000)
		{
			imm12 <<= 12;
		}
		if (rd == 0x1f)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->regs.PC += 4;
			return true;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subs "));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rd, &wd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->regs.PC += 4;
			*wd = *wn - imm12;
			return true;
		}
	}
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
	return false;
}

Bool __stdcall DasmARM64_7B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	return false;
}

Bool __stdcall DasmARM64_83(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_84(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_85(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_86(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_87(Manage::DasmARM64::DasmARM64_Sess *sess)
{
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
	return false;
}

Bool __stdcall DasmARM64_8B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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

Bool __stdcall DasmARM64_90(Manage::DasmARM64::DasmARM64_Sess* sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	UInt32 rd = code & 0x1F;
	Int32 imm = (DasmARM64_ExtractSigned(code, 5, 19) << 2) | (Int32)((code >> 29) & 3);
	UInt64 addr = (UInt64)(((Int64)sess->regs.PC & ~0xfffLL) + imm);
	UInt64 *xd;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adrp "));
	sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	if (sess->addrResol)
	{
		UTF8Char* sptr;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, addr);
		if (sess->sbuff == 0)
		{
			sess->sbuff = sptr;
		}
	}
	*xd = addr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	return true;
}

Bool __stdcall DasmARM64_91(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFF800000) == 0x91000000)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xFFF;
		UInt64* xd;
		UInt64* xn;
		if (((code & 0x400000) == 0) && imm12 == 0 && (rd == 31 || rn == 31))
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
			*xd = *xn;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = Text::StrUInt32(sess->sbuff, imm12);
			if (code & 0x400000)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",LSL #12"));
				imm12 = imm12 << 12;
			}
			*xd = *xn + imm12;
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		return true;
	}
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	Int32 imm26 = (Int32)code & 0x3FFFFFF;
	if (imm26 & 0x2000000)
	{
		imm26 = -0x2000000 + (imm26 & 0x1FFFFFF);
	}
	UInt64 addr = (UInt64)((Int64)sess->regs.PC + (imm26 * 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bl "));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	if (sess->addrResol)
	{
		UTF8Char* sptr;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, addr);
		if (sess->sbuff == 0)
		{
			sess->sbuff = sptr;
		}
		if (DasmARM64_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
		{
			sess->endType = Manage::DasmARM64::ET_EXIT;
			sess->retAddr = sess->regs.PC;
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	sess->callAddrs->Add(addr);
	return true;
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
	return false;
}

Bool __stdcall DasmARM64_9B(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFC00000) == 0xA8C00000)
	{
		UInt32 rt = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 rt2 = (code >> 10) & 0x1F;
		UInt32 imm7 = (code >> 15) & 0x7F;
		Int32 addrOfst;
		UInt64* xt;
		UInt64* xn;
		UInt64* xt2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldp "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt2, &xt2);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
		if (imm7 & 0x40)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],#-0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, (0x40 - (imm7 & 0x3F)) * 8);
			addrOfst = (-0x40 + (Int32)(imm7 & 0x3F)) * 8;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],#0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm7 * 8);
			addrOfst = (Int32)imm7 * 8;
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*xt = sess->memReader->ReadMemUInt64(*xn);
		*xt2 = sess->memReader->ReadMemUInt64(*xn + 8);
		*xn = (UInt64)((Int64)*xn + addrOfst);
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_A9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFF400000) == 0xA9400000)
	{
		Bool wback = (code & 0x800000) != 0;
		UInt32 rt = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 rt2 = (code >> 10) & 0x1F;
		UInt32 imm7 = (code >> 15) & 0x7F;
		Int32 addrOfst;
		UInt64* xt;
		UInt64* xn;
		UInt64* xt2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldp "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt2, &xt2);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
		if (imm7 & 0x40)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#-0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, (0x40 - (imm7 & 0x3F)) * 8);
			addrOfst = (-0x40 + (Int32)(imm7 & 0x3F)) * 8;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm7 * 8);
			addrOfst = (Int32)imm7 * 8;
		}
		if (wback)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]!\r\n"));
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
		}
		sess->regs.PC += 4;
		if (wback)
		{
			*xn = (UInt64)((Int64)*xn + addrOfst);
			*xt = sess->memReader->ReadMemUInt64(*xn);
			*xt2 = sess->memReader->ReadMemUInt64(*xn + 8);
		}
		else
		{
			UInt64 addr = (UInt64)((Int64)*xn + addrOfst);
			*xt = sess->memReader->ReadMemUInt64(addr);
			*xt2 = sess->memReader->ReadMemUInt64(addr + 8);	
		}
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_AA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFE0FFE0) == 0xAA0003E0)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rm = (code >> 16) & 0x1F;
		UInt64* xd;
		UInt64 *xm;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rm, &xm);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*xd = *xm;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_AB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	UInt32 rt = code & 0x1F;
	Int32 imm19 = (Int32)((code >> 5) & 0x7FFFF);
	UInt64* xt;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cbz "));
	sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, (UInt64)((Int64)sess->regs.PC + (imm19 * 4)));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	return true;
}

Bool __stdcall DasmARM64_B5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	UInt32 rt = code & 0x1F;
	Int32 imm19 = (Int32)((code >> 5) & 0x7FFFF);
	UInt64* xt;
	UInt64 addr = (UInt64)((Int64)sess->regs.PC + (imm19 * 4));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cbnz "));
	sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	if (sess->addrResol)
	{
		UTF8Char* sptr;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, addr);
		if (sess->sbuff == 0)
		{
			sess->sbuff = sptr;
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.PC += 4;
	return true;
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFC00000) == 0xB9000000)
	{
		UInt32 rt = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xfff;
		UInt32 *wn;
		UInt32 *wt;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("str "));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rt, &wt);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
		sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12 << 2);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]"));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		return true;
	}
	else if ((code & 0xFFC00000) == 0xB9400000)
	{
		UInt32 rt = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xfff;
		UInt32* wn;
		UInt32* wt;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldr "));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rt, &wt);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg32(sess, sess->sbuff, rn, &wn);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
		sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12 << 2);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]"));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*wt = sess->memReader->ReadMemUInt32(*wn + imm12 * 4);
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_BA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_BB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	return false;
}

Bool __stdcall DasmARM64_CB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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

Bool __stdcall DasmARM64_D1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_D2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFF800000) == 0xD2800000)
	{
		UInt32 rd = code & 0x1F;
		UInt64 imm16 = (code >> 5) & 0xFFFF;
		UInt32 hw = (code >> 21) & 3;
		UInt64* xd;
		if (hw != 0)
		{
			imm16 = imm16 << (16 * hw);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
		sess->sbuff = Text::StrHexVal64V(sess->sbuff, imm16);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*xd = imm16;
		return true;
	}
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

Bool __stdcall DasmARM64_D5(Manage::DasmARM64::DasmARM64_Sess* sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if (code == 0xD50323FF)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("autibsp\r\n"));
		sess->regs.PC += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_D6(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFFFFC1F) == 0xD65F0000)
	{
		UInt32 reg = (code >> 5) & 0x1F;
		if (reg == 30)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ret"));
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ret "));
			sess->sbuff = Text::StrUInt32(sess->sbuff, reg);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC = sess->regs.indexes[reg];
		sess->endType = Manage::DasmARM64::ET_FUNC_RET;
		sess->retAddr = sess->regs.PC;
		return true;
	}
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFFFFC00) == 0xDAC02000)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt64 *xd;
		UInt64 *xn;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("abs "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if (*xn & 0x8000000000000000)
		{
			*xd = (~*xn) + 1;
		}
		else
		{
			*xd = *xn;
		}
		sess->regs.PC += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmARM64_DB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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
	return false;
}

Bool __stdcall DasmARM64_E1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E2(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E3(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E4(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_E5(Manage::DasmARM64::DasmARM64_Sess *sess)
{
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
	return false;
}

Bool __stdcall DasmARM64_E9(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_EA(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
}

Bool __stdcall DasmARM64_EB(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	return false;
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

Bool __stdcall DasmARM64_F1(Manage::DasmARM64::DasmARM64_Sess *sess)
{
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0x800000) == 0)
	{
		UInt32 rd = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xFFF;
		UInt64* xd;
		UInt64* xn;
		if (code & 0x400000)
		{
			imm12 <<= 12;
		}
		if (rd == 0x1f)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->regs.PC += 4;
			return true;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subs "));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rd, &xd);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
			sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
			sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->regs.PC += 4;
			*xd = *xn - imm12;
			return true;
		}
	}
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
	UInt32 code = ReadUInt32(sess->codeBuff);
	if ((code & 0xFFC00000) == 0xF9000000)
	{
		UInt32 rt = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xfff;
		UInt64* xn;
		UInt64* xt;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("str "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
		sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12 << 3);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]"));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		return true;
	}
	else if ((code & 0xFFC00000) == 0xF9400000)
	{
		UInt32 rt = code & 0x1F;
		UInt32 rn = (code >> 5) & 0x1F;
		UInt32 imm12 = (code >> 10) & 0xfff;
		UInt64* xn;
		UInt64* xt;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldr "));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rt, &xt);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", ["));
		sess->sbuff = DasmARM64_ParseReg64(sess, sess->sbuff, rn, &xn);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(",#0x"));
		sess->sbuff = Text::StrHexVal32V(sess->sbuff, imm12 << 3);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]"));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.PC += 4;
		*xt = sess->memReader->ReadMemUInt64(*xn + imm12 * 8);
		return true;
	}
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
	codes[0x15] = DasmARM64_14;
	codes[0x16] = DasmARM64_14;
	codes[0x17] = DasmARM64_14;
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
	codes[0x95] = DasmARM64_94;
	codes[0x96] = DasmARM64_94;
	codes[0x97] = DasmARM64_94;
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
	codes[0xB0] = DasmARM64_90;
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
	codes[0xD0] = DasmARM64_90;
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
	codes[0xF0] = DasmARM64_90;
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
}

Manage::DasmARM64::~DasmARM64()
{
	MemFree(this->codes);
}

Text::CString Manage::DasmARM64::GetHeader(Bool fullRegs)
{
	if (fullRegs)
	{
		return CSTR(" SP       LR       PC       CPSR     X0       X1       X2       X3       X4       X5       X6       X7       X8       X9       X10      X11       X12       Code");
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
	Text::StringBuilderUTF8 outStr;
	UOSInt initJmpCnt = jmpAddrs->GetCount();
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
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	sess.codeBuff = buff;
	*blockStart = sess.regs.PC;

	while (true)
	{
		Bool ret;

		outStr.ClearStr();
		outStr.AppendHex64(sess.regs.SP);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex64(sess.regs.LR);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex64(sess.regs.PC);
		outStr.AppendC(UTF8STRC(" "));
		if (fullRegs)
		{
			outStr.AppendHex64(sess.regs.X0);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X1);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X2);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X3);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X4);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X5);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X6);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X7);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X8);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X9);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X10);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X11);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X12);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X13);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X14);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X15);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X16);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X17);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X18);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X19);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X20);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X21);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X22);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X23);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X24);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X25);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X26);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X27);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X28);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex64(sess.regs.X29);
			outStr.AppendC(UTF8STRC(" "));
		}
		sess.sbuff = sbuff;
		oriPC = sess.regs.PC;
		if (sess.memReader->ReadMemory(oriPC, buff, 4) != 4)
		{
			ret = false;
		}
		else
		{
			ret = this->codes[buff[3]](&sess);
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
			writer->WriteStrC(outStr.ToString(), outStr.GetLength());
			return false;
		}
		outStr.AppendSlow(sbuff);
		writer->WriteStrC(outStr.ToString(), outStr.GetLength());
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
