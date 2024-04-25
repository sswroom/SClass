#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Manage/DasmX86_16.h"
#include "Text/MyString.h"
//#include <memory.h>
#ifndef __GNUC__
#pragma warning( disable : 4311 4312)
#endif

#define BIT1 (-1)

UTF8Char *DasmX86_16_AppInt32(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, Int32 val)
{
	if (val >= 0 && (((val % 100) == 0) || val < 200))
	{
		buff = Text::StrInt32(buff, val);
	}
	else
	{
		if (((UInt32)val) > 0x9fffffff)
		{
			buff = Text::StrConcatC(buff, UTF8STRC("0"));
		}
		buff = Text::StrHexVal32(buff, (UInt32)val);
		buff = Text::StrConcatC(buff, UTF8STRC("h"));
	}
	return buff;
}


UTF8Char *DasmX86_16_AppInt16(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, UInt16 val)
{
	if (((val % 100) == 0) || val < 200)
	{
		buff = Text::StrInt32(buff, val);
	}
	else
	{
		if (val > 0x9fff)
		{
			buff = Text::StrConcatC(buff, UTF8STRC("0"));
		}
		buff = Text::StrHexVal16(buff, val);
		buff = Text::StrConcatC(buff, UTF8STRC("h"));
	}
	return buff;
}

UTF8Char *DasmX86_16_AppInt8(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, UInt8 val)
{
	if (val < 10)
	{
		buff = Text::StrInt32(buff, val);
	}
	else
	{
		if (val > 0x9f)
		{
			buff = Text::StrConcatC(buff, UTF8STRC("0"));
		}
		buff = Text::StrHexByte(buff, val);
		buff = Text::StrConcatC(buff, UTF8STRC("h"));
	}
	return buff;
}

UTF8Char *DasmX86_16_AppAddrN(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, UInt32 addr)
{
	if ((addr & 0xffff) > 0x9fff)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("0"));
	}
	buff = Text::StrHexVal16(buff, (UInt16)addr);
	buff = Text::StrConcatC(buff, UTF8STRC("h"));
	return buff;
}

UTF8Char *DasmX86_16_AppAddrM(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, UInt16 addr)
{
	if ((sess->thisStatus & 7) == 1)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("CS:"));
	}
	else if ((sess->thisStatus & 7) == 2)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("DS:"));
	}
	else if ((sess->thisStatus & 7) == 3)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("ES:"));
	}
	else if ((sess->thisStatus & 7) == 4)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("SS:"));
	}
	else if ((sess->thisStatus & 7) == 5)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("FS:"));
	}
	else if ((sess->thisStatus & 7) == 6)
	{
		buff = Text::StrConcatC(buff, UTF8STRC("GS:"));
	}
	buff = Text::StrConcatC(buff, UTF8STRC("["));
	buff = DasmX86_16_AppAddrN(sess, buff, addr);
	buff = Text::StrConcatC(buff, UTF8STRC("]"));
	return buff;
}

UTF8Char *DasmX86_16_ParseSegReg16(UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("ES"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("CS"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("SS"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("DS"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("FS"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("GS"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("??"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("??"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_16_ParseReg32(UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("EAX"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("ECX"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("EDX"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("EBX"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("ESP"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("EBP"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("ESI"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("EDI"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_16_ParseReg16(UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("AX"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("CX"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("DX"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("BX"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("SP"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("BP"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("SI"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("DI"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_16_ParseReg8(UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("AL"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("CL"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("DL"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("BL"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("AH"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("CH"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("DH"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("BH"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_16_ParseAddr16(Manage::DasmX86_16::DasmX86_16_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->code[sess->regs.IP];
	*reg = (b >> 3) & 7;
	if ((b >> 6) == 3)
		return memName;
	if ((sess->thisStatus & 7) == 1)
	{
		memName = Text::StrConcatC(memName, UTF8STRC("CS:"));
	}
	else if ((sess->thisStatus & 7) == 2)
	{
		memName = Text::StrConcatC(memName, UTF8STRC("DS:"));
	}
	else if ((sess->thisStatus & 7) == 3)
	{
		memName = Text::StrConcatC(memName, UTF8STRC("ES:"));
	}
	else if ((sess->thisStatus & 7) == 4)
	{
		memName = Text::StrConcatC(memName, UTF8STRC("SS:"));
	}
	else if ((sess->thisStatus & 7) == 5)
	{
		memName = Text::StrConcatC(memName, UTF8STRC("FS:"));
	}
	else if ((sess->thisStatus & 7) == 6)
	{
		memName = Text::StrConcatC(memName, UTF8STRC("GS:"));
	}

	sess->regs.IP++;

	if (((b >> 6) == 0) && ((b & 7) == 6))
	{
		memName = Text::StrConcatC(memName, UTF8STRC("["));
		memName = DasmX86_16_AppAddrN(sess, memName, *(UInt16*)&sess->code[sess->regs.IP]);
		memName = Text::StrConcatC(memName, UTF8STRC("]"));
		sess->regs.IP = (UInt16)(sess->regs.IP + 2);
		return memName;
	}
	switch (b & 7)
	{
	case 0:
		memName = Text::StrConcatC(memName, UTF8STRC("[BX+SI"));
		break;
	case 1:
		memName = Text::StrConcatC(memName, UTF8STRC("[BX+DI"));
		break;
	case 2:
		memName = Text::StrConcatC(memName, UTF8STRC("[BP+SI"));
		break;
	case 3:
		memName = Text::StrConcatC(memName, UTF8STRC("[BP+DI"));
		break;
	case 4:
		memName = Text::StrConcatC(memName, UTF8STRC("[SI"));
		break;
	case 5:
		memName = Text::StrConcatC(memName, UTF8STRC("[DI"));
		break;
	case 6:
		memName = Text::StrConcatC(memName, UTF8STRC("[BP"));
		break;
	case 7:
		memName = Text::StrConcatC(memName, UTF8STRC("[BX"));
		break;
	}
	if ((b >> 6) == 1)
	{
		b = sess->code[sess->regs.IP];
		if (b & 0x80)
		{
			memName = Text::StrInt32(memName, (Int8)b);
		}
		else
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+"));
			memName = Text::StrInt32(memName, b);
		}
		sess->regs.IP++;
	}
	else if ((b >> 6) == 2)
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 2);
		if (val & 0x8000)
		{
			memName = Text::StrInt32(memName, (Int16)val);
		}
		else
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+"));
			memName = Text::StrInt32(memName, val);
		}
	}
	memName = Text::StrConcatC(memName, UTF8STRC("]"));
	return memName;
}

UTF8Char *DasmX86_16_ParseModRM32(Manage::DasmX86_16::DasmX86_16_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->code[sess->regs.IP];
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		memName = DasmX86_16_ParseReg32(memName, b & 7);
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	else
	{
		memName = DasmX86_16_ParseAddr16(sess, Text::StrConcatC(memName, UTF8STRC("DWORD PTR ")), reg);
	}
	return memName;
}

UTF8Char *DasmX86_16_ParseModRM16(Manage::DasmX86_16::DasmX86_16_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->code[sess->regs.IP];
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		memName = DasmX86_16_ParseReg16(memName, b & 7);
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	else
	{
		memName = DasmX86_16_ParseAddr16(sess, Text::StrConcatC(memName, UTF8STRC("WORD PTR ")), reg);
	}
	return memName;
}

UTF8Char *DasmX86_16_ParseModRM8(Manage::DasmX86_16::DasmX86_16_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->code[sess->regs.IP];
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		memName = DasmX86_16_ParseReg8(memName, b & 7);
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	else
	{
		memName = DasmX86_16_ParseAddr16(sess, Text::StrConcatC(memName, UTF8STRC("BYTE PTR ")), reg);
	}
	return memName;
}

UTF8Char *DasmX86_16_IntName(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, UInt8 intNo)
{
	buff = Text::StrConcatC(buff, UTF8STRC(" ;"));
	switch (intNo)
	{
	case 0x0:
		buff = Text::StrConcatC(buff, UTF8STRC("Divide by zero"));
		break;
	case 0x1:
		buff = Text::StrConcatC(buff, UTF8STRC("Single Step"));
		break;
	case 0x2:
		buff = Text::StrConcatC(buff, UTF8STRC("NMI"));
		break;
	case 0x3:
		buff = Text::StrConcatC(buff, UTF8STRC("Breakpoints"));
		break;
	case 0x4:
		buff = Text::StrConcatC(buff, UTF8STRC("Divide overthrow"));
		break;
	case 0x5:
		buff = Text::StrConcatC(buff, UTF8STRC("Print screen"));
		break;
	case 0x6:
		buff = Text::StrConcatC(buff, UTF8STRC("Invalid Opcode"));
		break;
	case 0x7:
		buff = Text::StrConcatC(buff, UTF8STRC("Device not available"));
		break;
	case 0x8:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 0 - Clock Tick"));
		break;
	case 0x9:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 1 - Keyboard Action"));
		break;
	case 0xa:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 2 - EOI"));
		break;
	case 0xb:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 3 - COM2"));
		break;
	case 0xc:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 4 - COM1"));
		break;
	case 0xd:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 5 - Sound Blaster"));
		break;
	case 0xe:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 6 - Floppy"));
		break;
	case 0xf:
		buff = Text::StrConcatC(buff, UTF8STRC("IRQ 7 - Sound Blaster"));
		break;
	case 0x10:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Video Display Service - "));
		switch (((UInt8*)&sess->regs.EAX)[1])
		{
		case 0x0:
			buff = Text::StrConcatC(buff, UTF8STRC("Set Screen Mode"));
			break;
		case 0x4:
			buff = Text::StrConcatC(buff, UTF8STRC("Read Light-Pen Position"));
			break;
		case 0x5:
			buff = Text::StrConcatC(buff, UTF8STRC("Set Active Display Page"));
			break;
		case 0x6:
			buff = Text::StrConcatC(buff, UTF8STRC("Scroll Window Up"));
			break;
		case 0x4F:
			buff = Text::StrConcatC(buff, UTF8STRC("Extended - "));
			switch (sess->regs.EAX & 0xff)
			{
			case 2:
				buff = Text::StrConcatC(buff, UTF8STRC("Set Screen Mode"));
				break;
			case 3:
				buff = Text::StrConcatC(buff, UTF8STRC("Get Screen Mode"));
				break;
			case 5:
				buff = Text::StrConcatC(buff, UTF8STRC("Bank Control"));
				break;
			}
			break;
		}
		break;
	case 0x11:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Equipment List Service - "));
		break;
	case 0x12:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Memory Size Service - "));
		break;
	case 0x13:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Disk Service - "));
		switch (((UInt8*)&sess->regs.EAX)[1])
		{
		case 0x0:
			buff = Text::StrConcatC(buff, UTF8STRC("Reset the Information of the disk"));
			break;
		case 0x1:
			buff = Text::StrConcatC(buff, UTF8STRC("Read the State"));
			break;
		case 0x2:
			buff = Text::StrConcatC(buff, UTF8STRC("Read Sector"));
			break;
		case 0x3:
			buff = Text::StrConcatC(buff, UTF8STRC("Write Sector"));
			break;
		case 0x4:
			buff = Text::StrConcatC(buff, UTF8STRC("Verify Sector"));
			break;
		case 0x5:
			buff = Text::StrConcatC(buff, UTF8STRC("Format Disk Track"));
			break;
		case 0x6:
			buff = Text::StrConcatC(buff, UTF8STRC("Format Disk Track and Set Bad Sector Flags"));
			break;
		case 0x7:
			buff = Text::StrConcatC(buff, UTF8STRC("Format Drive"));
			break;
		case 0x8:
			buff = Text::StrConcatC(buff, UTF8STRC("Get Drive Parameters"));
			break;
		case 0x9:
			buff = Text::StrConcatC(buff, UTF8STRC("Initialize Hard Disk Base Tables"));
			break;
		case 0xa:
			buff = Text::StrConcatC(buff, UTF8STRC("Read Long Sector"));
			break;
		case 0xb:
			buff = Text::StrConcatC(buff, UTF8STRC("Write Long Sector"));
			break;
		case 0xc:
			buff = Text::StrConcatC(buff, UTF8STRC("Seek to Cylinder"));
			break;
		case 0xd:
			buff = Text::StrConcatC(buff, UTF8STRC("Alternate Hard Disk Reset"));
			break;
		case 0x10:
			buff = Text::StrConcatC(buff, UTF8STRC("Test for Drive Ready"));
			break;
		case 0x11:
			buff = Text::StrConcatC(buff, UTF8STRC("Recalibrate Drive"));
			break;
		case 0x14:
			buff = Text::StrConcatC(buff, UTF8STRC("Controller Diagnostics"));
			break;
		case 0x15:
			buff = Text::StrConcatC(buff, UTF8STRC("Get Disk Type"));
			break;
		case 0x16:
			buff = Text::StrConcatC(buff, UTF8STRC("Get Floppy Disk Change Status"));
			break;
		case 0x17:
			buff = Text::StrConcatC(buff, UTF8STRC("Set Floppy Type"));
			break;
		case 0x18:
			buff = Text::StrConcatC(buff, UTF8STRC("Set Media Type For Format"));
			break;
		case 0x19:
			buff = Text::StrConcatC(buff, UTF8STRC("Park Heads"));
			break;
		case 0x1a:
			buff = Text::StrConcatC(buff, UTF8STRC("Format ESDI"));
			break;
		}
		break;
	case 0x14:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Communications Service - "));
		break;
	case 0x15:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS System Service - "));
		break;
	case 0x16:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Standard Keyboard Service - "));
		break;
	case 0x17:
		buff = Text::StrConcatC(buff, UTF8STRC("BIOS Printer Service - "));
		break;
	case 0x20:
		buff = Text::StrConcatC(buff, UTF8STRC("Exit COM program"));
		sess->endStatus = 2;
		sess->endIP = sess->regs.IP;
		break;
	case 0x21:
		buff = Text::StrConcatC(buff, UTF8STRC("DOS Func Service - "));
		switch (((UInt8*)&sess->regs.EAX)[1])
		{
		case 0x9:
			buff = Text::StrConcatC(buff, UTF8STRC("Print String"));
			break;
		case 0x4C:
			buff = Text::StrConcatC(buff, UTF8STRC("Exit program"));
			sess->endStatus = 2;
			sess->endIP = sess->regs.IP;
			break;
		}
		break;
	case 0x22:
		buff = Text::StrConcatC(buff, UTF8STRC("Program Terminate Routine"));
		sess->endStatus = 2;
		sess->endIP = sess->regs.IP;
		break;
	case 0x23:
		buff = Text::StrConcatC(buff, UTF8STRC("Keyboard Break Handler"));
		break;
	case 0x24:
		buff = Text::StrConcatC(buff, UTF8STRC("Critical-Error Handler"));
		break;
	case 0x25:
		buff = Text::StrConcatC(buff, UTF8STRC("Absolute Disk-Read Service"));
		break;
	case 0x26:
		buff = Text::StrConcatC(buff, UTF8STRC("Absolute Disk-Write Service"));
		break;
	case 0x33:
		buff = Text::StrConcatC(buff, UTF8STRC("Mouse Service - "));
		break;
	default:
		buff = Text::StrConcatC(buff, UTF8STRC("Unknown Int"));
		break;
	}
	return buff;
}

Bool __stdcall DasmX86_16_00(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			AddEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
		else
		{
			AddEqU8((UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_01(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	AddEqU16(&sess->regs.IP, 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[regNo];
			sess->regs.indexes[modRM & 7] += val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[regNo];
			AddEqU16((UInt16*)&sess->regs.indexes[modRM & 7], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_02(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			AddEqU8(1 + (UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
		else
		{
			AddEqU8((UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_03(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	AddEqU16(&sess->regs.IP, 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[modRM & 7];
			sess->regs.indexes[regNo] += val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[modRM & 7];
			AddEqU16((UInt16*)&sess->regs.indexes[regNo], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_04(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val;
	val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add AL"));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	AddEqU8((UInt8*)&sess->regs.EAX, val);
	return true;
}

Bool __stdcall DasmX86_16_05(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		val = *(Int32*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 5);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add EAX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX += val;
	}
	else
	{
		UInt16 val;
		val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 3);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add AX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		AddEqU16((UInt16*)&sess->regs.EAX, val);
	}
	return true;
}

Bool __stdcall DasmX86_16_06(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push ES"));
	sess->regs.ESP -= 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_07(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop ES"));
	sess->regs.ESP += 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_08(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			((UInt8*)&sess->regs.indexes[modRM & 3])[1] |= val;
		}
		else
		{
			*(UInt8*)&sess->regs.indexes[modRM & 3] |= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_09(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[regNo];
			sess->regs.indexes[modRM & 7] |= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[regNo];
			*(UInt16*)&sess->regs.indexes[modRM & 7] |= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_0A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			((UInt8*)&sess->regs.indexes[regNo & 3])[1] |= val;
		}
		else
		{
			*(UInt8*)&sess->regs.indexes[regNo & 3] |= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_0B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[modRM & 7];
			sess->regs.indexes[regNo] |= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[modRM & 7];
			*(UInt16*)&sess->regs.indexes[regNo] |= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_0C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val;
	val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or AL"));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	*(UInt8*)&sess->regs.EAX |= val;
	return true;
}

Bool __stdcall DasmX86_16_0D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		val = *(Int32*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 5);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or EAX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX |= val;
	}
	else
	{
		UInt16 val;
		val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 3);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or AX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EAX |= val;
	}
	return true;
}

Bool __stdcall DasmX86_16_0E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push CS"));
	sess->regs.ESP -= 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_0F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return ((Manage::DasmX86_16::DasmX86_16_Code)sess->code0fHdlrs[sess->code[sess->regs.IP + 1]])(sess);
}

Bool __stdcall DasmX86_16_10(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			AddEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
		else
		{
			AddEqU8((UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_11(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[regNo];
			sess->regs.indexes[modRM & 7] += val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[regNo];
			AddEqU16((UInt16*)&sess->regs.indexes[modRM & 7], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_12(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			AddEqU8(1 + (UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
		else
		{
			AddEqU8((UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_13(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[modRM & 7];
			sess->regs.indexes[regNo] += val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[modRM & 7];
			AddEqU16((UInt16*)&sess->regs.indexes[regNo], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_14(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val;
	val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc AL"));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	AddEqU8((UInt8*)&sess->regs.EAX, val);
	return true;
}

Bool __stdcall DasmX86_16_15(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		val = *(Int32*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 5);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc EAX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX += val;
	}
	else
	{
		UInt16 val;
		val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 3);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc AX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		AddEqU16((UInt16*)&sess->regs.EAX, val);
	}
	return true;
}

Bool __stdcall DasmX86_16_16(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push SS"));
	sess->regs.ESP -= 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_17(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop SS"));
	sess->regs.ESP += 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_18(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			SubEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
		else
		{
			SubEqU8((UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_19(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[regNo];
			sess->regs.indexes[modRM & 7] -= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[regNo];
			SubEqU16((UInt16*)&sess->regs.indexes[modRM & 7], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_1A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			SubEqU8(1 + (UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
		else
		{
			SubEqU8((UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_1B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[modRM & 7];
			sess->regs.indexes[regNo] -= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[modRM & 7];
			SubEqU16((UInt16*)&sess->regs.indexes[regNo], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_1C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val;
	val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb AL"));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	SubEqU8((UInt8*)&sess->regs.EAX, val);
	return true;
}

Bool __stdcall DasmX86_16_1D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		val = *(Int32*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 5);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb EAX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX -= val;
	}
	else
	{
		UInt16 val;
		val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 3);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb AX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		SubEqU16((UInt16*)&sess->regs.EAX, val);
	}
	return true;
}

Bool __stdcall DasmX86_16_1E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push DS"));
	sess->regs.ESP -= 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_1F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop DS"));
	sess->regs.ESP += 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_20(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			((UInt8*)&sess->regs.indexes[modRM & 3])[1] &= val;
		}
		else
		{
			*(UInt8*)&sess->regs.indexes[modRM & 3] &= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_21(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[regNo];
			sess->regs.indexes[modRM & 7] &= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[regNo];
			*(UInt16*)&sess->regs.indexes[modRM & 7] &= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_22(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			((UInt8*)&sess->regs.indexes[regNo & 3])[1] &= val;
		}
		else
		{
			*(UInt8*)&sess->regs.indexes[regNo & 3] &= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_23(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[modRM & 7];
			sess->regs.indexes[regNo] &= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[modRM & 7];
			*(UInt16*)&sess->regs.indexes[regNo] &= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_24(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val;
	val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and AL"));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	*(UInt8*)&sess->regs.EAX &= val;
	return true;
}

Bool __stdcall DasmX86_16_25(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		val = *(Int32*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 5);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and EAX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX &= val;
	}
	else
	{
		UInt16 val;
		val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 3);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and AX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EAX &= val;
	}
	return true;
}

Bool __stdcall DasmX86_16_26(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus = (sess->thisStatus & ~7) | 3;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return ((Manage::DasmX86_16::DasmX86_16_Code)sess->codeHdlrs[sess->code[sess->regs.IP]])(sess);
}

Bool __stdcall DasmX86_16_27(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("daa"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_28(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			SubEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
		else
		{
			SubEqU8((UInt8*)&sess->regs.indexes[modRM & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_29(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[regNo];
			sess->regs.indexes[modRM & 7] -= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[regNo];
			SubEqU16((UInt16*)&sess->regs.indexes[modRM & 7], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_2A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			SubEqU8(1 + (UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
		else
		{
			SubEqU8((UInt8*)&sess->regs.indexes[regNo & 3], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_2B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			Int32 val;
			val = sess->regs.indexes[modRM & 7];
			sess->regs.indexes[regNo] -= val;
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			UInt16 val;
			val = *(UInt16*)&sess->regs.indexes[modRM & 7];
			SubEqU16((UInt16*)&sess->regs.indexes[regNo], val);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_2C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val;
	val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub AL"));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	SubEqU8((UInt8*)&sess->regs.EAX, val);
	return true;
}

Bool __stdcall DasmX86_16_2D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		val = *(Int32*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 5);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub EAX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX -= val;
	}
	else
	{
		UInt16 val;
		val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		AddEqU16(&sess->regs.IP, 3);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub AX"));
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		SubEqU16((UInt16*)&sess->regs.EAX, val);
	}
	return true;
}

Bool __stdcall DasmX86_16_2E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus = (sess->thisStatus & ~7) | 1;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return ((Manage::DasmX86_16::DasmX86_16_Code)sess->codeHdlrs[sess->code[sess->regs.IP]])(sess);
}

Bool __stdcall DasmX86_16_2F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("das"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_30(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	UInt8 modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (regNo & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[regNo & 3])[1];
		}
		else
		{
			val = (UInt8)sess->regs.indexes[regNo & 3];
		}
		if (modRM & 4)
		{
			((UInt8*)&sess->regs.indexes[modRM & 3])[1] ^= val;
		}
		else
		{
			*(UInt8*)&sess->regs.indexes[modRM & 3] ^= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_31(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	UInt8 modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] ^= sess->regs.indexes[regNo];
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		if ((modRM >> 6) == 3)
		{
			*(UInt16*)&sess->regs.indexes[modRM & 7] ^= *(UInt16*)&sess->regs.indexes[regNo];
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_32(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	UInt8 modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	if ((modRM >> 6) == 3)
	{
		UInt8 val;
		if (modRM & 4)
		{
			val = ((UInt8*)&sess->regs.indexes[modRM & 3])[1];
		}
		else
		{
			val = *(UInt8*)&sess->regs.indexes[modRM & 3];
		}
		if (regNo & 4)
		{
			((UInt8*)&sess->regs.indexes[regNo & 3])[1] ^= val;
		}
		else
		{
			*(UInt8*)&sess->regs.indexes[regNo & 3] ^= val;
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_33(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	UInt8 modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[regNo] ^= sess->regs.indexes[modRM & 7];
		}
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			*(UInt16*)&sess->regs.indexes[regNo] ^= *(UInt16*)&sess->regs.indexes[modRM & 7];
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_34(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor AL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	sess->regs.EAX ^= val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_35(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor EAX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX ^= val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor AX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		sess->regs.EAX ^= val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_36(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus = (sess->thisStatus & ~7) | 4;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return ((Manage::DasmX86_16::DasmX86_16_Code)sess->codeHdlrs[sess->code[sess->regs.IP]])(sess);
}

Bool __stdcall DasmX86_16_37(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("aaa"));
	return true;
}

Bool __stdcall DasmX86_16_38(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	return true;
}

Bool __stdcall DasmX86_16_39(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	}
	return true;
}

Bool __stdcall DasmX86_16_3A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_3B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	return true;
}

Bool __stdcall DasmX86_16_3C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp AL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP + 1]);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_3D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp EAX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, *(Int32*)&sess->code[sess->regs.IP + 1]);
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp AX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP + 1]);
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_3E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus = (sess->thisStatus & ~7) | 2;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return ((Manage::DasmX86_16::DasmX86_16_Code)sess->codeHdlrs[sess->code[sess->regs.IP]])(sess);
}

Bool __stdcall DasmX86_16_3F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("aas"));
	return true;
}

Bool __stdcall DasmX86_16_40(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc EAX"));
		sess->regs.EAX++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc AX"));
		(*(UInt16*)&sess->regs.EAX)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_41(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc ECX"));
		sess->regs.ECX++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc CX"));
		(*(UInt16*)&sess->regs.ECX)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_42(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc EDX"));
		sess->regs.EDX++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc DX"));
		(*(UInt16*)&sess->regs.EDX)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_43(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc EBX"));
		sess->regs.EBX++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc BX"));
		(*(UInt16*)&sess->regs.EBX)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_44(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc ESP"));
		sess->regs.ESP++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc SP"));
		(*(UInt16*)&sess->regs.ESP)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_45(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc EBP"));
		sess->regs.EBP++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc BP"));
		(*(UInt16*)&sess->regs.EBP)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_46(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc ESI"));
		sess->regs.ESI++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc SI"));
		(*(UInt16*)&sess->regs.ESI)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_47(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc EDI"));
		sess->regs.EDI++;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc DI"));
		(*(UInt16*)&sess->regs.EDI)++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_48(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.EAX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec EAX"));
	}
	else
	{
		sess->regs.AX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec AX"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_49(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.ECX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec ECX"));
	}
	else
	{
		sess->regs.CX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec CX"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_4A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.EDX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec EDX"));
	}
	else
	{
		sess->regs.DX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec DX"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_4B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.EBX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec EBX"));
	}
	else
	{
		sess->regs.BX--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec BX"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_4C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.ESP--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec ESP"));
	}
	else
	{
		sess->regs.SP--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec SP"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_4D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.EBP--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec EBP"));
	}
	else
	{
		sess->regs.BP--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec BP"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_4E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.ESI--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec ESI"));
	}
	else
	{
		sess->regs.SI--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec SI"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_4F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->regs.EDI--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec EDI"));
	}
	else
	{
		sess->regs.DI--;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec DI"));
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_50(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push EAX"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push AX"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_51(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push ECX"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push CX"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_52(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push EDX"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push DX"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_53(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push EBX"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push BX"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_54(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push ESP"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push SP"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_55(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push EBP"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push BP"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_56(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push ESI"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push SI"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_57(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push EDI"));
		sess->regs.ESP -= 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push DI"));
		sess->regs.ESP -= 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_58(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop EAX"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop AX"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_59(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop ECX"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop CX"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_5A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop EDX"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop DX"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_5B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop EBX"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop BX"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_5C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop ESP"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop SP"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_5D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop EBP"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop BP"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_5E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop ESI"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop SI"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_5F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop EDI"));
		sess->regs.ESP += 4;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop DI"));
		sess->regs.ESP += 2;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_60(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pushad"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pusha"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_61(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("popad"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("popa"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_62(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_63(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_64(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus = (sess->thisStatus & ~7) | 5;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return ((Manage::DasmX86_16::DasmX86_16_Code*)sess->codeHdlrs)[sess->code[sess->regs.IP]](sess);
}

Bool __stdcall DasmX86_16_65(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus = (sess->thisStatus & ~7) | 6;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return ((Manage::DasmX86_16::DasmX86_16_Code*)sess->codeHdlrs)[sess->code[sess->regs.IP]](sess);
}

Bool __stdcall DasmX86_16_66(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->thisStatus |= 8;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	
	return ((Manage::DasmX86_16::DasmX86_16_Code*)sess->codeHdlrs)[sess->code[sess->regs.IP]](sess);
}

Bool __stdcall DasmX86_16_67(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_68(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_69(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrInt32(sess->outSPtr, (Int8)sess->code[sess->regs.IP]);
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_6A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_6B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrInt32(sess->outSPtr, *(Int32*)&sess->code[sess->regs.IP]);
		sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrInt32(sess->outSPtr, *(Int16*)&sess->code[sess->regs.IP]);
		sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	}
	return true;
}

Bool __stdcall DasmX86_16_6C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("insb"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->regs.DF)
	{
		SubEqU16(&sess->regs.DI, 1);
	}
	else
	{
		AddEqU16(&sess->regs.DI, 1);
	}
	return true;
}

Bool __stdcall DasmX86_16_6D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("insd"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.DI, 4);
		}
		else
		{
			AddEqU16(&sess->regs.DI, 4);
		}
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("insw"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.DI, 2);
		}
		else
		{
			AddEqU16(&sess->regs.DI, 2);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_6E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("outsb"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->regs.DF)
	{
		SubEqU16(&sess->regs.SI, 1);
	}
	else
	{
		AddEqU16(&sess->regs.SI, 1);
	}
	return true;
}

Bool __stdcall DasmX86_16_6F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("outsd"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.SI, 4);
		}
		else
		{
			AddEqU16(&sess->regs.SI, 4);
		}
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("outsw"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.SI, 2);
		}
		else
		{
			AddEqU16(&sess->regs.SI, 2);
		}
	}
	return true;
}

Bool __stdcall DasmX86_16_70(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jo "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_71(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jno "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_72(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jb "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_73(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jnb "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_74(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jz "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_75(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jnz "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_76(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jbe "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_77(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ja "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_78(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("js "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_79(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jns "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_7A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jp "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_7B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jnp "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_7C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jl "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_7D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jge "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_7E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jle "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_7F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jg "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_80(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	UInt8 val = sess->code[sess->regs.IP];
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);

	if (regNo == 0)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				AddEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
			else
			{
				AddEqU8((UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
	}
	else if (regNo == 1)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				((UInt8*)&sess->regs.indexes[modRM & 3])[1] |= sess->code[sess->regs.IP];
			}
			else
			{
				*(UInt8*)&sess->regs.indexes[modRM & 3] |= sess->code[sess->regs.IP];
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
	}
	else if (regNo == 2)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				AddEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
			else
			{
				AddEqU8((UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
	}
	else if (regNo == 3)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				SubEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
			else
			{
				SubEqU8((UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
	}
	else if (regNo == 4)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				((UInt8*)&sess->regs.indexes[modRM & 3])[1] &= sess->code[sess->regs.IP];
			}
			else
			{
				*(UInt8*)&sess->regs.indexes[modRM & 3] &= sess->code[sess->regs.IP];
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
	}
	else if (regNo == 5)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				SubEqU8(1 + (UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
			else
			{
				SubEqU8((UInt8*)&sess->regs.indexes[modRM & 3], sess->code[sess->regs.IP]);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
	}
	else if (regNo == 6)
	{
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				((UInt8*)&sess->regs.indexes[modRM & 3])[1] ^= sess->code[sess->regs.IP];
			}
			else
			{
				*(UInt8*)&sess->regs.indexes[modRM & 3] ^= sess->code[sess->regs.IP];
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
	}
	else if (regNo == 7)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	return true;
}

Bool __stdcall DasmX86_16_81(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM = sess->code[sess->regs.IP];
	Int32 val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		val = *(Int32*)&sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		val = *(UInt16*)&sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	}

	if (regNo == 0)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] += val;
			}
			else
			{
				AddEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
	}
	else if (regNo == 1)
	{
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] |= val;
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
	}
	else if (regNo == 2)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] += val;
			}
			else
			{
				AddEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
	}
	else if (regNo == 3)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] -= val;
			}
			else
			{
				SubEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
	}
	else if (regNo == 4)
	{
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] &= val;
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
	}
	else if (regNo == 5)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] -= val;
			}
			else
			{
				SubEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
	}
	else if (regNo == 6)
	{
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] ^= val;
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
	}
	else if (regNo == 7)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
	return true;
}

Bool __stdcall DasmX86_16_82(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_83(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM = sess->code[sess->regs.IP];
	Int32 val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		val = (Int32)(Int8)sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		val = 0xffff & (Int32)(Int8)sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}

	if (regNo == 0)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] += val;
			}
			else
			{
				AddEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("add "));
	}
	else if (regNo == 1)
	{
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] |= val;
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("or "));
	}
	else if (regNo == 2)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] += val;
			}
			else
			{
				AddEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("adc "));
	}
	else if (regNo == 3)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] -= val;
			}
			else
			{
				SubEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sbb "));
	}
	else if (regNo == 4)
	{
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] &= val;
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("and "));
	}
	else if (regNo == 5)
	{
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7] -= val;
			}
			else
			{
				SubEqU16((UInt16*)&sess->regs.indexes[modRM & 7], (UInt16)val);
			}
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sub "));
	}
	else if (regNo == 6)
	{
		if ((modRM >> 6) == 3)
		{
			sess->regs.indexes[modRM & 7] ^= val;
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xor "));
	}
	else if (regNo == 7)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmp "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
	return true;
}

Bool __stdcall DasmX86_16_84(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test "));
	sess->outSPtr = DasmX86_16_ParseModRM8(sess, sess->outSPtr, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	return true;
}

Bool __stdcall DasmX86_16_85(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test "));
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = DasmX86_16_ParseModRM32(sess, sess->outSPtr, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	}
	else
	{
		sess->outSPtr = DasmX86_16_ParseModRM16(sess, sess->outSPtr, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	}
	return true;
}

Bool __stdcall DasmX86_16_86(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg "));
	sess->outSPtr = DasmX86_16_ParseReg8(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_87(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	return true;
}

Bool __stdcall DasmX86_16_88(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 destReg;
	sess->regs.IP++;
	DasmX86_16_ParseModRM8(sess, mem, &destReg);
	DasmX86_16_ParseReg8(reg, destReg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, reg);
	return true;
}

Bool __stdcall DasmX86_16_89(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 destReg;
	sess->regs.IP++;
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &destReg);
		DasmX86_16_ParseReg32(reg, destReg);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &destReg);
		DasmX86_16_ParseReg16(reg, destReg);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, reg);
	return true;
}

Bool __stdcall DasmX86_16_8A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 destReg;
	sess->regs.IP++;
	DasmX86_16_ParseModRM8(sess, mem, &destReg);
	DasmX86_16_ParseReg8(reg, destReg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, reg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_8B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 destReg;
	sess->regs.IP++;
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &destReg);
		DasmX86_16_ParseReg32(reg, destReg);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &destReg);
		DasmX86_16_ParseReg16(reg, destReg);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, reg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_8C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 destReg;
	sess->regs.IP++;
	DasmX86_16_ParseModRM16(sess, mem, &destReg);
	DasmX86_16_ParseSegReg16(reg, destReg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, reg);
	return true;
}

Bool __stdcall DasmX86_16_8D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseAddr16(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("lea "));
	sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_8E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 destReg;
	sess->regs.IP++;
	DasmX86_16_ParseModRM16(sess, mem, &destReg);
	DasmX86_16_ParseSegReg16(reg, destReg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, reg);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_8F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
	}
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pop "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else
	{
		return false;
	}
	return true;
}

Bool __stdcall DasmX86_16_90(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("nop"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_91(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, ECX"));
		val = sess->regs.ECX;
		sess->regs.ECX = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, CX"));
		val = sess->regs.CX;
		sess->regs.CX = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_92(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, EDX"));
		val = sess->regs.EDX;
		sess->regs.EDX = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, DX"));
		val = sess->regs.DX;
		sess->regs.DX = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_93(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, EBX"));
		val = sess->regs.EBX;
		sess->regs.EBX = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, BX"));
		val = sess->regs.BX;
		sess->regs.BX = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_94(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, ESP"));
		val = sess->regs.ESP;
		sess->regs.ESP = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, SP"));
		val = sess->regs.SP;
		sess->regs.SP = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_95(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, EBP"));
		val = sess->regs.EBP;
		sess->regs.EBP = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, BP"));
		val = sess->regs.BP;
		sess->regs.BP = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_96(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, ESI"));
		val = sess->regs.ESI;
		sess->regs.ESI = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, SI"));
		val = sess->regs.SI;
		sess->regs.SI = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_97(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg EAX, EDI"));
		val = sess->regs.EDI;
		sess->regs.EDI = sess->regs.EAX;
		sess->regs.EAX = val;
	}
	else
	{
		UInt16 val;
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xchg AX, DI"));
		val = sess->regs.DI;
		sess->regs.DI = sess->regs.AX;
		sess->regs.AX = val;
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_98(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cwde"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cbw"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_99(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cdq"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cwd"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_9A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_9B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_9C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pushfd"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("pushf"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_9D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("popfd"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("popf"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_9E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sahf"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_9F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("lahf"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_A0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov AL,BYTE PTR "));
	sess->outSPtr = DasmX86_16_AppAddrM(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP+1]);
	AddEqU16(&sess->regs.IP, 3);
	return true;
}

Bool __stdcall DasmX86_16_A1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov EAX,DWORD PTR "));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov AX,WORD PTR "));
	}
	sess->outSPtr = DasmX86_16_AppAddrM(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP+1]);
	AddEqU16(&sess->regs.IP, 3);
	return true;
}

Bool __stdcall DasmX86_16_A2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov BYTE PTR "));
	sess->outSPtr = DasmX86_16_AppAddrM(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP+1]);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", AL"));
	AddEqU16(&sess->regs.IP, 3);
	return true;
}

Bool __stdcall DasmX86_16_A3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov DWORD PTR "));
		sess->outSPtr = DasmX86_16_AppAddrM(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP+1]);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", EAX"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov WORD PTR "));
		sess->outSPtr = DasmX86_16_AppAddrM(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP+1]);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", AX"));
	}
	AddEqU16(&sess->regs.IP, 3);
	return true;
}

Bool __stdcall DasmX86_16_A4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movsb"));
	if (sess->regs.DF)
	{
		sess->regs.SI--;
		sess->regs.DI--;
	}
	else
	{
		sess->regs.SI++;
		sess->regs.DI++;
	}
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_A5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movsd"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.SI, 4);
			SubEqU16(&sess->regs.DI, 4);
		}
		else
		{
			AddEqU16(&sess->regs.SI, 4);
			AddEqU16(&sess->regs.DI, 4);
		}
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movsw"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.SI, 2);
			SubEqU16(&sess->regs.DI, 2);
		}
		else
		{
			AddEqU16(&sess->regs.SI, 2);
			AddEqU16(&sess->regs.DI, 2);
		}
	}
	AddEqU16(&sess->regs.IP, 1);
	return true;
}

Bool __stdcall DasmX86_16_A6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_A7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_A8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test AL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP + 1]);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_A9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test EAX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, *(Int32*)&sess->code[sess->regs.IP + 1]);
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test AX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP + 1]);
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_AA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("stosb"));
	if (sess->regs.DF)
	{
		SubEqU16(&sess->regs.DI, 1);
	}
	else
	{
		AddEqU16(&sess->regs.DI, 1);
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_AB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("stosd"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.DI, 4);
		}
		else
		{
			AddEqU16(&sess->regs.DI, 4);
		}
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("stosw"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.DI, 2);
		}
		else
		{
			AddEqU16(&sess->regs.DI, 2);
		}
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_AC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("lodsb"));
	if (sess->regs.DF)
	{
		SubEqU16(&sess->regs.SI, 1);
	}
	else
	{
		AddEqU16(&sess->regs.SI, 1);
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_AD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("lodsd"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.SI, 4);
		}
		else
		{
			AddEqU16(&sess->regs.SI, 4);
		}
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("lodsw"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.SI, 2);
		}
		else
		{
			AddEqU16(&sess->regs.SI, 2);
		}
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_AE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("scasb"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->regs.DF)
	{
		SubEqU16(&sess->regs.DI, 1);
	}
	else
	{
		AddEqU16(&sess->regs.DI,  1);
	}
	return true;
}

Bool __stdcall DasmX86_16_AF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("scasd"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.DI, 4);
		}
		else
		{
			AddEqU16(&sess->regs.DI, 4);
		}
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("scasw"));
		if (sess->regs.DF)
		{
			SubEqU16(&sess->regs.DI, 2);
		}
		else
		{
			AddEqU16(&sess->regs.DI, 2);
		}
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_B0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov AL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	*(UInt8*)&sess->regs.EAX = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov CL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	*(UInt8*)&sess->regs.ECX = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov DL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	*(UInt8*)&sess->regs.EDX = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov BL,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	*(UInt8*)&sess->regs.EBX = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov AH,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	((UInt8*)&sess->regs.EAX)[1] = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov CH,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	((UInt8*)&sess->regs.ECX)[1] = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov DH,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	((UInt8*)&sess->regs.EDX)[1] = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov BH,"));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
	((UInt8*)&sess->regs.EBX)[1] = val;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_B8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov EAX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EAX = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov AX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EAX = val;
		AddEqU16(&sess->regs.IP, 3);
	}

	return true;
}

Bool __stdcall DasmX86_16_B9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov ECX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.ECX = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov CX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.ECX = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_BA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov EDX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EDX = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov DX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EDX = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_BB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov EBX,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EBX = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov BX,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EBX = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_BC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov ESP,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.ESP = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov SP,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.ESP = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_BD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov EBP,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EBP = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov BP,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EBP = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_BE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov ESI,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.ESI = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov SI,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.ESI = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_BF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		Int32 val = *(Int32*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov EDI,"));
		sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
		sess->regs.EDI = val;
		AddEqU16(&sess->regs.IP, 5);
	}
	else
	{
		UInt16 val = *(UInt16*)&sess->code[sess->regs.IP + 1];
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov DI,"));
		sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
		*(UInt16*)&sess->regs.EDI = val;
		AddEqU16(&sess->regs.IP, 3);
	}
	return true;
}

Bool __stdcall DasmX86_16_C0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	UInt8 *regPtr;
	UInt8 val;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if ((modRM >> 6) == 3)
	{
		if (modRM & 4)
		{
			regPtr = 1 + (UInt8*)&sess->regs.indexes[modRM & 3];
		}
		else
		{
			regPtr = (UInt8*)&sess->regs.indexes[modRM & 3];
		}
	}
	else
	{
		regPtr = &modRM;
	}
	val = sess->code[sess->regs.IP];
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rol "));
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ror "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcl "));
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcr "));
	}
    else if (regNo == 4)
	{
		ShlEqU8(regPtr, val);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shl "));
	}
	else if (regNo == 5)
	{
		ShrEq8(regPtr, val);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shr "));
	}
	else if (regNo == 7)
	{
		SarEq8((Int8*)regPtr, val);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sar "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrInt32(sess->outSPtr, val);
	return true;
}

Bool __stdcall DasmX86_16_C1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	Int32 *regPtr;
	UInt8 val;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
	}
	if ((modRM >> 6) == 3)
	{
		regPtr = &sess->regs.indexes[modRM & 7];
	}
	else
	{
		regPtr = &regNo;
	}
	val = sess->code[sess->regs.IP];
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rol "));
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ror "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcl "));
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcr "));
	}
	else if (regNo == 4)
	{
		if (sess->thisStatus & 8)
		{
			*regPtr <<= val;
		}
		else
		{
			ShlEqU16((UInt16*)regPtr, val);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shl "));
	}
	else if (regNo == 5)
	{
		if (sess->thisStatus & 8)
		{
			*(UInt32*)regPtr >>= val;
		}
		else
		{
			ShrEq16((UInt16*)regPtr, val);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shr "));
	}
	else if (regNo == 7)
	{
		if (sess->thisStatus & 8)
		{
			*(Int32*)regPtr >>= val;
		}
		else
		{
			SarEq16((Int16*)regPtr, val);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sar "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrInt32(sess->outSPtr, val);
	return true;
}

Bool __stdcall DasmX86_16_C2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->endStatus = 3;
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ret "));
	sess->outSPtr = Text::StrInt32(sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP + 1]);
	AddEqU16(&sess->regs.IP, 3);
	sess->endIP = sess->regs.IP;
	return true;
}

Bool __stdcall DasmX86_16_C3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->endStatus = 3;
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ret"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->endIP = sess->regs.IP;
	return true;
}

Bool __stdcall DasmX86_16_C4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseModRM32(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("les "));
	sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_C5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseModRM32(sess, mem, &regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("lds "));
	sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_C6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	UInt8 val;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	val = sess->code[sess->regs.IP];
	AddEqU16(&sess->regs.IP, 1);
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, val);
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				((UInt8*)&sess->regs.indexes[modRM & 3])[1] = val;
			}
			else
			{
				*(UInt8*)&sess->regs.indexes[modRM & 3] = val;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_16_C7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		Int32 val;
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		val = *(Int32*)&sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 4);
		if (regNo == 0)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
			sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
			sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, val);
			if ((modRM >> 6) == 3)
			{
				sess->regs.indexes[modRM & 7] = val;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		UInt16 val;
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		val = *(UInt16*)&sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 2);
		if (regNo == 0)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mov "));
			sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
			sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, val);
			if ((modRM >> 6) == 3)
			{
				*(UInt16*)&sess->regs.indexes[modRM & 7] = val;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}

Bool __stdcall DasmX86_16_C8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_C9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("leave"));
	AddEqU16(&sess->regs.IP, 1);
	return true;
}

Bool __stdcall DasmX86_16_CA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->endStatus = 3;
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ret "));
	sess->outSPtr = Text::StrInt32(sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP + 1]);
	AddEqU16(&sess->regs.IP, 3);
	sess->endIP = sess->regs.IP;
	return true;
}

Bool __stdcall DasmX86_16_CB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->endStatus = 3;
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ret"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->endIP = sess->regs.IP;
	return true;
}

Bool __stdcall DasmX86_16_CC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("int 3"));
	sess->outSPtr = DasmX86_16_IntName(sess, sess->outSPtr, 3);
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_CD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("int "));
	UInt8 val = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	if (val < 10)
	{
		sess->outSPtr = Text::StrInt32(sess->outSPtr, val);
	}
	else if (val < 0xa0)
	{
		sess->outSPtr = Text::StrHexByte(sess->outSPtr, val);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("h"));;
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("0"));;
		sess->outSPtr = Text::StrHexByte(sess->outSPtr, val);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("h"));;
	}
	sess->outSPtr = DasmX86_16_IntName(sess, sess->outSPtr, val);
	return true;
}

Bool __stdcall DasmX86_16_CE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("into"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_CF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_D0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	UInt8 *regPtr;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if ((modRM >> 6) == 3)
	{
		if (modRM & 4)
		{
			regPtr = 1 + (UInt8*)&sess->regs.indexes[modRM & 3];
		}
		else
		{
			regPtr = (UInt8*)&sess->regs.indexes[modRM & 3];
		}
	}
	else
	{
		regPtr = &modRM;
	}
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rol "));
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ror "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcl "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcr "));
	}
	else if (regNo == 4)
	{
		ShlEqU8(regPtr, 1);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shl "));
	}
	else if (regNo == 5)
	{
		ShrEq8(regPtr, 1);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shr "));
	}
	else if (regNo == 7)
	{
		SarEq8((Int8*)regPtr, 1);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sar "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", 1"));
	return true;
}

Bool __stdcall DasmX86_16_D1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	Int32 *regPtr;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
	}
	if ((modRM >> 6) == 3)
	{
		regPtr = &sess->regs.indexes[modRM & 7];
	}
	else
	{
		regPtr = &regNo;
	}
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rol "));
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ror "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcl "));
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcr "));
	}
	else if (regNo == 4)
	{
		if (sess->thisStatus & 8)
		{
			*regPtr <<= 1;
		}
		else
		{
			ShlEqU16((UInt16*)regPtr, 1);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shl "));
	}
	else if (regNo == 5)
	{
		if (sess->thisStatus & 8)
		{
			*(UInt32*)regPtr >>= 1;
		}
		else
		{
			ShrEq16((UInt16*)regPtr, 1);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shr "));
	}
	else if (regNo == 7)
	{
		if (sess->thisStatus & 8)
		{
			*(Int32*)regPtr >>= 1;
		}
		else
		{
			SarEq16((Int16*)regPtr, 1);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sar "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", 1"));
	return true;
}

Bool __stdcall DasmX86_16_D2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	UInt8 *regPtr;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if ((modRM >> 6) == 3)
	{
		if (modRM & 4)
		{
			regPtr = 1 + (UInt8*)&sess->regs.indexes[modRM & 3];
		}
		else
		{
			regPtr = (UInt8*)&sess->regs.indexes[modRM & 3];
		}
	}
	else
	{
		regPtr = &modRM;
	}
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rol "));
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ror "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcl "));
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcr "));
	}
	else if (regNo == 4)
	{
		ShlEqU8(regPtr, (UInt8)sess->regs.ECX);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shl "));
	}
	else if (regNo == 5)
	{
		ShrEq8(regPtr, (UInt8)sess->regs.ECX);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shr "));
	}
	else if (regNo == 7)
	{
		SarEq8((Int8*)regPtr, (UInt8)sess->regs.ECX);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sar "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", CL"));
	return true;
}

Bool __stdcall DasmX86_16_D3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	Int32 *regPtr;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
	}
	if ((modRM >> 6) == 3)
	{
		regPtr = &sess->regs.indexes[modRM & 7];
	}
	else
	{
		regPtr = &regNo;
	}
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rol "));
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ror "));
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcl "));
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rcr "));
	}
	else if (regNo == 4)
	{
		if (sess->thisStatus & 8)
		{
			*regPtr <<= (UInt8)sess->regs.ECX;
		}
		else
		{
			ShlEqU16((UInt16*)regPtr, (UInt8)sess->regs.ECX);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shl "));
	}
	else if (regNo == 5)
	{
		if (sess->thisStatus & 8)
		{
			*(UInt32*)regPtr >>= (UInt8)sess->regs.ECX;
		}
		else
		{
			ShrEq16((UInt16*)regPtr, (UInt8)sess->regs.ECX);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("shr "));
	}
	else if (regNo == 7)
	{
		if (sess->thisStatus & 8)
		{
			*regPtr >>= (UInt8)sess->regs.ECX;
		}
		else
		{
			SarEq16((Int16*)regPtr, (UInt8)sess->regs.ECX);
		}
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sar "));
	}
	else
	{
		return false;
	}
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", CL"));
	return true;
}

Bool __stdcall DasmX86_16_D4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("aam"));
	return true;
}

Bool __stdcall DasmX86_16_D5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("aad"));
	return true;
}

Bool __stdcall DasmX86_16_D6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_D7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("xlatb"));
	return true;
}

Bool __stdcall DasmX86_16_D8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_D9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_DA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_DB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_DC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_DD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_DE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_DF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_E0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("loopnz "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);
	sess->regs.CX = 0;

	return true;
}

Bool __stdcall DasmX86_16_E1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("loopz "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);
	sess->regs.CX = 0;

	return true;
}

Bool __stdcall DasmX86_16_E2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (2 + sess->regs.IP + (Int8)sess->code[sess->regs.IP + 1]));
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("loop "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->jmpAddrs.SortedInsert(addr);
	sess->regs.CX = 0;

	return true;
}

Bool __stdcall DasmX86_16_E3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (sess->regs.IP + 2 + (Int8)sess->code[sess->regs.IP]));
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jecxz "));
		sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jcxz "));
		sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	}
	sess->jmpAddrs.SortedInsert(addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_E4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("in AL, "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP + 1]);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_E5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("in EAX, "));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("in AX, "));
	}
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP + 1]);
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_E6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("out "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP + 1]);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", AL"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_E7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("out "));
	sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP + 1]);
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", EAX"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", AX"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	return true;
}

Bool __stdcall DasmX86_16_E8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (sess->regs.IP + 3 + *(UInt16*)&sess->code[sess->regs.IP + 1]));
	OSInt i = sess->callAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->callAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("call "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	AddEqU16(&sess->regs.IP, 3);
	return true;
}

Bool __stdcall DasmX86_16_E9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (sess->regs.IP + 3 + *(UInt16*)&sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jmp "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	AddEqU16(&sess->regs.IP, 3);
	sess->endStatus = 1;
	sess->endIP = sess->regs.IP;
	sess->regs.IP = (UInt16)addr;
	return true;
}

Bool __stdcall DasmX86_16_EA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_EB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (sess->regs.IP + 2 + (Int8)sess->code[sess->regs.IP + 1]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jmp "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->endStatus = 1;
	sess->endIP = AddU16(sess->regs.IP, 2);
	sess->regs.IP = (UInt16)addr;
	sess->jmpAddrs.SortedInsert(addr);
	return true;
}

Bool __stdcall DasmX86_16_EC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("in AL,DX"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_ED(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("in EAX,DX"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("in AX,DX"));
	}
	return true;
}

Bool __stdcall DasmX86_16_EE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("out DX,AL"));
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
}

Bool __stdcall DasmX86_16_EF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("out DX,EAX"));
	}
	else
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("out DX,AX"));
	}
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	return true;
	return false;
}

Bool __stdcall DasmX86_16_F0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_F1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_F2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 nextCmd = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	if (nextCmd == 0x66)
	{
		sess->thisStatus |= 8;
		nextCmd = sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	if (nextCmd == 0xA6)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repnz cmpsb"));
	}
	else if (nextCmd == 0xA7)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repnz cmpsd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repnz cmpsw"));
		}
	}
	else if (nextCmd == 0xAE)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repnz scasb"));
	}
	else if (nextCmd == 0xAF)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repnz scasd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repnz scasw"));
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

Bool __stdcall DasmX86_16_F3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt8 nextCmd = sess->code[sess->regs.IP + 1];
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	if (nextCmd == 0x66)
	{
		sess->thisStatus |= 8;
		nextCmd = sess->code[sess->regs.IP];
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	if (nextCmd == 0x6C)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep insb"));
		if (sess->regs.DF)
		{
			SubEqU16((UInt16*)&sess->regs.EDI, *(UInt16*)&sess->regs.ECX);
		}
		else
		{
			AddEqU16((UInt16*)&sess->regs.EDI, *(UInt16*)&sess->regs.ECX);
		}
		*(UInt16*)&sess->regs.ECX = 0;
	}
	else if (nextCmd == 0x6D)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep insd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep insw"));
		}
	}
	else if (nextCmd == 0x6E)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep outsb"));
		if (sess->regs.DF)
		{
			SubEqU16((UInt16*)&sess->regs.ESI, *(UInt16*)&sess->regs.ECX);
		}
		else
		{
			AddEqU16((UInt16*)&sess->regs.ESI, *(UInt16*)&sess->regs.ECX);
		}
		*(UInt16*)&sess->regs.ECX = 0;
	}
	else if (nextCmd == 0x6F)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep outsd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep outsw"));
		}
	}
	else if (nextCmd == 0xA4)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep movsb"));
		if (sess->regs.DF)
		{
			SubEqU16((UInt16*)&sess->regs.ESI, *(UInt16*)&sess->regs.ECX);
			SubEqU16((UInt16*)&sess->regs.EDI, *(UInt16*)&sess->regs.ECX);
		}
		else
		{
			AddEqU16((UInt16*)&sess->regs.ESI, *(UInt16*)&sess->regs.ECX);
			AddEqU16((UInt16*)&sess->regs.EDI, *(UInt16*)&sess->regs.ECX);
		}
		*(UInt16*)&sess->regs.ECX = 0;
	}
	else if (nextCmd == 0xA5)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep movsd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep movsw"));
		}
	}
	else if (nextCmd == 0xA6)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repz cmpsb"));
	}
	else if (nextCmd == 0xA7)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repz cmpsd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repz cmpsw"));
		}
	}
	else if (nextCmd == 0xAA)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep stosb"));
	}
	else if (nextCmd == 0xAB)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep stosd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep stosw"));
		}
	}
	else if (nextCmd == 0xAC)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep lodsb"));
	}
	else if (nextCmd == 0xAD)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep lodsd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("rep lodsw"));
		}
	}
	else if (nextCmd == 0xAE)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repz scasb"));
	}
	else if (nextCmd == 0xAF)
	{
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repz scasd"));
		}
		else
		{
			sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("repz scasw"));
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

Bool __stdcall DasmX86_16_F4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_F5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cmc"));
	sess->regs.CF ^= BIT1;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_F6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = DasmX86_16_AppInt8(sess, sess->outSPtr, sess->code[sess->regs.IP]);
		sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("not "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("neg "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 4)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mul "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 5)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 6)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("div "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 7)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("idiv "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else
	{
		return false;
	}
	return true;
}

Bool __stdcall DasmX86_16_F7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 1);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
	}

	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("test "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		if (sess->thisStatus & 8)
		{
			sess->outSPtr = DasmX86_16_AppInt32(sess, sess->outSPtr, *(Int32*)&sess->code[sess->regs.IP]);
			sess->regs.IP = (UInt16)(sess->regs.IP + 4);
		}
		else
		{
			sess->outSPtr = DasmX86_16_AppInt16(sess, sess->outSPtr, *(UInt16*)&sess->code[sess->regs.IP]);
			sess->regs.IP = (UInt16)(sess->regs.IP + 2);
		}
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("not "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("neg "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 4)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("mul "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 5)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 6)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("div "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else if (regNo == 7)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("idiv "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else
	{
		return false;
	}
	return true;
}

Bool __stdcall DasmX86_16_F8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("clc"));
	sess->regs.CF = 0;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_F9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("stc"));
	sess->regs.CF = BIT1;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_FA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cli"));
	sess->regs.IF = 0;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_FB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("sti"));
	sess->regs.IF = BIT1;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_FC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("cld"));
	sess->regs.DF = 0;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_FD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("std"));
	sess->regs.DF = BIT1;
	sess->regs.IP++;
	return true;
}

Bool __stdcall DasmX86_16_FE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				((UInt8*)&sess->regs.indexes[modRM & 3])[1]++;
			}
			else
			{
				(*(UInt8*)&sess->regs.indexes[modRM & 3])++;
			}
		}
		return true;
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			if (modRM & 4)
			{
				((UInt8*)&sess->regs.indexes[modRM & 3])[1]--;
			}
			else
			{
				(*(UInt8*)&sess->regs.indexes[modRM & 3])--;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_16_FF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 modRM;
	sess->regs.IP++;
	modRM = sess->code[sess->regs.IP];
	if (((modRM >> 3) & 7) == 3)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("call FAR "));
		sess->outSPtr = DasmX86_16_ParseAddr16(sess, sess->outSPtr, &regNo);
		return true;
	}
	else if (((modRM >> 3) & 7) == 5)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jmp FAR "));
		sess->outSPtr = DasmX86_16_ParseAddr16(sess, sess->outSPtr, &regNo);
		return true;
	}
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
	}
	if (regNo == 0)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("inc "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7]++;
			}
			else
			{
				(*(UInt16*)&sess->regs.indexes[modRM & 7])++;
			}
		}
		return true;
	}
	else if (regNo == 1)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("dec "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		if ((modRM >> 6) == 3)
		{
			if (sess->thisStatus & 8)
			{
				sess->regs.indexes[modRM & 7]--;
			}
			else
			{
				(*(UInt16*)&sess->regs.indexes[modRM & 7])--;
			}
		}
		return true;
	}
	else if (regNo == 2)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("call "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		return true;
	}
	else if (regNo == 4)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jmp "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		return true;
	}
	else if (regNo == 6)
	{
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("push "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_16_0F00(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F01(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F02(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F03(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F04(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F05(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F06(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F07(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F08(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F09(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F0A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F0B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F0C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F0D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F0E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F0F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F10(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F11(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F12(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F13(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F14(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F15(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F16(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F17(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F18(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F19(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F1A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F1B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F1C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F1D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F1E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F1F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F20(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F21(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F22(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F23(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F24(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F25(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F26(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F27(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F28(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F29(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F2A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F2B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F2C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F2D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F2E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F2F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F30(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F31(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F32(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F33(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F34(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F35(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F36(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F37(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F38(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F39(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F3A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F3B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F3C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F3D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F3E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F3F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F40(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F41(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F42(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F43(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F44(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F45(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F46(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F47(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F48(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F49(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F4A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F4B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F4C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F4D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F4E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F4F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F50(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F51(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F52(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F53(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F54(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F55(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F56(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F57(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F58(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F59(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F5A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F5B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F5C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F5D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F5E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F5F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F60(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F61(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F62(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F63(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F64(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F65(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F66(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F67(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F68(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F69(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F6A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F6B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F6C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F6D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F6E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F6F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F70(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F71(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F72(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F73(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F74(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F75(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F76(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F77(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F78(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F79(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F7A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F7B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F7C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F7D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F7E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F7F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F80(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jo "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F81(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jno "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F82(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jb "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F83(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jnb "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F84(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jz "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F85(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jnz "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F86(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jbe "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F87(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("ja "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F88(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("js "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F89(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jns "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F8A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jp "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F8B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jnp "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F8C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jl "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F8D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jge "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F8E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jle "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F8F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UInt32 addr = sess->regs.CS;
	addr = (addr << 16) + (0xffff & (4 + sess->regs.IP + *(Int16*)&sess->code[sess->regs.IP + 2]));
	OSInt i = sess->jmpAddrs.SortedIndexOf(addr);
	if (i < 0)
	{
		sess->jmpAddrs.Insert((UOSInt)(-i - 1), addr);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("jg "));
	sess->outSPtr = DasmX86_16_AppAddrN(sess, sess->outSPtr, addr);
	sess->regs.IP = (UInt16)(sess->regs.IP + 4);
	sess->jmpAddrs.SortedInsert(addr);

	return true;
}

Bool __stdcall DasmX86_16_0F90(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F91(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F92(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F93(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F94(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F95(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F96(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F97(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F98(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F99(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F9A(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F9B(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F9C(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F9D(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F9E(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0F9F(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FA9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FAA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FAB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FAC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FAD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FAE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FAF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	if (sess->thisStatus & 8)
	{
		DasmX86_16_ParseModRM32(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	else
	{
		DasmX86_16_ParseModRM16(sess, mem, &regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("imul "));
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
		sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
		sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	}
	return true;
}

Bool __stdcall DasmX86_16_0FB0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movzx "));
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	}
	else
	{
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_0FB7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movzx "));
	DasmX86_16_ParseModRM16(sess, mem, &regNo);
	sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_0FB8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FB9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FBA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FBB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FBC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FBD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FBE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movsx "));
	DasmX86_16_ParseModRM8(sess, mem, &regNo);
	if (sess->thisStatus & 8)
	{
		sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	}
	else
	{
		sess->outSPtr = DasmX86_16_ParseReg16(sess->outSPtr, regNo);
	}
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_0FBF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.IP = (UInt16)(sess->regs.IP + 2);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC("movsx "));
	DasmX86_16_ParseModRM16(sess, mem, &regNo);
	sess->outSPtr = DasmX86_16_ParseReg32(sess->outSPtr, regNo);
	sess->outSPtr = Text::StrConcatC(sess->outSPtr, UTF8STRC(", "));
	sess->outSPtr = Text::StrConcat(sess->outSPtr, mem);
	return true;
}

Bool __stdcall DasmX86_16_0FC0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FC9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FCA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FCB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FCC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FCD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FCE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FCF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FD9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FDA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FDB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FDC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FDD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FDE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FDF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FE9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FEA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FEB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FEC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FED(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FEE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FEF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF0(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF1(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF2(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF3(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF4(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF5(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF6(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF7(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF8(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FF9(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FFA(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FFB(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FFC(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FFD(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FFE(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_16_0FFF(Manage::DasmX86_16::DasmX86_16_Sess* sess)
{
	return false;
}

Manage::DasmX86_16::DasmX86_16()
{
	DasmX86_16_Code *codes;
	codes = this->codes = MemAlloc(DasmX86_16_Code, 256);
	codes[0x00] = DasmX86_16_00;
	codes[0x01] = DasmX86_16_01;
	codes[0x02] = DasmX86_16_02;
	codes[0x03] = DasmX86_16_03;
	codes[0x04] = DasmX86_16_04;
	codes[0x05] = DasmX86_16_05;
	codes[0x06] = DasmX86_16_06;
	codes[0x07] = DasmX86_16_07;
	codes[0x08] = DasmX86_16_08;
	codes[0x09] = DasmX86_16_09;
	codes[0x0A] = DasmX86_16_0A;
	codes[0x0B] = DasmX86_16_0B;
	codes[0x0C] = DasmX86_16_0C;
	codes[0x0D] = DasmX86_16_0D;
	codes[0x0E] = DasmX86_16_0E;
	codes[0x0F] = DasmX86_16_0F;
	codes[0x10] = DasmX86_16_10;
	codes[0x11] = DasmX86_16_11;
	codes[0x12] = DasmX86_16_12;
	codes[0x13] = DasmX86_16_13;
	codes[0x14] = DasmX86_16_14;
	codes[0x15] = DasmX86_16_15;
	codes[0x16] = DasmX86_16_16;
	codes[0x17] = DasmX86_16_17;
	codes[0x18] = DasmX86_16_18;
	codes[0x19] = DasmX86_16_19;
	codes[0x1A] = DasmX86_16_1A;
	codes[0x1B] = DasmX86_16_1B;
	codes[0x1C] = DasmX86_16_1C;
	codes[0x1D] = DasmX86_16_1D;
	codes[0x1E] = DasmX86_16_1E;
	codes[0x1F] = DasmX86_16_1F;
	codes[0x20] = DasmX86_16_20;
	codes[0x21] = DasmX86_16_21;
	codes[0x22] = DasmX86_16_22;
	codes[0x23] = DasmX86_16_23;
	codes[0x24] = DasmX86_16_24;
	codes[0x25] = DasmX86_16_25;
	codes[0x26] = DasmX86_16_26;
	codes[0x27] = DasmX86_16_27;
	codes[0x28] = DasmX86_16_28;
	codes[0x29] = DasmX86_16_29;
	codes[0x2A] = DasmX86_16_2A;
	codes[0x2B] = DasmX86_16_2B;
	codes[0x2C] = DasmX86_16_2C;
	codes[0x2D] = DasmX86_16_2D;
	codes[0x2E] = DasmX86_16_2E;
	codes[0x2F] = DasmX86_16_2F;
	codes[0x30] = DasmX86_16_30;
	codes[0x31] = DasmX86_16_31;
	codes[0x32] = DasmX86_16_32;
	codes[0x33] = DasmX86_16_33;
	codes[0x34] = DasmX86_16_34;
	codes[0x35] = DasmX86_16_35;
	codes[0x36] = DasmX86_16_36;
	codes[0x37] = DasmX86_16_37;
	codes[0x38] = DasmX86_16_38;
	codes[0x39] = DasmX86_16_39;
	codes[0x3A] = DasmX86_16_3A;
	codes[0x3B] = DasmX86_16_3B;
	codes[0x3C] = DasmX86_16_3C;
	codes[0x3D] = DasmX86_16_3D;
	codes[0x3E] = DasmX86_16_3E;
	codes[0x3F] = DasmX86_16_3F;
	codes[0x40] = DasmX86_16_40;
	codes[0x41] = DasmX86_16_41;
	codes[0x42] = DasmX86_16_42;
	codes[0x43] = DasmX86_16_43;
	codes[0x44] = DasmX86_16_44;
	codes[0x45] = DasmX86_16_45;
	codes[0x46] = DasmX86_16_46;
	codes[0x47] = DasmX86_16_47;
	codes[0x48] = DasmX86_16_48;
	codes[0x49] = DasmX86_16_49;
	codes[0x4A] = DasmX86_16_4A;
	codes[0x4B] = DasmX86_16_4B;
	codes[0x4C] = DasmX86_16_4C;
	codes[0x4D] = DasmX86_16_4D;
	codes[0x4E] = DasmX86_16_4E;
	codes[0x4F] = DasmX86_16_4F;
	codes[0x50] = DasmX86_16_50;
	codes[0x51] = DasmX86_16_51;
	codes[0x52] = DasmX86_16_52;
	codes[0x53] = DasmX86_16_53;
	codes[0x54] = DasmX86_16_54;
	codes[0x55] = DasmX86_16_55;
	codes[0x56] = DasmX86_16_56;
	codes[0x57] = DasmX86_16_57;
	codes[0x58] = DasmX86_16_58;
	codes[0x59] = DasmX86_16_59;
	codes[0x5A] = DasmX86_16_5A;
	codes[0x5B] = DasmX86_16_5B;
	codes[0x5C] = DasmX86_16_5C;
	codes[0x5D] = DasmX86_16_5D;
	codes[0x5E] = DasmX86_16_5E;
	codes[0x5F] = DasmX86_16_5F;
	codes[0x60] = DasmX86_16_60;
	codes[0x61] = DasmX86_16_61;
	codes[0x62] = DasmX86_16_62;
	codes[0x63] = DasmX86_16_63;
	codes[0x64] = DasmX86_16_64;
	codes[0x65] = DasmX86_16_65;
	codes[0x66] = DasmX86_16_66;
	codes[0x67] = DasmX86_16_67;
	codes[0x68] = DasmX86_16_68;
	codes[0x69] = DasmX86_16_69;
	codes[0x6A] = DasmX86_16_6A;
	codes[0x6B] = DasmX86_16_6B;
	codes[0x6C] = DasmX86_16_6C;
	codes[0x6D] = DasmX86_16_6D;
	codes[0x6E] = DasmX86_16_6E;
	codes[0x6F] = DasmX86_16_6F;
	codes[0x70] = DasmX86_16_70;
	codes[0x71] = DasmX86_16_71;
	codes[0x72] = DasmX86_16_72;
	codes[0x73] = DasmX86_16_73;
	codes[0x74] = DasmX86_16_74;
	codes[0x75] = DasmX86_16_75;
	codes[0x76] = DasmX86_16_76;
	codes[0x77] = DasmX86_16_77;
	codes[0x78] = DasmX86_16_78;
	codes[0x79] = DasmX86_16_79;
	codes[0x7A] = DasmX86_16_7A;
	codes[0x7B] = DasmX86_16_7B;
	codes[0x7C] = DasmX86_16_7C;
	codes[0x7D] = DasmX86_16_7D;
	codes[0x7E] = DasmX86_16_7E;
	codes[0x7F] = DasmX86_16_7F;
	codes[0x80] = DasmX86_16_80;
	codes[0x81] = DasmX86_16_81;
	codes[0x82] = DasmX86_16_82;
	codes[0x83] = DasmX86_16_83;
	codes[0x84] = DasmX86_16_84;
	codes[0x85] = DasmX86_16_85;
	codes[0x86] = DasmX86_16_86;
	codes[0x87] = DasmX86_16_87;
	codes[0x88] = DasmX86_16_88;
	codes[0x89] = DasmX86_16_89;
	codes[0x8A] = DasmX86_16_8A;
	codes[0x8B] = DasmX86_16_8B;
	codes[0x8C] = DasmX86_16_8C;
	codes[0x8D] = DasmX86_16_8D;
	codes[0x8E] = DasmX86_16_8E;
	codes[0x8F] = DasmX86_16_8F;
	codes[0x90] = DasmX86_16_90;
	codes[0x91] = DasmX86_16_91;
	codes[0x92] = DasmX86_16_92;
	codes[0x93] = DasmX86_16_93;
	codes[0x94] = DasmX86_16_94;
	codes[0x95] = DasmX86_16_95;
	codes[0x96] = DasmX86_16_96;
	codes[0x97] = DasmX86_16_97;
	codes[0x98] = DasmX86_16_98;
	codes[0x99] = DasmX86_16_99;
	codes[0x9A] = DasmX86_16_9A;
	codes[0x9B] = DasmX86_16_9B;
	codes[0x9C] = DasmX86_16_9C;
	codes[0x9D] = DasmX86_16_9D;
	codes[0x9E] = DasmX86_16_9E;
	codes[0x9F] = DasmX86_16_9F;
	codes[0xA0] = DasmX86_16_A0;
	codes[0xA1] = DasmX86_16_A1;
	codes[0xA2] = DasmX86_16_A2;
	codes[0xA3] = DasmX86_16_A3;
	codes[0xA4] = DasmX86_16_A4;
	codes[0xA5] = DasmX86_16_A5;
	codes[0xA6] = DasmX86_16_A6;
	codes[0xA7] = DasmX86_16_A7;
	codes[0xA8] = DasmX86_16_A8;
	codes[0xA9] = DasmX86_16_A9;
	codes[0xAA] = DasmX86_16_AA;
	codes[0xAB] = DasmX86_16_AB;
	codes[0xAC] = DasmX86_16_AC;
	codes[0xAD] = DasmX86_16_AD;
	codes[0xAE] = DasmX86_16_AE;
	codes[0xAF] = DasmX86_16_AF;
	codes[0xB0] = DasmX86_16_B0;
	codes[0xB1] = DasmX86_16_B1;
	codes[0xB2] = DasmX86_16_B2;
	codes[0xB3] = DasmX86_16_B3;
	codes[0xB4] = DasmX86_16_B4;
	codes[0xB5] = DasmX86_16_B5;
	codes[0xB6] = DasmX86_16_B6;
	codes[0xB7] = DasmX86_16_B7;
	codes[0xB8] = DasmX86_16_B8;
	codes[0xB9] = DasmX86_16_B9;
	codes[0xBA] = DasmX86_16_BA;
	codes[0xBB] = DasmX86_16_BB;
	codes[0xBC] = DasmX86_16_BC;
	codes[0xBD] = DasmX86_16_BD;
	codes[0xBE] = DasmX86_16_BE;
	codes[0xBF] = DasmX86_16_BF;
	codes[0xC0] = DasmX86_16_C0;
	codes[0xC1] = DasmX86_16_C1;
	codes[0xC2] = DasmX86_16_C2;
	codes[0xC3] = DasmX86_16_C3;
	codes[0xC4] = DasmX86_16_C4;
	codes[0xC5] = DasmX86_16_C5;
	codes[0xC6] = DasmX86_16_C6;
	codes[0xC7] = DasmX86_16_C7;
	codes[0xC8] = DasmX86_16_C8;
	codes[0xC9] = DasmX86_16_C9;
	codes[0xCA] = DasmX86_16_CA;
	codes[0xCB] = DasmX86_16_CB;
	codes[0xCC] = DasmX86_16_CC;
	codes[0xCD] = DasmX86_16_CD;
	codes[0xCE] = DasmX86_16_CE;
	codes[0xCF] = DasmX86_16_CF;
	codes[0xD0] = DasmX86_16_D0;
	codes[0xD1] = DasmX86_16_D1;
	codes[0xD2] = DasmX86_16_D2;
	codes[0xD3] = DasmX86_16_D3;
	codes[0xD4] = DasmX86_16_D4;
	codes[0xD5] = DasmX86_16_D5;
	codes[0xD6] = DasmX86_16_D6;
	codes[0xD7] = DasmX86_16_D7;
	codes[0xD8] = DasmX86_16_D8;
	codes[0xD9] = DasmX86_16_D9;
	codes[0xDA] = DasmX86_16_DA;
	codes[0xDB] = DasmX86_16_DB;
	codes[0xDC] = DasmX86_16_DC;
	codes[0xDD] = DasmX86_16_DD;
	codes[0xDE] = DasmX86_16_DE;
	codes[0xDF] = DasmX86_16_DF;
	codes[0xE0] = DasmX86_16_E0;
	codes[0xE1] = DasmX86_16_E1;
	codes[0xE2] = DasmX86_16_E2;
	codes[0xE3] = DasmX86_16_E3;
	codes[0xE4] = DasmX86_16_E4;
	codes[0xE5] = DasmX86_16_E5;
	codes[0xE6] = DasmX86_16_E6;
	codes[0xE7] = DasmX86_16_E7;
	codes[0xE8] = DasmX86_16_E8;
	codes[0xE9] = DasmX86_16_E9;
	codes[0xEA] = DasmX86_16_EA;
	codes[0xEB] = DasmX86_16_EB;
	codes[0xEC] = DasmX86_16_EC;
	codes[0xED] = DasmX86_16_ED;
	codes[0xEE] = DasmX86_16_EE;
	codes[0xEF] = DasmX86_16_EF;
	codes[0xF0] = DasmX86_16_F0;
	codes[0xF1] = DasmX86_16_F1;
	codes[0xF2] = DasmX86_16_F2;
	codes[0xF3] = DasmX86_16_F3;
	codes[0xF4] = DasmX86_16_F4;
	codes[0xF5] = DasmX86_16_F5;
	codes[0xF6] = DasmX86_16_F6;
	codes[0xF7] = DasmX86_16_F7;
	codes[0xF8] = DasmX86_16_F8;
	codes[0xF9] = DasmX86_16_F9;
	codes[0xFA] = DasmX86_16_FA;
	codes[0xFB] = DasmX86_16_FB;
	codes[0xFC] = DasmX86_16_FC;
	codes[0xFD] = DasmX86_16_FD;
	codes[0xFE] = DasmX86_16_FE;
	codes[0xFF] = DasmX86_16_FF;

	codes = this->codes0f = MemAlloc(DasmX86_16_Code, 256);
	codes[0x00] = DasmX86_16_0F00;
	codes[0x01] = DasmX86_16_0F01;
	codes[0x02] = DasmX86_16_0F02;
	codes[0x03] = DasmX86_16_0F03;
	codes[0x04] = DasmX86_16_0F04;
	codes[0x05] = DasmX86_16_0F05;
	codes[0x06] = DasmX86_16_0F06;
	codes[0x07] = DasmX86_16_0F07;
	codes[0x08] = DasmX86_16_0F08;
	codes[0x09] = DasmX86_16_0F09;
	codes[0x0A] = DasmX86_16_0F0A;
	codes[0x0B] = DasmX86_16_0F0B;
	codes[0x0C] = DasmX86_16_0F0C;
	codes[0x0D] = DasmX86_16_0F0D;
	codes[0x0E] = DasmX86_16_0F0E;
	codes[0x0F] = DasmX86_16_0F0F;
	codes[0x10] = DasmX86_16_0F10;
	codes[0x11] = DasmX86_16_0F11;
	codes[0x12] = DasmX86_16_0F12;
	codes[0x13] = DasmX86_16_0F13;
	codes[0x14] = DasmX86_16_0F14;
	codes[0x15] = DasmX86_16_0F15;
	codes[0x16] = DasmX86_16_0F16;
	codes[0x17] = DasmX86_16_0F17;
	codes[0x18] = DasmX86_16_0F18;
	codes[0x19] = DasmX86_16_0F19;
	codes[0x1A] = DasmX86_16_0F1A;
	codes[0x1B] = DasmX86_16_0F1B;
	codes[0x1C] = DasmX86_16_0F1C;
	codes[0x1D] = DasmX86_16_0F1D;
	codes[0x1E] = DasmX86_16_0F1E;
	codes[0x1F] = DasmX86_16_0F1F;
	codes[0x20] = DasmX86_16_0F20;
	codes[0x21] = DasmX86_16_0F21;
	codes[0x22] = DasmX86_16_0F22;
	codes[0x23] = DasmX86_16_0F23;
	codes[0x24] = DasmX86_16_0F24;
	codes[0x25] = DasmX86_16_0F25;
	codes[0x26] = DasmX86_16_0F26;
	codes[0x27] = DasmX86_16_0F27;
	codes[0x28] = DasmX86_16_0F28;
	codes[0x29] = DasmX86_16_0F29;
	codes[0x2A] = DasmX86_16_0F2A;
	codes[0x2B] = DasmX86_16_0F2B;
	codes[0x2C] = DasmX86_16_0F2C;
	codes[0x2D] = DasmX86_16_0F2D;
	codes[0x2E] = DasmX86_16_0F2E;
	codes[0x2F] = DasmX86_16_0F2F;
	codes[0x30] = DasmX86_16_0F30;
	codes[0x31] = DasmX86_16_0F31;
	codes[0x32] = DasmX86_16_0F32;
	codes[0x33] = DasmX86_16_0F33;
	codes[0x34] = DasmX86_16_0F34;
	codes[0x35] = DasmX86_16_0F35;
	codes[0x36] = DasmX86_16_0F36;
	codes[0x37] = DasmX86_16_0F37;
	codes[0x38] = DasmX86_16_0F38;
	codes[0x39] = DasmX86_16_0F39;
	codes[0x3A] = DasmX86_16_0F3A;
	codes[0x3B] = DasmX86_16_0F3B;
	codes[0x3C] = DasmX86_16_0F3C;
	codes[0x3D] = DasmX86_16_0F3D;
	codes[0x3E] = DasmX86_16_0F3E;
	codes[0x3F] = DasmX86_16_0F3F;
	codes[0x40] = DasmX86_16_0F40;
	codes[0x41] = DasmX86_16_0F41;
	codes[0x42] = DasmX86_16_0F42;
	codes[0x43] = DasmX86_16_0F43;
	codes[0x44] = DasmX86_16_0F44;
	codes[0x45] = DasmX86_16_0F45;
	codes[0x46] = DasmX86_16_0F46;
	codes[0x47] = DasmX86_16_0F47;
	codes[0x48] = DasmX86_16_0F48;
	codes[0x49] = DasmX86_16_0F49;
	codes[0x4A] = DasmX86_16_0F4A;
	codes[0x4B] = DasmX86_16_0F4B;
	codes[0x4C] = DasmX86_16_0F4C;
	codes[0x4D] = DasmX86_16_0F4D;
	codes[0x4E] = DasmX86_16_0F4E;
	codes[0x4F] = DasmX86_16_0F4F;
	codes[0x50] = DasmX86_16_0F50;
	codes[0x51] = DasmX86_16_0F51;
	codes[0x52] = DasmX86_16_0F52;
	codes[0x53] = DasmX86_16_0F53;
	codes[0x54] = DasmX86_16_0F54;
	codes[0x55] = DasmX86_16_0F55;
	codes[0x56] = DasmX86_16_0F56;
	codes[0x57] = DasmX86_16_0F57;
	codes[0x58] = DasmX86_16_0F58;
	codes[0x59] = DasmX86_16_0F59;
	codes[0x5A] = DasmX86_16_0F5A;
	codes[0x5B] = DasmX86_16_0F5B;
	codes[0x5C] = DasmX86_16_0F5C;
	codes[0x5D] = DasmX86_16_0F5D;
	codes[0x5E] = DasmX86_16_0F5E;
	codes[0x5F] = DasmX86_16_0F5F;
	codes[0x60] = DasmX86_16_0F60;
	codes[0x61] = DasmX86_16_0F61;
	codes[0x62] = DasmX86_16_0F62;
	codes[0x63] = DasmX86_16_0F63;
	codes[0x64] = DasmX86_16_0F64;
	codes[0x65] = DasmX86_16_0F65;
	codes[0x66] = DasmX86_16_0F66;
	codes[0x67] = DasmX86_16_0F67;
	codes[0x68] = DasmX86_16_0F68;
	codes[0x69] = DasmX86_16_0F69;
	codes[0x6A] = DasmX86_16_0F6A;
	codes[0x6B] = DasmX86_16_0F6B;
	codes[0x6C] = DasmX86_16_0F6C;
	codes[0x6D] = DasmX86_16_0F6D;
	codes[0x6E] = DasmX86_16_0F6E;
	codes[0x6F] = DasmX86_16_0F6F;
	codes[0x70] = DasmX86_16_0F70;
	codes[0x71] = DasmX86_16_0F71;
	codes[0x72] = DasmX86_16_0F72;
	codes[0x73] = DasmX86_16_0F73;
	codes[0x74] = DasmX86_16_0F74;
	codes[0x75] = DasmX86_16_0F75;
	codes[0x76] = DasmX86_16_0F76;
	codes[0x77] = DasmX86_16_0F77;
	codes[0x78] = DasmX86_16_0F78;
	codes[0x79] = DasmX86_16_0F79;
	codes[0x7A] = DasmX86_16_0F7A;
	codes[0x7B] = DasmX86_16_0F7B;
	codes[0x7C] = DasmX86_16_0F7C;
	codes[0x7D] = DasmX86_16_0F7D;
	codes[0x7E] = DasmX86_16_0F7E;
	codes[0x7F] = DasmX86_16_0F7F;
	codes[0x80] = DasmX86_16_0F80;
	codes[0x81] = DasmX86_16_0F81;
	codes[0x82] = DasmX86_16_0F82;
	codes[0x83] = DasmX86_16_0F83;
	codes[0x84] = DasmX86_16_0F84;
	codes[0x85] = DasmX86_16_0F85;
	codes[0x86] = DasmX86_16_0F86;
	codes[0x87] = DasmX86_16_0F87;
	codes[0x88] = DasmX86_16_0F88;
	codes[0x89] = DasmX86_16_0F89;
	codes[0x8A] = DasmX86_16_0F8A;
	codes[0x8B] = DasmX86_16_0F8B;
	codes[0x8C] = DasmX86_16_0F8C;
	codes[0x8D] = DasmX86_16_0F8D;
	codes[0x8E] = DasmX86_16_0F8E;
	codes[0x8F] = DasmX86_16_0F8F;
	codes[0x90] = DasmX86_16_0F90;
	codes[0x91] = DasmX86_16_0F91;
	codes[0x92] = DasmX86_16_0F92;
	codes[0x93] = DasmX86_16_0F93;
	codes[0x94] = DasmX86_16_0F94;
	codes[0x95] = DasmX86_16_0F95;
	codes[0x96] = DasmX86_16_0F96;
	codes[0x97] = DasmX86_16_0F97;
	codes[0x98] = DasmX86_16_0F98;
	codes[0x99] = DasmX86_16_0F99;
	codes[0x9A] = DasmX86_16_0F9A;
	codes[0x9B] = DasmX86_16_0F9B;
	codes[0x9C] = DasmX86_16_0F9C;
	codes[0x9D] = DasmX86_16_0F9D;
	codes[0x9E] = DasmX86_16_0F9E;
	codes[0x9F] = DasmX86_16_0F9F;
	codes[0xA0] = DasmX86_16_0FA0;
	codes[0xA1] = DasmX86_16_0FA1;
	codes[0xA2] = DasmX86_16_0FA2;
	codes[0xA3] = DasmX86_16_0FA3;
	codes[0xA4] = DasmX86_16_0FA4;
	codes[0xA5] = DasmX86_16_0FA5;
	codes[0xA6] = DasmX86_16_0FA6;
	codes[0xA7] = DasmX86_16_0FA7;
	codes[0xA8] = DasmX86_16_0FA8;
	codes[0xA9] = DasmX86_16_0FA9;
	codes[0xAA] = DasmX86_16_0FAA;
	codes[0xAB] = DasmX86_16_0FAB;
	codes[0xAC] = DasmX86_16_0FAC;
	codes[0xAD] = DasmX86_16_0FAD;
	codes[0xAE] = DasmX86_16_0FAE;
	codes[0xAF] = DasmX86_16_0FAF;
	codes[0xB0] = DasmX86_16_0FB0;
	codes[0xB1] = DasmX86_16_0FB1;
	codes[0xB2] = DasmX86_16_0FB2;
	codes[0xB3] = DasmX86_16_0FB3;
	codes[0xB4] = DasmX86_16_0FB4;
	codes[0xB5] = DasmX86_16_0FB5;
	codes[0xB6] = DasmX86_16_0FB6;
	codes[0xB7] = DasmX86_16_0FB7;
	codes[0xB8] = DasmX86_16_0FB8;
	codes[0xB9] = DasmX86_16_0FB9;
	codes[0xBA] = DasmX86_16_0FBA;
	codes[0xBB] = DasmX86_16_0FBB;
	codes[0xBC] = DasmX86_16_0FBC;
	codes[0xBD] = DasmX86_16_0FBD;
	codes[0xBE] = DasmX86_16_0FBE;
	codes[0xBF] = DasmX86_16_0FBF;
	codes[0xC0] = DasmX86_16_0FC0;
	codes[0xC1] = DasmX86_16_0FC1;
	codes[0xC2] = DasmX86_16_0FC2;
	codes[0xC3] = DasmX86_16_0FC3;
	codes[0xC4] = DasmX86_16_0FC4;
	codes[0xC5] = DasmX86_16_0FC5;
	codes[0xC6] = DasmX86_16_0FC6;
	codes[0xC7] = DasmX86_16_0FC7;
	codes[0xC8] = DasmX86_16_0FC8;
	codes[0xC9] = DasmX86_16_0FC9;
	codes[0xCA] = DasmX86_16_0FCA;
	codes[0xCB] = DasmX86_16_0FCB;
	codes[0xCC] = DasmX86_16_0FCC;
	codes[0xCD] = DasmX86_16_0FCD;
	codes[0xCE] = DasmX86_16_0FCE;
	codes[0xCF] = DasmX86_16_0FCF;
	codes[0xD0] = DasmX86_16_0FD0;
	codes[0xD1] = DasmX86_16_0FD1;
	codes[0xD2] = DasmX86_16_0FD2;
	codes[0xD3] = DasmX86_16_0FD3;
	codes[0xD4] = DasmX86_16_0FD4;
	codes[0xD5] = DasmX86_16_0FD5;
	codes[0xD6] = DasmX86_16_0FD6;
	codes[0xD7] = DasmX86_16_0FD7;
	codes[0xD8] = DasmX86_16_0FD8;
	codes[0xD9] = DasmX86_16_0FD9;
	codes[0xDA] = DasmX86_16_0FDA;
	codes[0xDB] = DasmX86_16_0FDB;
	codes[0xDC] = DasmX86_16_0FDC;
	codes[0xDD] = DasmX86_16_0FDD;
	codes[0xDE] = DasmX86_16_0FDE;
	codes[0xDF] = DasmX86_16_0FDF;
	codes[0xE0] = DasmX86_16_0FE0;
	codes[0xE1] = DasmX86_16_0FE1;
	codes[0xE2] = DasmX86_16_0FE2;
	codes[0xE3] = DasmX86_16_0FE3;
	codes[0xE4] = DasmX86_16_0FE4;
	codes[0xE5] = DasmX86_16_0FE5;
	codes[0xE6] = DasmX86_16_0FE6;
	codes[0xE7] = DasmX86_16_0FE7;
	codes[0xE8] = DasmX86_16_0FE8;
	codes[0xE9] = DasmX86_16_0FE9;
	codes[0xEA] = DasmX86_16_0FEA;
	codes[0xEB] = DasmX86_16_0FEB;
	codes[0xEC] = DasmX86_16_0FEC;
	codes[0xED] = DasmX86_16_0FED;
	codes[0xEE] = DasmX86_16_0FEE;
	codes[0xEF] = DasmX86_16_0FEF;
	codes[0xF0] = DasmX86_16_0FF0;
	codes[0xF1] = DasmX86_16_0FF1;
	codes[0xF2] = DasmX86_16_0FF2;
	codes[0xF3] = DasmX86_16_0FF3;
	codes[0xF4] = DasmX86_16_0FF4;
	codes[0xF5] = DasmX86_16_0FF5;
	codes[0xF6] = DasmX86_16_0FF6;
	codes[0xF7] = DasmX86_16_0FF7;
	codes[0xF8] = DasmX86_16_0FF8;
	codes[0xF9] = DasmX86_16_0FF9;
	codes[0xFA] = DasmX86_16_0FFA;
	codes[0xFB] = DasmX86_16_0FFB;
	codes[0xFC] = DasmX86_16_0FFC;
	codes[0xFD] = DasmX86_16_0FFD;
	codes[0xFE] = DasmX86_16_0FFE;
	codes[0xFF] = DasmX86_16_0FFF;

}

Manage::DasmX86_16::~DasmX86_16()
{
	MemFree(this->codes);
	MemFree(this->codes0f);
}

Manage::DasmX86_16::DasmX86_16_Sess *Manage::DasmX86_16::CreateSess(Manage::DasmX86_16::DasmX86_16_Regs *regs, UInt8 *code, UInt16 codeSegm)
{
	Manage::DasmX86_16::DasmX86_16_Sess *sess;
	NEW_CLASS(sess, Manage::DasmX86_16::DasmX86_16_Sess());
	sess->code = code;
	sess->codeSegm = codeSegm;
	sess->codeHdlrs = (void**)this->codes;
	sess->code0fHdlrs = (void**)this->codes0f;
	MemCopyNO(&sess->regs, regs, sizeof(Manage::DasmX86_16::DasmX86_16_Regs));
	return sess;
}

void Manage::DasmX86_16::DeleteSess(Manage::DasmX86_16::DasmX86_16_Sess *sess)
{
	if (sess)
	{
		DEL_CLASS(sess);
	}
}

Text::CString Manage::DasmX86_16::GetHeader(Bool fullRegs) const
{
	return CSTR("EAX\tEDX\tECX\tEBX");
}

Bool Manage::DasmX86_16::Disasm16(NN<IO::Writer> writer, Manage::AddressResolver *addrResol, UInt16 *currInst, UInt16 *currStack, UInt16 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt16 *blockStart, UInt16 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs)
{
	return false;
}

Manage::Dasm::Dasm_Regs *Manage::DasmX86_16::CreateRegs() const
{
	DasmX86_16_Regs *regs = MemAlloc(DasmX86_16_Regs, 1);
	return regs;
}

void Manage::DasmX86_16::FreeRegs(Dasm_Regs *regs) const
{
	MemFree(regs);
}

Bool Manage::DasmX86_16::DasmNext(Manage::DasmX86_16::DasmX86_16_Sess *sess, UTF8Char *buff, UOSInt *outBuffSize)
{
	*buff = 0;
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
	*outBuffSize = (UOSInt)(sess->outSPtr - buff);
	return isSucc;
} //True = succ
