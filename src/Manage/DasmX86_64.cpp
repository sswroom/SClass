#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Manage/DasmX86_64.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"
#include <wchar.h>

#ifndef __GNUC__
#pragma warning( disable : 4311 4312)
#endif

static const UTF8Char *DasmX86_64_Segms[] = {(const UTF8Char*)"cs:", (const UTF8Char*)"ds:", (const UTF8Char*)"es:", (const UTF8Char*)"fs:", (const UTF8Char*)"gs:", (const UTF8Char*)"ss:"};

Bool DasmX86_64_IsEndFunc(const UTF8Char *funcName, UOSInt nameLen)
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

UTF8Char *DasmX86_64_ParseReg8(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo, UInt8 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = (UInt8*)&sess->regs.rax;
		return Text::StrConcatC(regName, UTF8STRC("AL"));
	case 1:
		*regPtr = (UInt8*)&sess->regs.rcx;
		return Text::StrConcatC(regName, UTF8STRC("CL"));
	case 2:
		*regPtr = (UInt8*)&sess->regs.rdx;
		return Text::StrConcatC(regName, UTF8STRC("DL"));
	case 3:
		*regPtr = (UInt8*)&sess->regs.rbx;
		return Text::StrConcatC(regName, UTF8STRC("BL"));
	case 4:
		*regPtr = 1 + (UInt8*)&sess->regs.rax;
		return Text::StrConcatC(regName, UTF8STRC("AH"));
	case 5:
		*regPtr = 1 + (UInt8*)&sess->regs.rcx;
		return Text::StrConcatC(regName, UTF8STRC("CH"));
	case 6:
		*regPtr = 1 + (UInt8*)&sess->regs.rdx;
		return Text::StrConcatC(regName, UTF8STRC("DH"));
	case 7:
		*regPtr = 1 + (UInt8*)&sess->regs.rbx;
		return Text::StrConcatC(regName, UTF8STRC("BH"));
	case 8:
		*regPtr = (UInt8*)&sess->regs.r8;
		return Text::StrConcatC(regName, UTF8STRC("R8B"));
	case 9:
		*regPtr = (UInt8*)&sess->regs.r9;
		return Text::StrConcatC(regName, UTF8STRC("R9B"));
	case 10:
		*regPtr = (UInt8*)&sess->regs.r10;
		return Text::StrConcatC(regName, UTF8STRC("R10B"));
	case 11:
		*regPtr = (UInt8*)&sess->regs.r11;
		return Text::StrConcatC(regName, UTF8STRC("R11B"));
	case 12:
		*regPtr = (UInt8*)&sess->regs.r12;
		return Text::StrConcatC(regName, UTF8STRC("R12B"));
	case 13:
		*regPtr = (UInt8*)&sess->regs.r13;
		return Text::StrConcatC(regName, UTF8STRC("R13B"));
	case 14:
		*regPtr = (UInt8*)&sess->regs.r14;
		return Text::StrConcatC(regName, UTF8STRC("R14B"));
	case 15:
		*regPtr = (UInt8*)&sess->regs.r15;
		return Text::StrConcatC(regName, UTF8STRC("R15B"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseReg16(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo, UInt64 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = (UInt64*)&sess->regs.rax;
		return Text::StrConcatC(regName, UTF8STRC("AX"));
	case 1:
		*regPtr = (UInt64*)&sess->regs.rcx;
		return Text::StrConcatC(regName, UTF8STRC("CX"));
	case 2:
		*regPtr = (UInt64*)&sess->regs.rdx;
		return Text::StrConcatC(regName, UTF8STRC("DX"));
	case 3:
		*regPtr = (UInt64*)&sess->regs.rbx;
		return Text::StrConcatC(regName, UTF8STRC("BX"));
	case 4:
		*regPtr = (UInt64*)&sess->regs.rsp;
		return Text::StrConcatC(regName, UTF8STRC("SP"));
	case 5:
		*regPtr = (UInt64*)&sess->regs.rbp;
		return Text::StrConcatC(regName, UTF8STRC("BP"));
	case 6:
		*regPtr = (UInt64*)&sess->regs.rsi;
		return Text::StrConcatC(regName, UTF8STRC("SI"));
	case 7:
		*regPtr = (UInt64*)&sess->regs.rdi;
		return Text::StrConcatC(regName, UTF8STRC("DI"));
	case 8:
		*regPtr = (UInt64*)&sess->regs.r8;
		return Text::StrConcatC(regName, UTF8STRC("R8W"));
	case 9:
		*regPtr = (UInt64*)&sess->regs.r9;
		return Text::StrConcatC(regName, UTF8STRC("R9W"));
	case 10:
		*regPtr = (UInt64*)&sess->regs.r10;
		return Text::StrConcatC(regName, UTF8STRC("R10W"));
	case 11:
		*regPtr = (UInt64*)&sess->regs.r11;
		return Text::StrConcatC(regName, UTF8STRC("R11W"));
	case 12:
		*regPtr = (UInt64*)&sess->regs.r12;
		return Text::StrConcatC(regName, UTF8STRC("R12W"));
	case 13:
		*regPtr = (UInt64*)&sess->regs.r13;
		return Text::StrConcatC(regName, UTF8STRC("R13W"));
	case 14:
		*regPtr = (UInt64*)&sess->regs.r14;
		return Text::StrConcatC(regName, UTF8STRC("R14W"));
	case 15:
		*regPtr = (UInt64*)&sess->regs.r15;
		return Text::StrConcatC(regName, UTF8STRC("R15W"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseReg32(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo, UInt64 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = (UInt64*)&sess->regs.rax;
		return Text::StrConcatC(regName, UTF8STRC("EAX"));
	case 1:
		*regPtr = (UInt64*)&sess->regs.rcx;
		return Text::StrConcatC(regName, UTF8STRC("ECX"));
	case 2:
		*regPtr = (UInt64*)&sess->regs.rdx;
		return Text::StrConcatC(regName, UTF8STRC("EDX"));
	case 3:
		*regPtr = (UInt64*)&sess->regs.rbx;
		return Text::StrConcatC(regName, UTF8STRC("EBX"));
	case 4:
		*regPtr = (UInt64*)&sess->regs.rsp;
		return Text::StrConcatC(regName, UTF8STRC("ESP"));
	case 5:
		*regPtr = (UInt64*)&sess->regs.rbp;
		return Text::StrConcatC(regName, UTF8STRC("EBP"));
	case 6:
		*regPtr = (UInt64*)&sess->regs.rsi;
		return Text::StrConcatC(regName, UTF8STRC("ESI"));
	case 7:
		*regPtr = (UInt64*)&sess->regs.rdi;
		return Text::StrConcatC(regName, UTF8STRC("EDI"));
	case 8:
		*regPtr = (UInt64*)&sess->regs.r8;
		return Text::StrConcatC(regName, UTF8STRC("R8D"));
	case 9:
		*regPtr = (UInt64*)&sess->regs.r9;
		return Text::StrConcatC(regName, UTF8STRC("R9D"));
	case 10:
		*regPtr = (UInt64*)&sess->regs.r10;
		return Text::StrConcatC(regName, UTF8STRC("R10D"));
	case 11:
		*regPtr = (UInt64*)&sess->regs.r11;
		return Text::StrConcatC(regName, UTF8STRC("R11D"));
	case 12:
		*regPtr = (UInt64*)&sess->regs.r12;
		return Text::StrConcatC(regName, UTF8STRC("R12D"));
	case 13:
		*regPtr = (UInt64*)&sess->regs.r13;
		return Text::StrConcatC(regName, UTF8STRC("R13D"));
	case 14:
		*regPtr = (UInt64*)&sess->regs.r14;
		return Text::StrConcatC(regName, UTF8STRC("R14D"));
	case 15:
		*regPtr = (UInt64*)&sess->regs.r15;
		return Text::StrConcatC(regName, UTF8STRC("R15D"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseReg64(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo, UInt64 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = (UInt64*)&sess->regs.rax;
		return Text::StrConcatC(regName, UTF8STRC("RAX"));
	case 1:
		*regPtr = (UInt64*)&sess->regs.rcx;
		return Text::StrConcatC(regName, UTF8STRC("RCX"));
	case 2:
		*regPtr = (UInt64*)&sess->regs.rdx;
		return Text::StrConcatC(regName, UTF8STRC("RDX"));
	case 3:
		*regPtr = (UInt64*)&sess->regs.rbx;
		return Text::StrConcatC(regName, UTF8STRC("RBX"));
	case 4:
		*regPtr = (UInt64*)&sess->regs.rsp;
		return Text::StrConcatC(regName, UTF8STRC("RSP"));
	case 5:
		*regPtr = (UInt64*)&sess->regs.rbp;
		return Text::StrConcatC(regName, UTF8STRC("RBP"));
	case 6:
		*regPtr = (UInt64*)&sess->regs.rsi;
		return Text::StrConcatC(regName, UTF8STRC("RSI"));
	case 7:
		*regPtr = (UInt64*)&sess->regs.rdi;
		return Text::StrConcatC(regName, UTF8STRC("RDI"));
	case 8:
		*regPtr = (UInt64*)&sess->regs.r8;
		return Text::StrConcatC(regName, UTF8STRC("R8"));
	case 9:
		*regPtr = (UInt64*)&sess->regs.r9;
		return Text::StrConcatC(regName, UTF8STRC("R9"));
	case 10:
		*regPtr = (UInt64*)&sess->regs.r10;
		return Text::StrConcatC(regName, UTF8STRC("R10"));
	case 11:
		*regPtr = (UInt64*)&sess->regs.r11;
		return Text::StrConcatC(regName, UTF8STRC("R11"));
	case 12:
		*regPtr = (UInt64*)&sess->regs.r12;
		return Text::StrConcatC(regName, UTF8STRC("R12"));
	case 13:
		*regPtr = (UInt64*)&sess->regs.r13;
		return Text::StrConcatC(regName, UTF8STRC("R13"));
	case 14:
		*regPtr = (UInt64*)&sess->regs.r14;
		return Text::StrConcatC(regName, UTF8STRC("R14"));
	case 15:
		*regPtr = (UInt64*)&sess->regs.r15;
		return Text::StrConcatC(regName, UTF8STRC("R15"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseSReg(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo)
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
		return Text::StrConcatC(regName, UTF8STRC(""));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC(""));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseReg(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo, UInt64 **regPtr)
{
	if (sess->thisStatus & 1)
	{
		return DasmX86_64_ParseReg16(sess, regName, regNo, regPtr);
	}
	else if (sess->thisStatus & 0x800)
	{
		return DasmX86_64_ParseReg64(sess, regName, regNo, regPtr);
	}
	else
	{
		return DasmX86_64_ParseReg32(sess, regName, regNo, regPtr);
	}
}

UTF8Char *DasmX86_64_ParseRegMM(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("MM0"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("MM1"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("MM2"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("MM3"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("MM4"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("MM5"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("MM6"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("MM7"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseRegXMM(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("XMM0"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("XMM1"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("XMM2"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("XMM3"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("XMM4"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("XMM5"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("XMM6"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("XMM7"));
	case 8:
		return Text::StrConcatC(regName, UTF8STRC("XMM8"));
	case 9:
		return Text::StrConcatC(regName, UTF8STRC("XMM9"));
	case 10:
		return Text::StrConcatC(regName, UTF8STRC("XMM10"));
	case 11:
		return Text::StrConcatC(regName, UTF8STRC("XMM11"));
	case 12:
		return Text::StrConcatC(regName, UTF8STRC("XMM12"));
	case 13:
		return Text::StrConcatC(regName, UTF8STRC("XMM13"));
	case 14:
		return Text::StrConcatC(regName, UTF8STRC("XMM14"));
	case 15:
		return Text::StrConcatC(regName, UTF8STRC("XMM15"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseRegYMM(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("YMM0"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("YMM1"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("YMM2"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("YMM3"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("YMM4"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("YMM5"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("YMM6"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("YMM7"));
	case 8:
		return Text::StrConcatC(regName, UTF8STRC("YMM8"));
	case 9:
		return Text::StrConcatC(regName, UTF8STRC("YMM9"));
	case 10:
		return Text::StrConcatC(regName, UTF8STRC("YMM10"));
	case 11:
		return Text::StrConcatC(regName, UTF8STRC("YMM11"));
	case 12:
		return Text::StrConcatC(regName, UTF8STRC("YMM12"));
	case 13:
		return Text::StrConcatC(regName, UTF8STRC("YMM13"));
	case 14:
		return Text::StrConcatC(regName, UTF8STRC("YMM14"));
	case 15:
		return Text::StrConcatC(regName, UTF8STRC("YMM15"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseRegCR(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("CR0"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("CR1"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("CR2"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("CR3"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("CR4"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("CR5"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("CR6"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("CR7"));
	default:
		*regName = 0;
		return regName;
	}
}

UTF8Char *DasmX86_64_ParseRegDR(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("DR0"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("DR1"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("DR2"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("DR3"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("DR4"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("DR5"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("DR6"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("DR7"));
	default:
		*regName = 0;
		return regName;
	}
}

void DasmX86_64_ParseSIB(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Bool allowEbp, UInt64 *memAddr)
{
	UInt8 sib = sess->memReader->ReadMemUInt8(sess->regs.rip);
	UInt8 sibi = (UInt8)(((sib >> 3) & 7) | ((sess->thisStatus & 0x200) >> 6));
	UInt8 sibs = (UInt8)(sib >> 6);
	UInt8 sibb = (UInt8)((sib & 7) | ((sess->thisStatus & 0x100) >> 5));
	Int32 baseReg = 16;
	Int32 indexReg = 16;
	Int32 addr = 0;
	Int32 indexMul = sibs;
	UInt64 srcAddr;
	UInt64 *regPtr;
	sess->regs.rip += 1;

	if (sibi == 4)
	{
		indexMul = 0;
	}
	else
	{
		indexReg = sibi;
	}
	if (sibb == 5)
	{
		if (sib == 0x85)
		{
			sib = 0x85;
		}
		if (sibs == 0)
		{
			baseReg = 5;
		}
		else if (sibs == 1)
		{
			addr = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip);
			baseReg = 5;
			sess->regs.rip += 1;
		}
		else if (sibs == 2)
		{
			if (allowEbp)
			{
				baseReg = 5;
			}
			else
			{
				addr = (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip);
				sess->regs.rip += 4;
			}
		}
		else if (sibs == 3)
		{
			addr = (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip);
			sess->regs.rip += 4;
		}
	}
	else
	{
		baseReg = sibb;
	}

	if (baseReg != 16)
	{
		memName = DasmX86_64_ParseReg64(sess, memName, baseReg, &regPtr);
		srcAddr = *regPtr;
		if (indexReg != 16)
		{
			*memName++ = '+';
			memName = DasmX86_64_ParseReg64(sess, memName, indexReg, &regPtr);
			if (indexMul == 1)
			{
				*memName++ = '*';
				*memName++ = '2';
				srcAddr += *regPtr * 2;
			}
			else if (indexMul == 2)
			{
				*memName++ = '*';
				*memName++ = '4';
				srcAddr += *regPtr * 4;
			}
			else if (indexMul == 3)
			{
				*memName++ = '*';
				*memName++ = '8';
				srcAddr += *regPtr * 8;
			}
			else
			{
				srcAddr += *regPtr;
			}
		}
	}
	else
	{
		memName = DasmX86_64_ParseReg64(sess, memName, indexReg, &regPtr);
		if (indexMul == 1)
		{
			*memName++ = '*';
			*memName++ = '2';
			srcAddr = *regPtr * 2;
		}
		else if (indexMul == 2)
		{
			*memName++ = '*';
			*memName++ = '4';
			srcAddr = *regPtr * 4;
		}
		else if (indexMul == 3)
		{
			*memName++ = '*';
			*memName++ = '8';
			srcAddr = *regPtr * 8;
		}
		else
		{
			srcAddr = *regPtr;
		}
	}

	if (addr)
	{
		if (addr < 0)
		{
			memName = Text::StrInt32(memName, addr);
		}
		else if (addr > 1024)
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+0x"));
			memName = Text::StrHexVal32(memName, (UInt32)addr);
		}
		else
		{
			*memName++ = '+';
			memName = Text::StrInt32(memName, addr);
		}
		srcAddr = (UInt64)((Int64)srcAddr + addr);
	}
	*memName = 0;
	if (memAddr)
		*memAddr = srcAddr;
}

UTF8Char *DasmX86_64_ParseAddr32(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg, UInt64 *memAddr)
{
	UTF8Char mem[64];
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	Int32 addr;
	UInt64 srcAddr;
	UInt64 *regPtr;
	UInt8 rm = (b & 7);
	*reg = ((b >> 3) & 7) | ((sess->thisStatus & 0x400) >> 7);
	switch (b >> 6)
	{
	case 0:
		if (rm == 5)
		{
			if (sess->thisStatus & 0x10000)
				memName = Text::StrConcat(memName, DasmX86_64_Segms[sess->segmId]);
			srcAddr = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
			sess->regs.rip += 5;
			memName = Text::StrConcatC(memName, UTF8STRC("[0x"));
			memName = Text::StrHexVal64(memName, (UInt64)((Int64)sess->regs.rip + (Int32)srcAddr));
			*memName++ = ']';
			*memName = 0;
			if (memAddr)
				*memAddr = (UInt64)((Int64)sess->regs.rip + (Int32)srcAddr);
		}
		else if (rm == 4)
		{
			sess->regs.rip += 1;

			if (sess->thisStatus & 0x10000)
				memName = Text::StrConcat(memName, DasmX86_64_Segms[sess->segmId]);
			*memName++ = '[';
			DasmX86_64_ParseSIB(sess, mem, false, &srcAddr);
			memName = Text::StrConcat(memName, mem);
			*memName++ = ']';
			*memName = 0;
			if (memAddr)
				*memAddr = srcAddr;
		}
		else
		{
			if (sess->thisStatus & 0x10000)
				memName = Text::StrConcat(memName, DasmX86_64_Segms[sess->segmId]);
			memName = Text::StrConcatC(memName, UTF8STRC("["));
			memName = DasmX86_64_ParseReg64(sess, memName, rm | ((sess->thisStatus & 0x100) >> 5), &regPtr);
			*memName++ = ']';
			*memName = 0;
			sess->regs.rip += 1;
			if (memAddr)
			{
				*memAddr = *regPtr;
			}
		}

		break;
	case 1:
		if (sess->thisStatus & 0x10000)
			memName = Text::StrConcat(memName, DasmX86_64_Segms[sess->segmId]);
		memName = Text::StrConcatC(memName, UTF8STRC("["));
		sess->regs.rip += 1;
		if (rm == 4)
		{
			DasmX86_64_ParseSIB(sess, mem, true, &srcAddr);
			memName = Text::StrConcat(memName, mem);
		}
		else
		{
			memName = DasmX86_64_ParseReg64(sess, memName, rm | ((sess->thisStatus & 0x100) >> 5), &regPtr);
			srcAddr = *regPtr;
		}
		if (sess->memReader->ReadMemUInt8(sess->regs.rip) & 0x80)
		{
			*memName++ = '-';
			memName = Text::StrInt32(memName, -(Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		}
		else
		{
			*memName++ = '+';
			memName = Text::StrInt32(memName, sess->memReader->ReadMemUInt8(sess->regs.rip));
		}
		if (memAddr)
		{
			srcAddr = (UInt64)((Int64)srcAddr + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
			*memAddr = srcAddr;
		}
		*memName++ = ']';
		*memName = 0;
		sess->regs.rip += 1;
		break;
	case 2:
		if (sess->thisStatus & 0x10000)
			memName = Text::StrConcat(memName, DasmX86_64_Segms[sess->segmId]);
		memName = Text::StrConcatC(memName, UTF8STRC("["));
		sess->regs.rip += 1;
		if (rm == 4)
		{
			DasmX86_64_ParseSIB(sess, mem, true, &srcAddr);
			memName = Text::StrConcat(memName, mem);
		}
		else
		{
			memName = DasmX86_64_ParseReg64(sess, memName, rm | ((sess->thisStatus & 0x100) >> 5), &regPtr);
			srcAddr = *regPtr;
		}
		addr = (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip);
		srcAddr += (UInt32)addr;
		if (((UInt32)addr & 0xffff0000) == 0xffff0000)
		{
			memName = Text::StrInt32(memName, addr);
		}
		else if (addr < 1024)
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+"));
			memName = Text::StrInt32(memName, addr);
		}
		else
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+0x"));
			memName = Text::StrHexVal32(memName, (UInt32)addr);
		}
		if (memAddr)
		{
			*memAddr = srcAddr;
		}
		*memName++ = ']';
		*memName = 0;
		sess->regs.rip += 4;
		break;
	case 3:
		break;
	}
	return memName;
}

void DasmX86_64_ParseModR32M8(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg, UInt32 *memVal)
{
	UInt64 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.rip += 1;
		if (memVal)
		{
			*memVal = *(UInt32*)regPtr;
		}
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("BYTE PTR ")), reg, &addr);
	}
}

void DasmX86_64_ParseModRM8(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg, UInt8 *memVal)
{
	UInt8 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg8(sess, memName, b & 7, &regPtr);
		if (memVal)
		{
			*memVal = *regPtr;
		}
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("BYTE PTR ")), reg, &addr);
		if (memVal)
		{
			*memVal = sess->memReader->ReadMemUInt8(addr);
		}
	}
}

void DasmX86_64_ParseModR32M16(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg, UInt32 *memVal)
{
	UInt64 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg32(sess, memName, b & 7, &regPtr);
		if (memVal)
		{
			*memVal = *(UInt32*)regPtr;
		}
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("WORD PTR ")), reg, &addr);
		if (memVal)
		{
			*memVal = sess->memReader->ReadMemUInt16(addr);
		}
	}
}

void DasmX86_64_ParseModRM16(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg, UInt16 *memVal)
{
	UInt64 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg16(sess, memName, b & 7, &regPtr);
		if (memVal)
		{
			*memVal = *(UInt16*)regPtr;
		}
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("WORD PTR ")), reg, &addr);
		if (memVal)
		{
			*memVal = sess->memReader->ReadMemUInt16(addr);
		}
	}
}

void DasmX86_64_ParseModRM32(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg, UInt64 *memVal, UInt64 *memAddr)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v;
		DasmX86_64_ParseModRM16(sess, memName, reg, &v);
		if (memVal)
			*memVal = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
		if ((b >> 6) == 3)
		{
			*reg = ((b >> 3) & 7) | ((sess->thisStatus & 0x400) >> 7);
			if (sess->thisStatus & 0x800)
			{
				DasmX86_64_ParseReg64(sess, memName, ((sess->thisStatus & 0x100) >> 5) | (b & 7), &regPtr);
				if (memVal)
				{
					*memVal = *regPtr;
				}
			}
			else
			{
				DasmX86_64_ParseReg32(sess, memName, ((sess->thisStatus & 0x100) >> 5) | (b & 7), &regPtr);
				if (memVal)
				{
					*memVal = *(UInt32*)regPtr;
				}
			}
			sess->regs.rip += 1;
		}
		else
		{
			if (sess->thisStatus & 0x800)
			{
				UInt64 addr;
				DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("QWORD PTR ")), reg, &addr);
				if (memVal)
				{
					*memVal = sess->memReader->ReadMemUInt64(addr);
				}
				if (memAddr)
				{
					*memAddr = addr;
				}
			}
			else
			{
				UInt64 addr;
				DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("DWORD PTR ")), reg, &addr);
				if (memVal)
				{
					*memVal = sess->memReader->ReadMemUInt32(addr);
				}
				if (memAddr)
				{
					*memAddr = addr;
				}
			}
		}
	}
}

void DasmX86_64_ParseModRM48(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt64 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("FWORD PTR ")), reg, &addr);
	}
}

void DasmX86_64_ParseModRM64(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt64 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("QWORD PTR ")), reg, &addr);
	}
}

/*void DasmX86_64_ParseModRMR80(Manage::DasmX86_64::DasmX86_64_Sess* sess, WChar *memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseRegx87(memName, b & 7);
		sess->regs.rip += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("REAL10 PTR ")), reg, &addr);
	}
}*/

void DasmX86_64_ParseModRM128(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt64 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("")), reg, &addr);
	}
}

void DasmX86_64_ParseModRMMM64(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseRegMM(sess, memName, b & 7);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("MMWORD PTR ")), reg, &addr);
	}
}

void DasmX86_64_ParseModRMXMM128(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_64_ParseRegXMM(sess, memName, b & 7);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("XMMWORD PTR ")), reg, &addr);
	}
}

void DasmX86_64_ParseModVXMM128(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		UInt8 destReg = (UInt8)((b & 7) | ((sess->thisStatus & 0x100) >> 5));
		*reg = ((b >> 3) & 7) | ((sess->thisStatus & 0x400) >> 7);
		DasmX86_64_ParseRegXMM(sess, memName, destReg);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("XMMWORD PTR ")), reg, &addr);
	}
}

void DasmX86_64_ParseModVYMM256(Manage::DasmX86_64::DasmX86_64_Sess* sess, UTF8Char *memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((b >> 6) == 3)
	{
		UInt8 destReg = (UInt8)((b & 7) | ((sess->thisStatus & 0x100) >> 5));
		*reg = ((b >> 3) & 7) | ((sess->thisStatus & 0x400) >> 7);
		DasmX86_64_ParseRegYMM(sess, memName, destReg);
		sess->regs.rip += 1;
	}
	else
	{
		UInt64 addr;
		DasmX86_64_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("YMMWORD PTR ")), reg, &addr);
	}
}

Int32 __stdcall DasmX86_64_GetFuncStack(Manage::DasmX86_64::DasmX86_64_Sess* sess, UInt64 funcAddr, UInt64 *outRsp)
{
	Manage::DasmX86_64::DasmX86_64_Sess tmpSess;
	Data::ArrayListUInt64 callAddrs;
	Data::ArrayListUInt64 jmpAddrs;
	UTF8Char sbuff[256];
	Text::StringBuilderUTF8 sb;
	OSInt instCnt = 0;
	if (outRsp)
	{
		*outRsp = 0;
	}

	tmpSess.callAddrs = &callAddrs;
	tmpSess.jmpAddrs = &jmpAddrs;
	MemCopyNO(&tmpSess.regs, &sess->regs, sizeof(tmpSess.regs));
	tmpSess.regs.rip = funcAddr;
	tmpSess.regs.rsp = sess->regs.rsp - 4;
//	tmpSess.outStr = &sb;
	tmpSess.endType = Manage::DasmX86_64::ET_NOT_END;// isEnd = false;
	tmpSess.rspOfst = 0;
	tmpSess.lastStatus = 0;
	tmpSess.thisStatus = 0;
	tmpSess.codes = sess->codes;
	tmpSess.codes0f = sess->codes0f;
	tmpSess.codes0f38 = sess->codes0f38;
	tmpSess.codes0f3a = sess->codes0f3a;
	tmpSess.addrResol = sess->addrResol;
	tmpSess.memReader = sess->memReader;

	while (true)
	{
		UInt8 b = tmpSess.memReader->ReadMemUInt8(tmpSess.regs.rip);
		if (b == 0xc2)
		{
			tmpSess.regs.rsp += 4 + (UOSInt)sess->memReader->ReadMemUInt16(tmpSess.regs.rip + 1);
			if (outRsp)
			{
				*outRsp = tmpSess.regs.rsp;
			}
			return (sess->memReader->ReadMemUInt16(tmpSess.regs.rip + 1));
		}
		if (b == 0xc3)
		{
			tmpSess.regs.rsp += 4;
			if (outRsp)
			{
				*outRsp = tmpSess.regs.rsp;
			}
			return 0;
		}
		tmpSess.sbuff = sbuff;
		Bool ret = ((Manage::DasmX86_64::DasmX86_64_Code*)tmpSess.codes)[tmpSess.memReader->ReadMemUInt8(tmpSess.regs.rip)](&tmpSess);
		if (!ret)
		{
#ifdef _CONSOLE
			IO::ConsoleWriter console;
#else
			IO::FileStream fs((const UTF8Char*)"ErrorMsg.txt", IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer console(&fs);
#endif
			UInt8 *buff;
			UOSInt buffSize;
			buffSize = tmpSess.regs.rip - funcAddr;
			if (buffSize < 256)
				buffSize = 256;
			buff = MemAlloc(UInt8, buffSize);
			console.WriteLine();
			console.WriteStrC(sb.ToString(), sb.GetLength());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Func Buff 0x"));
			sb.AppendHex64(funcAddr);
			sb.AppendC(UTF8STRC(" "));
			tmpSess.addrResol->ResolveNameSB(&sb, funcAddr);
			console.WriteLineC(sb.ToString(), sb.GetLength());
			buffSize = tmpSess.memReader->ReadMemory(funcAddr, buff, tmpSess.regs.rip - funcAddr);
			if (buffSize > 0)
			{
				sb.ClearStr();
				sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
				console.WriteLineC(sb.ToString(), sb.GetLength());
			}

			console.WriteLineC(UTF8STRC("Unknown opcode"));
			buffSize = tmpSess.memReader->ReadMemory(tmpSess.regs.rip, buff, 256);
			if (buffSize > 0)
			{
				sb.ClearStr();
				sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
				console.WriteLineC(sb.ToString(), sb.GetLength());
			}
			MemFree(buff);
			return -1;
		}
		instCnt++;
		sb.AppendSlow(sbuff);
		if (tmpSess.endType == Manage::DasmX86_64::ET_EXIT)
		{
			tmpSess.regs.rip = tmpSess.retAddr;
			return 0;
		}
		else if (tmpSess.endType != Manage::DasmX86_64::ET_NOT_END)
		{
			tmpSess.regs.rip = tmpSess.retAddr;
			tmpSess.endType = Manage::DasmX86_64::ET_NOT_END;
		}
		else if (instCnt >= 4096)
		{
			return -1;
		}
		tmpSess.lastStatus = tmpSess.thisStatus;
		tmpSess.thisStatus = (Int32)0x80000000;
	}
	return 0;
}

Bool __stdcall DasmX86_64_00(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 *regPtr;
	UInt8 memVal;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_64_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_01(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_64_ParseReg(sess, regs, reg, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		if ((b & 0xc7) == 0xc4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
			return false;
		}
	}
	return true;
}

Bool __stdcall DasmX86_64_02(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_64_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = (UInt8)(*regPtr + memVal);
	return true;
}

Bool __stdcall DasmX86_64_03(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_64_ParseReg(sess, regs, reg, &regPtr);
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr = (UInt16)((memVal & 0xffff) + *(UInt16*)regPtr);
	}
	else
	{
		*regPtr = memVal + *regPtr;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
/*	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		if (reg == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
			return false;
		}
	}*/
	return true;
}

Bool __stdcall DasmX86_64_04(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add AL,"));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_05(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add AX,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
		sess->regs.rip += 3;
		return true;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add EAX,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->regs.rip += 5;
		return true;
	}
}

Bool __stdcall DasmX86_64_06(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push ES\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_07(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop ES\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_08(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_09(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr |= memVal;
	return true;
}

Bool __stdcall DasmX86_64_0b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 0x800)
	{
		*regPtr |= memVal;
	}
	else if (sess->thisStatus)
	{
		*(UInt16*)regPtr = (UInt16)(*(UInt16*)regPtr | memVal);
	}
	else
	{
		*(UInt32*)regPtr |= (UInt32)memVal;
	}
	return true;
}

Bool __stdcall DasmX86_64_0c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	sess->regs.rax |= v;
	return true;
}

Bool __stdcall DasmX86_64_0d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.rip += 3;
		sess->regs.rax |= v;
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.rip += 5;
		sess->regs.rax |= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push CS\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_0f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes0f)[sess->memReader->ReadMemUInt8(sess->regs.rip + 1)](sess);
}

Bool __stdcall DasmX86_64_10(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	DasmX86_64_ParseReg8(sess, mem, reg, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_11(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 reg;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	DasmX86_64_ParseReg(sess, mem, reg, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((b & 0xc7) == 0xc4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
		return false;
	}
	else
	{
		return true;
	}
}

Bool __stdcall DasmX86_64_12(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_64_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = (UInt8)(*regPtr + memVal);
	return true;
}

Bool __stdcall DasmX86_64_13(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_64_ParseReg(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr = (UInt16)(*(UInt16*)regPtr + (memVal & 0xffff));
	}
	else if (sess->thisStatus & 0x800)
	{
		*regPtr = *regPtr + memVal;
	}
	else
	{
		*(UInt32*)regPtr = *(UInt32*)regPtr + (UInt32)memVal;
	}
	if ((b & 0xc7) == 0xc4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
		return false;
	}
	else
	{
		return true;
	}
}

Bool __stdcall DasmX86_64_14(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc al,"));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_15(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc ax,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 3;
		return true;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc eax,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
}

Bool __stdcall DasmX86_64_16(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push SS\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_17(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop SS\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_18(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_19(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_1a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = (UInt8)(*regPtr - memVal);
	return true;
}

Bool __stdcall DasmX86_64_1b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr = (UInt16)(*(UInt16*)regPtr - (memVal & 0xffff));
	}
	else if (sess->thisStatus & 0x800)
	{
		*regPtr -= memVal;
	}
	else
	{
		*(UInt32*)regPtr -= (UInt32)memVal;
	}
	return true;
}

Bool __stdcall DasmX86_64_1c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*(UInt8*)&sess->regs.rax = (UInt8)(*(UInt8*)&sess->regs.rax - v);
	return true;
}

Bool __stdcall DasmX86_64_1d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb AL, "));
		sess->sbuff = Text::StrUInt32(sess->sbuff, sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
		sess->regs.rip += 3;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb AL, "));
		sess->sbuff = Text::StrUInt32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->regs.rip += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_1e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push DS\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_1f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop DS\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_20(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_64_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_21(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_64_ParseReg(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((b & 0xc7) == 0xc4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
		return false;
	}
	else
	{
		return true;
	}
}

Bool __stdcall DasmX86_64_22(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_64_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr &= memVal;
	return true;
}

Bool __stdcall DasmX86_64_23(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_64_ParseReg(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr &= (UInt16)(memVal & 0xffff);
	}
	else if (sess->thisStatus & 0x800)
	{
		*regPtr &= memVal;
	}
	else
	{
		*(UInt32*)regPtr &= (UInt32)(memVal & 0xffffffff);
	}
	if ((b & 0xc7) == 0xc4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
		return false;
	}
	else
	{
		return true;
	}
}

Bool __stdcall DasmX86_64_24(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rax &= v;
	return true;
}

Bool __stdcall DasmX86_64_25(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.rip += 3;
		sess->regs.rax &= v;
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.rip += 5;
		sess->regs.rax &= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_26(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 2;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_27(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("daa\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_28(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;

	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_29(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;

	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_2a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;

	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = (UInt8)(*regPtr - memVal);
	return true;
}

Bool __stdcall DasmX86_64_2b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;

	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr = (UInt16)(*(UInt16*)regPtr - (memVal & 0xffff));
	}
	else if (sess->thisStatus & 0x800)
	{
		*regPtr -= memVal;
	}
	else
	{
		*(UInt32*)regPtr -= (UInt32)(memVal & 0xffffffff);
	}
	return true;
}

Bool __stdcall DasmX86_64_2c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*(UInt8*)&sess->regs.rax = (UInt8)(*(UInt8*)&sess->regs.rax - v);
	return true;
}

Bool __stdcall DasmX86_64_2d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.rip += 3;
		*(UInt16*)&sess->regs.rax = (UInt16)(*(UInt16*)&sess->regs.rax - v);
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.rip += 5;
		sess->regs.rax -= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_2e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 0;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_2f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("das\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_30(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_31(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_32(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr ^= memVal;
	return true;
}

Bool __stdcall DasmX86_64_33(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*regPtr ^= memVal & 0xffff;
	}
	else
	{
		*regPtr ^= memVal;
	}
	return true;
}

Bool __stdcall DasmX86_64_34(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	sess->regs.rax ^= v;
	return true;
}

Bool __stdcall DasmX86_64_35(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.rip += 3;
		sess->regs.rax ^= v;
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.rip += 5;
		sess->regs.rax ^= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_36(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 5;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_37(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aaa\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_38(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_39(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_3a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_3b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_3c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_3d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp AX, "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
		sess->regs.rip += 3;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->regs.rip += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));

	return true;
}

Bool __stdcall DasmX86_64_3e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 1;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_3f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aas\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_40(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x0;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_41(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x100;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_42(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x200;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_43(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x300;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_44(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x400;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_45(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x500;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_46(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x600;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_47(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x700;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_48(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x800;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_49(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x900;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_4a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0xA00;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_4b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0xB00;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_4c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0xC00;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_4d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0xD00;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_4e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0xE00;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_4f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0xF00;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_50(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 0, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_51(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 1, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_52(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 2, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_53(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 3, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_54(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 4, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_64_55(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 5, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_56(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 6, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_57(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 7, &regPtr);
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		sess->regs.rsp -= 8;
	}
	return true;
}

Bool __stdcall DasmX86_64_58(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 0, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_59(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 1, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_5a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 2, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_5b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 3, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_5c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 4, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		if (sess->thisStatus & 0x100)
		{
			*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
			sess->rspOfst += 8;
			sess->regs.rsp += 8;
		}
		else
		{
			sess->rspOfst += (Int32)(sess->memReader->ReadMemUInt32(sess->regs.rip) - sess->regs.rip);
			sess->regs.rsp = sess->memReader->ReadMemUInt32(sess->regs.rip);
		}
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_5d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 5, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_5e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 6, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_5f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[5];
	UInt64 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_64_ParseReg64(sess, reg, ((sess->thisStatus & 0x100) >> 5) | 7, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & (Int32)0x80000000) == 0)
	{
		*regPtr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->rspOfst += 8;
		sess->regs.rsp += 8;
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_60(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pusha\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pushad\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_61(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popa\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popad\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_62(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt64 memVal;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bound "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_63(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt16 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("arpl "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_64(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 3;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_65(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 4;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_66(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	sess->thisStatus |= 0x1;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_67(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_68(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 iVal = (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
	if (iVal > -128 && iVal < 128)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, (UInt32)iVal);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 5;
	sess->regs.rsp -= 4;
	return true;
}

Bool __stdcall DasmX86_64_69(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip));
		sess->regs.rip += 2;
	}
	else
	{
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip));
		sess->regs.rip += 4;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_6a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	sess->regs.rsp -= 4;
	return true;
}

Bool __stdcall DasmX86_64_6b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_6c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insb\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_6d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insd\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_6e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("outsb\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_6f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("outsw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("outsd\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_70(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jo 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_71(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jno 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_72(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jb 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_73(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnb 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_74(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jz 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_75(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnz 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_76(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jbe 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_77(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ja 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_78(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("js 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_79(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jns 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_7a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jp 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_7b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnp 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_7c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jl 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_7d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jge 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_7e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jle 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_7f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)(((Int64)sess->regs.rip) + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jg 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_80(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = ((sess->memReader->ReadMemUInt8(sess->regs.rip + 1) >> 3) & 7);
	UTF8Char mem[64];
	UInt8 memVal;
	Int32 reg;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &reg, &memVal);

	if (b == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if (b == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_81(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 reg;
	Int32 iVal;
	UInt64 memVal;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	if (sess->thisStatus & 1)
	{
		iVal = (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip);
		sess->regs.rip += 2;
	}
	else
	{
		iVal = (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip);
		sess->regs.rip += 4;
	}

	if (reg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp + iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp + iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (sess->regs.rsp | (UInt32)iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (sess->regs.rbp | (UInt32)iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp + iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp + iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp - iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp - iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (sess->regs.rsp & (UInt32)iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (sess->regs.rbp & (UInt32)iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp - iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp - iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			sess->regs.rsp = (sess->regs.rsp ^ (UInt32)iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.rbp = (sess->regs.rbp ^ (UInt32)iVal);
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (reg == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_82(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_83(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 reg;
	Int32 iVal;
	UInt64 memVal;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
	if (reg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & (Int32)0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp + iVal);
				sess->stabesp = sess->regs.rsp;
				return true;
/*				if (sess->lastStatus & 2)
				{
					sess->regs.rsp += iVal;
					return true;
				}
				else
				{
					sess->regs.rsp += iVal;
					return true;
//					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
//					return false;
				}*/
			}
			else if ((b & 0xc7) == 0xc5)
			{
				sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp + iVal);
				return true;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & (Int32)0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.rsp = (sess->regs.rsp | (UInt32)iVal);
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
					return false;
				}
			}
			else if ((b & 0xc7) == 0xc5)
			{
				sess->regs.rbp = (sess->regs.rbp | (UInt32)iVal);
				return true;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & (Int32)0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp + iVal);
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
					return false;
				}
			}
			else if ((b & 0xc7) == 0xc5)
			{
				sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp + iVal);
				return true;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & (Int32)0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp - iVal);
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
					return false;
				}
			}
			else if ((b & 0xc7) == 0xc5)
			{
				sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp - iVal);
				return true;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & (Int32)0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.rsp = (sess->regs.rsp & (UInt32)iVal);
					return true;
				}
				else
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
					return false;
				}
			}
			else if ((b & 0xc7) == 0xc4)
			{
				sess->regs.rbp = (sess->regs.rbp & (UInt32)iVal);
				return true;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & (Int32)0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp - iVal);
					return true;
				}
				else
				{
					sess->regs.rsp = (UInt64)((Int64)sess->regs.rsp - iVal);
					return true;
				}
			}
			else if ((b & 0xc7) == 0xc4)
			{
				sess->regs.rbp = (UInt64)((Int64)sess->regs.rbp - iVal);
				return true;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((b & 0xc7) == 0xc4)
		{
			if (sess->lastStatus & 2)
			{
				return true;
			}
			else
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else if (reg == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_84(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_85(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;

	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_86(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_64_ParseReg8(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_87(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_88(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_64_ParseReg8(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_89(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip++;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip);
	DasmX86_64_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));

	if ((b & 0xc7) == 0xc4)
	{
		if (destReg == 5)
		{
			sess->regs.rsp = sess->regs.rbp;
			return true;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
			return false;
		}
	}
	else if ((b & 0xc7) == 0xc5)
	{
		if (destReg == 4)
		{
			sess->regs.rbp = sess->regs.rsp;
			return true;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to ebp\r\n"));
			return false;
		}
	}
	else
	{
		return true;
	}
}

Bool __stdcall DasmX86_64_8a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_64_ParseReg8(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_8b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip++;

	DasmX86_64_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	if (sess->thisStatus & 0x800)
	{
		DasmX86_64_ParseReg64(sess, reg, destReg, &regPtr);
		*regPtr = memVal;
	}
	else
	{
		DasmX86_64_ParseReg32(sess, reg, destReg, &regPtr);
		*(UInt32*)regPtr = (UInt32)(memVal & 0xffffffff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_8c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_64_ParseSReg(sess, reg, destReg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_8d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt64 memVal;
	UInt64 *regPtr;
	UInt64 memAddr;
	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &destReg, &memVal, &memAddr);
	DasmX86_64_ParseReg(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lea "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memAddr;
	return true;
}

Bool __stdcall DasmX86_64_8e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_64_ParseSReg(sess, reg, destReg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_8f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_90(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pause\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("nop\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_91(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 1, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 1, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_92(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 2, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 2, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_93(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 3, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 3, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_94(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 4, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 4, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_95(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 5, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 5, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_96(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 6, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 6, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_97(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt64 *regPtr;
		UInt16 v;
		DasmX86_64_ParseReg16(sess, reg, 7, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.rax & 0xffff;
		*(UInt16*)&sess->regs.rax = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt64 *regPtr;
		UInt32 v;
		DasmX86_64_ParseReg32(sess, reg, 7, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt32)(sess->regs.rax & 0xffffffff);
		*(UInt32*)&sess->regs.rax = *(UInt32*)regPtr;
		*(UInt32*)regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_98(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cbw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cwd\r\n"));
	}
	return true;
}

Bool __stdcall DasmX86_64_99(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip++;
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cwd\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cdq\r\n"));
	}
	return true;
}

Bool __stdcall DasmX86_64_9a(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_9b(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fwait\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_9c(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pushf\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pushfd\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_9d(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popf\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popfd\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_9e(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sahf\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_9f(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lahf\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_a0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
	*(UInt8*)&sess->regs.rax = sess->memReader->ReadMemUInt8(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov AL,BYTE PTR ["));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
	sess->regs.rip += 5;
	return true;
}

Bool __stdcall DasmX86_64_a1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov AX,WORD PTR "));
		*(UInt16*)&sess->regs.rax = sess->memReader->ReadMemUInt16(addr);
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov EAX,DWORD PTR "));
		sess->regs.rax = sess->memReader->ReadMemUInt32(addr);
	}
	if (sess->thisStatus & 0x10000)
	{
		sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_64_Segms[sess->segmId]);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("[0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
	sess->regs.rip += 5;
	return true;
}

Bool __stdcall DasmX86_64_a2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov BYTE PTR "));
	if (sess->thisStatus & 0x10000)
	{
		sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_64_Segms[sess->segmId]);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("["));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],AL\r\n"));
	sess->regs.rip += 5;
	return true;
}

Bool __stdcall DasmX86_64_a3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov WORD PTR "));
		if (sess->thisStatus & 0x10000)
		{
			sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_64_Segms[sess->segmId]);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("["));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],AX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov DWORD PTR "));
		if (sess->thisStatus & 0x10000)
		{
			sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_64_Segms[sess->segmId]);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("["));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],EAX\r\n"));
	}
	sess->regs.rip += 5;
	return true;
}

Bool __stdcall DasmX86_64_a4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsb\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_a5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsd\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_a6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("repz "));
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("repnz "));
	}

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpsb\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_a7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("repz "));
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("repnz "));
	}

	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpsw\r\n"));
		sess->regs.rip += 1;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpsd\r\n"));
		sess->regs.rip += 1;
	}
	return true;
}

Bool __stdcall DasmX86_64_a8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_a9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
		sess->regs.rip += 3;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->regs.rip += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_aa(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.rip++;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stosb\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_ab(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.rip++;
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stosw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stosd\r\n"));
	}
	return true;
}

Bool __stdcall DasmX86_64_ac(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lodsb\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_ad(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.rip += 1;
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lodsw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lodsd\r\n"));
	}
	return true;
}

Bool __stdcall DasmX86_64_ae(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("scasb\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_af(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("scasw\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("scasd\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_b0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 0, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 1, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 2, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 3, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 4, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 5, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 6, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UInt8 *regPtr;
	DasmX86_64_ParseReg8(sess, reg, 7, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_b8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 0, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*(UInt32*)regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_b9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 1, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_ba(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 2, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt16(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_bb(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 3, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt16(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_bc(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 4, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_bd(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 5, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_be(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 6, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_bf(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg(sess, reg, 7, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.rip += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.rip += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_c0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_c1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_c2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 cnt = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
	sess->endType = Manage::DasmX86_64::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ret "));
	sess->sbuff = Text::StrUInt32(sess->sbuff, cnt);
	if (sess->thisStatus & (Int32)0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		UInt8 *buff;
		UOSInt buffSize;
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.rsp);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\t;"));
		buff = MemAlloc(UInt8, cnt);
		buffSize = sess->memReader->ReadMemory(sess->regs.rsp + 4, buff, cnt);
		if (buffSize > 0)
		{
			sess->sbuff = Text::StrHexBytes(sess->sbuff, buff, buffSize, ' ');
		}
		sess->regs.rsp += 4 + cnt;
		MemFree(buff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 3;
	return true;
}

Bool __stdcall DasmX86_64_c3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->endType = Manage::DasmX86_64::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ret\r\n"));
	if (sess->thisStatus & (Int32)0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		sess->retAddr = sess->memReader->ReadMemUInt64(sess->regs.rsp);
		sess->regs.rsp += 8;
	}
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_c4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->memReader->ReadMemory(sess->regs.rip, sess->vex, 3);
	sess->thisStatus |= 0x20000;
	sess->regs.rip += 2;
	switch (sess->vex[2] & 3)
	{
	case 1:
		sess->thisStatus |= 0x1;
		break;
	case 2:
		sess->thisStatus |= 0x5;
		break;
	case 3:
		sess->thisStatus |= 0x4;
		break;
	}
	if (sess->vex[2] & 4)
	{
		sess->thisStatus |= 0x400000;
	}
	sess->thisStatus |= ((sess->vex[1] & 0xe0) << 3) | ((sess->vex[2] & 0x80) << 4);
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes0f)[sess->memReader->ReadMemUInt8(sess->regs.rip + 1)](sess);
}

Bool __stdcall DasmX86_64_c5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->memReader->ReadMemory(sess->regs.rip, sess->vex, 2);
	sess->thisStatus |= 0x20000;
	sess->regs.rip += 1;
	switch (sess->vex[1] & 3)
	{
	case 1:
		sess->thisStatus |= 0x1;
		break;
	case 2:
		sess->thisStatus |= 0x5;
		break;
	case 3:
		sess->thisStatus |= 0x4;
		break;
	}
	if (sess->vex[1] & 4)
	{
		sess->thisStatus |= 0x400000;
	}
	sess->thisStatus |= ((sess->vex[1] & 0x80) << 3);
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes0f)[sess->memReader->ReadMemUInt8(sess->regs.rip + 1)](sess);
}

Bool __stdcall DasmX86_64_c6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt8 memVal;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	if (destReg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_c7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt64 memVal;
	sess->regs.rip++;
	if (((sess->memReader->ReadMemUInt8(sess->regs.rip) >> 3) & 7) == 0)
	{
		DasmX86_64_ParseModRM32(sess, mem, &destReg, &memVal, 0);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		if (sess->thisStatus & 1)
		{
			sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip));
			sess->regs.rip += 2;
		}
		else
		{
			sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip));
			sess->regs.rip += 4;
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
		return false;
	}
}

Bool __stdcall DasmX86_64_c8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("enter "));
	sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
	sess->regs.rip += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_c9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("leave\r\n"));
	sess->regs.rip += 1;
	sess->regs.rsp = sess->regs.rbp;
	sess->regs.rbp = sess->memReader->ReadMemUInt32(sess->regs.rsp);
	sess->regs.rsp += 4;
	return true;
}

Bool __stdcall DasmX86_64_ca(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 cnt = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
	sess->endType = Manage::DasmX86_64::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("retf "));
	sess->sbuff = Text::StrUInt32(sess->sbuff, cnt);
	if (sess->thisStatus & (Int32)0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		UInt8 *buff;
		UOSInt buffSize;
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.rsp);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\t;"));
		buff = MemAlloc(UInt8, cnt);
		buffSize = sess->memReader->ReadMemory(sess->regs.rsp + 8, buff, cnt);
		if (buffSize > 0)
		{
			sess->sbuff = Text::StrHexBytes(sess->sbuff, buff, buffSize, ' ');
		}
		sess->regs.rsp += 8 + cnt;
		MemFree(buff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 3;
	return true;
}

Bool __stdcall DasmX86_64_cb(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->endType = Manage::DasmX86_64::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("retf\r\n"));
	if (sess->thisStatus & (Int32)0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.rsp);
		sess->regs.rsp += 8;
	}
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_cc(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("int 3\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_cd(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("int 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_ce(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("into\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_cf(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("iret\r\n"));
	}
	else if (sess->thisStatus & 0x800)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("iretq\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("iretd\r\n"));
	}
	if (sess->thisStatus & (Int32)0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.rsp);
		sess->regs.rsp += 24;
	}
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_d0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_d1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", 1\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_d2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_d3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_d4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->memReader->ReadMemUInt8(sess->regs.rip + 1) == 0xa)
	{
		sess->regs.rip += 2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aam\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_d5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->memReader->ReadMemUInt8(sess->regs.rip + 1) == 0xa)
	{
		sess->regs.rip += 2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aad\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_d6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_d7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xlatb\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_d8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.rip += 2;
		b2 = (b >> 3) & 7;
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fadd "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmul "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcom "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomp "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsub "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubr "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdiv "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivr "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		UInt32 memVal;
		sess->regs.rip += 1;
		DasmX86_64_ParseModRM32(sess, mem, &b2, &memVal, 0);
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fadd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmul "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcom "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomp "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsub "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdiv "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else
		{
			return false;
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_d9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.rip += 2;
		if (b & 0x20)
		{
			switch (b & 0x1f)
			{
			case 0:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fchs\r\n"));
				return true;
			case 1:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fabs\r\n"));
				return true;
			case 2:
				break;
			case 3:
				break;
			case 4:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ftst\r\n"));
				return true;
			case 5:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fxam\r\n"));
				return true;
			case 6:
				break;
			case 7:
				break;
			case 8:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fld1\r\n"));
				return true;
			case 9:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fldl2t\r\n"));
				return true;
			case 0xa:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fldl2e\r\n"));
				return true;
			case 0xb:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fldpi\r\n"));
				return true;
			case 0xc:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fldlg2\r\n"));
				return true;
			case 0xd:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fldln2\r\n"));
				return true;
			case 0xe:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fldz\r\n"));
				return true;
			case 0xf:
				break;
			case 0x10:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("f2xm1\r\n"));
				return true;
			case 0x11:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fyl2x\r\n"));
				return true;
			case 0x12:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fptan\r\n"));
				return true;
			case 0x13:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fpatan\r\n"));
				return true;
			case 0x14:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fxtract\r\n"));
				return true;
			case 0x15:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fprem1\r\n"));
				return true;
			case 0x16:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdecstp\r\n"));
				return true;
			case 0x17:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fincstp\r\n"));
				return true;
			case 0x18:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fprem\r\n"));
				return true;
			case 0x19:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fyl2xp1\r\n"));
				return true;
			case 0x1a:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsqrt\r\n"));
				return true;
			case 0x1b:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsincos\r\n"));
				return true;
			case 0x1c:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("frndint\r\n"));
				return true;
			case 0x1d:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fscale\r\n"));
				return true;
			case 0x1e:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsin\r\n"));
				return true;
			case 0x1f:
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcos\r\n"));
				return true;
			}
		}
		else
		{
			b2 = (b >> 3) & 7;
			if (b == 0xd0)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fnop\r\n"));
				return true;
			}
			else if (b2 == 0)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fld "));
				DasmX86_64_ParseRegx87(mem, 0);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				DasmX86_64_ParseRegx87(mem, b & 7);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 1)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fxch "));
				DasmX86_64_ParseRegx87(mem, 0);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				DasmX86_64_ParseRegx87(mem, b & 7);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
		return false;
	}
	else
	{
		if (b & 0x20)
		{
		}
		else
		{
			UInt32 memVal;
			sess->regs.rip += 1;
			DasmX86_64_ParseModRM32(sess, mem, &b2, &memVal, 0);
			if (b2 == 0)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fld "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 2)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fst "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 3)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fstp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
		return false;
	}*/
	return false;
}

Bool __stdcall DasmX86_64_da(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.rip += 2;
		b2 = (b >> 3) & 7;
		if (b == 0xe9)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucompp\r\n"));
			return true;
		}
		else if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovb "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmove "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovbe "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovu "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		UInt32 memVal;
		sess->regs.rip += 1;
		DasmX86_64_ParseModRM32(sess, mem, &b2, &memVal, 0);
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fiadd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fimul "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ficom "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ficomp "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisub "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisubr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fidiv "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fidivr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else
		{
			return false;
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_db(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.rip += 2;
		b2 = (b >> 3) & 7;
		if (b == 0xe2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fnclex\r\n"));
			sess->regs.rip += 2;
			return true;
		}
		else if (b == 0xe3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fninit\r\n"));
			sess->regs.rip += 2;
			return true;
		}
		else if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovnb "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovne "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovnbe "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovnu "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucomi "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomi "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		sess->regs.rip += 1;
		if (b & 0x20)
		{
//			DasmX86_64_ParseModRM80(sess, mem, &b2);
//			if (b2 == 5)
//			{
//				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fld "));
//				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
//				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
//				return true;
//			}
//			else if (b2 == 7)
//			{
//				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fstp "));
//				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
//				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
//				return true;
//			}
		}
		else
		{
			UInt32 memVal;
			DasmX86_64_ParseModRM32(sess, mem, &b2, &memVal, 0);
			if (b2 == 0)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fild "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 1)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisttp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 2)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fist "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 3)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fistp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_dc(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		b2 = (b >> 3) & 7;
		sess->regs.rip += 2;
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fadd "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmul "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubr "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsub "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivr "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdiv "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		sess->regs.rip += 1;
		DasmX86_64_ParseModRM64(sess, mem, &b2);
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fadd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmul "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcom "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomp "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsub "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdiv "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else
		{
			return false;
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_dd(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.rip += 2;
		b2 = (b >> 3) & 7;
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ffree "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fst "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fstp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucom "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucomp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		if (b & 0x20)
		{
		}
		else
		{
			sess->regs.rip += 1;
			DasmX86_64_ParseModRM64(sess, mem, &b2);
			if (b2 == 0)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fld "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				sess->thisStatus |= 2;
				return true;
			}
			else if (b2 == 1)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisttp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				sess->thisStatus |= 2;
				return true;
			}
			else if (b2 == 2)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fst "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				sess->thisStatus |= 2;
				return true;
			}
			else if (b2 == 3)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fstp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				sess->thisStatus |= 2;
				return true;
			}
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_de(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		b2 = (b >> 3) & 7;
		sess->regs.rip += 2;
		if (b == 0xd9)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcompp\r\n"));
			return true;
		}
		else if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("faddp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmulp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubrp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivrp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivp "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		UInt16 memVal;
		sess->regs.rip += 1;
		DasmX86_64_ParseModRM16(sess, mem, &b2, &memVal);
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fiadd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fimul "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ficom "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ficomp "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisub "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisubr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fidiv "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fidivr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->thisStatus |= 2;
			return true;
		}
		else
		{
			return false;
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_df(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		b2 = (b >> 3) & 7;
		if (b == 0xe0)
		{
			sess->regs.rip += 2;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fnstsw AX\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->regs.rip += 2;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucomip "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->regs.rip += 2;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomip "));
			DasmX86_64_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_64_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		if (b & 0x20)
		{
			b2 = (b >> 3) & 7;
			if (b2 & 1)
			{
				sess->regs.rip += 1;
				DasmX86_64_ParseModRM64(sess, mem, &b2);
				if (b2 == 5)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fild "));
					sess->sbuff = Text::StrConcat(sess->sbuff, mem);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
				else if (b2 == 7)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fistp "));
					sess->sbuff = Text::StrConcat(sess->sbuff, mem);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
			else
			{
				sess->regs.rip += 1;
				DasmX86_64_ParseModRMR80(sess, mem, &b2);
				if (b2 == 4)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fbld "));
					sess->sbuff = Text::StrConcat(sess->sbuff, mem);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
				else if (b2 == 6)
				{
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fbstp "));
					sess->sbuff = Text::StrConcat(sess->sbuff, mem);
					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
					return true;
				}
			}
		}
		else
		{
			UInt16 memVal;
			sess->regs.rip += 1;
			DasmX86_64_ParseModRM16(sess, mem, &b2, &memVal);
			if (b2 == 0)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fild "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 1)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fisttp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 2)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fist "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 3)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fistp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
	}*/
	return false;
}

Bool __stdcall DasmX86_64_e0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.rip) + 2 + sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("loopnz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_e1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.rip) + 2 + sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("loopz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_e2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.rip) + 2 + sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->regs.rip += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("loop 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_e3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.rip) + 2 + sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	sess->regs.rip += 2;
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jcxz 0x"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jecxz 0x"));
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_e4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AL, 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_e5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AX, 0x"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in EAX, 0x"));
	}
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_e6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", AL\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_e7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", AX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", EAX\r\n"));
	}
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_e8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char *sptr;
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)sess->regs.rip + ((Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 1)) + 3);
		sess->regs.rip += 3;
	}
	else
	{
		addr = (UInt64)((Int64)sess->regs.rip + (Int32)(sess->memReader->ReadMemUInt32(sess->regs.rip + 1)) + 5);
		sess->regs.rip += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("call 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);

	if (sess->thisStatus & (Int32)0x80000000)
	{
	}
	else
	{
		UInt64 outRsp;
		Int32 stackCnt = DasmX86_64_GetFuncStack(sess, addr, &outRsp);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ParamStack = "));
		sess->sbuff = Text::StrInt32(sess->sbuff, stackCnt);
		if (outRsp != 0)
		{
			if ((UInt64)((Int64)sess->regs.rsp + stackCnt + 64) >= outRsp && (UInt64)((Int64)sess->regs.rsp + stackCnt - 64) <= outRsp)
			{
				sess->regs.rsp += (UInt64)(Int64)stackCnt;
			}
		}
		else if (stackCnt > 0)
		{
			sess->regs.rsp += (UInt32)stackCnt;
		}
	}
	if (sess->addrResol)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, addr);
		if (sess->sbuff == 0)
		{
			sess->sbuff = sptr;
		}
		if (DasmX86_64_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
		{
			sess->endType = Manage::DasmX86_64::ET_EXIT;
			sess->retAddr = sess->regs.rip;
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->callAddrs->Add(addr);
	sess->stabesp = sess->regs.rsp;
	sess->thisStatus |= 2;
	return true;
}

Bool __stdcall DasmX86_64_e9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	UInt64 addr2;
	UOSInt i;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)sess->regs.rip + 3 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 1));
		sess->regs.rip += 3;
	}
	else
	{
		addr = (UInt64)((Int64)sess->regs.rip + 5 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 1));
		sess->regs.rip += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(addr);
	addr2 = sess->regs.rip;

	sess->endType = Manage::DasmX86_64::ET_JMP;
	sess->retAddr = addr;
	i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		addr = sess->jmpAddrs->GetItem(i);
		if (addr >= addr2 && addr <= addr2 + 16)
		{
			sess->endType = Manage::DasmX86_64::ET_NOT_END;
			break;
		}
	}
	return true;
}

Bool __stdcall DasmX86_64_ea(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	UInt32 addr2;
	UOSInt i;
	if (sess->thisStatus & 1)
	{
		addr = sess->memReader->ReadMemUInt16(sess->regs.rip + 1);
		addr2 = sess->memReader->ReadMemUInt16(sess->regs.rip + 3);
		sess->regs.rip += 5;
	}
	else
	{
		addr = sess->memReader->ReadMemUInt32(sess->regs.rip + 1);
		addr2 = sess->memReader->ReadMemUInt32(sess->regs.rip + 5);
		sess->regs.rip += 7;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp "));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr2);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(":"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, (UInt32)addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(addr);
	addr2 = (UInt32)sess->regs.rip;

	sess->endType = Manage::DasmX86_64::ET_JMP;
	sess->retAddr = addr;
	i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		addr = sess->jmpAddrs->GetItem(i);
		if (addr >= addr2 && addr <= addr2 + 16)
		{
			sess->endType = Manage::DasmX86_64::ET_NOT_END;
			break;
		}
	}
	return true;
}

Bool __stdcall DasmX86_64_eb(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr = (UInt64)((Int64)sess->regs.rip + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
	UInt64 addr2;
	UOSInt i;
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(addr);
	addr2 = sess->regs.rip;

	sess->endType = Manage::DasmX86_64::ET_JMP;
	sess->retAddr = addr;
	i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		addr = sess->jmpAddrs->GetItem(i);
		if (addr == addr2)
		{
			sess->endType = Manage::DasmX86_64::ET_NOT_END;
			break;
		}
	}
	return true;
}

Bool __stdcall DasmX86_64_ec(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AL, DX\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_ed(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AX, DX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in EAX, DX\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_ee(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out DX, AL\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_ef(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out DX, AX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out DX, EAX\r\n"));
	}
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_f0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lock "));
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.rip + 1);
	if (b == 0xaa)
	{
		sess->regs.rip += 2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lock stosb\r\n"));
		return true;
	}
	else
	{
		return false;
	}*/
}

Bool __stdcall DasmX86_64_f1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_f2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 1;
	sess->thisStatus |= 4;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_f3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 1;
	sess->thisStatus |= 5;
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.rip)](sess);
}

Bool __stdcall DasmX86_64_f4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("hlt\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_f5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmc\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_f6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt8 memVal;
	sess->regs.rip++;
	DasmX86_64_ParseModRM8(sess, mem, &destReg, &memVal);
	if (destReg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("not "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("neg "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mul "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("div "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("idiv "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_f7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt64 memVal;
	sess->regs.rip++;
	DasmX86_64_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	if (destReg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		if (sess->thisStatus & 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("0x"));
			sess->sbuff = Text::StrHexVal16(sess->sbuff, sess->memReader->ReadMemUInt16(sess->regs.rip));
			sess->regs.rip += 2;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("0x"));
			sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.rip));
			sess->regs.rip += 4;
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("not "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("neg "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("div "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (destReg == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("idiv "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
		return false;
	}
}

Bool __stdcall DasmX86_64_f8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clc\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_f9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stc\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_fa(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cli\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_fb(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sti\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_fc(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cld\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_fd(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("std\r\n"));
	sess->regs.rip++;
	return true;
}

Bool __stdcall DasmX86_64_fe(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 1;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_ff(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	UInt8 v;
	sess->regs.rip += 1;
	v = sess->memReader->ReadMemUInt8(sess->regs.rip);
	if ((v & 0x38) == 0x10)
	{
		sess->thisStatus |= 0x800;
	}
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("call "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		if (sess->thisStatus & (Int32)0x80000000)
		{
		}
		else
		{
			UInt64 outRsp;
			Int32 stackCnt = DasmX86_64_GetFuncStack(sess, memVal, &outRsp);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" (0x"));
			sess->sbuff = Text::StrHexVal64(sess->sbuff, memVal);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));

			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ParamStack = "));
			sess->sbuff = Text::StrInt32(sess->sbuff, stackCnt);
			if (outRsp != 0)
			{
				if ((UInt64)((Int64)sess->regs.rsp + stackCnt + 16) >= outRsp && (UInt64)((Int64)sess->regs.rsp + stackCnt - 16) <= outRsp)
				{
					sess->regs.rsp += (UInt32)stackCnt;
				}
			}
			else if (stackCnt > 0)
			{
				sess->regs.rsp += (UInt32)stackCnt;
			}
		}
		if (sess->addrResol)
		{
			UTF8Char *sptr;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
			sess->sbuff = sess->addrResol->ResolveName(sptr = sess->sbuff, memVal);
			if (sess->sbuff == 0)
			{
				sess->sbuff = sptr;
			}
			if (DasmX86_64_IsEndFunc(sptr, (UOSInt)(sess->sbuff - sptr)))
			{
				sess->endType = Manage::DasmX86_64::ET_EXIT;
				sess->retAddr = sess->regs.rip;
			}
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f00(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModR32M16(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sldt "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("str "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lldt "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ltr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("verr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("verw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f01(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xc1)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmcall\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xc2)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmlaunch\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xc3)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmresume\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xc4)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmxoff\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xc8)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("monitor\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xc8)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mwait\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xca)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clac\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xd0)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xgetbv\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xd1)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xsetbv\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) == 0xf9)
	{
		sess->regs.rip += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdtscp\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f02(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 2;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		UInt64 *regPtr;
		DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt32 memVal;
		UInt64 *regPtr;
		DasmX86_64_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lar "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f03(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 2;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		UInt64 *regPtr;
		DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt32 memVal;
		UInt64 *regPtr;
		DasmX86_64_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f04(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f05(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("syscall\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f06(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clts\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f07(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sysret\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f08(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("invd\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f09(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("wbinvd\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f0A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f0B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ud2\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f0C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f0D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModR32M16(sess, mem, &regNo, &memVal);
	if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("prefetchw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("prefetchwt1 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f0E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("femms\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f0F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
/*	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
	DasmX86_64_ParseRegMM(sess, reg, regNo);
	UInt8 cmdId = sess->memReader->ReadMemUInt8(sess->regs.rip);
	sess->regs.rip++;
	switch (cmdId)
	{
	case 0x0C:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pi2fw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x0D:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pi2fd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x1C:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pf2iw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x1D:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pf2id "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x8A:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfnacc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x8E:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfpnacc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x90:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfcmpge "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x94:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfmin "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x96:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfrcp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x97:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfrsqrt "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x9A:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfsub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0x9E:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfadd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xA0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfcmpgt "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xA4:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfmax "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xA6:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfrcpit1 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xA7:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfrsqit1 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xAA:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfsubr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xAE:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfacc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xB0:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfcmpeq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xB4:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfmul "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xB6:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pfrcpit2 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xB7:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xBB:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pswapd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	case 0xBF:
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}*/
	return false;
}

Bool __stdcall DasmX86_64_0f10(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movups "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movupd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f11(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movups "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movupd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f12(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		if (mem[4] == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhlps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", X"));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movddup "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsldup "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f13(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f14(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpcklps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpcklpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f15(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpckhps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpckhpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f16(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		if (mem[4] == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlhps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", X"));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movshdup "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f17(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f18(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f19(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f1A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f1B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f1C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f1D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f1E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f1F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("nop "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;

	}
	return false;
}

Bool __stdcall DasmX86_64_0f20(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseRegCR(sess, reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f21(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseRegDR(sess, reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f22(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseRegCR(sess, reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f23(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseRegDR(sess, reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f24(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f25(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f26(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f27(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f28(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movaps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movapd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f29(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movaps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movapd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f2A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpi2ps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpi2pd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt64 memVal;
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsi2sd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UInt64 memVal;
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsi2ss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f2B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		UInt64 memVal;
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f2C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttps2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttpd2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt64 *regPtr;
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttsd2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UInt64 *regPtr;
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttss2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f2D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtps2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpd2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt64 *regPtr;
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsd2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UInt64 *regPtr;
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtss2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f2E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ucomiss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ucomisd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f2F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("comiss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("comisd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f30(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("wrmsr\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f31(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdtsc\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f32(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdmsr\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f33(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdpmc\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f34(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sysenter\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f35(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sysexit\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f36(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f37(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("getsec\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f38(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes0f38)[sess->memReader->ReadMemUInt8(sess->regs.rip + 2)](sess);
}

Bool __stdcall DasmX86_64_0f39(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return ((Manage::DasmX86_64::DasmX86_64_Code*)sess->codes0f3a)[sess->memReader->ReadMemUInt8(sess->regs.rip + 2)](sess);
}

Bool __stdcall DasmX86_64_0f3B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f40(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovo "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f41(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovno "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f42(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f43(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovnb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f44(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovz "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f45(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovnz "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f46(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovbe "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f47(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmova "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f48(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovs "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f49(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovns "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f4A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f4B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovnp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f4C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovl "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f4D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovge "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f4E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovle "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f4F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f50(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt64 *regPtr;
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movmskps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movmskpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f51(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sqrtps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sqrtsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sqrtss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f52(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsqrtps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsqrtss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f53(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcpps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcpss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f54(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f55(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andnps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andnpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f56(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("orps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("orpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f57(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xorps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xorpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f58(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f59(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f5A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtps2pd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpd2ps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsd2ss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f5B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtdq2ps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtps2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttps2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f5C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f5D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f5E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f5F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f60(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
/*		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;*/
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f61(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f62(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f63(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packsswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packsswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f64(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f65(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f66(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f67(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packuswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packuswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f68(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f69(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f6A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f6B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packssdw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packssdw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f6C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklqdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f6D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhqdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f6E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f6F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if (sess->thisStatus & 0x20000)
	{
		if (sess->thisStatus & 0x40000)
		{
			if ((sess->thisStatus & 5) == 1)
			{
				sess->regs.rip += 2;
				DasmX86_64_ParseModVYMM256(sess, mem, &regNo);
				DasmX86_64_ParseRegYMM(sess, reg, regNo);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovdqa "));
				sess->sbuff = Text::StrConcat(sess->sbuff, reg);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if ((sess->thisStatus & 5) == 5)
			{
				sess->regs.rip += 2;
				DasmX86_64_ParseModVYMM256(sess, mem, &regNo);
				DasmX86_64_ParseRegYMM(sess, reg, regNo);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovdqu "));
				sess->sbuff = Text::StrConcat(sess->sbuff, reg);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
		else
		{
			if ((sess->thisStatus & 5) == 0)
			{
				sess->regs.rip += 2;
				DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
				DasmX86_64_ParseRegMM(sess, reg, regNo);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovq "));
				sess->sbuff = Text::StrConcat(sess->sbuff, reg);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if ((sess->thisStatus & 5) == 1)
			{
				sess->regs.rip += 2;
				DasmX86_64_ParseModVXMM128(sess, mem, &regNo);
				DasmX86_64_ParseRegXMM(sess, reg, regNo);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovdqa "));
				sess->sbuff = Text::StrConcat(sess->sbuff, reg);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if ((sess->thisStatus & 5) == 5)
			{
				sess->regs.rip += 2;
				DasmX86_64_ParseModVXMM128(sess, mem, &regNo);
				DasmX86_64_ParseRegXMM(sess, reg, regNo);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovdqu "));
				sess->sbuff = Text::StrConcat(sess->sbuff, reg);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
		}
	}
	else if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqa "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f70(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshuflw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufhw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f71(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	Int32 b2 = (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) >> 3) & 7;
	if (b2 == 2)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 4)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psraw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 6)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f72(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	Int32 b2 = (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) >> 3) & 7;
	if (b2 == 2)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 4)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrad "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 6)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f73(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	Int32 b2 = (sess->memReader->ReadMemUInt8(sess->regs.rip + 2) >> 3) & 7;
	if (b2 == 2)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 3)
	{
		DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 6)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		else
		{
			DasmX86_64_ParseRegMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}
	else if (b2 == 7)
	{
		DasmX86_64_ParseRegXMM(sess, reg, sess->memReader->ReadMemUInt8(sess->regs.rip + 2) & 7);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 4;
		return true;
	}

	return false;
}

Bool __stdcall DasmX86_64_0f74(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f75(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f76(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f77(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->regs.rip += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("emms\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f78(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmread "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
    else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		if (regNo == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("extrq "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->regs.rip += 2;
			return true;
		}
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insertq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip += 2;
		return true;
	}

	return false;
}

Bool __stdcall DasmX86_64_0f79(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmwrite "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("extrq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insertq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f7A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f7B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f7C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("haddpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("haddps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f7D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("hsubpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("hsubps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f7E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		sess->thisStatus &= ~5;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f7F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqa "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f80(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jo 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f81(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jno 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f82(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jb 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f83(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnb 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f84(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jz 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f85(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnz 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f86(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jbe 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f87(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ja 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f88(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("js 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f89(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jns 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f8A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jp 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f8B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnp 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f8C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jl 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f8D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jge 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f8E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jle 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f8F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UInt64 addr;
	if (sess->thisStatus & 1)
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 4 + (Int16)sess->memReader->ReadMemUInt16(sess->regs.rip + 2));
		sess->regs.rip += 4;
	}
	else
	{
		addr = (UInt64)((Int64)(sess->regs.rip) + 6 + (Int32)sess->memReader->ReadMemUInt32(sess->regs.rip + 2));
		sess->regs.rip += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jg 0x"));
	sess->sbuff = Text::StrHexVal64(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0f90(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("seto "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f91(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setno "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f92(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f93(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setnb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f94(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setz "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f95(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setnz "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f96(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setbe "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f97(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("seta "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f98(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sets "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f99(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setns "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f9A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f9B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setnp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f9C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f9D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setge "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f9E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setle "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0f9F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("setg "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0fA0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push FS\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_0fA1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop FS\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_0fA2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cpuid\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_0fA3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bt "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fA4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shld "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_0fA5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shld "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fA6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fA7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fA8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push GS\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_0fA9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop GS\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_0fAA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsm\r\n"));
	sess->regs.rip += 2;
	return true;
}

Bool __stdcall DasmX86_64_0fAB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bts "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fAC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shrd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.rip += 1;
	return true;
}

Bool __stdcall DasmX86_64_0fAD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shrd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fAE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.rip += 2;
	if (sess->memReader->ReadMemUInt8(sess->regs.rip) == 0xe8)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lfence\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip) == 0xf0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mfence\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.rip) == 0xf8)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sfence\r\n"));
		sess->regs.rip += 1;
		return true;
	}
	else
	{
		UInt64 memVal;
		regNo = (sess->memReader->ReadMemUInt8(sess->regs.rip) >> 3) & 7;
		if (regNo == 2)
		{
			DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldmxcsr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (regNo == 2)
		{
			DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stmxcsr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (regNo == 7)
		{
			UInt8 memVal8;
			DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal8);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clflush "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else
		{
			return false;
		}
	}
}

Bool __stdcall DasmX86_64_0fAF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fB0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpxchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fB1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpxchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fB2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fB3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btr "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fB4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fB5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fB6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movzx "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		*regPtr = memVal;
	}
	return true;
}

Bool __stdcall DasmX86_64_0fB7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt16 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movzx "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_64_0fB8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt64 memVal;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popcnt "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fB9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fBA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bt "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bts "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->regs.rip += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_64_0fBB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fBC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bsf "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fBD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bsr "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fBE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsx "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(Int16*)regPtr = (Int8)memVal;
	}
	else
	{
		*(Int32*)regPtr = (Int8)memVal;
	}
	return true;
}

Bool __stdcall DasmX86_64_0fBF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt16 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsx "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*(Int32*)regPtr = (Int16)memVal;
	return true;
}

Bool __stdcall DasmX86_64_0fC0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_64_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xadd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fC1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_64_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xadd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fC2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 cmdId;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.rip);
		sess->regs.rip++;
		switch (cmdId & 7)
		{
		case 0:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpeqps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 1:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpltps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 2:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpleps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 3:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpunordps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 4:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpneqps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 5:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnltps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 6:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnleps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 7:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpordps "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		return false;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.rip);
		sess->regs.rip++;
		switch (cmdId & 7)
		{
		case 0:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpeqpd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 1:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpltpd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 2:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmplepd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 3:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpunordpd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 4:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpneqpd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 5:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnltpd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 6:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnlepd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 7:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpordpd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		return false;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.rip);
		sess->regs.rip++;
		switch (cmdId & 7)
		{
		case 0:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpeqsd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 1:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpltsd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 2:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmplesd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 3:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpunordsd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 4:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpneqsd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 5:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnltsd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 6:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnlesd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 7:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpordsd "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		return false;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.rip);
		sess->regs.rip++;
		switch (cmdId & 7)
		{
		case 0:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpeqss "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 1:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpltss "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 2:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpless "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 3:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpunordss "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 4:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpneqss "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 5:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnltss "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 6:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpnless "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		case 7:
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpordss "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		return false;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fC3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	UInt64 *regPtr;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movnti "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fC4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fC5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;}

Bool __stdcall DasmX86_64_0fC6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shufps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shufpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fC7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.rip += 2;
	DasmX86_64_ParseModRM64(sess, mem, &regNo);
	if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpxchg8b "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 6)
	{
		if ((sess->thisStatus & 5) == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmptrld "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if ((sess->thisStatus & 5) == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmclear "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if ((sess->thisStatus & 5) == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmxon "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmptrst "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fC8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 0, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fC9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 1, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fCA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 2, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fCB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 3, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fCC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 4, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fCD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 5, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fCE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 6, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fCF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char reg[8];
	UInt64 *regPtr;
	DasmX86_64_ParseReg32(sess, reg, 7, &regPtr);
	sess->regs.rip+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_64_0fD0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addsubpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addsubps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmullw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmullw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0fD8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fD9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fDA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fDB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pand "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pand "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fDC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fDD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fDE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fDF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pandn "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pandn "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psraw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psraw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrad "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrad "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttpd2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpd2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtdq2pd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if (sess->thisStatus & 0x20000)
	{
		if (sess->thisStatus & 0x40000)
		{
			sess->regs.rip += 2;
			DasmX86_64_ParseModVYMM256(sess, mem, &regNo);
			DasmX86_64_ParseRegYMM(sess, reg, regNo);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovntdq "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else
		{
			sess->regs.rip += 2;
			DasmX86_64_ParseModVXMM128(sess, mem, &regNo);
			DasmX86_64_ParseRegXMM(sess, reg, regNo);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmovntdq "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrConcat(sess->sbuff, reg);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fE9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fEA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fEB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("por "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("por "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fEC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fED(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fEE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fEF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pxor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pxor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lddqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmuludq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmuludq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psadbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psadbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maskmovq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maskmovdqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fF9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fFA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fFB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fFC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fFD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fFE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 2;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0fFF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3800(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3801(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3802(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3803(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3804(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3805(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3806(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3807(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3808(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3809(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f380A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f380B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhrsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhrsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f380C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f380D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f380E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f380F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3810(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pblendvb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3811(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3812(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3813(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3814(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendvps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3815(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendvpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3816(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3817(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ptest "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3818(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3819(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f381A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f381B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f381C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f381D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f381E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f381F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3820(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3821(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxbd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3822(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxbq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3823(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3824(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxwq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3825(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3826(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3827(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3828(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmuldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3829(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f382A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntdqa "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f382B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packusdw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f382C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f382D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f382E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f382F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3830(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3831(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxbd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3832(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxbq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3833(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3834(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxwq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3835(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3836(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3837(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3838(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3839(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f383A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f383B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminud "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f383C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f383D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f383E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f383F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxud "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3840(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3841(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phminposuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3842(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3843(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3844(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3845(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3846(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3847(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3848(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3849(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f384A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f384B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f384C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f384D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f384E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f384F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3850(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3851(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3852(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3853(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3854(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3855(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3856(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3857(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3858(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3859(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f385A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f385B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f385C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f385D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f385E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f385F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3860(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3861(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3862(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3863(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3864(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3865(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3866(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3867(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3868(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3869(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f386A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f386B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f386C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f386D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f386E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f386F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3870(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3871(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3872(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3873(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3874(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3875(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3876(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3877(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3878(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3879(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f387A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f387B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f387C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f387D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f387E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f387F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3880(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 *regPtr;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("invept "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3881(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 *regPtr;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM128(sess, mem, &regNo);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("invvpid "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3882(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3883(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3884(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3885(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3886(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3887(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3888(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3889(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f388A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f388B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f388C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f388D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f388E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f388F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3890(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3891(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3892(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3893(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3894(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3895(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3896(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3897(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3898(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3899(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f389A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f389B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f389C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f389D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f389E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f389F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38A9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38AA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38AB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38AC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38AD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38AE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38AF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38B9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38BA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38BB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38BC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38BD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38BE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38BF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38C9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38CA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38CB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38CC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38CD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38CE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38CF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38D9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38DA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38DB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesimc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38DC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesenc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38DD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesenclast "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38DE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesdec "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38DF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesdeclast "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38E0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38E8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38E9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38EA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38EB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38EC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38ED(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38EE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38EF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		UInt64 memVal;
		UInt64 *regPtr;
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movbe "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		UInt16 memVal;
		UInt64 *regPtr;
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movbe "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt8 memVal;
		UInt64 *regPtr;
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM8(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("crc32 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38F1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		UInt64 memVal;
		UInt64 *regPtr;
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movbe "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		UInt16 memVal;
		UInt64 *regPtr;
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseReg16(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movbe "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt64 memVal;
		UInt64 *regPtr;
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("crc32 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38F2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	if ((sess->thisStatus & 5) == 1)
	{
		UTF8Char mem[64];
		UTF8Char regs[8];
		Int32 reg;
		UInt64 memVal;
		UInt64 *regPtr;

		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
		DasmX86_64_ParseReg(sess, regs, reg, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adcx "));
		sess->sbuff = Text::StrConcat(sess->sbuff, regs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UTF8Char mem[64];
		UTF8Char regs[8];
		Int32 reg;
		UInt64 memVal;
		UInt64 *regPtr;

		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &reg, &memVal, 0);
		DasmX86_64_ParseReg(sess, regs, reg, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adox "));
		sess->sbuff = Text::StrConcat(sess->sbuff, regs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f38F7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38F9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38FA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38FB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38FC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38FD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38FE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f38FF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a00(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a01(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a02(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a03(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a04(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a05(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a06(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a07(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a08(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("roundps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a09(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("roundpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a0A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("roundss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a0B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a0C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a0D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a0E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pblendw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a0F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_64_ParseRegMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("palignr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("palignr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a10(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a11(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a12(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a13(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a14(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModR32M8(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a15(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a16(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a17(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("extractps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a18(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a19(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a1A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a1B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a1C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a1D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a1E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a1F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a20(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModR32M8(sess, mem, &regNo, &memVal);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a21(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insertps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a22(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt64 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a23(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a24(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a25(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a26(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a27(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a28(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a29(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a2A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a2B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a2C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a2D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a2E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a2F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a30(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a31(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a32(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a33(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a34(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a35(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a36(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a37(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a38(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a39(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a3A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a3B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a3C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a3D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a3E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a3F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a40(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dpps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a41(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dppd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a42(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mpsadbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a43(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a44(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pclmulqdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a45(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a46(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a47(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a48(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a49(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a4A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a4B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a4C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a4D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a4E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a4F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a50(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a51(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a52(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a53(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a54(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a55(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a56(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a57(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a58(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a59(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a5A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a5B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a5C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a5D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a5E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a5F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a60(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpestrm "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a61(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpestri "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a62(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpistrm "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a63(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.rip += 3;
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpistri "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3a64(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a65(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a66(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a67(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a68(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a69(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a6A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a6B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a6C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a6D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a6E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a6F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a70(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a71(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a72(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a73(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a74(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a75(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a76(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a77(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a78(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a79(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a7A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a7B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a7C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a7D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a7E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a7F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a80(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a81(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a82(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a83(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a84(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a85(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a86(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a87(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a88(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a89(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a8A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a8B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a8C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a8D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a8E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a8F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a90(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a91(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a92(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a93(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a94(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a95(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a96(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a97(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a98(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a99(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a9A(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a9B(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a9C(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a9D(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a9E(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3a9F(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aA9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aAA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aAB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aAC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aAD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aAE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aAF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aB9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aBA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aBB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aBC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aBD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aBE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aBF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aC9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aCA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aCB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aCC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aCD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aCE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aCF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aD9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aDA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aDB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aDC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aDD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aDE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aDF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.rip += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_64_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_64_ParseRegXMM(sess, reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aeskeygenassist "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.rip));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.rip++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_64_0f3aE0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aE9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aEA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aEB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aEC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aED(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aEE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aEF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF0(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF1(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF2(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF3(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF4(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF5(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF6(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF7(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF8(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aF9(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aFA(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aFB(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aFC(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aFD(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aFE(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Bool __stdcall DasmX86_64_0f3aFF(Manage::DasmX86_64::DasmX86_64_Sess* sess)
{
	return false;
}

Manage::DasmX86_64::DasmX86_64()
{
	DasmX86_64_Code *codes;
	DasmX86_64_Code *codes0f;
	DasmX86_64_Code *codes0f38;
	DasmX86_64_Code *codes0f3a;
//	this->nameFunc = nameFunc;
//	this->userObj = userObj;
	codes = this->codes = MemAlloc(DasmX86_64_Code, 256);
	codes0f = this->codes0f = MemAlloc(DasmX86_64_Code, 256);
	codes0f38 = this->codes0f38 = MemAlloc(DasmX86_64_Code, 256);
	codes0f3a = this->codes0f3a = MemAlloc(DasmX86_64_Code, 256);
	codes[0x00] = DasmX86_64_00;
	codes[0x01] = DasmX86_64_01;
	codes[0x02] = DasmX86_64_02;
	codes[0x03] = DasmX86_64_03;
	codes[0x04] = DasmX86_64_04;
	codes[0x05] = DasmX86_64_05;
	codes[0x06] = DasmX86_64_06;
	codes[0x07] = DasmX86_64_07;
	codes[0x08] = DasmX86_64_08;
	codes[0x09] = DasmX86_64_09;
	codes[0x0a] = DasmX86_64_0a;
	codes[0x0b] = DasmX86_64_0b;
	codes[0x0c] = DasmX86_64_0c;
	codes[0x0d] = DasmX86_64_0d;
	codes[0x0e] = DasmX86_64_0e;
	codes[0x0f] = DasmX86_64_0f;
	codes[0x10] = DasmX86_64_10;
	codes[0x11] = DasmX86_64_11;
	codes[0x12] = DasmX86_64_12;
	codes[0x13] = DasmX86_64_13;
	codes[0x14] = DasmX86_64_14;
	codes[0x15] = DasmX86_64_15;
	codes[0x16] = DasmX86_64_16;
	codes[0x17] = DasmX86_64_17;
	codes[0x18] = DasmX86_64_18;
	codes[0x19] = DasmX86_64_19;
	codes[0x1a] = DasmX86_64_1a;
	codes[0x1b] = DasmX86_64_1b;
	codes[0x1c] = DasmX86_64_1c;
	codes[0x1d] = DasmX86_64_1d;
	codes[0x1e] = DasmX86_64_1e;
	codes[0x1f] = DasmX86_64_1f;
	codes[0x20] = DasmX86_64_20;
	codes[0x21] = DasmX86_64_21;
	codes[0x22] = DasmX86_64_22;
	codes[0x23] = DasmX86_64_23;
	codes[0x24] = DasmX86_64_24;
	codes[0x25] = DasmX86_64_25;
	codes[0x26] = DasmX86_64_26;
	codes[0x27] = DasmX86_64_27;
	codes[0x28] = DasmX86_64_28;
	codes[0x29] = DasmX86_64_29;
	codes[0x2a] = DasmX86_64_2a;
	codes[0x2b] = DasmX86_64_2b;
	codes[0x2c] = DasmX86_64_2c;
	codes[0x2d] = DasmX86_64_2d;
	codes[0x2e] = DasmX86_64_2e;
	codes[0x2f] = DasmX86_64_2f;
	codes[0x30] = DasmX86_64_30;
	codes[0x31] = DasmX86_64_31;
	codes[0x32] = DasmX86_64_32;
	codes[0x33] = DasmX86_64_33;
	codes[0x34] = DasmX86_64_34;
	codes[0x35] = DasmX86_64_35;
	codes[0x36] = DasmX86_64_36;
	codes[0x37] = DasmX86_64_37;
	codes[0x38] = DasmX86_64_38;
	codes[0x39] = DasmX86_64_39;
	codes[0x3a] = DasmX86_64_3a;
	codes[0x3b] = DasmX86_64_3b;
	codes[0x3c] = DasmX86_64_3c;
	codes[0x3d] = DasmX86_64_3d;
	codes[0x3e] = DasmX86_64_3e;
	codes[0x3f] = DasmX86_64_3f;
	codes[0x40] = DasmX86_64_40;
	codes[0x41] = DasmX86_64_41;
	codes[0x42] = DasmX86_64_42;
	codes[0x43] = DasmX86_64_43;
	codes[0x44] = DasmX86_64_44;
	codes[0x45] = DasmX86_64_45;
	codes[0x46] = DasmX86_64_46;
	codes[0x47] = DasmX86_64_47;
	codes[0x48] = DasmX86_64_48;
	codes[0x49] = DasmX86_64_49;
	codes[0x4a] = DasmX86_64_4a;
	codes[0x4b] = DasmX86_64_4b;
	codes[0x4c] = DasmX86_64_4c;
	codes[0x4d] = DasmX86_64_4d;
	codes[0x4e] = DasmX86_64_4e;
	codes[0x4f] = DasmX86_64_4f;
	codes[0x50] = DasmX86_64_50;
	codes[0x51] = DasmX86_64_51;
	codes[0x52] = DasmX86_64_52;
	codes[0x53] = DasmX86_64_53;
	codes[0x54] = DasmX86_64_54;
	codes[0x55] = DasmX86_64_55;
	codes[0x56] = DasmX86_64_56;
	codes[0x57] = DasmX86_64_57;
	codes[0x58] = DasmX86_64_58;
	codes[0x59] = DasmX86_64_59;
	codes[0x5a] = DasmX86_64_5a;
	codes[0x5b] = DasmX86_64_5b;
	codes[0x5c] = DasmX86_64_5c;
	codes[0x5d] = DasmX86_64_5d;
	codes[0x5e] = DasmX86_64_5e;
	codes[0x5f] = DasmX86_64_5f;
	codes[0x60] = DasmX86_64_60;
	codes[0x61] = DasmX86_64_61;
	codes[0x62] = DasmX86_64_62;
	codes[0x63] = DasmX86_64_63;
	codes[0x64] = DasmX86_64_64;
	codes[0x65] = DasmX86_64_65;
	codes[0x66] = DasmX86_64_66;
	codes[0x67] = DasmX86_64_67;
	codes[0x68] = DasmX86_64_68;
	codes[0x69] = DasmX86_64_69;
	codes[0x6a] = DasmX86_64_6a;
	codes[0x6b] = DasmX86_64_6b;
	codes[0x6c] = DasmX86_64_6c;
	codes[0x6d] = DasmX86_64_6d;
	codes[0x6e] = DasmX86_64_6e;
	codes[0x6f] = DasmX86_64_6f;
	codes[0x70] = DasmX86_64_70;
	codes[0x71] = DasmX86_64_71;
	codes[0x72] = DasmX86_64_72;
	codes[0x73] = DasmX86_64_73;
	codes[0x74] = DasmX86_64_74;
	codes[0x75] = DasmX86_64_75;
	codes[0x76] = DasmX86_64_76;
	codes[0x77] = DasmX86_64_77;
	codes[0x78] = DasmX86_64_78;
	codes[0x79] = DasmX86_64_79;
	codes[0x7a] = DasmX86_64_7a;
	codes[0x7b] = DasmX86_64_7b;
	codes[0x7c] = DasmX86_64_7c;
	codes[0x7d] = DasmX86_64_7d;
	codes[0x7e] = DasmX86_64_7e;
	codes[0x7f] = DasmX86_64_7f;
	codes[0x80] = DasmX86_64_80;
	codes[0x81] = DasmX86_64_81;
	codes[0x82] = DasmX86_64_82;
	codes[0x83] = DasmX86_64_83;
	codes[0x84] = DasmX86_64_84;
	codes[0x85] = DasmX86_64_85;
	codes[0x86] = DasmX86_64_86;
	codes[0x87] = DasmX86_64_87;
	codes[0x88] = DasmX86_64_88;
	codes[0x89] = DasmX86_64_89;
	codes[0x8a] = DasmX86_64_8a;
	codes[0x8b] = DasmX86_64_8b;
	codes[0x8c] = DasmX86_64_8c;
	codes[0x8d] = DasmX86_64_8d;
	codes[0x8e] = DasmX86_64_8e;
	codes[0x8f] = DasmX86_64_8f;
	codes[0x90] = DasmX86_64_90;
	codes[0x91] = DasmX86_64_91;
	codes[0x92] = DasmX86_64_92;
	codes[0x93] = DasmX86_64_93;
	codes[0x94] = DasmX86_64_94;
	codes[0x95] = DasmX86_64_95;
	codes[0x96] = DasmX86_64_96;
	codes[0x97] = DasmX86_64_97;
	codes[0x98] = DasmX86_64_98;
	codes[0x99] = DasmX86_64_99;
	codes[0x9a] = DasmX86_64_9a;
	codes[0x9b] = DasmX86_64_9b;
	codes[0x9c] = DasmX86_64_9c;
	codes[0x9d] = DasmX86_64_9d;
	codes[0x9e] = DasmX86_64_9e;
	codes[0x9f] = DasmX86_64_9f;
	codes[0xa0] = DasmX86_64_a0;
	codes[0xa1] = DasmX86_64_a1;
	codes[0xa2] = DasmX86_64_a2;
	codes[0xa3] = DasmX86_64_a3;
	codes[0xa4] = DasmX86_64_a4;
	codes[0xa5] = DasmX86_64_a5;
	codes[0xa6] = DasmX86_64_a6;
	codes[0xa7] = DasmX86_64_a7;
	codes[0xa8] = DasmX86_64_a8;
	codes[0xa9] = DasmX86_64_a9;
	codes[0xaa] = DasmX86_64_aa;
	codes[0xab] = DasmX86_64_ab;
	codes[0xac] = DasmX86_64_ac;
	codes[0xad] = DasmX86_64_ad;
	codes[0xae] = DasmX86_64_ae;
	codes[0xaf] = DasmX86_64_af;
	codes[0xb0] = DasmX86_64_b0;
	codes[0xb1] = DasmX86_64_b1;
	codes[0xb2] = DasmX86_64_b2;
	codes[0xb3] = DasmX86_64_b3;
	codes[0xb4] = DasmX86_64_b4;
	codes[0xb5] = DasmX86_64_b5;
	codes[0xb6] = DasmX86_64_b6;
	codes[0xb7] = DasmX86_64_b7;
	codes[0xb8] = DasmX86_64_b8;
	codes[0xb9] = DasmX86_64_b9;
	codes[0xba] = DasmX86_64_ba;
	codes[0xbb] = DasmX86_64_bb;
	codes[0xbc] = DasmX86_64_bc;
	codes[0xbd] = DasmX86_64_bd;
	codes[0xbe] = DasmX86_64_be;
	codes[0xbf] = DasmX86_64_bf;
	codes[0xc0] = DasmX86_64_c0;
	codes[0xc1] = DasmX86_64_c1;
	codes[0xc2] = DasmX86_64_c2;
	codes[0xc3] = DasmX86_64_c3;
	codes[0xc4] = DasmX86_64_c4;
	codes[0xc5] = DasmX86_64_c5;
	codes[0xc6] = DasmX86_64_c6;
	codes[0xc7] = DasmX86_64_c7;
	codes[0xc8] = DasmX86_64_c8;
	codes[0xc9] = DasmX86_64_c9;
	codes[0xca] = DasmX86_64_ca;
	codes[0xcb] = DasmX86_64_cb;
	codes[0xcc] = DasmX86_64_cc;
	codes[0xcd] = DasmX86_64_cd;
	codes[0xce] = DasmX86_64_ce;
	codes[0xcf] = DasmX86_64_cf;
	codes[0xd0] = DasmX86_64_d0;
	codes[0xd1] = DasmX86_64_d1;
	codes[0xd2] = DasmX86_64_d2;
	codes[0xd3] = DasmX86_64_d3;
	codes[0xd4] = DasmX86_64_d4;
	codes[0xd5] = DasmX86_64_d5;
	codes[0xd6] = DasmX86_64_d6;
	codes[0xd7] = DasmX86_64_d7;
	codes[0xd8] = DasmX86_64_d8;
	codes[0xd9] = DasmX86_64_d9;
	codes[0xda] = DasmX86_64_da;
	codes[0xdb] = DasmX86_64_db;
	codes[0xdc] = DasmX86_64_dc;
	codes[0xdd] = DasmX86_64_dd;
	codes[0xde] = DasmX86_64_de;
	codes[0xdf] = DasmX86_64_df;
	codes[0xe0] = DasmX86_64_e0;
	codes[0xe1] = DasmX86_64_e1;
	codes[0xe2] = DasmX86_64_e2;
	codes[0xe3] = DasmX86_64_e3;
	codes[0xe4] = DasmX86_64_e4;
	codes[0xe5] = DasmX86_64_e5;
	codes[0xe6] = DasmX86_64_e6;
	codes[0xe7] = DasmX86_64_e7;
	codes[0xe8] = DasmX86_64_e8;
	codes[0xe9] = DasmX86_64_e9;
	codes[0xea] = DasmX86_64_ea;
	codes[0xeb] = DasmX86_64_eb;
	codes[0xec] = DasmX86_64_ec;
	codes[0xed] = DasmX86_64_ed;
	codes[0xee] = DasmX86_64_ee;
	codes[0xef] = DasmX86_64_ef;
	codes[0xf0] = DasmX86_64_f0;
	codes[0xf1] = DasmX86_64_f1;
	codes[0xf2] = DasmX86_64_f2;
	codes[0xf3] = DasmX86_64_f3;
	codes[0xf4] = DasmX86_64_f4;
	codes[0xf5] = DasmX86_64_f5;
	codes[0xf6] = DasmX86_64_f6;
	codes[0xf7] = DasmX86_64_f7;
	codes[0xf8] = DasmX86_64_f8;
	codes[0xf9] = DasmX86_64_f9;
	codes[0xfa] = DasmX86_64_fa;
	codes[0xfb] = DasmX86_64_fb;
	codes[0xfc] = DasmX86_64_fc;
	codes[0xfd] = DasmX86_64_fd;
	codes[0xfe] = DasmX86_64_fe;
	codes[0xff] = DasmX86_64_ff;

	codes0f[0x00] = DasmX86_64_0f00;
	codes0f[0x01] = DasmX86_64_0f01;
	codes0f[0x02] = DasmX86_64_0f02;
	codes0f[0x03] = DasmX86_64_0f03;
	codes0f[0x04] = DasmX86_64_0f04;
	codes0f[0x05] = DasmX86_64_0f05;
	codes0f[0x06] = DasmX86_64_0f06;
	codes0f[0x07] = DasmX86_64_0f07;
	codes0f[0x08] = DasmX86_64_0f08;
	codes0f[0x09] = DasmX86_64_0f09;
	codes0f[0x0A] = DasmX86_64_0f0A;
	codes0f[0x0B] = DasmX86_64_0f0B;
	codes0f[0x0C] = DasmX86_64_0f0C;
	codes0f[0x0D] = DasmX86_64_0f0D;
	codes0f[0x0E] = DasmX86_64_0f0E;
	codes0f[0x0F] = DasmX86_64_0f0F;
	codes0f[0x10] = DasmX86_64_0f10;
	codes0f[0x11] = DasmX86_64_0f11;
	codes0f[0x12] = DasmX86_64_0f12;
	codes0f[0x13] = DasmX86_64_0f13;
	codes0f[0x14] = DasmX86_64_0f14;
	codes0f[0x15] = DasmX86_64_0f15;
	codes0f[0x16] = DasmX86_64_0f16;
	codes0f[0x17] = DasmX86_64_0f17;
	codes0f[0x18] = DasmX86_64_0f18;
	codes0f[0x19] = DasmX86_64_0f19;
	codes0f[0x1A] = DasmX86_64_0f1A;
	codes0f[0x1B] = DasmX86_64_0f1B;
	codes0f[0x1C] = DasmX86_64_0f1C;
	codes0f[0x1D] = DasmX86_64_0f1D;
	codes0f[0x1E] = DasmX86_64_0f1E;
	codes0f[0x1F] = DasmX86_64_0f1F;
	codes0f[0x20] = DasmX86_64_0f20;
	codes0f[0x21] = DasmX86_64_0f21;
	codes0f[0x22] = DasmX86_64_0f22;
	codes0f[0x23] = DasmX86_64_0f23;
	codes0f[0x24] = DasmX86_64_0f24;
	codes0f[0x25] = DasmX86_64_0f25;
	codes0f[0x26] = DasmX86_64_0f26;
	codes0f[0x27] = DasmX86_64_0f27;
	codes0f[0x28] = DasmX86_64_0f28;
	codes0f[0x29] = DasmX86_64_0f29;
	codes0f[0x2A] = DasmX86_64_0f2A;
	codes0f[0x2B] = DasmX86_64_0f2B;
	codes0f[0x2C] = DasmX86_64_0f2C;
	codes0f[0x2D] = DasmX86_64_0f2D;
	codes0f[0x2E] = DasmX86_64_0f2E;
	codes0f[0x2F] = DasmX86_64_0f2F;
	codes0f[0x30] = DasmX86_64_0f30;
	codes0f[0x31] = DasmX86_64_0f31;
	codes0f[0x32] = DasmX86_64_0f32;
	codes0f[0x33] = DasmX86_64_0f33;
	codes0f[0x34] = DasmX86_64_0f34;
	codes0f[0x35] = DasmX86_64_0f35;
	codes0f[0x36] = DasmX86_64_0f36;
	codes0f[0x37] = DasmX86_64_0f37;
	codes0f[0x38] = DasmX86_64_0f38;
	codes0f[0x39] = DasmX86_64_0f39;
	codes0f[0x3A] = DasmX86_64_0f3A;
	codes0f[0x3B] = DasmX86_64_0f3B;
	codes0f[0x3C] = DasmX86_64_0f3C;
	codes0f[0x3D] = DasmX86_64_0f3D;
	codes0f[0x3E] = DasmX86_64_0f3E;
	codes0f[0x3F] = DasmX86_64_0f3F;
	codes0f[0x40] = DasmX86_64_0f40;
	codes0f[0x41] = DasmX86_64_0f41;
	codes0f[0x42] = DasmX86_64_0f42;
	codes0f[0x43] = DasmX86_64_0f43;
	codes0f[0x44] = DasmX86_64_0f44;
	codes0f[0x45] = DasmX86_64_0f45;
	codes0f[0x46] = DasmX86_64_0f46;
	codes0f[0x47] = DasmX86_64_0f47;
	codes0f[0x48] = DasmX86_64_0f48;
	codes0f[0x49] = DasmX86_64_0f49;
	codes0f[0x4A] = DasmX86_64_0f4A;
	codes0f[0x4B] = DasmX86_64_0f4B;
	codes0f[0x4C] = DasmX86_64_0f4C;
	codes0f[0x4D] = DasmX86_64_0f4D;
	codes0f[0x4E] = DasmX86_64_0f4E;
	codes0f[0x4F] = DasmX86_64_0f4F;
	codes0f[0x50] = DasmX86_64_0f50;
	codes0f[0x51] = DasmX86_64_0f51;
	codes0f[0x52] = DasmX86_64_0f52;
	codes0f[0x53] = DasmX86_64_0f53;
	codes0f[0x54] = DasmX86_64_0f54;
	codes0f[0x55] = DasmX86_64_0f55;
	codes0f[0x56] = DasmX86_64_0f56;
	codes0f[0x57] = DasmX86_64_0f57;
	codes0f[0x58] = DasmX86_64_0f58;
	codes0f[0x59] = DasmX86_64_0f59;
	codes0f[0x5A] = DasmX86_64_0f5A;
	codes0f[0x5B] = DasmX86_64_0f5B;
	codes0f[0x5C] = DasmX86_64_0f5C;
	codes0f[0x5D] = DasmX86_64_0f5D;
	codes0f[0x5E] = DasmX86_64_0f5E;
	codes0f[0x5F] = DasmX86_64_0f5F;
	codes0f[0x60] = DasmX86_64_0f60;
	codes0f[0x61] = DasmX86_64_0f61;
	codes0f[0x62] = DasmX86_64_0f62;
	codes0f[0x63] = DasmX86_64_0f63;
	codes0f[0x64] = DasmX86_64_0f64;
	codes0f[0x65] = DasmX86_64_0f65;
	codes0f[0x66] = DasmX86_64_0f66;
	codes0f[0x67] = DasmX86_64_0f67;
	codes0f[0x68] = DasmX86_64_0f68;
	codes0f[0x69] = DasmX86_64_0f69;
	codes0f[0x6A] = DasmX86_64_0f6A;
	codes0f[0x6B] = DasmX86_64_0f6B;
	codes0f[0x6C] = DasmX86_64_0f6C;
	codes0f[0x6D] = DasmX86_64_0f6D;
	codes0f[0x6E] = DasmX86_64_0f6E;
	codes0f[0x6F] = DasmX86_64_0f6F;
	codes0f[0x70] = DasmX86_64_0f70;
	codes0f[0x71] = DasmX86_64_0f71;
	codes0f[0x72] = DasmX86_64_0f72;
	codes0f[0x73] = DasmX86_64_0f73;
	codes0f[0x74] = DasmX86_64_0f74;
	codes0f[0x75] = DasmX86_64_0f75;
	codes0f[0x76] = DasmX86_64_0f76;
	codes0f[0x77] = DasmX86_64_0f77;
	codes0f[0x78] = DasmX86_64_0f78;
	codes0f[0x79] = DasmX86_64_0f79;
	codes0f[0x7A] = DasmX86_64_0f7A;
	codes0f[0x7B] = DasmX86_64_0f7B;
	codes0f[0x7C] = DasmX86_64_0f7C;
	codes0f[0x7D] = DasmX86_64_0f7D;
	codes0f[0x7E] = DasmX86_64_0f7E;
	codes0f[0x7F] = DasmX86_64_0f7F;
	codes0f[0x80] = DasmX86_64_0f80;
	codes0f[0x81] = DasmX86_64_0f81;
	codes0f[0x82] = DasmX86_64_0f82;
	codes0f[0x83] = DasmX86_64_0f83;
	codes0f[0x84] = DasmX86_64_0f84;
	codes0f[0x85] = DasmX86_64_0f85;
	codes0f[0x86] = DasmX86_64_0f86;
	codes0f[0x87] = DasmX86_64_0f87;
	codes0f[0x88] = DasmX86_64_0f88;
	codes0f[0x89] = DasmX86_64_0f89;
	codes0f[0x8A] = DasmX86_64_0f8A;
	codes0f[0x8B] = DasmX86_64_0f8B;
	codes0f[0x8C] = DasmX86_64_0f8C;
	codes0f[0x8D] = DasmX86_64_0f8D;
	codes0f[0x8E] = DasmX86_64_0f8E;
	codes0f[0x8F] = DasmX86_64_0f8F;
	codes0f[0x90] = DasmX86_64_0f90;
	codes0f[0x91] = DasmX86_64_0f91;
	codes0f[0x92] = DasmX86_64_0f92;
	codes0f[0x93] = DasmX86_64_0f93;
	codes0f[0x94] = DasmX86_64_0f94;
	codes0f[0x95] = DasmX86_64_0f95;
	codes0f[0x96] = DasmX86_64_0f96;
	codes0f[0x97] = DasmX86_64_0f97;
	codes0f[0x98] = DasmX86_64_0f98;
	codes0f[0x99] = DasmX86_64_0f99;
	codes0f[0x9A] = DasmX86_64_0f9A;
	codes0f[0x9B] = DasmX86_64_0f9B;
	codes0f[0x9C] = DasmX86_64_0f9C;
	codes0f[0x9D] = DasmX86_64_0f9D;
	codes0f[0x9E] = DasmX86_64_0f9E;
	codes0f[0x9F] = DasmX86_64_0f9F;
	codes0f[0xA0] = DasmX86_64_0fA0;
	codes0f[0xA1] = DasmX86_64_0fA1;
	codes0f[0xA2] = DasmX86_64_0fA2;
	codes0f[0xA3] = DasmX86_64_0fA3;
	codes0f[0xA4] = DasmX86_64_0fA4;
	codes0f[0xA5] = DasmX86_64_0fA5;
	codes0f[0xA6] = DasmX86_64_0fA6;
	codes0f[0xA7] = DasmX86_64_0fA7;
	codes0f[0xA8] = DasmX86_64_0fA8;
	codes0f[0xA9] = DasmX86_64_0fA9;
	codes0f[0xAA] = DasmX86_64_0fAA;
	codes0f[0xAB] = DasmX86_64_0fAB;
	codes0f[0xAC] = DasmX86_64_0fAC;
	codes0f[0xAD] = DasmX86_64_0fAD;
	codes0f[0xAE] = DasmX86_64_0fAE;
	codes0f[0xAF] = DasmX86_64_0fAF;
	codes0f[0xB0] = DasmX86_64_0fB0;
	codes0f[0xB1] = DasmX86_64_0fB1;
	codes0f[0xB2] = DasmX86_64_0fB2;
	codes0f[0xB3] = DasmX86_64_0fB3;
	codes0f[0xB4] = DasmX86_64_0fB4;
	codes0f[0xB5] = DasmX86_64_0fB5;
	codes0f[0xB6] = DasmX86_64_0fB6;
	codes0f[0xB7] = DasmX86_64_0fB7;
	codes0f[0xB8] = DasmX86_64_0fB8;
	codes0f[0xB9] = DasmX86_64_0fB9;
	codes0f[0xBA] = DasmX86_64_0fBA;
	codes0f[0xBB] = DasmX86_64_0fBB;
	codes0f[0xBC] = DasmX86_64_0fBC;
	codes0f[0xBD] = DasmX86_64_0fBD;
	codes0f[0xBE] = DasmX86_64_0fBE;
	codes0f[0xBF] = DasmX86_64_0fBF;
	codes0f[0xC0] = DasmX86_64_0fC0;
	codes0f[0xC1] = DasmX86_64_0fC1;
	codes0f[0xC2] = DasmX86_64_0fC2;
	codes0f[0xC3] = DasmX86_64_0fC3;
	codes0f[0xC4] = DasmX86_64_0fC4;
	codes0f[0xC5] = DasmX86_64_0fC5;
	codes0f[0xC6] = DasmX86_64_0fC6;
	codes0f[0xC7] = DasmX86_64_0fC7;
	codes0f[0xC8] = DasmX86_64_0fC8;
	codes0f[0xC9] = DasmX86_64_0fC9;
	codes0f[0xCA] = DasmX86_64_0fCA;
	codes0f[0xCB] = DasmX86_64_0fCB;
	codes0f[0xCC] = DasmX86_64_0fCC;
	codes0f[0xCD] = DasmX86_64_0fCD;
	codes0f[0xCE] = DasmX86_64_0fCE;
	codes0f[0xCF] = DasmX86_64_0fCF;
	codes0f[0xD0] = DasmX86_64_0fD0;
	codes0f[0xD1] = DasmX86_64_0fD1;
	codes0f[0xD2] = DasmX86_64_0fD2;
	codes0f[0xD3] = DasmX86_64_0fD3;
	codes0f[0xD4] = DasmX86_64_0fD4;
	codes0f[0xD5] = DasmX86_64_0fD5;
	codes0f[0xD6] = DasmX86_64_0fD6;
	codes0f[0xD7] = DasmX86_64_0fD7;
	codes0f[0xD8] = DasmX86_64_0fD8;
	codes0f[0xD9] = DasmX86_64_0fD9;
	codes0f[0xDA] = DasmX86_64_0fDA;
	codes0f[0xDB] = DasmX86_64_0fDB;
	codes0f[0xDC] = DasmX86_64_0fDC;
	codes0f[0xDD] = DasmX86_64_0fDD;
	codes0f[0xDE] = DasmX86_64_0fDE;
	codes0f[0xDF] = DasmX86_64_0fDF;
	codes0f[0xE0] = DasmX86_64_0fE0;
	codes0f[0xE1] = DasmX86_64_0fE1;
	codes0f[0xE2] = DasmX86_64_0fE2;
	codes0f[0xE3] = DasmX86_64_0fE3;
	codes0f[0xE4] = DasmX86_64_0fE4;
	codes0f[0xE5] = DasmX86_64_0fE5;
	codes0f[0xE6] = DasmX86_64_0fE6;
	codes0f[0xE7] = DasmX86_64_0fE7;
	codes0f[0xE8] = DasmX86_64_0fE8;
	codes0f[0xE9] = DasmX86_64_0fE9;
	codes0f[0xEA] = DasmX86_64_0fEA;
	codes0f[0xEB] = DasmX86_64_0fEB;
	codes0f[0xEC] = DasmX86_64_0fEC;
	codes0f[0xED] = DasmX86_64_0fED;
	codes0f[0xEE] = DasmX86_64_0fEE;
	codes0f[0xEF] = DasmX86_64_0fEF;
	codes0f[0xF0] = DasmX86_64_0fF0;
	codes0f[0xF1] = DasmX86_64_0fF1;
	codes0f[0xF2] = DasmX86_64_0fF2;
	codes0f[0xF3] = DasmX86_64_0fF3;
	codes0f[0xF4] = DasmX86_64_0fF4;
	codes0f[0xF5] = DasmX86_64_0fF5;
	codes0f[0xF6] = DasmX86_64_0fF6;
	codes0f[0xF7] = DasmX86_64_0fF7;
	codes0f[0xF8] = DasmX86_64_0fF8;
	codes0f[0xF9] = DasmX86_64_0fF9;
	codes0f[0xFA] = DasmX86_64_0fFA;
	codes0f[0xFB] = DasmX86_64_0fFB;
	codes0f[0xFC] = DasmX86_64_0fFC;
	codes0f[0xFD] = DasmX86_64_0fFD;
	codes0f[0xFE] = DasmX86_64_0fFE;
	codes0f[0xFF] = DasmX86_64_0fFF;

	codes0f38[0x00] = DasmX86_64_0f3800;
	codes0f38[0x01] = DasmX86_64_0f3801;
	codes0f38[0x02] = DasmX86_64_0f3802;
	codes0f38[0x03] = DasmX86_64_0f3803;
	codes0f38[0x04] = DasmX86_64_0f3804;
	codes0f38[0x05] = DasmX86_64_0f3805;
	codes0f38[0x06] = DasmX86_64_0f3806;
	codes0f38[0x07] = DasmX86_64_0f3807;
	codes0f38[0x08] = DasmX86_64_0f3808;
	codes0f38[0x09] = DasmX86_64_0f3809;
	codes0f38[0x0A] = DasmX86_64_0f380A;
	codes0f38[0x0B] = DasmX86_64_0f380B;
	codes0f38[0x0C] = DasmX86_64_0f380C;
	codes0f38[0x0D] = DasmX86_64_0f380D;
	codes0f38[0x0E] = DasmX86_64_0f380E;
	codes0f38[0x0F] = DasmX86_64_0f380F;
	codes0f38[0x10] = DasmX86_64_0f3810;
	codes0f38[0x11] = DasmX86_64_0f3811;
	codes0f38[0x12] = DasmX86_64_0f3812;
	codes0f38[0x13] = DasmX86_64_0f3813;
	codes0f38[0x14] = DasmX86_64_0f3814;
	codes0f38[0x15] = DasmX86_64_0f3815;
	codes0f38[0x16] = DasmX86_64_0f3816;
	codes0f38[0x17] = DasmX86_64_0f3817;
	codes0f38[0x18] = DasmX86_64_0f3818;
	codes0f38[0x19] = DasmX86_64_0f3819;
	codes0f38[0x1A] = DasmX86_64_0f381A;
	codes0f38[0x1B] = DasmX86_64_0f381B;
	codes0f38[0x1C] = DasmX86_64_0f381C;
	codes0f38[0x1D] = DasmX86_64_0f381D;
	codes0f38[0x1E] = DasmX86_64_0f381E;
	codes0f38[0x1F] = DasmX86_64_0f381F;
	codes0f38[0x20] = DasmX86_64_0f3820;
	codes0f38[0x21] = DasmX86_64_0f3821;
	codes0f38[0x22] = DasmX86_64_0f3822;
	codes0f38[0x23] = DasmX86_64_0f3823;
	codes0f38[0x24] = DasmX86_64_0f3824;
	codes0f38[0x25] = DasmX86_64_0f3825;
	codes0f38[0x26] = DasmX86_64_0f3826;
	codes0f38[0x27] = DasmX86_64_0f3827;
	codes0f38[0x28] = DasmX86_64_0f3828;
	codes0f38[0x29] = DasmX86_64_0f3829;
	codes0f38[0x2A] = DasmX86_64_0f382A;
	codes0f38[0x2B] = DasmX86_64_0f382B;
	codes0f38[0x2C] = DasmX86_64_0f382C;
	codes0f38[0x2D] = DasmX86_64_0f382D;
	codes0f38[0x2E] = DasmX86_64_0f382E;
	codes0f38[0x2F] = DasmX86_64_0f382F;
	codes0f38[0x30] = DasmX86_64_0f3830;
	codes0f38[0x31] = DasmX86_64_0f3831;
	codes0f38[0x32] = DasmX86_64_0f3832;
	codes0f38[0x33] = DasmX86_64_0f3833;
	codes0f38[0x34] = DasmX86_64_0f3834;
	codes0f38[0x35] = DasmX86_64_0f3835;
	codes0f38[0x36] = DasmX86_64_0f3836;
	codes0f38[0x37] = DasmX86_64_0f3837;
	codes0f38[0x38] = DasmX86_64_0f3838;
	codes0f38[0x39] = DasmX86_64_0f3839;
	codes0f38[0x3A] = DasmX86_64_0f383A;
	codes0f38[0x3B] = DasmX86_64_0f383B;
	codes0f38[0x3C] = DasmX86_64_0f383C;
	codes0f38[0x3D] = DasmX86_64_0f383D;
	codes0f38[0x3E] = DasmX86_64_0f383E;
	codes0f38[0x3F] = DasmX86_64_0f383F;
	codes0f38[0x40] = DasmX86_64_0f3840;
	codes0f38[0x41] = DasmX86_64_0f3841;
	codes0f38[0x42] = DasmX86_64_0f3842;
	codes0f38[0x43] = DasmX86_64_0f3843;
	codes0f38[0x44] = DasmX86_64_0f3844;
	codes0f38[0x45] = DasmX86_64_0f3845;
	codes0f38[0x46] = DasmX86_64_0f3846;
	codes0f38[0x47] = DasmX86_64_0f3847;
	codes0f38[0x48] = DasmX86_64_0f3848;
	codes0f38[0x49] = DasmX86_64_0f3849;
	codes0f38[0x4A] = DasmX86_64_0f384A;
	codes0f38[0x4B] = DasmX86_64_0f384B;
	codes0f38[0x4C] = DasmX86_64_0f384C;
	codes0f38[0x4D] = DasmX86_64_0f384D;
	codes0f38[0x4E] = DasmX86_64_0f384E;
	codes0f38[0x4F] = DasmX86_64_0f384F;
	codes0f38[0x50] = DasmX86_64_0f3850;
	codes0f38[0x51] = DasmX86_64_0f3851;
	codes0f38[0x52] = DasmX86_64_0f3852;
	codes0f38[0x53] = DasmX86_64_0f3853;
	codes0f38[0x54] = DasmX86_64_0f3854;
	codes0f38[0x55] = DasmX86_64_0f3855;
	codes0f38[0x56] = DasmX86_64_0f3856;
	codes0f38[0x57] = DasmX86_64_0f3857;
	codes0f38[0x58] = DasmX86_64_0f3858;
	codes0f38[0x59] = DasmX86_64_0f3859;
	codes0f38[0x5A] = DasmX86_64_0f385A;
	codes0f38[0x5B] = DasmX86_64_0f385B;
	codes0f38[0x5C] = DasmX86_64_0f385C;
	codes0f38[0x5D] = DasmX86_64_0f385D;
	codes0f38[0x5E] = DasmX86_64_0f385E;
	codes0f38[0x5F] = DasmX86_64_0f385F;
	codes0f38[0x60] = DasmX86_64_0f3860;
	codes0f38[0x61] = DasmX86_64_0f3861;
	codes0f38[0x62] = DasmX86_64_0f3862;
	codes0f38[0x63] = DasmX86_64_0f3863;
	codes0f38[0x64] = DasmX86_64_0f3864;
	codes0f38[0x65] = DasmX86_64_0f3865;
	codes0f38[0x66] = DasmX86_64_0f3866;
	codes0f38[0x67] = DasmX86_64_0f3867;
	codes0f38[0x68] = DasmX86_64_0f3868;
	codes0f38[0x69] = DasmX86_64_0f3869;
	codes0f38[0x6A] = DasmX86_64_0f386A;
	codes0f38[0x6B] = DasmX86_64_0f386B;
	codes0f38[0x6C] = DasmX86_64_0f386C;
	codes0f38[0x6D] = DasmX86_64_0f386D;
	codes0f38[0x6E] = DasmX86_64_0f386E;
	codes0f38[0x6F] = DasmX86_64_0f386F;
	codes0f38[0x70] = DasmX86_64_0f3870;
	codes0f38[0x71] = DasmX86_64_0f3871;
	codes0f38[0x72] = DasmX86_64_0f3872;
	codes0f38[0x73] = DasmX86_64_0f3873;
	codes0f38[0x74] = DasmX86_64_0f3874;
	codes0f38[0x75] = DasmX86_64_0f3875;
	codes0f38[0x76] = DasmX86_64_0f3876;
	codes0f38[0x77] = DasmX86_64_0f3877;
	codes0f38[0x78] = DasmX86_64_0f3878;
	codes0f38[0x79] = DasmX86_64_0f3879;
	codes0f38[0x7A] = DasmX86_64_0f387A;
	codes0f38[0x7B] = DasmX86_64_0f387B;
	codes0f38[0x7C] = DasmX86_64_0f387C;
	codes0f38[0x7D] = DasmX86_64_0f387D;
	codes0f38[0x7E] = DasmX86_64_0f387E;
	codes0f38[0x7F] = DasmX86_64_0f387F;
	codes0f38[0x80] = DasmX86_64_0f3880;
	codes0f38[0x81] = DasmX86_64_0f3881;
	codes0f38[0x82] = DasmX86_64_0f3882;
	codes0f38[0x83] = DasmX86_64_0f3883;
	codes0f38[0x84] = DasmX86_64_0f3884;
	codes0f38[0x85] = DasmX86_64_0f3885;
	codes0f38[0x86] = DasmX86_64_0f3886;
	codes0f38[0x87] = DasmX86_64_0f3887;
	codes0f38[0x88] = DasmX86_64_0f3888;
	codes0f38[0x89] = DasmX86_64_0f3889;
	codes0f38[0x8A] = DasmX86_64_0f388A;
	codes0f38[0x8B] = DasmX86_64_0f388B;
	codes0f38[0x8C] = DasmX86_64_0f388C;
	codes0f38[0x8D] = DasmX86_64_0f388D;
	codes0f38[0x8E] = DasmX86_64_0f388E;
	codes0f38[0x8F] = DasmX86_64_0f388F;
	codes0f38[0x90] = DasmX86_64_0f3890;
	codes0f38[0x91] = DasmX86_64_0f3891;
	codes0f38[0x92] = DasmX86_64_0f3892;
	codes0f38[0x93] = DasmX86_64_0f3893;
	codes0f38[0x94] = DasmX86_64_0f3894;
	codes0f38[0x95] = DasmX86_64_0f3895;
	codes0f38[0x96] = DasmX86_64_0f3896;
	codes0f38[0x97] = DasmX86_64_0f3897;
	codes0f38[0x98] = DasmX86_64_0f3898;
	codes0f38[0x99] = DasmX86_64_0f3899;
	codes0f38[0x9A] = DasmX86_64_0f389A;
	codes0f38[0x9B] = DasmX86_64_0f389B;
	codes0f38[0x9C] = DasmX86_64_0f389C;
	codes0f38[0x9D] = DasmX86_64_0f389D;
	codes0f38[0x9E] = DasmX86_64_0f389E;
	codes0f38[0x9F] = DasmX86_64_0f389F;
	codes0f38[0xA0] = DasmX86_64_0f38A0;
	codes0f38[0xA1] = DasmX86_64_0f38A1;
	codes0f38[0xA2] = DasmX86_64_0f38A2;
	codes0f38[0xA3] = DasmX86_64_0f38A3;
	codes0f38[0xA4] = DasmX86_64_0f38A4;
	codes0f38[0xA5] = DasmX86_64_0f38A5;
	codes0f38[0xA6] = DasmX86_64_0f38A6;
	codes0f38[0xA7] = DasmX86_64_0f38A7;
	codes0f38[0xA8] = DasmX86_64_0f38A8;
	codes0f38[0xA9] = DasmX86_64_0f38A9;
	codes0f38[0xAA] = DasmX86_64_0f38AA;
	codes0f38[0xAB] = DasmX86_64_0f38AB;
	codes0f38[0xAC] = DasmX86_64_0f38AC;
	codes0f38[0xAD] = DasmX86_64_0f38AD;
	codes0f38[0xAE] = DasmX86_64_0f38AE;
	codes0f38[0xAF] = DasmX86_64_0f38AF;
	codes0f38[0xB0] = DasmX86_64_0f38B0;
	codes0f38[0xB1] = DasmX86_64_0f38B1;
	codes0f38[0xB2] = DasmX86_64_0f38B2;
	codes0f38[0xB3] = DasmX86_64_0f38B3;
	codes0f38[0xB4] = DasmX86_64_0f38B4;
	codes0f38[0xB5] = DasmX86_64_0f38B5;
	codes0f38[0xB6] = DasmX86_64_0f38B6;
	codes0f38[0xB7] = DasmX86_64_0f38B7;
	codes0f38[0xB8] = DasmX86_64_0f38B8;
	codes0f38[0xB9] = DasmX86_64_0f38B9;
	codes0f38[0xBA] = DasmX86_64_0f38BA;
	codes0f38[0xBB] = DasmX86_64_0f38BB;
	codes0f38[0xBC] = DasmX86_64_0f38BC;
	codes0f38[0xBD] = DasmX86_64_0f38BD;
	codes0f38[0xBE] = DasmX86_64_0f38BE;
	codes0f38[0xBF] = DasmX86_64_0f38BF;
	codes0f38[0xC0] = DasmX86_64_0f38C0;
	codes0f38[0xC1] = DasmX86_64_0f38C1;
	codes0f38[0xC2] = DasmX86_64_0f38C2;
	codes0f38[0xC3] = DasmX86_64_0f38C3;
	codes0f38[0xC4] = DasmX86_64_0f38C4;
	codes0f38[0xC5] = DasmX86_64_0f38C5;
	codes0f38[0xC6] = DasmX86_64_0f38C6;
	codes0f38[0xC7] = DasmX86_64_0f38C7;
	codes0f38[0xC8] = DasmX86_64_0f38C8;
	codes0f38[0xC9] = DasmX86_64_0f38C9;
	codes0f38[0xCA] = DasmX86_64_0f38CA;
	codes0f38[0xCB] = DasmX86_64_0f38CB;
	codes0f38[0xCC] = DasmX86_64_0f38CC;
	codes0f38[0xCD] = DasmX86_64_0f38CD;
	codes0f38[0xCE] = DasmX86_64_0f38CE;
	codes0f38[0xCF] = DasmX86_64_0f38CF;
	codes0f38[0xD0] = DasmX86_64_0f38D0;
	codes0f38[0xD1] = DasmX86_64_0f38D1;
	codes0f38[0xD2] = DasmX86_64_0f38D2;
	codes0f38[0xD3] = DasmX86_64_0f38D3;
	codes0f38[0xD4] = DasmX86_64_0f38D4;
	codes0f38[0xD5] = DasmX86_64_0f38D5;
	codes0f38[0xD6] = DasmX86_64_0f38D6;
	codes0f38[0xD7] = DasmX86_64_0f38D7;
	codes0f38[0xD8] = DasmX86_64_0f38D8;
	codes0f38[0xD9] = DasmX86_64_0f38D9;
	codes0f38[0xDA] = DasmX86_64_0f38DA;
	codes0f38[0xDB] = DasmX86_64_0f38DB;
	codes0f38[0xDC] = DasmX86_64_0f38DC;
	codes0f38[0xDD] = DasmX86_64_0f38DD;
	codes0f38[0xDE] = DasmX86_64_0f38DE;
	codes0f38[0xDF] = DasmX86_64_0f38DF;
	codes0f38[0xE0] = DasmX86_64_0f38E0;
	codes0f38[0xE1] = DasmX86_64_0f38E1;
	codes0f38[0xE2] = DasmX86_64_0f38E2;
	codes0f38[0xE3] = DasmX86_64_0f38E3;
	codes0f38[0xE4] = DasmX86_64_0f38E4;
	codes0f38[0xE5] = DasmX86_64_0f38E5;
	codes0f38[0xE6] = DasmX86_64_0f38E6;
	codes0f38[0xE7] = DasmX86_64_0f38E7;
	codes0f38[0xE8] = DasmX86_64_0f38E8;
	codes0f38[0xE9] = DasmX86_64_0f38E9;
	codes0f38[0xEA] = DasmX86_64_0f38EA;
	codes0f38[0xEB] = DasmX86_64_0f38EB;
	codes0f38[0xEC] = DasmX86_64_0f38EC;
	codes0f38[0xED] = DasmX86_64_0f38ED;
	codes0f38[0xEE] = DasmX86_64_0f38EE;
	codes0f38[0xEF] = DasmX86_64_0f38EF;
	codes0f38[0xF0] = DasmX86_64_0f38F0;
	codes0f38[0xF1] = DasmX86_64_0f38F1;
	codes0f38[0xF2] = DasmX86_64_0f38F2;
	codes0f38[0xF3] = DasmX86_64_0f38F3;
	codes0f38[0xF4] = DasmX86_64_0f38F4;
	codes0f38[0xF5] = DasmX86_64_0f38F5;
	codes0f38[0xF6] = DasmX86_64_0f38F6;
	codes0f38[0xF7] = DasmX86_64_0f38F7;
	codes0f38[0xF8] = DasmX86_64_0f38F8;
	codes0f38[0xF9] = DasmX86_64_0f38F9;
	codes0f38[0xFA] = DasmX86_64_0f38FA;
	codes0f38[0xFB] = DasmX86_64_0f38FB;
	codes0f38[0xFC] = DasmX86_64_0f38FC;
	codes0f38[0xFD] = DasmX86_64_0f38FD;
	codes0f38[0xFE] = DasmX86_64_0f38FE;
	codes0f38[0xFF] = DasmX86_64_0f38FF;

	codes0f3a[0x00] = DasmX86_64_0f3a00;
	codes0f3a[0x01] = DasmX86_64_0f3a01;
	codes0f3a[0x02] = DasmX86_64_0f3a02;
	codes0f3a[0x03] = DasmX86_64_0f3a03;
	codes0f3a[0x04] = DasmX86_64_0f3a04;
	codes0f3a[0x05] = DasmX86_64_0f3a05;
	codes0f3a[0x06] = DasmX86_64_0f3a06;
	codes0f3a[0x07] = DasmX86_64_0f3a07;
	codes0f3a[0x08] = DasmX86_64_0f3a08;
	codes0f3a[0x09] = DasmX86_64_0f3a09;
	codes0f3a[0x0A] = DasmX86_64_0f3a0A;
	codes0f3a[0x0B] = DasmX86_64_0f3a0B;
	codes0f3a[0x0C] = DasmX86_64_0f3a0C;
	codes0f3a[0x0D] = DasmX86_64_0f3a0D;
	codes0f3a[0x0E] = DasmX86_64_0f3a0E;
	codes0f3a[0x0F] = DasmX86_64_0f3a0F;
	codes0f3a[0x10] = DasmX86_64_0f3a10;
	codes0f3a[0x11] = DasmX86_64_0f3a11;
	codes0f3a[0x12] = DasmX86_64_0f3a12;
	codes0f3a[0x13] = DasmX86_64_0f3a13;
	codes0f3a[0x14] = DasmX86_64_0f3a14;
	codes0f3a[0x15] = DasmX86_64_0f3a15;
	codes0f3a[0x16] = DasmX86_64_0f3a16;
	codes0f3a[0x17] = DasmX86_64_0f3a17;
	codes0f3a[0x18] = DasmX86_64_0f3a18;
	codes0f3a[0x19] = DasmX86_64_0f3a19;
	codes0f3a[0x1A] = DasmX86_64_0f3a1A;
	codes0f3a[0x1B] = DasmX86_64_0f3a1B;
	codes0f3a[0x1C] = DasmX86_64_0f3a1C;
	codes0f3a[0x1D] = DasmX86_64_0f3a1D;
	codes0f3a[0x1E] = DasmX86_64_0f3a1E;
	codes0f3a[0x1F] = DasmX86_64_0f3a1F;
	codes0f3a[0x20] = DasmX86_64_0f3a20;
	codes0f3a[0x21] = DasmX86_64_0f3a21;
	codes0f3a[0x22] = DasmX86_64_0f3a22;
	codes0f3a[0x23] = DasmX86_64_0f3a23;
	codes0f3a[0x24] = DasmX86_64_0f3a24;
	codes0f3a[0x25] = DasmX86_64_0f3a25;
	codes0f3a[0x26] = DasmX86_64_0f3a26;
	codes0f3a[0x27] = DasmX86_64_0f3a27;
	codes0f3a[0x28] = DasmX86_64_0f3a28;
	codes0f3a[0x29] = DasmX86_64_0f3a29;
	codes0f3a[0x2A] = DasmX86_64_0f3a2A;
	codes0f3a[0x2B] = DasmX86_64_0f3a2B;
	codes0f3a[0x2C] = DasmX86_64_0f3a2C;
	codes0f3a[0x2D] = DasmX86_64_0f3a2D;
	codes0f3a[0x2E] = DasmX86_64_0f3a2E;
	codes0f3a[0x2F] = DasmX86_64_0f3a2F;
	codes0f3a[0x30] = DasmX86_64_0f3a30;
	codes0f3a[0x31] = DasmX86_64_0f3a31;
	codes0f3a[0x32] = DasmX86_64_0f3a32;
	codes0f3a[0x33] = DasmX86_64_0f3a33;
	codes0f3a[0x34] = DasmX86_64_0f3a34;
	codes0f3a[0x35] = DasmX86_64_0f3a35;
	codes0f3a[0x36] = DasmX86_64_0f3a36;
	codes0f3a[0x37] = DasmX86_64_0f3a37;
	codes0f3a[0x38] = DasmX86_64_0f3a38;
	codes0f3a[0x39] = DasmX86_64_0f3a39;
	codes0f3a[0x3A] = DasmX86_64_0f3a3A;
	codes0f3a[0x3B] = DasmX86_64_0f3a3B;
	codes0f3a[0x3C] = DasmX86_64_0f3a3C;
	codes0f3a[0x3D] = DasmX86_64_0f3a3D;
	codes0f3a[0x3E] = DasmX86_64_0f3a3E;
	codes0f3a[0x3F] = DasmX86_64_0f3a3F;
	codes0f3a[0x40] = DasmX86_64_0f3a40;
	codes0f3a[0x41] = DasmX86_64_0f3a41;
	codes0f3a[0x42] = DasmX86_64_0f3a42;
	codes0f3a[0x43] = DasmX86_64_0f3a43;
	codes0f3a[0x44] = DasmX86_64_0f3a44;
	codes0f3a[0x45] = DasmX86_64_0f3a45;
	codes0f3a[0x46] = DasmX86_64_0f3a46;
	codes0f3a[0x47] = DasmX86_64_0f3a47;
	codes0f3a[0x48] = DasmX86_64_0f3a48;
	codes0f3a[0x49] = DasmX86_64_0f3a49;
	codes0f3a[0x4A] = DasmX86_64_0f3a4A;
	codes0f3a[0x4B] = DasmX86_64_0f3a4B;
	codes0f3a[0x4C] = DasmX86_64_0f3a4C;
	codes0f3a[0x4D] = DasmX86_64_0f3a4D;
	codes0f3a[0x4E] = DasmX86_64_0f3a4E;
	codes0f3a[0x4F] = DasmX86_64_0f3a4F;
	codes0f3a[0x50] = DasmX86_64_0f3a50;
	codes0f3a[0x51] = DasmX86_64_0f3a51;
	codes0f3a[0x52] = DasmX86_64_0f3a52;
	codes0f3a[0x53] = DasmX86_64_0f3a53;
	codes0f3a[0x54] = DasmX86_64_0f3a54;
	codes0f3a[0x55] = DasmX86_64_0f3a55;
	codes0f3a[0x56] = DasmX86_64_0f3a56;
	codes0f3a[0x57] = DasmX86_64_0f3a57;
	codes0f3a[0x58] = DasmX86_64_0f3a58;
	codes0f3a[0x59] = DasmX86_64_0f3a59;
	codes0f3a[0x5A] = DasmX86_64_0f3a5A;
	codes0f3a[0x5B] = DasmX86_64_0f3a5B;
	codes0f3a[0x5C] = DasmX86_64_0f3a5C;
	codes0f3a[0x5D] = DasmX86_64_0f3a5D;
	codes0f3a[0x5E] = DasmX86_64_0f3a5E;
	codes0f3a[0x5F] = DasmX86_64_0f3a5F;
	codes0f3a[0x60] = DasmX86_64_0f3a60;
	codes0f3a[0x61] = DasmX86_64_0f3a61;
	codes0f3a[0x62] = DasmX86_64_0f3a62;
	codes0f3a[0x63] = DasmX86_64_0f3a63;
	codes0f3a[0x64] = DasmX86_64_0f3a64;
	codes0f3a[0x65] = DasmX86_64_0f3a65;
	codes0f3a[0x66] = DasmX86_64_0f3a66;
	codes0f3a[0x67] = DasmX86_64_0f3a67;
	codes0f3a[0x68] = DasmX86_64_0f3a68;
	codes0f3a[0x69] = DasmX86_64_0f3a69;
	codes0f3a[0x6A] = DasmX86_64_0f3a6A;
	codes0f3a[0x6B] = DasmX86_64_0f3a6B;
	codes0f3a[0x6C] = DasmX86_64_0f3a6C;
	codes0f3a[0x6D] = DasmX86_64_0f3a6D;
	codes0f3a[0x6E] = DasmX86_64_0f3a6E;
	codes0f3a[0x6F] = DasmX86_64_0f3a6F;
	codes0f3a[0x70] = DasmX86_64_0f3a70;
	codes0f3a[0x71] = DasmX86_64_0f3a71;
	codes0f3a[0x72] = DasmX86_64_0f3a72;
	codes0f3a[0x73] = DasmX86_64_0f3a73;
	codes0f3a[0x74] = DasmX86_64_0f3a74;
	codes0f3a[0x75] = DasmX86_64_0f3a75;
	codes0f3a[0x76] = DasmX86_64_0f3a76;
	codes0f3a[0x77] = DasmX86_64_0f3a77;
	codes0f3a[0x78] = DasmX86_64_0f3a78;
	codes0f3a[0x79] = DasmX86_64_0f3a79;
	codes0f3a[0x7A] = DasmX86_64_0f3a7A;
	codes0f3a[0x7B] = DasmX86_64_0f3a7B;
	codes0f3a[0x7C] = DasmX86_64_0f3a7C;
	codes0f3a[0x7D] = DasmX86_64_0f3a7D;
	codes0f3a[0x7E] = DasmX86_64_0f3a7E;
	codes0f3a[0x7F] = DasmX86_64_0f3a7F;
	codes0f3a[0x80] = DasmX86_64_0f3a80;
	codes0f3a[0x81] = DasmX86_64_0f3a81;
	codes0f3a[0x82] = DasmX86_64_0f3a82;
	codes0f3a[0x83] = DasmX86_64_0f3a83;
	codes0f3a[0x84] = DasmX86_64_0f3a84;
	codes0f3a[0x85] = DasmX86_64_0f3a85;
	codes0f3a[0x86] = DasmX86_64_0f3a86;
	codes0f3a[0x87] = DasmX86_64_0f3a87;
	codes0f3a[0x88] = DasmX86_64_0f3a88;
	codes0f3a[0x89] = DasmX86_64_0f3a89;
	codes0f3a[0x8A] = DasmX86_64_0f3a8A;
	codes0f3a[0x8B] = DasmX86_64_0f3a8B;
	codes0f3a[0x8C] = DasmX86_64_0f3a8C;
	codes0f3a[0x8D] = DasmX86_64_0f3a8D;
	codes0f3a[0x8E] = DasmX86_64_0f3a8E;
	codes0f3a[0x8F] = DasmX86_64_0f3a8F;
	codes0f3a[0x90] = DasmX86_64_0f3a90;
	codes0f3a[0x91] = DasmX86_64_0f3a91;
	codes0f3a[0x92] = DasmX86_64_0f3a92;
	codes0f3a[0x93] = DasmX86_64_0f3a93;
	codes0f3a[0x94] = DasmX86_64_0f3a94;
	codes0f3a[0x95] = DasmX86_64_0f3a95;
	codes0f3a[0x96] = DasmX86_64_0f3a96;
	codes0f3a[0x97] = DasmX86_64_0f3a97;
	codes0f3a[0x98] = DasmX86_64_0f3a98;
	codes0f3a[0x99] = DasmX86_64_0f3a99;
	codes0f3a[0x9A] = DasmX86_64_0f3a9A;
	codes0f3a[0x9B] = DasmX86_64_0f3a9B;
	codes0f3a[0x9C] = DasmX86_64_0f3a9C;
	codes0f3a[0x9D] = DasmX86_64_0f3a9D;
	codes0f3a[0x9E] = DasmX86_64_0f3a9E;
	codes0f3a[0x9F] = DasmX86_64_0f3a9F;
	codes0f3a[0xA0] = DasmX86_64_0f3aA0;
	codes0f3a[0xA1] = DasmX86_64_0f3aA1;
	codes0f3a[0xA2] = DasmX86_64_0f3aA2;
	codes0f3a[0xA3] = DasmX86_64_0f3aA3;
	codes0f3a[0xA4] = DasmX86_64_0f3aA4;
	codes0f3a[0xA5] = DasmX86_64_0f3aA5;
	codes0f3a[0xA6] = DasmX86_64_0f3aA6;
	codes0f3a[0xA7] = DasmX86_64_0f3aA7;
	codes0f3a[0xA8] = DasmX86_64_0f3aA8;
	codes0f3a[0xA9] = DasmX86_64_0f3aA9;
	codes0f3a[0xAA] = DasmX86_64_0f3aAA;
	codes0f3a[0xAB] = DasmX86_64_0f3aAB;
	codes0f3a[0xAC] = DasmX86_64_0f3aAC;
	codes0f3a[0xAD] = DasmX86_64_0f3aAD;
	codes0f3a[0xAE] = DasmX86_64_0f3aAE;
	codes0f3a[0xAF] = DasmX86_64_0f3aAF;
	codes0f3a[0xB0] = DasmX86_64_0f3aB0;
	codes0f3a[0xB1] = DasmX86_64_0f3aB1;
	codes0f3a[0xB2] = DasmX86_64_0f3aB2;
	codes0f3a[0xB3] = DasmX86_64_0f3aB3;
	codes0f3a[0xB4] = DasmX86_64_0f3aB4;
	codes0f3a[0xB5] = DasmX86_64_0f3aB5;
	codes0f3a[0xB6] = DasmX86_64_0f3aB6;
	codes0f3a[0xB7] = DasmX86_64_0f3aB7;
	codes0f3a[0xB8] = DasmX86_64_0f3aB8;
	codes0f3a[0xB9] = DasmX86_64_0f3aB9;
	codes0f3a[0xBA] = DasmX86_64_0f3aBA;
	codes0f3a[0xBB] = DasmX86_64_0f3aBB;
	codes0f3a[0xBC] = DasmX86_64_0f3aBC;
	codes0f3a[0xBD] = DasmX86_64_0f3aBD;
	codes0f3a[0xBE] = DasmX86_64_0f3aBE;
	codes0f3a[0xBF] = DasmX86_64_0f3aBF;
	codes0f3a[0xC0] = DasmX86_64_0f3aC0;
	codes0f3a[0xC1] = DasmX86_64_0f3aC1;
	codes0f3a[0xC2] = DasmX86_64_0f3aC2;
	codes0f3a[0xC3] = DasmX86_64_0f3aC3;
	codes0f3a[0xC4] = DasmX86_64_0f3aC4;
	codes0f3a[0xC5] = DasmX86_64_0f3aC5;
	codes0f3a[0xC6] = DasmX86_64_0f3aC6;
	codes0f3a[0xC7] = DasmX86_64_0f3aC7;
	codes0f3a[0xC8] = DasmX86_64_0f3aC8;
	codes0f3a[0xC9] = DasmX86_64_0f3aC9;
	codes0f3a[0xCA] = DasmX86_64_0f3aCA;
	codes0f3a[0xCB] = DasmX86_64_0f3aCB;
	codes0f3a[0xCC] = DasmX86_64_0f3aCC;
	codes0f3a[0xCD] = DasmX86_64_0f3aCD;
	codes0f3a[0xCE] = DasmX86_64_0f3aCE;
	codes0f3a[0xCF] = DasmX86_64_0f3aCF;
	codes0f3a[0xD0] = DasmX86_64_0f3aD0;
	codes0f3a[0xD1] = DasmX86_64_0f3aD1;
	codes0f3a[0xD2] = DasmX86_64_0f3aD2;
	codes0f3a[0xD3] = DasmX86_64_0f3aD3;
	codes0f3a[0xD4] = DasmX86_64_0f3aD4;
	codes0f3a[0xD5] = DasmX86_64_0f3aD5;
	codes0f3a[0xD6] = DasmX86_64_0f3aD6;
	codes0f3a[0xD7] = DasmX86_64_0f3aD7;
	codes0f3a[0xD8] = DasmX86_64_0f3aD8;
	codes0f3a[0xD9] = DasmX86_64_0f3aD9;
	codes0f3a[0xDA] = DasmX86_64_0f3aDA;
	codes0f3a[0xDB] = DasmX86_64_0f3aDB;
	codes0f3a[0xDC] = DasmX86_64_0f3aDC;
	codes0f3a[0xDD] = DasmX86_64_0f3aDD;
	codes0f3a[0xDE] = DasmX86_64_0f3aDE;
	codes0f3a[0xDF] = DasmX86_64_0f3aDF;
	codes0f3a[0xE0] = DasmX86_64_0f3aE0;
	codes0f3a[0xE1] = DasmX86_64_0f3aE1;
	codes0f3a[0xE2] = DasmX86_64_0f3aE2;
	codes0f3a[0xE3] = DasmX86_64_0f3aE3;
	codes0f3a[0xE4] = DasmX86_64_0f3aE4;
	codes0f3a[0xE5] = DasmX86_64_0f3aE5;
	codes0f3a[0xE6] = DasmX86_64_0f3aE6;
	codes0f3a[0xE7] = DasmX86_64_0f3aE7;
	codes0f3a[0xE8] = DasmX86_64_0f3aE8;
	codes0f3a[0xE9] = DasmX86_64_0f3aE9;
	codes0f3a[0xEA] = DasmX86_64_0f3aEA;
	codes0f3a[0xEB] = DasmX86_64_0f3aEB;
	codes0f3a[0xEC] = DasmX86_64_0f3aEC;
	codes0f3a[0xED] = DasmX86_64_0f3aED;
	codes0f3a[0xEE] = DasmX86_64_0f3aEE;
	codes0f3a[0xEF] = DasmX86_64_0f3aEF;
	codes0f3a[0xF0] = DasmX86_64_0f3aF0;
	codes0f3a[0xF1] = DasmX86_64_0f3aF1;
	codes0f3a[0xF2] = DasmX86_64_0f3aF2;
	codes0f3a[0xF3] = DasmX86_64_0f3aF3;
	codes0f3a[0xF4] = DasmX86_64_0f3aF4;
	codes0f3a[0xF5] = DasmX86_64_0f3aF5;
	codes0f3a[0xF6] = DasmX86_64_0f3aF6;
	codes0f3a[0xF7] = DasmX86_64_0f3aF7;
	codes0f3a[0xF8] = DasmX86_64_0f3aF8;
	codes0f3a[0xF9] = DasmX86_64_0f3aF9;
	codes0f3a[0xFA] = DasmX86_64_0f3aFA;
	codes0f3a[0xFB] = DasmX86_64_0f3aFB;
	codes0f3a[0xFC] = DasmX86_64_0f3aFC;
	codes0f3a[0xFD] = DasmX86_64_0f3aFD;
	codes0f3a[0xFE] = DasmX86_64_0f3aFE;
	codes0f3a[0xFF] = DasmX86_64_0f3aFF;
}

Manage::DasmX86_64::~DasmX86_64()
{
	MemFree(this->codes);
	MemFree(this->codes0f);
	MemFree(this->codes0f38);
	MemFree(this->codes0f3a);
}

const UTF8Char *Manage::DasmX86_64::GetHeader(Bool fullRegs)
{
	if (fullRegs)
	{
		return (const UTF8Char*)" Rsp              Rbp              Rip              Rax              Rdx              Rcx              Rbx              Rsi              Rdi              R8               R9               R10              R11              R12              R13              R14              R15              Code";
	}
	else
	{
		return (const UTF8Char*)" Rsp              Rbp              Rip              Code";
	}
}

Bool Manage::DasmX86_64::Disasm64(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt64 *currRip, UInt64 *currRsp, UInt64 *currRbp, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs)
{
	UTF8Char sbuff[512];
	DasmX86_64_Sess sess;
	Text::StringBuilderUTF8 *outStr;
	UOSInt initJmpCnt = jmpAddrs->GetCount();
	NEW_CLASS(outStr, Text::StringBuilderUTF8());
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	MemCopyNO(&sess.regs, regs, sizeof(Manage::DasmX86_64::DasmX86_64_Regs));
	sess.regs.rip = *currRip;
	sess.regs.rsp = *currRsp;
	sess.regs.rbp = *currRbp;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmX86_64::ET_NOT_END;
	sess.rspOfst = 0;
	sess.lastStatus = 2;
	sess.thisStatus = 0;
	sess.codes = (void**)this->codes;
	sess.codes0f = (void**)this->codes0f;
	sess.stabesp = sess.regs.rsp;
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	*blockStart = sess.regs.rip;

	while (true)
	{
		UInt8 buff[16];
		Bool ret;

		outStr->ClearStr();
		outStr->AppendHex64(sess.regs.rsp);
		outStr->AppendC(UTF8STRC(" "));
		outStr->AppendHex64(sess.regs.rbp);
		outStr->AppendC(UTF8STRC(" "));
		outStr->AppendHex64(sess.regs.rip);
		outStr->AppendC(UTF8STRC(" "));
		if (fullRegs)
		{
			outStr->AppendHex64(sess.regs.rax);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.rdx);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.rcx);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.rbx);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.rsi);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.rdi);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r8);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r9);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r10);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r11);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r12);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r13);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r14);
			outStr->AppendC(UTF8STRC(" "));
			outStr->AppendHex64(sess.regs.r15);
			outStr->AppendC(UTF8STRC(" "));
		}
		sess.sbuff = sbuff;
		if (sess.memReader->ReadMemory(sess.regs.rip, buff, 1) == 0)
		{
			ret = false;
		}
		else
		{
			ret = this->codes[buff[0]](&sess);
		}
		if (!ret)
		{
			UOSInt buffSize;
			outStr->AppendC(UTF8STRC("Unknown opcode "));
			buffSize = sess.memReader->ReadMemory(sess.regs.rip, buff, 16);
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
		if (sess.endType == Manage::DasmX86_64::ET_JMP && (UInt64)sess.retAddr >= *blockStart && (UInt64)sess.retAddr <= sess.regs.rip)
		{
			UOSInt i;
			UInt64 minAddr = 0xffffffffffffffffLL;
			UInt64 jmpAddr;
			i = jmpAddrs->GetCount();
			while (i-- > initJmpCnt)
			{
				jmpAddr = jmpAddrs->GetItem(i);
				if (jmpAddr >= sess.regs.rip && jmpAddr < minAddr)
				{
					minAddr = jmpAddr;
				}
			}
			if (minAddr - sess.regs.rip > 0x1000)
			{
				*currRip = sess.retAddr;
				*currRsp = sess.regs.rsp;
				*currRbp = sess.regs.rbp;
				*blockEnd = sess.regs.rip;
				DEL_CLASS(outStr);
				MemCopyNO(regs, &sess.regs, sizeof(Manage::DasmX86_64::DasmX86_64_Regs));
				return false;
			}
			sess.regs.rip = minAddr;
			sess.endType = Manage::DasmX86_64::ET_NOT_END;
		}
		else if (sess.endType != Manage::DasmX86_64::ET_NOT_END)
		{
			*currRip = sess.retAddr;
			*currRsp = sess.regs.rsp;
			*currRbp = sess.regs.rbp;
			*blockEnd = sess.regs.rip;
			DEL_CLASS(outStr);
			MemCopyNO(regs, &sess.regs, sizeof(Manage::DasmX86_64::DasmX86_64_Regs));
			return sess.endType != Manage::DasmX86_64::ET_EXIT;
		}
		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = 0;
	}
}

Bool Manage::DasmX86_64::Disasm64In(Text::StringBuilderUTF8 *outStr, Manage::AddressResolver *addrResol, UInt64 *currRip, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, Manage::IMemoryReader *memReader)
{
	UTF8Char sbuff[256];
	UInt64 initIP = *currRip;
	DasmX86_64_Sess sess;
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	sess.regs.rip = *currRip;
	sess.regs.rsp = 0;
	sess.regs.rbp = 0;
	sess.regs.rax = 0;
	sess.regs.rbx = 0;
	sess.regs.rcx = 0;
	sess.regs.rdx = 0;
	sess.regs.rsi = 0;
	sess.regs.rdi = 0;
	sess.regs.r8 = 0;
	sess.regs.r9 = 0;
	sess.regs.r10 = 0;
	sess.regs.r11 = 0;
	sess.regs.r12 = 0;
	sess.regs.r13 = 0;
	sess.regs.r14 = 0;
	sess.regs.r15 = 0;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmX86_64::ET_NOT_END;
	sess.rspOfst = 0;
	sess.lastStatus = 0;
	sess.thisStatus = (Int32)0x80000000;
	sess.codes = (void**)this->codes;
	sess.codes0f = (void**)this->codes0f;
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	*blockStart = sess.regs.rip;

	while (sess.memReader->ReadMemUInt8(sess.regs.rip) == 0xe9)
	{
		outStr->AppendHex64(sess.regs.rip);
		outStr->AppendC(UTF8STRC(" "));
		sess.sbuff = sbuff;
		Bool ret = this->codes[sess.memReader->ReadMemUInt8(sess.regs.rip)](&sess);
		if (!ret)
			break;
		outStr->AppendSlow(sbuff);
		sess.regs.rip = sess.retAddr;
		*blockStart = sess.retAddr;
		sess.endType = Manage::DasmX86_64::ET_NOT_END;
	}

	IO::ConsoleWriter console;
	while (true)
	{
		UInt64 oriip = sess.regs.rip;

		outStr->AppendHex64(sess.regs.rip);
		outStr->AppendC(UTF8STRC(" "));
		sess.sbuff = sbuff;
		Bool ret = this->codes[sess.memReader->ReadMemUInt8(sess.regs.rip)](&sess);
		console.WriteStr(sbuff);
		outStr->AppendSlow(sbuff);
		if (!ret)
		{
			UInt8 buff[256];
			UOSInt buffSize;
			buffSize = sess.memReader->ReadMemory(oriip, buff, 16);
			outStr->AppendC(UTF8STRC("Unknown opcode "));
			if (buffSize > 0)
			{
				outStr->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::None);
			}
			outStr->AppendC(UTF8STRC("\r\n"));
			buffSize = sess.memReader->ReadMemory(initIP, buff, 256);
			if (buffSize > 0)
			{
				outStr->AppendC(UTF8STRC("Inst Buff:\r\n"));
				outStr->AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
				outStr->AppendC(UTF8STRC("\r\n"));
			}
			return false;
		}
		if (sess.endType != Manage::DasmX86_64::ET_NOT_END)
		{
			*currRip = sess.retAddr;
			*blockEnd = sess.regs.rip;
			return true;
		}
		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = (Int32)0x80000000;
	}
}

Manage::Dasm::Dasm_Regs *Manage::DasmX86_64::CreateRegs()
{
	DasmX86_64_Regs *regs = MemAlloc(DasmX86_64_Regs, 1);
	return regs;
}

void Manage::DasmX86_64::FreeRegs(Dasm_Regs *regs)
{
	MemFree(regs);
}

void *Manage::DasmX86_64::StartDasm(Manage::AddressResolver *addrResol, void *addr, Manage::IMemoryReader *memReader)
{
	DasmX86_64_Sess *sess;
	sess = MemAlloc(DasmX86_64_Sess, 1);
	NEW_CLASS(sess->callAddrs, Data::ArrayListUInt64());
	NEW_CLASS(sess->jmpAddrs, Data::ArrayListUInt64());
	sess->regs.rip = (UOSInt)addr;
	sess->regs.rsp = 0;
	sess->regs.rbp = 0;
	sess->regs.rax = 0;
	sess->regs.rbx = 0;
	sess->regs.rcx = 0;
	sess->regs.rdx = 0;
	sess->regs.rsi = 0;
	sess->regs.rdi = 0;
	sess->regs.r8 = 0;
	sess->regs.r9 = 0;
	sess->regs.r10 = 0;
	sess->regs.r11 = 0;
	sess->regs.r12 = 0;
	sess->regs.r13 = 0;
	sess->regs.r14 = 0;
	sess->regs.r15 = 0;
//	sess->outStr = 0;
	sess->endType = Manage::DasmX86_64::ET_NOT_END;
	sess->rspOfst = 0;
	sess->lastStatus = 2;
	sess->thisStatus = (Int32)0x80000000;
	sess->codes = (void**)this->codes;
	sess->codes0f = (void**)this->codes0f;
	sess->stabesp = 0;
	sess->addrResol = addrResol;
	sess->memReader = memReader;
	return sess;
}

void Manage::DasmX86_64::EndDasm(void *sess)
{
	DasmX86_64_Sess *ses = (DasmX86_64_Sess*)sess;
	DEL_CLASS(ses->callAddrs);
	DEL_CLASS(ses->jmpAddrs);
	MemFree(ses);
}

UTF8Char *Manage::DasmX86_64::DasmNext(void *sess, UTF8Char *buff)
{
	DasmX86_64_Sess *ses = (DasmX86_64_Sess*)sess;
	if (ses->endType != Manage::DasmX86_64::ET_NOT_END)
		return 0;
	ses->sbuff = buff;
	Bool ret = this->codes[ses->memReader->ReadMemUInt8(ses->regs.rip)](ses);
	UTF8Char *sptr = ses->sbuff;
	if (!ret)
	{
		UInt8 cbuff[16];
		UOSInt buffSize;
		sptr = Text::StrConcatC(buff, UTF8STRC("Unknown opcode "));
		buffSize = ses->memReader->ReadMemory(ses->regs.rip, cbuff, 16);
		if (buffSize > 0)
		{
			sptr = Text::StrHexBytes(sptr, cbuff, buffSize, ' ');
		}
		ses->endType = Manage::DasmX86_64::ET_INV_OP;
		return sptr;
	}
	if (ses->endType != Manage::DasmX86_64::ET_NOT_END)
	{
/*		*currEip = sess.retAddr;
		*currEsp = (Int32)sess.cesp;
		*currEbp = (Int32)sess.cebp;
		*blockEnd = (Int32)sess.ceip;*/
		return sptr;
	}
	ses->lastStatus = ses->thisStatus;
	ses->thisStatus = ses->thisStatus & (Int32)0x80000000;
	return sptr;
}

OSInt Manage::DasmX86_64::SessGetCodeOffset(void *sess)
{
	DasmX86_64_Sess *ses = (DasmX86_64_Sess*)sess;
	return (OSInt)ses->regs.rip;
}

Manage::DasmX86_64::EndType Manage::DasmX86_64::SessGetEndType(void *sess)
{
	DasmX86_64_Sess *ses = (DasmX86_64_Sess*)sess;
	return ses->endType;
}

Bool Manage::DasmX86_64::SessContJmp(void *sess)
{
	DasmX86_64_Sess *ses = (DasmX86_64_Sess*)sess;
	if (ses->endType == Manage::DasmX86_64::ET_JMP)
	{
		ses->endType = Manage::DasmX86_64::ET_NOT_END;
		ses->regs.rip = ses->retAddr;
		return true;
	}
	else
	{
		return false;
	}
}
