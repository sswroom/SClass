#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Manage/DasmX86_32.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#if defined(_MSC_VER)
#pragma warning( disable : 4311 4312)
#endif

static const UTF8Char *DasmX86_32_Segms[] = {(const UTF8Char*)"cs:", (const UTF8Char*)"ds:", (const UTF8Char*)"es:", (const UTF8Char*)"fs:", (const UTF8Char*)"gs:", (const UTF8Char*)"ss:"};

Bool DasmX86_32_IsEndFunc(UnsafeArray<const UTF8Char> funcName)
{
	if (Text::StrEndsWith(funcName, (const UTF8Char*)"ExitThread"))
	{
		return true;
	}
	else if (Text::StrEndsWith(funcName, (const UTF8Char*)"ExitProcess"))
	{
		return true;
	}
	else if (Text::StrEndsWith(funcName, (const UTF8Char*)"RtlExitUserThread"))
	{
		return true;
	}
	return false;
}

UnsafeArray<UTF8Char> DasmX86_32_ParseReg8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo, UInt8 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = (UInt8*)&sess->regs.EAX;
		return Text::StrConcatC(regName, UTF8STRC("AL"));
	case 1:
		*regPtr = (UInt8*)&sess->regs.ECX;
		return Text::StrConcatC(regName, UTF8STRC("CL"));
	case 2:
		*regPtr = (UInt8*)&sess->regs.EDX;
		return Text::StrConcatC(regName, UTF8STRC("DL"));
	case 3:
		*regPtr = (UInt8*)&sess->regs.EBX;
		return Text::StrConcatC(regName, UTF8STRC("BL"));
	case 4:
		*regPtr = 1 + (UInt8*)&sess->regs.EAX;
		return Text::StrConcatC(regName, UTF8STRC("AH"));
	case 5:
		*regPtr = 1 + (UInt8*)&sess->regs.ECX;
		return Text::StrConcatC(regName, UTF8STRC("CH"));
	case 6:
		*regPtr = 1 + (UInt8*)&sess->regs.EDX;
		return Text::StrConcatC(regName, UTF8STRC("DH"));
	case 7:
		*regPtr = 1 + (UInt8*)&sess->regs.EBX;
		return Text::StrConcatC(regName, UTF8STRC("BH"));
	default:
		*regName = 0;
		return regName;
	}
}

UnsafeArray<UTF8Char> DasmX86_32_ParseReg16(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo, UInt32 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = &sess->regs.EAX;
		return Text::StrConcatC(regName, UTF8STRC("AX"));
	case 1:
		*regPtr = &sess->regs.ECX;
		return Text::StrConcatC(regName, UTF8STRC("CX"));
	case 2:
		*regPtr = &sess->regs.EDX;
		return Text::StrConcatC(regName, UTF8STRC("DX"));
	case 3:
		*regPtr = &sess->regs.EBX;
		return Text::StrConcatC(regName, UTF8STRC("BX"));
	case 4:
		*regPtr = &sess->regs.ESP;
		return Text::StrConcatC(regName, UTF8STRC("SP"));
	case 5:
		*regPtr = &sess->regs.EBP;
		return Text::StrConcatC(regName, UTF8STRC("BP"));
	case 6:
		*regPtr = &sess->regs.ESI;
		return Text::StrConcatC(regName, UTF8STRC("SI"));
	case 7:
		*regPtr = &sess->regs.EDI;
		return Text::StrConcatC(regName, UTF8STRC("DI"));
	default:
		*regName = 0;
		return regName;
	}
}

UnsafeArray<UTF8Char> DasmX86_32_ParseReg32(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo, UInt32 **regPtr)
{
	switch (regNo)
	{
	case 0:
		*regPtr = &sess->regs.EAX;
		return Text::StrConcatC(regName, UTF8STRC("EAX"));
	case 1:
		*regPtr = &sess->regs.ECX;
		return Text::StrConcatC(regName, UTF8STRC("ECX"));
	case 2:
		*regPtr = &sess->regs.EDX;
		return Text::StrConcatC(regName, UTF8STRC("EDX"));
	case 3:
		*regPtr = &sess->regs.EBX;
		return Text::StrConcatC(regName, UTF8STRC("EBX"));
	case 4:
		*regPtr = &sess->regs.ESP;
		return Text::StrConcatC(regName, UTF8STRC("ESP"));
	case 5:
		*regPtr = &sess->regs.EBP;
		return Text::StrConcatC(regName, UTF8STRC("EBP"));
	case 6:
		*regPtr = &sess->regs.ESI;
		return Text::StrConcatC(regName, UTF8STRC("ESI"));
	case 7:
		*regPtr = &sess->regs.EDI;
		return Text::StrConcatC(regName, UTF8STRC("EDI"));
	default:
		*regName = 0;
		return regName;
	}
}

UnsafeArray<UTF8Char> DasmX86_32_ParseSReg(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo)
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

UnsafeArray<UTF8Char> DasmX86_32_ParseReg(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> regName, Int32 regNo, UInt32 **regPtr)
{
	if (sess->thisStatus & 1)
	{
		return DasmX86_32_ParseReg16(sess, regName, regNo, regPtr);
	}
	else
	{
		return DasmX86_32_ParseReg32(sess, regName, regNo, regPtr);
	}
}

UnsafeArray<UTF8Char> DasmX86_32_ParseRegx87(UnsafeArray<UTF8Char> regName, Int32 regNo)
{
	switch (regNo)
	{
	case 0:
		return Text::StrConcatC(regName, UTF8STRC("ST(0)"));
	case 1:
		return Text::StrConcatC(regName, UTF8STRC("ST(1)"));
	case 2:
		return Text::StrConcatC(regName, UTF8STRC("ST(2)"));
	case 3:
		return Text::StrConcatC(regName, UTF8STRC("ST(3)"));
	case 4:
		return Text::StrConcatC(regName, UTF8STRC("ST(4)"));
	case 5:
		return Text::StrConcatC(regName, UTF8STRC("ST(5)"));
	case 6:
		return Text::StrConcatC(regName, UTF8STRC("ST(6)"));
	case 7:
		return Text::StrConcatC(regName, UTF8STRC("ST(7)"));
	default:
		*regName = 0;
		return regName;
	}
}

UnsafeArray<UTF8Char> DasmX86_32_ParseRegMM(UnsafeArray<UTF8Char> regName, Int32 regNo)
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

UnsafeArray<UTF8Char> DasmX86_32_ParseRegXMM(UnsafeArray<UTF8Char> regName, Int32 regNo)
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
	default:
		*regName = 0;
		return regName;
	}
}

UnsafeArray<UTF8Char> DasmX86_32_ParseRegCR(UnsafeArray<UTF8Char> regName, Int32 regNo)
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

UnsafeArray<UTF8Char> DasmX86_32_ParseRegDR(UnsafeArray<UTF8Char> regName, Int32 regNo)
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

void DasmX86_32_ParseSIB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Bool allowEbp, UInt32 *memAddr)
{
	UInt8 sib = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	UInt8 sibi = (sib >> 3) & 7;
	UInt8 sibs = (UInt8)(sib >> 6);
	UInt8 sibb = sib & 7;
	Int32 baseReg = 8;
	Int32 indexReg = 8;
	UInt32 addr = 0;
	Int32 indexMul = sibs;
	UInt32 srcAddr;
	UInt32 *regPtr;
	sess->regs.EIP += 1;

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
			addr = (UInt32)(Int32)(Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP);
			baseReg = 5;
			sess->regs.EIP += 1;
		}
		else if (sibs == 2)
		{
			if (allowEbp)
			{
				baseReg = 5;
			}
			else
			{
				addr = sess->memReader->ReadMemUInt32(sess->regs.EIP);
				sess->regs.EIP += 4;
			}
		}
		else if (sibs == 3)
		{
			addr = sess->memReader->ReadMemUInt32(sess->regs.EIP);
			sess->regs.EIP += 4;
		}
	}
	else
	{
		baseReg = sibb;
	}

	if (baseReg != 8)
	{
		memName = DasmX86_32_ParseReg32(sess, memName, baseReg, &regPtr);
		srcAddr = *regPtr;
		if (indexReg != 8)
		{
			*memName++ = '+';
			memName = DasmX86_32_ParseReg32(sess, memName, indexReg, &regPtr);
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
		memName = DasmX86_32_ParseReg32(sess, memName, indexReg, &regPtr);
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
		if ((Int32)addr < 0)
		{
			memName = Text::StrInt32(memName, (Int32)addr);
		}
		else if (addr > 1024)
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+0x"));
			memName = Text::StrHexVal32(memName, addr);
		}
		else
		{
			*memName++ = '+';
			memName = Text::StrUInt32(memName, addr);
		}
		srcAddr += addr;
	}
	*memName = 0;
	if (memAddr)
		*memAddr = srcAddr;
}

void DasmX86_32_ParseAddr32(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg, UInt32 *memAddr)
{
	UTF8Char mem[64];
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	UInt32 addr;
	UInt32 srcAddr;
	UInt32 *regPtr;
	*reg = (b >> 3) & 7;
	switch (b >> 6)
	{
	case 0:
		if ((b & 7) == 5)
		{
			if (sess->thisStatus & 0x10000)
				memName = Text::StrConcat(memName, DasmX86_32_Segms[sess->segmId]);
			srcAddr = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
			memName = Text::StrConcatC(memName, UTF8STRC("[0x"));
			memName = Text::StrHexVal32(memName, srcAddr);
			*memName++ = ']';
			*memName = 0;
			sess->regs.EIP += 5;
			if (memAddr)
				*memAddr = srcAddr;
		}
		else if ((b & 7) == 4)
		{
			sess->regs.EIP += 1;

			if (sess->thisStatus & 0x10000)
				memName = Text::StrConcat(memName, DasmX86_32_Segms[sess->segmId]);
			*memName++ = '[';
			DasmX86_32_ParseSIB(sess, mem, false, &srcAddr);
			memName = Text::StrConcat(memName, mem);
			*memName++ = ']';
			*memName = 0;
			if (memAddr)
				*memAddr = srcAddr;
		}
		else
		{
			if (sess->thisStatus & 0x10000)
				memName = Text::StrConcat(memName, DasmX86_32_Segms[sess->segmId]);
			memName = Text::StrConcatC(memName, UTF8STRC("["));
			memName = DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
			*memName++ = ']';
			*memName = 0;
			sess->regs.EIP += 1;
			if (memAddr)
			{
				*memAddr = *regPtr;
			}
		}

		break;
	case 1:
		if (sess->thisStatus & 0x10000)
			memName = Text::StrConcat(memName, DasmX86_32_Segms[sess->segmId]);
		memName = Text::StrConcatC(memName, UTF8STRC("["));
		sess->regs.EIP += 1;
		if ((b & 7) == 4)
		{
			DasmX86_32_ParseSIB(sess, mem, true, &srcAddr);
			memName = Text::StrConcat(memName, mem);
		}
		else
		{
			memName = DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
			srcAddr = *regPtr;
		}
		if (sess->memReader->ReadMemUInt8(sess->regs.EIP) & 0x80)
		{
			*memName++ = '-';
			memName = Text::StrInt32(memName, -(Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		}
		else
		{
			*memName++ = '+';
			memName = Text::StrInt32(memName, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		}
		if (memAddr)
		{
			srcAddr += (UInt32)(Int32)(Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP);
			*memAddr = srcAddr;
		}
		*memName++ = ']';
		*memName = 0;
		sess->regs.EIP += 1;
		break;
	case 2:
		if (sess->thisStatus & 0x10000)
			memName = Text::StrConcat(memName, DasmX86_32_Segms[sess->segmId]);
		memName = Text::StrConcatC(memName, UTF8STRC("["));
		sess->regs.EIP += 1;
		if ((b & 7) == 4)
		{
			DasmX86_32_ParseSIB(sess, mem, true, &srcAddr);
			memName = Text::StrConcat(memName, mem);
		}
		else
		{
			memName = DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
			srcAddr = *regPtr;
		}
		addr = sess->memReader->ReadMemUInt32(sess->regs.EIP);
		srcAddr += addr;
		if ((addr & 0xffff0000) == 0xffff0000)
		{
			memName = Text::StrInt32(memName, (Int32)addr);
		}
		else if (addr < 1024)
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+"));
			memName = Text::StrInt32(memName, (Int32)addr);
		}
		else
		{
			memName = Text::StrConcatC(memName, UTF8STRC("+0x"));
			memName = Text::StrHexVal32(memName, addr);
		}
		if (memAddr)
		{
			*memAddr = srcAddr;
		}
		*memName++ = ']';
		*memName = 0;
		sess->regs.EIP += 4;
		break;
	case 3:
		break;
	}
}

void DasmX86_32_ParseModR32M8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg, UInt32 *memVal)
{
	UInt32 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.EIP += 1;
		if (memVal)
		{
			*memVal = *regPtr;
		}
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("BYTE PTR ")), reg, &addr);
	}
}

void DasmX86_32_ParseModRM8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg, UInt8 *memVal)
{
	UInt8 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg8(sess, memName, b & 7, &regPtr);
		if (memVal)
		{
			*memVal = *regPtr;
		}
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("BYTE PTR ")), reg, &addr);
		if (memVal)
		{
			*memVal = sess->memReader->ReadMemUInt8(addr);
		}
	}
}

void DasmX86_32_ParseModR32M16(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg, UInt32 *memVal)
{
	UInt32 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
		if (memVal)
		{
			*memVal = *regPtr;
		}
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("WORD PTR ")), reg, &addr);
		if (memVal)
		{
			*memVal = sess->memReader->ReadMemUInt16(addr);
		}
	}
}

void DasmX86_32_ParseModRM16(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg, UInt16 *memVal)
{
	UInt32 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg16(sess, memName, b & 7, &regPtr);
		if (memVal)
		{
			*memVal = *(UInt16*)regPtr;
		}
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("WORD PTR ")), reg, &addr);
		if (memVal)
		{
			*memVal = sess->memReader->ReadMemUInt16(addr);
		}
	}
}

void DasmX86_32_ParseModRM32(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg, UInt32 *memVal, UInt32 *memAddr)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v;
		DasmX86_32_ParseModRM16(sess, memName, reg, &v);
		if (memVal)
			*memVal = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
		if ((b >> 6) == 3)
		{
			*reg = (b >> 3) & 7;
			DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
			if (memVal)
			{
				*memVal = *regPtr;
			}
			sess->regs.EIP += 1;
		}
		else
		{
			UInt32 addr;
			DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("DWORD PTR ")), reg, &addr);
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

void DasmX86_32_ParseModRM48(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg)
{
	UInt32 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("FWORD PTR ")), reg, &addr);
	}
}

void DasmX86_32_ParseModRM64(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg)
{
	UInt32 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("QWORD PTR ")), reg, &addr);
	}
}

void DasmX86_32_ParseModRMR80(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseRegx87(memName, b & 7);
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("REAL10 PTR ")), reg, &addr);
	}
}

void DasmX86_32_ParseModRM128(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg)
{
	UInt32 *regPtr;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseReg32(sess, memName, b & 7, &regPtr);
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("")), reg, &addr);
	}
}

void DasmX86_32_ParseModRMMM64(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseRegMM(memName, b & 7);
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("MMWORD PTR ")), reg, &addr);
	}
}

void DasmX86_32_ParseModRMXMM128(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> memName, Int32 *reg)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	if ((b >> 6) == 3)
	{
		*reg = (b >> 3) & 7;
		DasmX86_32_ParseRegXMM(memName, b & 7);
		sess->regs.EIP += 1;
	}
	else
	{
		UInt32 addr;
		DasmX86_32_ParseAddr32(sess, Text::StrConcatC(memName, UTF8STRC("XMMWORD PTR ")), reg, &addr);
	}
}

Int32 __stdcall DasmX86_32_GetFuncStack(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess, UInt32 funcAddr, UInt32 *outEsp)
{
	Manage::DasmX86_32::DasmX86_32_Sess tmpSess;
	Data::ArrayListUInt32 callAddrs;
	Data::ArrayListUInt32 jmpAddrs;
	NN<Manage::AddressResolver> addrResol;
	UTF8Char sbuff[256];
	Text::StringBuilderUTF8 sb;
	OSInt instCnt = 0;
	if (outEsp)
	{
		*outEsp = 0;
	}

	tmpSess.callAddrs = &callAddrs;
	tmpSess.jmpAddrs = &jmpAddrs;
	MemCopyNO(&tmpSess.regs, &sess->regs, sizeof(Manage::DasmX86_32::DasmX86_32_Regs));
	tmpSess.regs.EIP = funcAddr;
	tmpSess.regs.ESP = sess->regs.ESP - 4;
//	tmpSess.outStr = &sb;
	tmpSess.endType = Manage::DasmX86_32::ET_NOT_END;// isEnd = false;
	tmpSess.espOfst = 0;
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
		UInt8 b = tmpSess.memReader->ReadMemUInt8(tmpSess.regs.EIP);
		if (b == 0xc2)
		{
			tmpSess.regs.ESP += 4 + (UInt32)sess->memReader->ReadMemUInt16(tmpSess.regs.EIP + 1);
			if (outEsp)
			{
				*outEsp = tmpSess.regs.ESP;
			}
			return (sess->memReader->ReadMemUInt16(tmpSess.regs.EIP + 1));
		}
		if (b == 0xc3)
		{
			tmpSess.regs.ESP += 4;
			if (outEsp)
			{
				*outEsp = tmpSess.regs.ESP;
			}
			return 0;
		}
		tmpSess.sbuff = sbuff;
		Bool ret = ((Manage::DasmX86_32::DasmX86_32_Code*)tmpSess.codes)[tmpSess.memReader->ReadMemUInt8(tmpSess.regs.EIP)](tmpSess);
		if (!ret)
		{
#ifdef _CONSOLE
			IO::ConsoleWriter console;
#else
			IO::FileStream fs(CSTR("ErrorMsg.txt"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer console(fs);
#endif
			UInt8 *buff;
			UOSInt buffSize;
			buffSize = tmpSess.regs.EIP - funcAddr;
			if (buffSize < 256)
				buffSize = 256;
			buff = MemAlloc(UInt8, buffSize);
			console.WriteLine();
			console.Write(sb.ToCString());

			sb.ClearStr();
			sb.AppendC(UTF8STRC("Func Buff 0x"));
			sb.AppendHex32(funcAddr);
			if (tmpSess.addrResol.SetTo(addrResol))
			{
				sb.AppendC(UTF8STRC(" "));
				addrResol->ResolveNameSB(sb, funcAddr);
			}
			console.WriteLine(sb.ToCString());
			buffSize = tmpSess.memReader->ReadMemory(funcAddr, buff, tmpSess.regs.EIP - funcAddr);
			if (buffSize > 0)
			{
				sb.ClearStr();
				sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
				console.WriteLine(sb.ToCString());
			}

			console.WriteLine(CSTR("Unknown opcode"));
			buffSize = tmpSess.memReader->ReadMemory(tmpSess.regs.EIP, buff, 256);
			if (buffSize > 0)
			{
				sb.ClearStr();
				sb.AppendHexBuff(buff, buffSize, ' ', Text::LineBreakType::CRLF);
				console.WriteLine(sb.ToCString());
			}
			MemFree(buff);
			return -1;
		}
		instCnt++;
		sb.AppendSlow(sbuff);
		if (tmpSess.endType == Manage::DasmX86_32::ET_EXIT)
		{
			tmpSess.regs.EIP = tmpSess.retAddr;
			return 0;
		}
		else if (tmpSess.endType != Manage::DasmX86_32::ET_NOT_END)
		{
			tmpSess.regs.EIP = tmpSess.retAddr;
			tmpSess.endType = Manage::DasmX86_32::ET_NOT_END;
		}
		else if (instCnt >= 4096)
		{
			return -1;
		}
		tmpSess.lastStatus = tmpSess.thisStatus;
		tmpSess.thisStatus = 0x80000000;
	}
	return 0;
}

Bool __stdcall DasmX86_32_00(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 *regPtr;
	UInt8 memVal;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_32_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_01(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_32_ParseReg(sess, regs, reg, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		if ((b & 0xc7) == 0xc4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
			return false;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_02(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr = 0;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_32_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = (UInt8)(*regPtr + memVal);
	return true;
}

Bool __stdcall DasmX86_32_03(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_32_ParseReg(sess, regs, reg, &regPtr);
	if (sess->thisStatus & 1)
	{
		AddEqU16((UInt16*)regPtr,(UInt16)(memVal & 0xffff));
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
/*	if ((sess->thisStatus & 0x80000000) == 0)
	{
		if (reg == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
			return false;
		}
	}*/
	return true;
}

Bool __stdcall DasmX86_32_04(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add AL,"));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_05(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add AX,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1));
		sess->regs.EIP += 3;
		return true;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add EAX,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->regs.EIP += 5;
		return true;
	}
}

Bool __stdcall DasmX86_32_06(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push ES\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_07(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop ES\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_08(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_09(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr = 0;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr |= memVal;
	return true;
}

Bool __stdcall DasmX86_32_0b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[6];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus)
	{
		OrEqU16((UInt16*)regPtr, (UInt16)memVal);
	}
	else
	{
		*regPtr |= memVal;
	}
	return true;
}

Bool __stdcall DasmX86_32_0c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	sess->regs.EAX |= v;
	return true;
}

Bool __stdcall DasmX86_32_0d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.EIP += 3;
		sess->regs.EAX |= v;
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.EIP += 5;
		sess->regs.EAX |= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push CS\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_0f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes0f)[sess->memReader->ReadMemUInt8(sess->regs.EIP + 1)](sess);
}

Bool __stdcall DasmX86_32_10(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	DasmX86_32_ParseReg8(sess, mem, reg, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_11(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 reg;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	DasmX86_32_ParseReg(sess, mem, reg, &regPtr);
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

Bool __stdcall DasmX86_32_12(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr = 0;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_32_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	AddEqU8(regPtr, memVal);
	return true;
}

Bool __stdcall DasmX86_32_13(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_32_ParseReg(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		AddEqU16((UInt16*)regPtr, (UInt16)(memVal & 0xffff));
	}
	else
	{
		*regPtr = *regPtr + memVal;
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

Bool __stdcall DasmX86_32_14(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc al,"));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_15(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc ax,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 3;
		return true;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc eax,"));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
}

Bool __stdcall DasmX86_32_16(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push SS\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_17(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop SS\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_18(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_19(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_1a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr = 0;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	SubEqU8(regPtr, memVal);
	return true;
}

Bool __stdcall DasmX86_32_1b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		SubEqU16((UInt16*)regPtr, (UInt16)(memVal & 0xffff));
	}
	else
	{
		*regPtr -= memVal;
	}
	return true;
}

Bool __stdcall DasmX86_32_1c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	SubEqU8((UInt8*)&sess->regs.EAX, v);
	return true;
}

Bool __stdcall DasmX86_32_1d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb AL, "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1));
		sess->regs.EIP += 3;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb AL, "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->regs.EIP += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_1e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push DS\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_1f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop DS\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_20(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_32_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_21(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_32_ParseReg(sess, regs, reg, &regPtr);
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

Bool __stdcall DasmX86_32_22(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
//	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt8 memVal;
	UInt8 *regPtr = 0;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);
	DasmX86_32_ParseReg8(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr &= memVal;
	return true;
}

Bool __stdcall DasmX86_32_23(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	UTF8Char regs[8];
	Int32 reg;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	DasmX86_32_ParseReg(sess, regs, reg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
	sess->sbuff = Text::StrConcat(sess->sbuff, regs);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		*(UInt16*)regPtr &= (UInt16)(memVal & 0xffff);
	}
	else
	{
		*regPtr &= memVal;
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

Bool __stdcall DasmX86_32_24(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EAX &= v;
	return true;
}

Bool __stdcall DasmX86_32_25(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.EIP += 3;
		sess->regs.EAX &= v;
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.EIP += 5;
		sess->regs.EAX &= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_26(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 2;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_27(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("daa\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_28(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 1;

	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_29(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;

	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_2a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr = 0;
	sess->regs.EIP += 1;

	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	SubEqU8(regPtr, memVal);
	return true;
}

Bool __stdcall DasmX86_32_2b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;

	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);

	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		SubEqU16((UInt16*)regPtr, (UInt16)(memVal & 0xffff));
	}
	else
	{
		*regPtr -= memVal;
	}
	return true;
}

Bool __stdcall DasmX86_32_2c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	SubEqU8((UInt8*)&sess->regs.EAX, v);
	return true;
}

Bool __stdcall DasmX86_32_2d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.EIP += 3;
		SubEqU16((UInt16*)&sess->regs.EAX, v);
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.EIP += 5;
		sess->regs.EAX -= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_2e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 0;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_2f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("das\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_30(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_31(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_32(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr = 0;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr ^= memVal;
	return true;
}

Bool __stdcall DasmX86_32_33(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
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

Bool __stdcall DasmX86_32_34(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 v = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, v);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	sess->regs.EAX ^= v;
	return true;
}

Bool __stdcall DasmX86_32_35(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		UInt16 v = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, v);
		sess->regs.EIP += 3;
		sess->regs.EAX ^= v;
	}
	else
	{
		UInt32 v = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, v);
		sess->regs.EIP += 5;
		sess->regs.EAX ^= v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_36(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 5;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_37(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aaa\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_38(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_39(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_3a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_3b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_3c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_3d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp AX, "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1));
		sess->regs.EIP += 3;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp EAX, "));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->regs.EIP += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));

	return true;
}

Bool __stdcall DasmX86_32_3e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 1;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_3f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aas\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_40(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 0, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		++*(UInt16*)regPtr;
	}
	else
	{
		++*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_41(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 1, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		++*(UInt16*)regPtr;
	}
	else
	{
		++*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_42(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 2, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		++*(UInt16*)regPtr;
	}
	else
	{
		++*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_43(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 3, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		++*(UInt16*)regPtr;
	}
	else
	{
		++*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_44(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 4, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		if (sess->thisStatus & 1)
		{
			++*(UInt16*)regPtr;
		}
		else
		{
			++*regPtr;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_45(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 5, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		if (sess->thisStatus & 1)
		{
			++*(UInt16*)regPtr;
		}
		else
		{
			++*regPtr;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_46(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 6, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		++*(UInt16*)regPtr;
	}
	else
	{
		++*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_47(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 7, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("inc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		++*(UInt16*)regPtr;
	}
	else
	{
		++*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_48(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 0, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		--*(UInt16*)regPtr;
	}
	else
	{
		--*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_49(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 1, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		--*(UInt16*)regPtr;
	}
	else
	{
		--*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_4a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 2, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		--*(UInt16*)regPtr;
	}
	else
	{
		--*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_4b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 3, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		--*(UInt16*)regPtr;
	}
	else
	{
		--*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_4c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 4, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		if (sess->thisStatus & 1)
		{
			--*(UInt16*)regPtr;
		}
		else
		{
			--*regPtr;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_4d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 5, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		if (sess->thisStatus & 1)
		{
			--*(UInt16*)regPtr;
		}
		else
		{
			--*regPtr;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_4e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 6, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		--*(UInt16*)regPtr;
	}
	else
	{
		--*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_4f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 7, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dec "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if (sess->thisStatus & 1)
	{
		--*(UInt16*)regPtr;
	}
	else
	{
		--*regPtr;
	}
	return true;
}

Bool __stdcall DasmX86_32_50(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 0, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_51(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 1, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_52(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 2, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_53(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 3, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_54(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 4, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_55(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 5, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
		//*(UInt8**)sess->regs.ESP = sess->regs.EBP;
	}
	return true;
}

Bool __stdcall DasmX86_32_56(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 6, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_57(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 7, &regPtr);
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP -= 4;
	}
	return true;
}

Bool __stdcall DasmX86_32_58(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 0, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->espOfst += 4;
		sess->regs.ESP += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_59(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 1, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->espOfst += 4;
		sess->regs.ESP += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_5a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 2, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->espOfst += 4;
		sess->regs.ESP += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_5b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 3, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->espOfst += 4;
		sess->regs.ESP += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_5c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 4, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->espOfst += (sess->memReader->ReadMemUInt32(sess->regs.EIP) - sess->regs.EIP);
		sess->regs.ESP = sess->memReader->ReadMemUInt32(sess->regs.EIP);
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_5d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 5, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.EBP = sess->memReader->ReadMemUInt32(sess->regs.ESP);
		sess->regs.ESP += 4;
		sess->espOfst += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_5e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 6, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->espOfst += 4;
		sess->regs.ESP += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_5f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[5];
	UInt32 *regPtr;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop "));
	DasmX86_32_ParseReg32(sess, reg, 7, &regPtr);
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	if ((sess->thisStatus & 0x80000000) == 0)
	{
		sess->regs.ESP += 4;
		sess->espOfst += 4;
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_60(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pusha\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pushad\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_61(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popa\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popad\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_62(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt32 memVal;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bound "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_63(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt16 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("arpl "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_64(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 3;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_65(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x10000;
	sess->segmId = 4;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_66(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
	sess->thisStatus |= 0x1;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_67(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_68(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 iVal = (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
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
	sess->regs.EIP += 5;
	sess->regs.ESP -= 4;
	return true;
}

Bool __stdcall DasmX86_32_69(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP));
		sess->regs.EIP += 2;
	}
	else
	{
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP));
		sess->regs.EIP += 4;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_6a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	sess->regs.ESP -= 4;
	return true;
}

Bool __stdcall DasmX86_32_6b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_6c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insb\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_6d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
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
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_6e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("outsb\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_6f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
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
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_70(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jo 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_71(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jno 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_72(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jb 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_73(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnb 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_74(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_75(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_76(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jbe 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_77(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ja 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_78(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("js 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_79(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jns 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_7a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jp 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_7b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnp 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_7c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jl 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_7d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jge 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_7e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jle 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_7f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jg 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_80(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = ((sess->memReader->ReadMemUInt8(sess->regs.EIP + 1) >> 3) & 7);
	UTF8Char mem[64];
	UInt8 memVal;
	Int32 reg;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &reg, &memVal);

	if (b == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("or "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sbb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("and "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if (b == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmp "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_81(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 reg;
	Int32 iVal;
	UInt32 memVal;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	if (sess->thisStatus & 1)
	{
		iVal = (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP);
		sess->regs.EIP += 2;
	}
	else
	{
		iVal = (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP);
		sess->regs.EIP += 4;
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
			sess->regs.ESP += (UInt32)iVal;
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP += (UInt32)iVal;
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
			sess->regs.ESP = (sess->regs.ESP | (UInt32)iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP = (sess->regs.EBP | (UInt32)iVal);
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
			sess->regs.ESP += (UInt32)iVal;
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP += (UInt32)iVal;
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
			sess->regs.ESP -= (UInt32)iVal;
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP -= (UInt32)iVal;
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
			sess->regs.ESP = (sess->regs.ESP & (UInt32)iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP = (sess->regs.EBP & (UInt32)iVal);
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
			sess->regs.ESP -= (UInt32)iVal;
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP -= (UInt32)iVal;
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
			sess->regs.ESP = (sess->regs.ESP ^ (UInt32)iVal);
			return true;
		}
		else if ((b & 0xc7) == 0xc5)
		{
			sess->regs.EBP = (sess->regs.EBP ^ (UInt32)iVal);
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

Bool __stdcall DasmX86_32_82(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_83(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 reg;
	Int32 iVal;
	UInt32 memVal;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
	if (reg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("add "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & 0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				sess->regs.ESP += (UInt32)iVal;
				sess->stabesp = sess->regs.ESP;
				return true;
/*				if (sess->lastStatus & 2)
				{
					sess->regs.ESP += iVal;
					return true;
				}
				else
				{
					sess->regs.ESP += iVal;
					return true;
//					sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("Unknown operation to stack\r\n"));
//					return false;
				}*/
			}
			else if ((b & 0xc7) == 0xc5)
			{
				sess->regs.EBP += (UInt32)iVal;
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
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & 0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.ESP = (sess->regs.ESP | (UInt32)iVal);
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
				sess->regs.EBP = (sess->regs.EBP | (UInt32)iVal);
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
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & 0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.ESP += (UInt32)iVal;
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
				sess->regs.EBP += (UInt32)iVal;
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
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & 0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.ESP -= (UInt32)iVal;
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
				sess->regs.EBP -= (UInt32)iVal;
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
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & 0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.ESP = (sess->regs.ESP & (UInt32)iVal);
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
				sess->regs.EBP = (sess->regs.EBP & (UInt32)iVal);
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
		sess->sbuff = Text::StrInt32(sess->sbuff, iVal = (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		if ((sess->thisStatus & 0x80000000) == 0)
		{
			if ((b & 0xc7) == 0xc4)
			{
				if (sess->lastStatus & 2)
				{
					sess->regs.ESP -= (UInt32)iVal;
					return true;
				}
				else
				{
					sess->regs.ESP -= (UInt32)iVal;
					return true;
				}
			}
			else if ((b & 0xc7) == 0xc4)
			{
				sess->regs.EBP -= (UInt32)iVal;
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
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;

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
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_84(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_85(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;

	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_86(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_32_ParseReg8(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_87(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_88(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_32_ParseReg8(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_89(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP++;
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	DasmX86_32_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));

	if ((b & 0xc7) == 0xc4)
	{
		if (destReg == 5)
		{
			sess->regs.ESP = sess->regs.EBP;
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
			sess->regs.EBP = sess->regs.ESP;
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

Bool __stdcall DasmX86_32_8a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	UInt8 *regPtr = 0;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_32_ParseReg8(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_8b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt32 memVal;
	UInt32 *regPtr = 0;
	sess->regs.EIP++;

	DasmX86_32_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	DasmX86_32_ParseReg32(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_8c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_32_ParseSReg(sess, reg, destReg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_8d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt32 memVal;
	UInt32 *regPtr;
	UInt32 memAddr = 0;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &destReg, &memVal, &memAddr);
	DasmX86_32_ParseReg(sess, reg, destReg, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lea "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memAddr;
	return true;
}

Bool __stdcall DasmX86_32_8e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt8 memVal;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	DasmX86_32_ParseSReg(sess, reg, destReg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_8f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
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

Bool __stdcall DasmX86_32_90(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pause\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("nop\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_91(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 1, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 1, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_92(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 2, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 2, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_93(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 3, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 3, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_94(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 4, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 4, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_95(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 5, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 5, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_96(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 6, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 6, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_97(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	if (sess->thisStatus & 1)
	{
		UInt32 *regPtr;
		UInt16 v;
		DasmX86_32_ParseReg16(sess, reg, 7, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg AX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = (UInt16)sess->regs.EAX & 0xffff;
		*(UInt16*)&sess->regs.EAX = *(UInt16*)regPtr;
		*(UInt16*)regPtr = v;
	}
	else
	{
		UInt32 *regPtr;
		UInt32 v;
		DasmX86_32_ParseReg32(sess, reg, 7, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xchg EAX, "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		v = sess->regs.EAX;
		sess->regs.EAX = *regPtr;
		*regPtr = v;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_98(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
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

Bool __stdcall DasmX86_32_99(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP++;
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

Bool __stdcall DasmX86_32_9a(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_9b(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fwait\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_9c(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pushf\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pushfd\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_9d(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popf\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popfd\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_9e(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sahf\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_9f(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lahf\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_a0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
	*(UInt8*)&sess->regs.EAX = sess->memReader->ReadMemUInt8(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov AL,BYTE PTR ["));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
	sess->regs.EIP += 5;
	return true;
}

Bool __stdcall DasmX86_32_a1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov AX,WORD PTR "));
		*(UInt16*)&sess->regs.EAX = sess->memReader->ReadMemUInt16(addr);
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov EAX,DWORD PTR "));
		sess->regs.EAX = sess->memReader->ReadMemUInt32(addr);
	}
	if (sess->thisStatus & 0x10000)
	{
		sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_32_Segms[sess->segmId]);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("[0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("]\r\n"));
	sess->regs.EIP += 5;
	return true;
}

Bool __stdcall DasmX86_32_a2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov BYTE PTR "));
	if (sess->thisStatus & 0x10000)
	{
		sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_32_Segms[sess->segmId]);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("["));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],AL\r\n"));
	sess->regs.EIP += 5;
	return true;
}

Bool __stdcall DasmX86_32_a3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov WORD PTR "));
		if (sess->thisStatus & 0x10000)
		{
			sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_32_Segms[sess->segmId]);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("["));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],AX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov DWORD PTR "));
		if (sess->thisStatus & 0x10000)
		{
			sess->sbuff = Text::StrConcat(sess->sbuff, DasmX86_32_Segms[sess->segmId]);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("["));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("],EAX\r\n"));
	}
	sess->regs.EIP += 5;
	return true;
}

Bool __stdcall DasmX86_32_a4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsb\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_a5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
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
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_a6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
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
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_a7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
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
		sess->regs.EIP += 1;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpsd\r\n"));
		sess->regs.EIP += 1;
	}
	return true;
}

Bool __stdcall DasmX86_32_a8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test AL, "));
	sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_a9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test AX, 0x"));
		sess->sbuff = Text::StrHexVal16(sess->sbuff, sess->memReader->ReadMemUInt16(sess->regs.EIP + 1));
		sess->regs.EIP += 3;
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test EAX, 0x"));
		sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.EIP + 1));
		sess->regs.EIP += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_aa(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.EIP++;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stosb\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_ab(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.EIP++;
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

Bool __stdcall DasmX86_32_ac(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lodsb\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_ad(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->regs.EIP += 1;
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

Bool __stdcall DasmX86_32_ae(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rep "));
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("scasb\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_af(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
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
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_b0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 0, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 1, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 2, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 3, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 4, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 5, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 6, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt8 memVal = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt8 *regPtr;
	DasmX86_32_ParseReg8(sess, reg, 7, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, memVal);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_b8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 0, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_b9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 1, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_ba(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 2, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_bb(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 3, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_bc(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 4, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_bd(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 5, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_be(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 6, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_bf(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg(sess, reg, 7, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	if (sess->thisStatus & 1)
	{
		UInt16 memVal = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)memVal);
		sess->regs.EIP += 3;
		*(UInt16*)regPtr = memVal;
	}
	else
	{
		UInt32 memVal = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)memVal);
		sess->regs.EIP += 5;
		*regPtr = memVal;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_c0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_c1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rol "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ror "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 2)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcl "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 3)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sal "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sar "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_c2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 cnt = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
	sess->endType = Manage::DasmX86_32::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ret "));
	sess->sbuff = Text::StrUInt32(sess->sbuff, cnt);
	if (sess->thisStatus & 0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		UInt8 *buff;
		UOSInt buffSize;
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.ESP);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\t;"));
		buff = MemAlloc(UInt8, cnt);
		buffSize = sess->memReader->ReadMemory(sess->regs.ESP + 4, buff, cnt);
		if (buffSize > 0)
		{
			sess->sbuff = Text::StrHexBytes(sess->sbuff, buff, buffSize, ' ');
		}
		sess->regs.ESP += 4 + cnt;
		MemFree(buff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 3;
	return true;
}

Bool __stdcall DasmX86_32_c3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->endType = Manage::DasmX86_32::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ret\r\n"));
	if (sess->thisStatus & 0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.ESP);
		sess->regs.ESP += 4;
	}
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_c4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM48(sess, mem, &regNo);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("les "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_c5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 *regPtr;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM48(sess, mem, &regNo);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lds "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_c6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt8 memVal;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	if (destReg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_c7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt32 memVal;
	sess->regs.EIP++;
	if (((sess->memReader->ReadMemUInt8(sess->regs.EIP) >> 3) & 7) == 0)
	{
		DasmX86_32_ParseModRM32(sess, mem, &destReg, &memVal, 0);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		if (sess->thisStatus & 1)
		{
			sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP));
			sess->regs.EIP += 2;
		}
		else
		{
			sess->sbuff = Text::StrInt32(sess->sbuff, (Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP));
			sess->regs.EIP += 4;
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

Bool __stdcall DasmX86_32_c8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("enter "));
	sess->sbuff = Text::StrInt32(sess->sbuff, (Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
	sess->regs.EIP += 4;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_c9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("leave\r\n"));
	sess->regs.EIP += 1;
	sess->regs.ESP = sess->regs.EBP;
	sess->regs.EBP = sess->memReader->ReadMemUInt32(sess->regs.ESP);
	sess->regs.ESP += 4;
	return true;
}

Bool __stdcall DasmX86_32_ca(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 cnt = sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
	sess->endType = Manage::DasmX86_32::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("retf "));
	sess->sbuff = Text::StrUInt32(sess->sbuff, cnt);
	if (sess->thisStatus & 0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		UInt8 *buff;
		UOSInt buffSize;
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.ESP);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\t;"));
		buff = MemAlloc(UInt8, cnt);
		buffSize = sess->memReader->ReadMemory(sess->regs.ESP + 8, buff, cnt);
		if (buffSize > 0)
		{
			sess->sbuff = Text::StrHexBytes(sess->sbuff, buff, buffSize, ' ');
		}
		sess->regs.ESP += 8 + cnt;
		MemFree(buff);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 3;
	return true;
}

Bool __stdcall DasmX86_32_cb(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->endType = Manage::DasmX86_32::ET_FUNC_RET;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("retf\r\n"));
	if (sess->thisStatus & 0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.ESP);
		sess->regs.ESP += 8;
	}
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_cc(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("int 3\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_cd(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("int 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_ce(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("into\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_cf(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("iret\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("iretd\r\n"));
	}
	if (sess->thisStatus & 0x80000000)
	{
		sess->retAddr = 0;
	}
	else
	{
		sess->retAddr = sess->memReader->ReadMemUInt32(sess->regs.ESP);
		sess->regs.ESP += 12;
	}
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_d0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_d1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
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

Bool __stdcall DasmX86_32_d2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_d3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
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

Bool __stdcall DasmX86_32_d4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 1) == 0xa)
	{
		sess->regs.EIP += 2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aam\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_d5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 1) == 0xa)
	{
		sess->regs.EIP += 2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aad\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_d6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_d7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xlatb\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_d8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.EIP += 2;
		b2 = (b >> 3) & 7;
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fadd "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmul "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcom "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomp "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsub "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubr "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdiv "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivr "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		UInt32 memVal;
		sess->regs.EIP += 1;
		DasmX86_32_ParseModRM32(sess, mem, &b2, &memVal, 0);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_d9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.EIP += 2;
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
				DasmX86_32_ParseRegx87(mem, 0);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				DasmX86_32_ParseRegx87(mem, b & 7);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 1)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fxch "));
				DasmX86_32_ParseRegx87(mem, 0);
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
				DasmX86_32_ParseRegx87(mem, b & 7);
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
			sess->regs.EIP += 1;
			DasmX86_32_ParseModRM32(sess, mem, &b2, &memVal, 0);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_da(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.EIP += 2;
		b2 = (b >> 3) & 7;
		if (b == 0xe9)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucompp\r\n"));
			return true;
		}
		else if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovb "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmove "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovbe "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovu "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		UInt32 memVal;
		sess->regs.EIP += 1;
		DasmX86_32_ParseModRM32(sess, mem, &b2, &memVal, 0);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_db(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.EIP += 2;
		b2 = (b >> 3) & 7;
		if (b == 0xe2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fnclex\r\n"));
			sess->regs.EIP += 2;
			return true;
		}
		else if (b == 0xe3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fninit\r\n"));
			sess->regs.EIP += 2;
			return true;
		}
		else if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovnb "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovne "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovnbe "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcmovnu "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucomi "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomi "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		sess->regs.EIP += 1;
		if (b & 0x20)
		{
/*	1		DasmX86_32_ParseModRM80(sess, mem, &b2);
			if (b2 == 5)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fld "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}
			else if (b2 == 7)
			{
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fstp "));
				sess->sbuff = Text::StrConcat(sess->sbuff, mem);
				sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
				return true;
			}*/
		}
		else
		{
			UInt32 memVal;
			DasmX86_32_ParseModRM32(sess, mem, &b2, &memVal, 0);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_dc(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		b2 = (b >> 3) & 7;
		sess->regs.EIP += 2;
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fadd "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmul "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubr "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsub "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivr "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdiv "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		sess->regs.EIP += 1;
		DasmX86_32_ParseModRM64(sess, mem, &b2);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_dd(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		sess->regs.EIP += 2;
		b2 = (b >> 3) & 7;
		if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ffree "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 2)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fst "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 3)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fstp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucom "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucomp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
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
			sess->regs.EIP += 1;
			DasmX86_32_ParseModRM64(sess, mem, &b2);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_de(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		b2 = (b >> 3) & 7;
		sess->regs.EIP += 2;
		if (b == 0xd9)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcompp\r\n"));
			return true;
		}
		else if (b2 == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("faddp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fmulp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 4)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubrp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fsubp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivrp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 7)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fdivp "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
	}
	else
	{
		UInt16 memVal;
		sess->regs.EIP += 1;
		DasmX86_32_ParseModRM16(sess, mem, &b2, &memVal);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_df(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UTF8Char mem[64];
	Int32 b2;
	if ((b & 0xc0) == 0xc0)
	{
		b2 = (b >> 3) & 7;
		if (b == 0xe0)
		{
			sess->regs.EIP += 2;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fnstsw AX\r\n"));
			return true;
		}
		else if (b2 == 5)
		{
			sess->regs.EIP += 2;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fucomip "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (b2 == 6)
		{
			sess->regs.EIP += 2;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("fcomip "));
			DasmX86_32_ParseRegx87(mem, 0);
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			DasmX86_32_ParseRegx87(mem, b & 7);
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
				sess->regs.EIP += 1;
				DasmX86_32_ParseModRM64(sess, mem, &b2);
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
				sess->regs.EIP += 1;
				DasmX86_32_ParseModRMR80(sess, mem, &b2);
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
			sess->regs.EIP += 1;
			DasmX86_32_ParseModRM16(sess, mem, &b2, &memVal);
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
	}
	return false;
}

Bool __stdcall DasmX86_32_e0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("loopnz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_e1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("loopz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_e2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("loop 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_e3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (UInt32)((Int32)sess->regs.EIP + 2 + (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->regs.EIP += 2;
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

Bool __stdcall DasmX86_32_e4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AL, 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_e5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AX, 0x"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in EAX, 0x"));
	}
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_e6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", AL\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_e7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out 0x"));
	sess->sbuff = Text::StrHexByte(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", AX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", EAX\r\n"));
	}
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_e8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UnsafeArray<UTF8Char> sptr;
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = sess->regs.EIP + ((UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1)) + 3;
		sess->regs.EIP += 3;
	}
	else
	{
		addr = sess->regs.EIP + (sess->memReader->ReadMemUInt32(sess->regs.EIP + 1)) + 5;
		sess->regs.EIP += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("call 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);

	if (sess->thisStatus & 0x80000000)
	{
	}
	else
	{
		UInt32 outEsp;
		Int32 stackCnt = DasmX86_32_GetFuncStack(sess, addr, &outEsp);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ParamStack = "));
		sess->sbuff = Text::StrInt32(sess->sbuff, stackCnt);
		if (outEsp != 0)
		{
			if ((sess->regs.ESP + (UInt32)stackCnt + 64) >= outEsp && (sess->regs.ESP + (UInt32)stackCnt - 64) <= outEsp)
			{
				sess->regs.ESP += (UInt32)stackCnt;
			}
			else
			{
				sess->regs.ESP = outEsp;
			}
		}
		else if (stackCnt > 0)
		{
			sess->regs.ESP += (UInt32)stackCnt;
		}
	}
	NN<Manage::AddressResolver> addrResol;
	if (sess->addrResol.SetTo(addrResol))
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
		sptr = sess->sbuff;
		sess->sbuff = addrResol->ResolveName(sess->sbuff, addr).Or(sess->sbuff);
		if (DasmX86_32_IsEndFunc(sptr))
		{
			sess->endType = Manage::DasmX86_32::ET_EXIT;
			sess->retAddr = sess->regs.EIP;
		}
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->callAddrs->Add(addr);
	sess->stabesp = sess->regs.ESP;
	sess->thisStatus |= 2;
	return true;
}

Bool __stdcall DasmX86_32_e9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	UInt32 addr2;
	UOSInt i;
	if (sess->thisStatus & 1)
	{
		addr = sess->regs.EIP + 3 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		sess->regs.EIP += 3;
	}
	else
	{
		addr = sess->regs.EIP + 5 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		sess->regs.EIP += 5;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(addr);
	addr2 = sess->regs.EIP;

	sess->endType = Manage::DasmX86_32::ET_JMP;
	sess->retAddr = addr;
	i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		addr = sess->jmpAddrs->GetItem(i);
		if (addr >= addr2 && addr <= addr2 + 16)
		{
			sess->endType = Manage::DasmX86_32::ET_NOT_END;
			break;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_ea(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	UInt32 addr2;
	UOSInt i;
	if (sess->thisStatus & 1)
	{
		addr = (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 1);
		addr2 = (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 3);
		sess->regs.EIP += 5;
	}
	else
	{
		addr = sess->memReader->ReadMemUInt32(sess->regs.EIP + 1);
		addr2 = sess->memReader->ReadMemUInt32(sess->regs.EIP + 5);
		sess->regs.EIP += 7;
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp "));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr2);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(":"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(addr);
	addr2 = sess->regs.EIP;

	sess->endType = Manage::DasmX86_32::ET_JMP;
	sess->retAddr = addr;
	i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		addr = sess->jmpAddrs->GetItem(i);
		if (addr >= addr2 && addr <= addr2 + 16)
		{
			sess->endType = Manage::DasmX86_32::ET_NOT_END;
			break;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_eb(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr = (sess->regs.EIP) + 2 + (UInt32)(Int32)(Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	UInt32 addr2;
	UOSInt i;
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jmp 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->jmpAddrs->Add(addr);
	addr2 = sess->regs.EIP;

	sess->endType = Manage::DasmX86_32::ET_JMP;
	sess->retAddr = addr;
	i = sess->jmpAddrs->GetCount();
	while (i-- > 0)
	{
		addr = sess->jmpAddrs->GetItem(i);
		if (addr == addr2)
		{
			sess->endType = Manage::DasmX86_32::ET_NOT_END;
			break;
		}
	}
	return true;
}

Bool __stdcall DasmX86_32_ec(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AL, DX\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_ed(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in AX, DX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("in EAX, DX\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_ee(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out DX, AL\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_ef(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->thisStatus & 1)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out DX, AX\r\n"));
	}
	else
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("out DX, EAX\r\n"));
	}
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_f0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 1;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lock "));
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
/*	UInt8 b = sess->memReader->ReadMemUInt8(sess->regs.EIP + 1);
	if (b == 0xaa)
	{
		sess->regs.EIP += 2;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lock stosb\r\n"));
		return true;
	}
	else
	{
		return false;
	}*/
}

Bool __stdcall DasmX86_32_f1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_f2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 1;
	sess->thisStatus |= 4;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_f3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 1;
	sess->thisStatus |= 5;
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes)[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
}

Bool __stdcall DasmX86_32_f4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("hlt\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_f5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmc\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_f6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt8 memVal;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM8(sess, mem, &destReg, &memVal);
	if (destReg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, (Int8)sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
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

Bool __stdcall DasmX86_32_f7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	Int32 destReg;
	UTF8Char mem[64];
	UInt32 memVal;
	sess->regs.EIP++;
	DasmX86_32_ParseModRM32(sess, mem, &destReg, &memVal, 0);
	if (destReg == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("test "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		if (sess->thisStatus & 1)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("0x"));
			sess->sbuff = Text::StrHexVal16(sess->sbuff, sess->memReader->ReadMemUInt16(sess->regs.EIP));
			sess->regs.EIP += 2;
		}
		else
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("0x"));
			sess->sbuff = Text::StrHexVal32(sess->sbuff, sess->memReader->ReadMemUInt32(sess->regs.EIP));
			sess->regs.EIP += 4;
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

Bool __stdcall DasmX86_32_f8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clc\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_f9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stc\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_fa(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cli\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_fb(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sti\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_fc(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cld\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_fd(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("std\r\n"));
	sess->regs.EIP++;
	return true;
}

Bool __stdcall DasmX86_32_fe(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_ff(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 1;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
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
		if (sess->thisStatus & 0x80000000)
		{
		}
		else
		{
			UInt32 outEsp;
			Int32 stackCnt = DasmX86_32_GetFuncStack(sess, memVal, &outEsp);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" (0x"));
			sess->sbuff = Text::StrHexVal32(sess->sbuff, memVal);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(")"));

			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" ParamStack = "));
			sess->sbuff = Text::StrInt32(sess->sbuff, stackCnt);
			if (outEsp != 0)
			{
				if ((sess->regs.ESP + (UInt32)stackCnt + 16) >= outEsp && (sess->regs.ESP + (UInt32)stackCnt - 16) <= outEsp)
				{
					sess->regs.ESP += (UInt32)stackCnt;
				}
				else
				{
					sess->regs.ESP = outEsp;
				}
			}
			else if (stackCnt > 0)
			{
				sess->regs.ESP += (UInt32)stackCnt;
			}
		}
		NN<Manage::AddressResolver> addrResol;
		if (sess->addrResol.SetTo(addrResol))
		{
			UnsafeArray<UTF8Char> sptr;
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(" "));
			sptr = sess->sbuff;
			sess->sbuff = addrResol->ResolveName(sess->sbuff, memVal).Or(sess->sbuff);
			if (DasmX86_32_IsEndFunc(sptr))
			{
				sess->endType = Manage::DasmX86_32::ET_EXIT;
				sess->retAddr = sess->regs.EIP;
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

Bool __stdcall DasmX86_32_0f00(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModR32M16(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f01(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xc1)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmcall\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xc2)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmlaunch\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xc3)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmresume\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xc4)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmxoff\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xc8)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("monitor\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xc8)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mwait\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xca)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clac\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xd0)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xgetbv\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xd1)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xsetbv\r\n"));
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) == 0xf9)
	{
		sess->regs.EIP += 3;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdtscp\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f02(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 2;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		UInt32 *regPtr;
		DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt32 memVal;
		UInt32 *regPtr;
		DasmX86_32_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lar "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f03(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 2;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		UInt32 *regPtr;
		DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt32 memVal;
		UInt32 *regPtr;
		DasmX86_32_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lsl "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f04(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f05(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("syscall\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f06(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("clts\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f07(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sysret\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f08(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("invd\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f09(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("wbinvd\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f0A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f0B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ud2\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f0C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f0D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f0E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("femms\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f0F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
	DasmX86_32_ParseRegMM(reg, regNo);
	UInt8 cmdId = sess->memReader->ReadMemUInt8(sess->regs.EIP);
	sess->regs.EIP++;
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
	}
	return false;
}

Bool __stdcall DasmX86_32_0f10(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movups "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movupd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f11(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movups "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movupd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f12(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
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
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movddup "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsldup "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f13(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movlpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f14(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpcklps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpcklpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f15(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpckhps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("unpckhpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f16(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
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
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movshdup "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f17(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movhpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f18(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f19(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f1A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f1B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f1C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f1D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f1E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f1F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("nop "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;

	}
	return false;
}

Bool __stdcall DasmX86_32_0f20(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseRegCR(reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f21(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseRegDR(reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f22(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseRegCR(reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f23(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseRegDR(reg, regNo);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mov "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f24(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f25(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f26(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f27(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f28(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movaps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movapd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f29(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movaps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movapd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f2A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpi2ps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpi2pd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt32 memVal;
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsi2sd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UInt32 memVal;
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsi2ss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f2B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		UInt32 memVal;
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f2C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttps2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttpd2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt32 *regPtr;
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttsd2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UInt32 *regPtr;
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttss2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f2D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtps2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpd2pi "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt32 *regPtr;
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsd2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		UInt32 *regPtr;
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtss2si "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f2E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ucomiss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ucomisd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f2F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("comiss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("comisd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f30(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("wrmsr\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f31(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdtsc\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f32(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdmsr\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f33(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rdpmc\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f34(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sysenter\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f35(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sysexit\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f36(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f37(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("getsec\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f38(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes0f38)[sess->memReader->ReadMemUInt8(sess->regs.EIP + 2)](sess);
}

Bool __stdcall DasmX86_32_0f39(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return ((Manage::DasmX86_32::DasmX86_32_Code*)sess->codes0f3a)[sess->memReader->ReadMemUInt8(sess->regs.EIP + 2)](sess);
}

Bool __stdcall DasmX86_32_0f3B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f40(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovo "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f41(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovno "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f42(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f43(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovnb "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f44(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovz "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f45(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovnz "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f46(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovbe "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f47(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmova "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f48(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovs "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f49(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovns "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f4A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f4B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovnp "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f4C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovl "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f4D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovge "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f4E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovle "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f4F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmovg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f50(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	UInt32 *regPtr;
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movmskps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movmskpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f51(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sqrtps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sqrtsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sqrtss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f52(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsqrtps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsqrtss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f53(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcpps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rcpss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f54(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f55(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andnps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("andnpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f56(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("orps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("orpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f57(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xorps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xorpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f58(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f59(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mulpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f5A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtps2pd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpd2ps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtsd2ss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f5B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtdq2ps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtps2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttps2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f5C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("subss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f5D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("minss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f5E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("divss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f5F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maxss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f60(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f61(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f62(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f63(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packsswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packsswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f64(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f65(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f66(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f67(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packuswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packuswb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f68(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f69(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f6A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f6B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packssdw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packssdw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f6C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpcklqdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f6D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("punpckhqdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f6E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f6F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqa "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f70(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshuflw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufhw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f71(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	Int32 b2 = (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) >> 3) & 7;
	if (b2 == 2)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 4)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psraw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 6)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f72(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	Int32 b2 = (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) >> 3) & 7;
	if (b2 == 2)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 4)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrad "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 6)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f73(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	Int32 b2 = (sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) >> 3) & 7;
	if (b2 == 2)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 3)
	{
		DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 6)
	{
		if ((sess->thisStatus & 5) == 1)
		{
			DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		else
		{
			DasmX86_32_ParseRegMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		}
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}
	else if (b2 == 7)
	{
		DasmX86_32_ParseRegXMM(reg, sess->memReader->ReadMemUInt8(sess->regs.EIP + 2) & 7);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 3));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 4;
		return true;
	}

	return false;
}

Bool __stdcall DasmX86_32_0f74(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f75(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f76(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f77(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->regs.EIP += 2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("emms\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f78(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmread "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
    else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		if (regNo == 0)
		{
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("extrq "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
			sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			sess->regs.EIP += 2;
			return true;
		}
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insertq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP + 1));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP += 2;
		return true;
	}

	return false;
}

Bool __stdcall DasmX86_32_0f79(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("vmwrite "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("extrq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insertq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f7A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f7B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f7C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("haddpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("haddps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f7D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("hsubpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("hsubps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f7E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		sess->thisStatus &= (UInt32)~5;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f7F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqa "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movdqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f80(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jo 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f81(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + (UInt32)(Int32)sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jno 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f82(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jb 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f83(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnb 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f84(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f85(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnz 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f86(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jbe 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f87(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ja 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f88(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("js 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f89(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jns 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f8A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jp 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f8B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jnp 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f8C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jl 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f8D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jge 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f8E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + (UInt32)sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jle 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f8F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UInt32 addr;
	if (sess->thisStatus & 1)
	{
		addr = (sess->regs.EIP) + 4 + (UInt32)(Int32)(Int16)sess->memReader->ReadMemUInt16(sess->regs.EIP + 2);
		sess->regs.EIP += 4;
	}
	else
	{
		addr = (sess->regs.EIP) + 6 + sess->memReader->ReadMemUInt32(sess->regs.EIP + 2);
		sess->regs.EIP += 6;
	}
	sess->jmpAddrs->Add(addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("jg 0x"));
	sess->sbuff = Text::StrHexVal32(sess->sbuff, addr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0f90(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f91(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f92(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f93(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f94(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f95(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f96(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f97(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f98(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f99(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f9A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f9B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f9C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f9D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f9E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0f9F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt8 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
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

Bool __stdcall DasmX86_32_0fA0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push FS\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_0fA1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop FS\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_0fA2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cpuid\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_0fA3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bt "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fA4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shld "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_0fA5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shld "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fA6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fA7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fA8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("push GS\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_0fA9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pop GS\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_0fAA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("rsm\r\n"));
	sess->regs.EIP += 2;
	return true;
}

Bool __stdcall DasmX86_32_0fAB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bts "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fAC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shrd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	sess->regs.EIP += 1;
	return true;
}

Bool __stdcall DasmX86_32_0fAD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shrd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", CL\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fAE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.EIP += 2;
	if (sess->memReader->ReadMemUInt8(sess->regs.EIP) == 0xe8)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lfence\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP) == 0xf0)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mfence\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else if (sess->memReader->ReadMemUInt8(sess->regs.EIP) == 0xf8)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("sfence\r\n"));
		sess->regs.EIP += 1;
		return true;
	}
	else
	{
		UInt32 memVal;
		regNo = (sess->memReader->ReadMemUInt8(sess->regs.EIP) >> 3) & 7;
		if (regNo == 2)
		{
			DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ldmxcsr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (regNo == 2)
		{
			DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("stmxcsr "));
			sess->sbuff = Text::StrConcat(sess->sbuff, mem);
			sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
			return true;
		}
		else if (regNo == 7)
		{
			UInt8 memVal8;
			DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal8);
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

Bool __stdcall DasmX86_32_0fAF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("imul "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fB0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpxchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fB1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cmpxchg "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fB2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fB3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btr "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fB4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fB5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fB6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
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

Bool __stdcall DasmX86_32_0fB7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt16 memVal;
	UInt32 *regPtr = 0;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movzx "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*regPtr = memVal;
	return true;
}

Bool __stdcall DasmX86_32_0fB8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	if (sess->thisStatus & 1)
	{
		UInt16 memVal;
		DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
	}
	else
	{
		UInt32 memVal;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
	}
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("popcnt "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fB9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fBA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	if (regNo == 4)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bt "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 5)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bts "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 6)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if (regNo == 7)
	{
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->regs.EIP += 1;
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else
	{
		return false;
	}
}

Bool __stdcall DasmX86_32_0fBB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("btc "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fBC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bsf "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fBD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bsr "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fBE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
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

Bool __stdcall DasmX86_32_0fBF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt16 memVal;
	UInt32 *regPtr = 0;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movsx "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	*(Int32*)regPtr = (Int16)memVal;
	return true;
}

Bool __stdcall DasmX86_32_0fC0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 memVal;
	UInt8 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
	DasmX86_32_ParseReg8(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xadd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fC1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
	DasmX86_32_ParseReg(sess, reg, regNo, &regPtr);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("xadd "));
	sess->sbuff = Text::StrConcat(sess->sbuff, mem);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fC2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt8 cmdId;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.EIP);
		sess->regs.EIP++;
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
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.EIP);
		sess->regs.EIP++;
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
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.EIP);
		sess->regs.EIP++;
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
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		cmdId = sess->memReader->ReadMemUInt8(sess->regs.EIP);
		sess->regs.EIP++;
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

Bool __stdcall DasmX86_32_0fC3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	UInt32 *regPtr;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movnti "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fC4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModR32M16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fC5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;}

Bool __stdcall DasmX86_32_0fC6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shufps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("shufpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fC7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	Int32 regNo;
	sess->regs.EIP += 2;
	DasmX86_32_ParseModRM64(sess, mem, &regNo);
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

Bool __stdcall DasmX86_32_0fC8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 0, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fC9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 1, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fCA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 2, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fCB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 3, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fCC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 4, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fCD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 5, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fCE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 6, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fCF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char reg[8];
	UInt32 *regPtr;
	DasmX86_32_ParseReg32(sess, reg, 7, &regPtr);
	sess->regs.EIP+=2;
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("bswap "));
	sess->sbuff = Text::StrConcat(sess->sbuff, reg);
	sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
	return true;
}

Bool __stdcall DasmX86_32_0fD0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addsubpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("addsubps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrlq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmullw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmullw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movq2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0fD8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fD9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fDA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fDB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pand "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pand "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fDC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fDD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddusw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fDE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxub "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fDF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pandn "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pandn "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psraw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psraw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrad "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psrad "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pavgw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvttpd2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtpd2dq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 5)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("cvtdq2pd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fE9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fEA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fEB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("por "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("por "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fEC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fED(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fEE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fEF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pxor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pxor "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 4)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("lddqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pslld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psllq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmuludq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmuludq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psadbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psadbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maskmovq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("maskmovdqu "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fF9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fFA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fFB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psubq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fFC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fFD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fFE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 2;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("paddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0fFF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3800(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pshufb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3801(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3802(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3803(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phaddsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3804(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaddubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3805(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3806(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3807(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phsubsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3808(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3809(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f380A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("psignd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f380B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhrsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulhrsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f380C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f380D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f380E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f380F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3810(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pblendvb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3811(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3812(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3813(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3814(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendvps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3815(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendvpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3816(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3817(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("ptest "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3818(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3819(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f381A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f381B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f381C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f381D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f381E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pabsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f381F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3820(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3821(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxbd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3822(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxbq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3823(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3824(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxwq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3825(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovsxdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3826(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3827(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3828(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmuldq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3829(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpeqq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f382A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntdqa "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f382B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("packusdw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f382C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f382D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f382E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f382F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3830(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3831(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxbd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3832(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxbq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3833(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxwd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3834(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxwq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3835(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmovzxdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3836(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3837(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpgtq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3838(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3839(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f383A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f383B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pminud "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f383C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f383D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxsd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f383E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f383F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmaxud "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3840(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pmulld "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3841(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("phminposuw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3842(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3843(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3844(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3845(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3846(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3847(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3848(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3849(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f384A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f384B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f384C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f384D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f384E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f384F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3850(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3851(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3852(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3853(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3854(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3855(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3856(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3857(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3858(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3859(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f385A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f385B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f385C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f385D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f385E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f385F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3860(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3861(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3862(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3863(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3864(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3865(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3866(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3867(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3868(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3869(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f386A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f386B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f386C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f386D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f386E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f386F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3870(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3871(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3872(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3873(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3874(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3875(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3876(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3877(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3878(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3879(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f387A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f387B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f387C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f387D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f387E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f387F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3880(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 *regPtr;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("invept "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3881(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 *regPtr;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM128(sess, mem, &regNo);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("invvpid "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3882(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3883(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3884(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3885(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3886(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3887(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3888(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3889(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f388A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f388B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f388C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f388D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f388E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f388F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3890(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3891(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3892(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3893(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3894(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3895(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3896(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3897(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3898(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3899(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f389A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f389B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f389C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f389D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f389E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f389F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38A9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38AA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38AB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38AC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38AD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38AE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38AF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38B9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38BA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38BB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38BC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38BD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38BE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38BF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38C9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38CA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38CB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38CC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38CD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38CE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38CF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38D9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38DA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38DB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesimc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38DC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesenc "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38DD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesenclast "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38DE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesdec "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38DF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aesdeclast "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38E0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movntdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38E8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38E9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38EA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38EB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38EC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38ED(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38EE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38EF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		UInt32 memVal;
		UInt32 *regPtr;
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
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
		UInt32 *regPtr;
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
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
		UInt32 *regPtr;
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM8(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("crc32 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38F1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		UInt32 memVal;
		UInt32 *regPtr;
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
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
		UInt32 *regPtr;
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM16(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseReg16(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("movbe "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	else if ((sess->thisStatus & 5) == 4)
	{
		UInt32 memVal;
		UInt32 *regPtr;
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseReg32(sess, reg, regNo, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("crc32 "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38F2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	if ((sess->thisStatus & 5) == 1)
	{
		UTF8Char mem[64];
		UTF8Char regs[8];
		Int32 reg;
		UInt32 memVal;
		UInt32 *regPtr;

		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
		DasmX86_32_ParseReg(sess, regs, reg, &regPtr);
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
		UInt32 memVal;
		UInt32 *regPtr;

		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &reg, &memVal, 0);
		DasmX86_32_ParseReg(sess, regs, reg, &regPtr);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("adox "));
		sess->sbuff = Text::StrConcat(sess->sbuff, regs);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f38F7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38F9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38FA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38FB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38FC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38FD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38FE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f38FF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a00(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a01(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a02(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a03(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a04(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a05(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a06(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a07(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a08(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("roundps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a09(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("roundpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a0A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("roundss "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a0B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a0C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a0D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("blendpd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a0E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pblendw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a0F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 0)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMMM64(sess, mem, &regNo);
		DasmX86_32_ParseRegMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("palignr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	else if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("palignr "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a10(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a11(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a12(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a13(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a14(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModR32M8(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a15(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a16(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pextrd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a17(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("extractps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a18(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a19(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a1A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a1B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a1C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a1D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a1E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a1F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a20(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModR32M8(sess, mem, &regNo, &memVal);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrb "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a21(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("insertps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a22(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	UInt32 memVal;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRM32(sess, mem, &regNo, &memVal, 0);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pinsrd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a23(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a24(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a25(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a26(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a27(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a28(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a29(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a2A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a2B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a2C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a2D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a2E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a2F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a30(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a31(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a32(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a33(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a34(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a35(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a36(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a37(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a38(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a39(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a3A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a3B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a3C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a3D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a3E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a3F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a40(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dpps "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a41(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("dppd "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a42(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("mpsadbw "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a43(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a44(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pclmulqdq "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a45(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a46(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a47(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a48(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a49(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a4A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a4B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a4C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a4D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a4E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a4F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a50(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a51(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a52(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a53(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a54(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a55(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a56(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a57(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a58(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a59(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a5A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a5B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a5C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a5D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a5E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a5F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a60(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpestrm "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a61(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpestri "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a62(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpistrm "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a63(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	if ((sess->thisStatus & 5) == 1)
	{
		sess->regs.EIP += 3;
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("pcmpistri "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(", "));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3a64(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a65(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a66(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a67(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a68(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a69(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a6A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a6B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a6C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a6D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a6E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a6F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a70(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a71(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a72(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a73(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a74(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a75(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a76(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a77(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a78(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a79(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a7A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a7B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a7C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a7D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a7E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a7F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a80(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a81(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a82(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a83(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a84(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a85(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a86(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a87(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a88(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a89(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a8A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a8B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a8C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a8D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a8E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a8F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a90(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a91(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a92(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a93(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a94(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a95(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a96(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a97(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a98(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a99(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a9A(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a9B(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a9C(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a9D(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a9E(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3a9F(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aA9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aAA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aAB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aAC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aAD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aAE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aAF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aB9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aBA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aBB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aBC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aBD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aBE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aBF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aC9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aCA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aCB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aCC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aCD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aCE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aCF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aD9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aDA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aDB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aDC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aDD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aDE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aDF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	UTF8Char mem[64];
	UTF8Char reg[8];
	Int32 regNo;
	sess->regs.EIP += 3;
	if ((sess->thisStatus & 5) == 1)
	{
		DasmX86_32_ParseModRMXMM128(sess, mem, &regNo);
		DasmX86_32_ParseRegXMM(reg, regNo);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("aeskeygenassist "));
		sess->sbuff = Text::StrConcat(sess->sbuff, reg);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrConcat(sess->sbuff, mem);
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC(","));
		sess->sbuff = Text::StrInt32(sess->sbuff, sess->memReader->ReadMemUInt8(sess->regs.EIP));
		sess->sbuff = Text::StrConcatC(sess->sbuff, UTF8STRC("\r\n"));
		sess->regs.EIP++;
		return true;
	}
	return false;
}

Bool __stdcall DasmX86_32_0f3aE0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aE9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aEA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aEB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aEC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aED(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aEE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aEF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF0(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF1(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF2(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF3(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF4(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF5(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF6(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF7(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF8(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aF9(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aFA(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aFB(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aFC(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aFD(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aFE(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Bool __stdcall DasmX86_32_0f3aFF(NN<Manage::DasmX86_32::DasmX86_32_Sess> sess)
{
	return false;
}

Manage::DasmX86_32::DasmX86_32()
{
	DasmX86_32_Code *codes;
	DasmX86_32_Code *codes0f;
	DasmX86_32_Code *codes0f38;
	DasmX86_32_Code *codes0f3a;
//	this->nameFunc = nameFunc;
//	this->userObj = userObj;
	codes = this->codes = MemAlloc(DasmX86_32_Code, 256);
	codes0f = this->codes0f = MemAlloc(DasmX86_32_Code, 256);
	codes0f38 = this->codes0f38 = MemAlloc(DasmX86_32_Code, 256);
	codes0f3a = this->codes0f3a = MemAlloc(DasmX86_32_Code, 256);
	codes[0x00] = DasmX86_32_00;
	codes[0x01] = DasmX86_32_01;
	codes[0x02] = DasmX86_32_02;
	codes[0x03] = DasmX86_32_03;
	codes[0x04] = DasmX86_32_04;
	codes[0x05] = DasmX86_32_05;
	codes[0x06] = DasmX86_32_06;
	codes[0x07] = DasmX86_32_07;
	codes[0x08] = DasmX86_32_08;
	codes[0x09] = DasmX86_32_09;
	codes[0x0a] = DasmX86_32_0a;
	codes[0x0b] = DasmX86_32_0b;
	codes[0x0c] = DasmX86_32_0c;
	codes[0x0d] = DasmX86_32_0d;
	codes[0x0e] = DasmX86_32_0e;
	codes[0x0f] = DasmX86_32_0f;
	codes[0x10] = DasmX86_32_10;
	codes[0x11] = DasmX86_32_11;
	codes[0x12] = DasmX86_32_12;
	codes[0x13] = DasmX86_32_13;
	codes[0x14] = DasmX86_32_14;
	codes[0x15] = DasmX86_32_15;
	codes[0x16] = DasmX86_32_16;
	codes[0x17] = DasmX86_32_17;
	codes[0x18] = DasmX86_32_18;
	codes[0x19] = DasmX86_32_19;
	codes[0x1a] = DasmX86_32_1a;
	codes[0x1b] = DasmX86_32_1b;
	codes[0x1c] = DasmX86_32_1c;
	codes[0x1d] = DasmX86_32_1d;
	codes[0x1e] = DasmX86_32_1e;
	codes[0x1f] = DasmX86_32_1f;
	codes[0x20] = DasmX86_32_20;
	codes[0x21] = DasmX86_32_21;
	codes[0x22] = DasmX86_32_22;
	codes[0x23] = DasmX86_32_23;
	codes[0x24] = DasmX86_32_24;
	codes[0x25] = DasmX86_32_25;
	codes[0x26] = DasmX86_32_26;
	codes[0x27] = DasmX86_32_27;
	codes[0x28] = DasmX86_32_28;
	codes[0x29] = DasmX86_32_29;
	codes[0x2a] = DasmX86_32_2a;
	codes[0x2b] = DasmX86_32_2b;
	codes[0x2c] = DasmX86_32_2c;
	codes[0x2d] = DasmX86_32_2d;
	codes[0x2e] = DasmX86_32_2e;
	codes[0x2f] = DasmX86_32_2f;
	codes[0x30] = DasmX86_32_30;
	codes[0x31] = DasmX86_32_31;
	codes[0x32] = DasmX86_32_32;
	codes[0x33] = DasmX86_32_33;
	codes[0x34] = DasmX86_32_34;
	codes[0x35] = DasmX86_32_35;
	codes[0x36] = DasmX86_32_36;
	codes[0x37] = DasmX86_32_37;
	codes[0x38] = DasmX86_32_38;
	codes[0x39] = DasmX86_32_39;
	codes[0x3a] = DasmX86_32_3a;
	codes[0x3b] = DasmX86_32_3b;
	codes[0x3c] = DasmX86_32_3c;
	codes[0x3d] = DasmX86_32_3d;
	codes[0x3e] = DasmX86_32_3e;
	codes[0x3f] = DasmX86_32_3f;
	codes[0x40] = DasmX86_32_40;
	codes[0x41] = DasmX86_32_41;
	codes[0x42] = DasmX86_32_42;
	codes[0x43] = DasmX86_32_43;
	codes[0x44] = DasmX86_32_44;
	codes[0x45] = DasmX86_32_45;
	codes[0x46] = DasmX86_32_46;
	codes[0x47] = DasmX86_32_47;
	codes[0x48] = DasmX86_32_48;
	codes[0x49] = DasmX86_32_49;
	codes[0x4a] = DasmX86_32_4a;
	codes[0x4b] = DasmX86_32_4b;
	codes[0x4c] = DasmX86_32_4c;
	codes[0x4d] = DasmX86_32_4d;
	codes[0x4e] = DasmX86_32_4e;
	codes[0x4f] = DasmX86_32_4f;
	codes[0x50] = DasmX86_32_50;
	codes[0x51] = DasmX86_32_51;
	codes[0x52] = DasmX86_32_52;
	codes[0x53] = DasmX86_32_53;
	codes[0x54] = DasmX86_32_54;
	codes[0x55] = DasmX86_32_55;
	codes[0x56] = DasmX86_32_56;
	codes[0x57] = DasmX86_32_57;
	codes[0x58] = DasmX86_32_58;
	codes[0x59] = DasmX86_32_59;
	codes[0x5a] = DasmX86_32_5a;
	codes[0x5b] = DasmX86_32_5b;
	codes[0x5c] = DasmX86_32_5c;
	codes[0x5d] = DasmX86_32_5d;
	codes[0x5e] = DasmX86_32_5e;
	codes[0x5f] = DasmX86_32_5f;
	codes[0x60] = DasmX86_32_60;
	codes[0x61] = DasmX86_32_61;
	codes[0x62] = DasmX86_32_62;
	codes[0x63] = DasmX86_32_63;
	codes[0x64] = DasmX86_32_64;
	codes[0x65] = DasmX86_32_65;
	codes[0x66] = DasmX86_32_66;
	codes[0x67] = DasmX86_32_67;
	codes[0x68] = DasmX86_32_68;
	codes[0x69] = DasmX86_32_69;
	codes[0x6a] = DasmX86_32_6a;
	codes[0x6b] = DasmX86_32_6b;
	codes[0x6c] = DasmX86_32_6c;
	codes[0x6d] = DasmX86_32_6d;
	codes[0x6e] = DasmX86_32_6e;
	codes[0x6f] = DasmX86_32_6f;
	codes[0x70] = DasmX86_32_70;
	codes[0x71] = DasmX86_32_71;
	codes[0x72] = DasmX86_32_72;
	codes[0x73] = DasmX86_32_73;
	codes[0x74] = DasmX86_32_74;
	codes[0x75] = DasmX86_32_75;
	codes[0x76] = DasmX86_32_76;
	codes[0x77] = DasmX86_32_77;
	codes[0x78] = DasmX86_32_78;
	codes[0x79] = DasmX86_32_79;
	codes[0x7a] = DasmX86_32_7a;
	codes[0x7b] = DasmX86_32_7b;
	codes[0x7c] = DasmX86_32_7c;
	codes[0x7d] = DasmX86_32_7d;
	codes[0x7e] = DasmX86_32_7e;
	codes[0x7f] = DasmX86_32_7f;
	codes[0x80] = DasmX86_32_80;
	codes[0x81] = DasmX86_32_81;
	codes[0x82] = DasmX86_32_82;
	codes[0x83] = DasmX86_32_83;
	codes[0x84] = DasmX86_32_84;
	codes[0x85] = DasmX86_32_85;
	codes[0x86] = DasmX86_32_86;
	codes[0x87] = DasmX86_32_87;
	codes[0x88] = DasmX86_32_88;
	codes[0x89] = DasmX86_32_89;
	codes[0x8a] = DasmX86_32_8a;
	codes[0x8b] = DasmX86_32_8b;
	codes[0x8c] = DasmX86_32_8c;
	codes[0x8d] = DasmX86_32_8d;
	codes[0x8e] = DasmX86_32_8e;
	codes[0x8f] = DasmX86_32_8f;
	codes[0x90] = DasmX86_32_90;
	codes[0x91] = DasmX86_32_91;
	codes[0x92] = DasmX86_32_92;
	codes[0x93] = DasmX86_32_93;
	codes[0x94] = DasmX86_32_94;
	codes[0x95] = DasmX86_32_95;
	codes[0x96] = DasmX86_32_96;
	codes[0x97] = DasmX86_32_97;
	codes[0x98] = DasmX86_32_98;
	codes[0x99] = DasmX86_32_99;
	codes[0x9a] = DasmX86_32_9a;
	codes[0x9b] = DasmX86_32_9b;
	codes[0x9c] = DasmX86_32_9c;
	codes[0x9d] = DasmX86_32_9d;
	codes[0x9e] = DasmX86_32_9e;
	codes[0x9f] = DasmX86_32_9f;
	codes[0xa0] = DasmX86_32_a0;
	codes[0xa1] = DasmX86_32_a1;
	codes[0xa2] = DasmX86_32_a2;
	codes[0xa3] = DasmX86_32_a3;
	codes[0xa4] = DasmX86_32_a4;
	codes[0xa5] = DasmX86_32_a5;
	codes[0xa6] = DasmX86_32_a6;
	codes[0xa7] = DasmX86_32_a7;
	codes[0xa8] = DasmX86_32_a8;
	codes[0xa9] = DasmX86_32_a9;
	codes[0xaa] = DasmX86_32_aa;
	codes[0xab] = DasmX86_32_ab;
	codes[0xac] = DasmX86_32_ac;
	codes[0xad] = DasmX86_32_ad;
	codes[0xae] = DasmX86_32_ae;
	codes[0xaf] = DasmX86_32_af;
	codes[0xb0] = DasmX86_32_b0;
	codes[0xb1] = DasmX86_32_b1;
	codes[0xb2] = DasmX86_32_b2;
	codes[0xb3] = DasmX86_32_b3;
	codes[0xb4] = DasmX86_32_b4;
	codes[0xb5] = DasmX86_32_b5;
	codes[0xb6] = DasmX86_32_b6;
	codes[0xb7] = DasmX86_32_b7;
	codes[0xb8] = DasmX86_32_b8;
	codes[0xb9] = DasmX86_32_b9;
	codes[0xba] = DasmX86_32_ba;
	codes[0xbb] = DasmX86_32_bb;
	codes[0xbc] = DasmX86_32_bc;
	codes[0xbd] = DasmX86_32_bd;
	codes[0xbe] = DasmX86_32_be;
	codes[0xbf] = DasmX86_32_bf;
	codes[0xc0] = DasmX86_32_c0;
	codes[0xc1] = DasmX86_32_c1;
	codes[0xc2] = DasmX86_32_c2;
	codes[0xc3] = DasmX86_32_c3;
	codes[0xc4] = DasmX86_32_c4;
	codes[0xc5] = DasmX86_32_c5;
	codes[0xc6] = DasmX86_32_c6;
	codes[0xc7] = DasmX86_32_c7;
	codes[0xc8] = DasmX86_32_c8;
	codes[0xc9] = DasmX86_32_c9;
	codes[0xca] = DasmX86_32_ca;
	codes[0xcb] = DasmX86_32_cb;
	codes[0xcc] = DasmX86_32_cc;
	codes[0xcd] = DasmX86_32_cd;
	codes[0xce] = DasmX86_32_ce;
	codes[0xcf] = DasmX86_32_cf;
	codes[0xd0] = DasmX86_32_d0;
	codes[0xd1] = DasmX86_32_d1;
	codes[0xd2] = DasmX86_32_d2;
	codes[0xd3] = DasmX86_32_d3;
	codes[0xd4] = DasmX86_32_d4;
	codes[0xd5] = DasmX86_32_d5;
	codes[0xd6] = DasmX86_32_d6;
	codes[0xd7] = DasmX86_32_d7;
	codes[0xd8] = DasmX86_32_d8;
	codes[0xd9] = DasmX86_32_d9;
	codes[0xda] = DasmX86_32_da;
	codes[0xdb] = DasmX86_32_db;
	codes[0xdc] = DasmX86_32_dc;
	codes[0xdd] = DasmX86_32_dd;
	codes[0xde] = DasmX86_32_de;
	codes[0xdf] = DasmX86_32_df;
	codes[0xe0] = DasmX86_32_e0;
	codes[0xe1] = DasmX86_32_e1;
	codes[0xe2] = DasmX86_32_e2;
	codes[0xe3] = DasmX86_32_e3;
	codes[0xe4] = DasmX86_32_e4;
	codes[0xe5] = DasmX86_32_e5;
	codes[0xe6] = DasmX86_32_e6;
	codes[0xe7] = DasmX86_32_e7;
	codes[0xe8] = DasmX86_32_e8;
	codes[0xe9] = DasmX86_32_e9;
	codes[0xea] = DasmX86_32_ea;
	codes[0xeb] = DasmX86_32_eb;
	codes[0xec] = DasmX86_32_ec;
	codes[0xed] = DasmX86_32_ed;
	codes[0xee] = DasmX86_32_ee;
	codes[0xef] = DasmX86_32_ef;
	codes[0xf0] = DasmX86_32_f0;
	codes[0xf1] = DasmX86_32_f1;
	codes[0xf2] = DasmX86_32_f2;
	codes[0xf3] = DasmX86_32_f3;
	codes[0xf4] = DasmX86_32_f4;
	codes[0xf5] = DasmX86_32_f5;
	codes[0xf6] = DasmX86_32_f6;
	codes[0xf7] = DasmX86_32_f7;
	codes[0xf8] = DasmX86_32_f8;
	codes[0xf9] = DasmX86_32_f9;
	codes[0xfa] = DasmX86_32_fa;
	codes[0xfb] = DasmX86_32_fb;
	codes[0xfc] = DasmX86_32_fc;
	codes[0xfd] = DasmX86_32_fd;
	codes[0xfe] = DasmX86_32_fe;
	codes[0xff] = DasmX86_32_ff;

	codes0f[0x00] = DasmX86_32_0f00;
	codes0f[0x01] = DasmX86_32_0f01;
	codes0f[0x02] = DasmX86_32_0f02;
	codes0f[0x03] = DasmX86_32_0f03;
	codes0f[0x04] = DasmX86_32_0f04;
	codes0f[0x05] = DasmX86_32_0f05;
	codes0f[0x06] = DasmX86_32_0f06;
	codes0f[0x07] = DasmX86_32_0f07;
	codes0f[0x08] = DasmX86_32_0f08;
	codes0f[0x09] = DasmX86_32_0f09;
	codes0f[0x0A] = DasmX86_32_0f0A;
	codes0f[0x0B] = DasmX86_32_0f0B;
	codes0f[0x0C] = DasmX86_32_0f0C;
	codes0f[0x0D] = DasmX86_32_0f0D;
	codes0f[0x0E] = DasmX86_32_0f0E;
	codes0f[0x0F] = DasmX86_32_0f0F;
	codes0f[0x10] = DasmX86_32_0f10;
	codes0f[0x11] = DasmX86_32_0f11;
	codes0f[0x12] = DasmX86_32_0f12;
	codes0f[0x13] = DasmX86_32_0f13;
	codes0f[0x14] = DasmX86_32_0f14;
	codes0f[0x15] = DasmX86_32_0f15;
	codes0f[0x16] = DasmX86_32_0f16;
	codes0f[0x17] = DasmX86_32_0f17;
	codes0f[0x18] = DasmX86_32_0f18;
	codes0f[0x19] = DasmX86_32_0f19;
	codes0f[0x1A] = DasmX86_32_0f1A;
	codes0f[0x1B] = DasmX86_32_0f1B;
	codes0f[0x1C] = DasmX86_32_0f1C;
	codes0f[0x1D] = DasmX86_32_0f1D;
	codes0f[0x1E] = DasmX86_32_0f1E;
	codes0f[0x1F] = DasmX86_32_0f1F;
	codes0f[0x20] = DasmX86_32_0f20;
	codes0f[0x21] = DasmX86_32_0f21;
	codes0f[0x22] = DasmX86_32_0f22;
	codes0f[0x23] = DasmX86_32_0f23;
	codes0f[0x24] = DasmX86_32_0f24;
	codes0f[0x25] = DasmX86_32_0f25;
	codes0f[0x26] = DasmX86_32_0f26;
	codes0f[0x27] = DasmX86_32_0f27;
	codes0f[0x28] = DasmX86_32_0f28;
	codes0f[0x29] = DasmX86_32_0f29;
	codes0f[0x2A] = DasmX86_32_0f2A;
	codes0f[0x2B] = DasmX86_32_0f2B;
	codes0f[0x2C] = DasmX86_32_0f2C;
	codes0f[0x2D] = DasmX86_32_0f2D;
	codes0f[0x2E] = DasmX86_32_0f2E;
	codes0f[0x2F] = DasmX86_32_0f2F;
	codes0f[0x30] = DasmX86_32_0f30;
	codes0f[0x31] = DasmX86_32_0f31;
	codes0f[0x32] = DasmX86_32_0f32;
	codes0f[0x33] = DasmX86_32_0f33;
	codes0f[0x34] = DasmX86_32_0f34;
	codes0f[0x35] = DasmX86_32_0f35;
	codes0f[0x36] = DasmX86_32_0f36;
	codes0f[0x37] = DasmX86_32_0f37;
	codes0f[0x38] = DasmX86_32_0f38;
	codes0f[0x39] = DasmX86_32_0f39;
	codes0f[0x3A] = DasmX86_32_0f3A;
	codes0f[0x3B] = DasmX86_32_0f3B;
	codes0f[0x3C] = DasmX86_32_0f3C;
	codes0f[0x3D] = DasmX86_32_0f3D;
	codes0f[0x3E] = DasmX86_32_0f3E;
	codes0f[0x3F] = DasmX86_32_0f3F;
	codes0f[0x40] = DasmX86_32_0f40;
	codes0f[0x41] = DasmX86_32_0f41;
	codes0f[0x42] = DasmX86_32_0f42;
	codes0f[0x43] = DasmX86_32_0f43;
	codes0f[0x44] = DasmX86_32_0f44;
	codes0f[0x45] = DasmX86_32_0f45;
	codes0f[0x46] = DasmX86_32_0f46;
	codes0f[0x47] = DasmX86_32_0f47;
	codes0f[0x48] = DasmX86_32_0f48;
	codes0f[0x49] = DasmX86_32_0f49;
	codes0f[0x4A] = DasmX86_32_0f4A;
	codes0f[0x4B] = DasmX86_32_0f4B;
	codes0f[0x4C] = DasmX86_32_0f4C;
	codes0f[0x4D] = DasmX86_32_0f4D;
	codes0f[0x4E] = DasmX86_32_0f4E;
	codes0f[0x4F] = DasmX86_32_0f4F;
	codes0f[0x50] = DasmX86_32_0f50;
	codes0f[0x51] = DasmX86_32_0f51;
	codes0f[0x52] = DasmX86_32_0f52;
	codes0f[0x53] = DasmX86_32_0f53;
	codes0f[0x54] = DasmX86_32_0f54;
	codes0f[0x55] = DasmX86_32_0f55;
	codes0f[0x56] = DasmX86_32_0f56;
	codes0f[0x57] = DasmX86_32_0f57;
	codes0f[0x58] = DasmX86_32_0f58;
	codes0f[0x59] = DasmX86_32_0f59;
	codes0f[0x5A] = DasmX86_32_0f5A;
	codes0f[0x5B] = DasmX86_32_0f5B;
	codes0f[0x5C] = DasmX86_32_0f5C;
	codes0f[0x5D] = DasmX86_32_0f5D;
	codes0f[0x5E] = DasmX86_32_0f5E;
	codes0f[0x5F] = DasmX86_32_0f5F;
	codes0f[0x60] = DasmX86_32_0f60;
	codes0f[0x61] = DasmX86_32_0f61;
	codes0f[0x62] = DasmX86_32_0f62;
	codes0f[0x63] = DasmX86_32_0f63;
	codes0f[0x64] = DasmX86_32_0f64;
	codes0f[0x65] = DasmX86_32_0f65;
	codes0f[0x66] = DasmX86_32_0f66;
	codes0f[0x67] = DasmX86_32_0f67;
	codes0f[0x68] = DasmX86_32_0f68;
	codes0f[0x69] = DasmX86_32_0f69;
	codes0f[0x6A] = DasmX86_32_0f6A;
	codes0f[0x6B] = DasmX86_32_0f6B;
	codes0f[0x6C] = DasmX86_32_0f6C;
	codes0f[0x6D] = DasmX86_32_0f6D;
	codes0f[0x6E] = DasmX86_32_0f6E;
	codes0f[0x6F] = DasmX86_32_0f6F;
	codes0f[0x70] = DasmX86_32_0f70;
	codes0f[0x71] = DasmX86_32_0f71;
	codes0f[0x72] = DasmX86_32_0f72;
	codes0f[0x73] = DasmX86_32_0f73;
	codes0f[0x74] = DasmX86_32_0f74;
	codes0f[0x75] = DasmX86_32_0f75;
	codes0f[0x76] = DasmX86_32_0f76;
	codes0f[0x77] = DasmX86_32_0f77;
	codes0f[0x78] = DasmX86_32_0f78;
	codes0f[0x79] = DasmX86_32_0f79;
	codes0f[0x7A] = DasmX86_32_0f7A;
	codes0f[0x7B] = DasmX86_32_0f7B;
	codes0f[0x7C] = DasmX86_32_0f7C;
	codes0f[0x7D] = DasmX86_32_0f7D;
	codes0f[0x7E] = DasmX86_32_0f7E;
	codes0f[0x7F] = DasmX86_32_0f7F;
	codes0f[0x80] = DasmX86_32_0f80;
	codes0f[0x81] = DasmX86_32_0f81;
	codes0f[0x82] = DasmX86_32_0f82;
	codes0f[0x83] = DasmX86_32_0f83;
	codes0f[0x84] = DasmX86_32_0f84;
	codes0f[0x85] = DasmX86_32_0f85;
	codes0f[0x86] = DasmX86_32_0f86;
	codes0f[0x87] = DasmX86_32_0f87;
	codes0f[0x88] = DasmX86_32_0f88;
	codes0f[0x89] = DasmX86_32_0f89;
	codes0f[0x8A] = DasmX86_32_0f8A;
	codes0f[0x8B] = DasmX86_32_0f8B;
	codes0f[0x8C] = DasmX86_32_0f8C;
	codes0f[0x8D] = DasmX86_32_0f8D;
	codes0f[0x8E] = DasmX86_32_0f8E;
	codes0f[0x8F] = DasmX86_32_0f8F;
	codes0f[0x90] = DasmX86_32_0f90;
	codes0f[0x91] = DasmX86_32_0f91;
	codes0f[0x92] = DasmX86_32_0f92;
	codes0f[0x93] = DasmX86_32_0f93;
	codes0f[0x94] = DasmX86_32_0f94;
	codes0f[0x95] = DasmX86_32_0f95;
	codes0f[0x96] = DasmX86_32_0f96;
	codes0f[0x97] = DasmX86_32_0f97;
	codes0f[0x98] = DasmX86_32_0f98;
	codes0f[0x99] = DasmX86_32_0f99;
	codes0f[0x9A] = DasmX86_32_0f9A;
	codes0f[0x9B] = DasmX86_32_0f9B;
	codes0f[0x9C] = DasmX86_32_0f9C;
	codes0f[0x9D] = DasmX86_32_0f9D;
	codes0f[0x9E] = DasmX86_32_0f9E;
	codes0f[0x9F] = DasmX86_32_0f9F;
	codes0f[0xA0] = DasmX86_32_0fA0;
	codes0f[0xA1] = DasmX86_32_0fA1;
	codes0f[0xA2] = DasmX86_32_0fA2;
	codes0f[0xA3] = DasmX86_32_0fA3;
	codes0f[0xA4] = DasmX86_32_0fA4;
	codes0f[0xA5] = DasmX86_32_0fA5;
	codes0f[0xA6] = DasmX86_32_0fA6;
	codes0f[0xA7] = DasmX86_32_0fA7;
	codes0f[0xA8] = DasmX86_32_0fA8;
	codes0f[0xA9] = DasmX86_32_0fA9;
	codes0f[0xAA] = DasmX86_32_0fAA;
	codes0f[0xAB] = DasmX86_32_0fAB;
	codes0f[0xAC] = DasmX86_32_0fAC;
	codes0f[0xAD] = DasmX86_32_0fAD;
	codes0f[0xAE] = DasmX86_32_0fAE;
	codes0f[0xAF] = DasmX86_32_0fAF;
	codes0f[0xB0] = DasmX86_32_0fB0;
	codes0f[0xB1] = DasmX86_32_0fB1;
	codes0f[0xB2] = DasmX86_32_0fB2;
	codes0f[0xB3] = DasmX86_32_0fB3;
	codes0f[0xB4] = DasmX86_32_0fB4;
	codes0f[0xB5] = DasmX86_32_0fB5;
	codes0f[0xB6] = DasmX86_32_0fB6;
	codes0f[0xB7] = DasmX86_32_0fB7;
	codes0f[0xB8] = DasmX86_32_0fB8;
	codes0f[0xB9] = DasmX86_32_0fB9;
	codes0f[0xBA] = DasmX86_32_0fBA;
	codes0f[0xBB] = DasmX86_32_0fBB;
	codes0f[0xBC] = DasmX86_32_0fBC;
	codes0f[0xBD] = DasmX86_32_0fBD;
	codes0f[0xBE] = DasmX86_32_0fBE;
	codes0f[0xBF] = DasmX86_32_0fBF;
	codes0f[0xC0] = DasmX86_32_0fC0;
	codes0f[0xC1] = DasmX86_32_0fC1;
	codes0f[0xC2] = DasmX86_32_0fC2;
	codes0f[0xC3] = DasmX86_32_0fC3;
	codes0f[0xC4] = DasmX86_32_0fC4;
	codes0f[0xC5] = DasmX86_32_0fC5;
	codes0f[0xC6] = DasmX86_32_0fC6;
	codes0f[0xC7] = DasmX86_32_0fC7;
	codes0f[0xC8] = DasmX86_32_0fC8;
	codes0f[0xC9] = DasmX86_32_0fC9;
	codes0f[0xCA] = DasmX86_32_0fCA;
	codes0f[0xCB] = DasmX86_32_0fCB;
	codes0f[0xCC] = DasmX86_32_0fCC;
	codes0f[0xCD] = DasmX86_32_0fCD;
	codes0f[0xCE] = DasmX86_32_0fCE;
	codes0f[0xCF] = DasmX86_32_0fCF;
	codes0f[0xD0] = DasmX86_32_0fD0;
	codes0f[0xD1] = DasmX86_32_0fD1;
	codes0f[0xD2] = DasmX86_32_0fD2;
	codes0f[0xD3] = DasmX86_32_0fD3;
	codes0f[0xD4] = DasmX86_32_0fD4;
	codes0f[0xD5] = DasmX86_32_0fD5;
	codes0f[0xD6] = DasmX86_32_0fD6;
	codes0f[0xD7] = DasmX86_32_0fD7;
	codes0f[0xD8] = DasmX86_32_0fD8;
	codes0f[0xD9] = DasmX86_32_0fD9;
	codes0f[0xDA] = DasmX86_32_0fDA;
	codes0f[0xDB] = DasmX86_32_0fDB;
	codes0f[0xDC] = DasmX86_32_0fDC;
	codes0f[0xDD] = DasmX86_32_0fDD;
	codes0f[0xDE] = DasmX86_32_0fDE;
	codes0f[0xDF] = DasmX86_32_0fDF;
	codes0f[0xE0] = DasmX86_32_0fE0;
	codes0f[0xE1] = DasmX86_32_0fE1;
	codes0f[0xE2] = DasmX86_32_0fE2;
	codes0f[0xE3] = DasmX86_32_0fE3;
	codes0f[0xE4] = DasmX86_32_0fE4;
	codes0f[0xE5] = DasmX86_32_0fE5;
	codes0f[0xE6] = DasmX86_32_0fE6;
	codes0f[0xE7] = DasmX86_32_0fE7;
	codes0f[0xE8] = DasmX86_32_0fE8;
	codes0f[0xE9] = DasmX86_32_0fE9;
	codes0f[0xEA] = DasmX86_32_0fEA;
	codes0f[0xEB] = DasmX86_32_0fEB;
	codes0f[0xEC] = DasmX86_32_0fEC;
	codes0f[0xED] = DasmX86_32_0fED;
	codes0f[0xEE] = DasmX86_32_0fEE;
	codes0f[0xEF] = DasmX86_32_0fEF;
	codes0f[0xF0] = DasmX86_32_0fF0;
	codes0f[0xF1] = DasmX86_32_0fF1;
	codes0f[0xF2] = DasmX86_32_0fF2;
	codes0f[0xF3] = DasmX86_32_0fF3;
	codes0f[0xF4] = DasmX86_32_0fF4;
	codes0f[0xF5] = DasmX86_32_0fF5;
	codes0f[0xF6] = DasmX86_32_0fF6;
	codes0f[0xF7] = DasmX86_32_0fF7;
	codes0f[0xF8] = DasmX86_32_0fF8;
	codes0f[0xF9] = DasmX86_32_0fF9;
	codes0f[0xFA] = DasmX86_32_0fFA;
	codes0f[0xFB] = DasmX86_32_0fFB;
	codes0f[0xFC] = DasmX86_32_0fFC;
	codes0f[0xFD] = DasmX86_32_0fFD;
	codes0f[0xFE] = DasmX86_32_0fFE;
	codes0f[0xFF] = DasmX86_32_0fFF;

	codes0f38[0x00] = DasmX86_32_0f3800;
	codes0f38[0x01] = DasmX86_32_0f3801;
	codes0f38[0x02] = DasmX86_32_0f3802;
	codes0f38[0x03] = DasmX86_32_0f3803;
	codes0f38[0x04] = DasmX86_32_0f3804;
	codes0f38[0x05] = DasmX86_32_0f3805;
	codes0f38[0x06] = DasmX86_32_0f3806;
	codes0f38[0x07] = DasmX86_32_0f3807;
	codes0f38[0x08] = DasmX86_32_0f3808;
	codes0f38[0x09] = DasmX86_32_0f3809;
	codes0f38[0x0A] = DasmX86_32_0f380A;
	codes0f38[0x0B] = DasmX86_32_0f380B;
	codes0f38[0x0C] = DasmX86_32_0f380C;
	codes0f38[0x0D] = DasmX86_32_0f380D;
	codes0f38[0x0E] = DasmX86_32_0f380E;
	codes0f38[0x0F] = DasmX86_32_0f380F;
	codes0f38[0x10] = DasmX86_32_0f3810;
	codes0f38[0x11] = DasmX86_32_0f3811;
	codes0f38[0x12] = DasmX86_32_0f3812;
	codes0f38[0x13] = DasmX86_32_0f3813;
	codes0f38[0x14] = DasmX86_32_0f3814;
	codes0f38[0x15] = DasmX86_32_0f3815;
	codes0f38[0x16] = DasmX86_32_0f3816;
	codes0f38[0x17] = DasmX86_32_0f3817;
	codes0f38[0x18] = DasmX86_32_0f3818;
	codes0f38[0x19] = DasmX86_32_0f3819;
	codes0f38[0x1A] = DasmX86_32_0f381A;
	codes0f38[0x1B] = DasmX86_32_0f381B;
	codes0f38[0x1C] = DasmX86_32_0f381C;
	codes0f38[0x1D] = DasmX86_32_0f381D;
	codes0f38[0x1E] = DasmX86_32_0f381E;
	codes0f38[0x1F] = DasmX86_32_0f381F;
	codes0f38[0x20] = DasmX86_32_0f3820;
	codes0f38[0x21] = DasmX86_32_0f3821;
	codes0f38[0x22] = DasmX86_32_0f3822;
	codes0f38[0x23] = DasmX86_32_0f3823;
	codes0f38[0x24] = DasmX86_32_0f3824;
	codes0f38[0x25] = DasmX86_32_0f3825;
	codes0f38[0x26] = DasmX86_32_0f3826;
	codes0f38[0x27] = DasmX86_32_0f3827;
	codes0f38[0x28] = DasmX86_32_0f3828;
	codes0f38[0x29] = DasmX86_32_0f3829;
	codes0f38[0x2A] = DasmX86_32_0f382A;
	codes0f38[0x2B] = DasmX86_32_0f382B;
	codes0f38[0x2C] = DasmX86_32_0f382C;
	codes0f38[0x2D] = DasmX86_32_0f382D;
	codes0f38[0x2E] = DasmX86_32_0f382E;
	codes0f38[0x2F] = DasmX86_32_0f382F;
	codes0f38[0x30] = DasmX86_32_0f3830;
	codes0f38[0x31] = DasmX86_32_0f3831;
	codes0f38[0x32] = DasmX86_32_0f3832;
	codes0f38[0x33] = DasmX86_32_0f3833;
	codes0f38[0x34] = DasmX86_32_0f3834;
	codes0f38[0x35] = DasmX86_32_0f3835;
	codes0f38[0x36] = DasmX86_32_0f3836;
	codes0f38[0x37] = DasmX86_32_0f3837;
	codes0f38[0x38] = DasmX86_32_0f3838;
	codes0f38[0x39] = DasmX86_32_0f3839;
	codes0f38[0x3A] = DasmX86_32_0f383A;
	codes0f38[0x3B] = DasmX86_32_0f383B;
	codes0f38[0x3C] = DasmX86_32_0f383C;
	codes0f38[0x3D] = DasmX86_32_0f383D;
	codes0f38[0x3E] = DasmX86_32_0f383E;
	codes0f38[0x3F] = DasmX86_32_0f383F;
	codes0f38[0x40] = DasmX86_32_0f3840;
	codes0f38[0x41] = DasmX86_32_0f3841;
	codes0f38[0x42] = DasmX86_32_0f3842;
	codes0f38[0x43] = DasmX86_32_0f3843;
	codes0f38[0x44] = DasmX86_32_0f3844;
	codes0f38[0x45] = DasmX86_32_0f3845;
	codes0f38[0x46] = DasmX86_32_0f3846;
	codes0f38[0x47] = DasmX86_32_0f3847;
	codes0f38[0x48] = DasmX86_32_0f3848;
	codes0f38[0x49] = DasmX86_32_0f3849;
	codes0f38[0x4A] = DasmX86_32_0f384A;
	codes0f38[0x4B] = DasmX86_32_0f384B;
	codes0f38[0x4C] = DasmX86_32_0f384C;
	codes0f38[0x4D] = DasmX86_32_0f384D;
	codes0f38[0x4E] = DasmX86_32_0f384E;
	codes0f38[0x4F] = DasmX86_32_0f384F;
	codes0f38[0x50] = DasmX86_32_0f3850;
	codes0f38[0x51] = DasmX86_32_0f3851;
	codes0f38[0x52] = DasmX86_32_0f3852;
	codes0f38[0x53] = DasmX86_32_0f3853;
	codes0f38[0x54] = DasmX86_32_0f3854;
	codes0f38[0x55] = DasmX86_32_0f3855;
	codes0f38[0x56] = DasmX86_32_0f3856;
	codes0f38[0x57] = DasmX86_32_0f3857;
	codes0f38[0x58] = DasmX86_32_0f3858;
	codes0f38[0x59] = DasmX86_32_0f3859;
	codes0f38[0x5A] = DasmX86_32_0f385A;
	codes0f38[0x5B] = DasmX86_32_0f385B;
	codes0f38[0x5C] = DasmX86_32_0f385C;
	codes0f38[0x5D] = DasmX86_32_0f385D;
	codes0f38[0x5E] = DasmX86_32_0f385E;
	codes0f38[0x5F] = DasmX86_32_0f385F;
	codes0f38[0x60] = DasmX86_32_0f3860;
	codes0f38[0x61] = DasmX86_32_0f3861;
	codes0f38[0x62] = DasmX86_32_0f3862;
	codes0f38[0x63] = DasmX86_32_0f3863;
	codes0f38[0x64] = DasmX86_32_0f3864;
	codes0f38[0x65] = DasmX86_32_0f3865;
	codes0f38[0x66] = DasmX86_32_0f3866;
	codes0f38[0x67] = DasmX86_32_0f3867;
	codes0f38[0x68] = DasmX86_32_0f3868;
	codes0f38[0x69] = DasmX86_32_0f3869;
	codes0f38[0x6A] = DasmX86_32_0f386A;
	codes0f38[0x6B] = DasmX86_32_0f386B;
	codes0f38[0x6C] = DasmX86_32_0f386C;
	codes0f38[0x6D] = DasmX86_32_0f386D;
	codes0f38[0x6E] = DasmX86_32_0f386E;
	codes0f38[0x6F] = DasmX86_32_0f386F;
	codes0f38[0x70] = DasmX86_32_0f3870;
	codes0f38[0x71] = DasmX86_32_0f3871;
	codes0f38[0x72] = DasmX86_32_0f3872;
	codes0f38[0x73] = DasmX86_32_0f3873;
	codes0f38[0x74] = DasmX86_32_0f3874;
	codes0f38[0x75] = DasmX86_32_0f3875;
	codes0f38[0x76] = DasmX86_32_0f3876;
	codes0f38[0x77] = DasmX86_32_0f3877;
	codes0f38[0x78] = DasmX86_32_0f3878;
	codes0f38[0x79] = DasmX86_32_0f3879;
	codes0f38[0x7A] = DasmX86_32_0f387A;
	codes0f38[0x7B] = DasmX86_32_0f387B;
	codes0f38[0x7C] = DasmX86_32_0f387C;
	codes0f38[0x7D] = DasmX86_32_0f387D;
	codes0f38[0x7E] = DasmX86_32_0f387E;
	codes0f38[0x7F] = DasmX86_32_0f387F;
	codes0f38[0x80] = DasmX86_32_0f3880;
	codes0f38[0x81] = DasmX86_32_0f3881;
	codes0f38[0x82] = DasmX86_32_0f3882;
	codes0f38[0x83] = DasmX86_32_0f3883;
	codes0f38[0x84] = DasmX86_32_0f3884;
	codes0f38[0x85] = DasmX86_32_0f3885;
	codes0f38[0x86] = DasmX86_32_0f3886;
	codes0f38[0x87] = DasmX86_32_0f3887;
	codes0f38[0x88] = DasmX86_32_0f3888;
	codes0f38[0x89] = DasmX86_32_0f3889;
	codes0f38[0x8A] = DasmX86_32_0f388A;
	codes0f38[0x8B] = DasmX86_32_0f388B;
	codes0f38[0x8C] = DasmX86_32_0f388C;
	codes0f38[0x8D] = DasmX86_32_0f388D;
	codes0f38[0x8E] = DasmX86_32_0f388E;
	codes0f38[0x8F] = DasmX86_32_0f388F;
	codes0f38[0x90] = DasmX86_32_0f3890;
	codes0f38[0x91] = DasmX86_32_0f3891;
	codes0f38[0x92] = DasmX86_32_0f3892;
	codes0f38[0x93] = DasmX86_32_0f3893;
	codes0f38[0x94] = DasmX86_32_0f3894;
	codes0f38[0x95] = DasmX86_32_0f3895;
	codes0f38[0x96] = DasmX86_32_0f3896;
	codes0f38[0x97] = DasmX86_32_0f3897;
	codes0f38[0x98] = DasmX86_32_0f3898;
	codes0f38[0x99] = DasmX86_32_0f3899;
	codes0f38[0x9A] = DasmX86_32_0f389A;
	codes0f38[0x9B] = DasmX86_32_0f389B;
	codes0f38[0x9C] = DasmX86_32_0f389C;
	codes0f38[0x9D] = DasmX86_32_0f389D;
	codes0f38[0x9E] = DasmX86_32_0f389E;
	codes0f38[0x9F] = DasmX86_32_0f389F;
	codes0f38[0xA0] = DasmX86_32_0f38A0;
	codes0f38[0xA1] = DasmX86_32_0f38A1;
	codes0f38[0xA2] = DasmX86_32_0f38A2;
	codes0f38[0xA3] = DasmX86_32_0f38A3;
	codes0f38[0xA4] = DasmX86_32_0f38A4;
	codes0f38[0xA5] = DasmX86_32_0f38A5;
	codes0f38[0xA6] = DasmX86_32_0f38A6;
	codes0f38[0xA7] = DasmX86_32_0f38A7;
	codes0f38[0xA8] = DasmX86_32_0f38A8;
	codes0f38[0xA9] = DasmX86_32_0f38A9;
	codes0f38[0xAA] = DasmX86_32_0f38AA;
	codes0f38[0xAB] = DasmX86_32_0f38AB;
	codes0f38[0xAC] = DasmX86_32_0f38AC;
	codes0f38[0xAD] = DasmX86_32_0f38AD;
	codes0f38[0xAE] = DasmX86_32_0f38AE;
	codes0f38[0xAF] = DasmX86_32_0f38AF;
	codes0f38[0xB0] = DasmX86_32_0f38B0;
	codes0f38[0xB1] = DasmX86_32_0f38B1;
	codes0f38[0xB2] = DasmX86_32_0f38B2;
	codes0f38[0xB3] = DasmX86_32_0f38B3;
	codes0f38[0xB4] = DasmX86_32_0f38B4;
	codes0f38[0xB5] = DasmX86_32_0f38B5;
	codes0f38[0xB6] = DasmX86_32_0f38B6;
	codes0f38[0xB7] = DasmX86_32_0f38B7;
	codes0f38[0xB8] = DasmX86_32_0f38B8;
	codes0f38[0xB9] = DasmX86_32_0f38B9;
	codes0f38[0xBA] = DasmX86_32_0f38BA;
	codes0f38[0xBB] = DasmX86_32_0f38BB;
	codes0f38[0xBC] = DasmX86_32_0f38BC;
	codes0f38[0xBD] = DasmX86_32_0f38BD;
	codes0f38[0xBE] = DasmX86_32_0f38BE;
	codes0f38[0xBF] = DasmX86_32_0f38BF;
	codes0f38[0xC0] = DasmX86_32_0f38C0;
	codes0f38[0xC1] = DasmX86_32_0f38C1;
	codes0f38[0xC2] = DasmX86_32_0f38C2;
	codes0f38[0xC3] = DasmX86_32_0f38C3;
	codes0f38[0xC4] = DasmX86_32_0f38C4;
	codes0f38[0xC5] = DasmX86_32_0f38C5;
	codes0f38[0xC6] = DasmX86_32_0f38C6;
	codes0f38[0xC7] = DasmX86_32_0f38C7;
	codes0f38[0xC8] = DasmX86_32_0f38C8;
	codes0f38[0xC9] = DasmX86_32_0f38C9;
	codes0f38[0xCA] = DasmX86_32_0f38CA;
	codes0f38[0xCB] = DasmX86_32_0f38CB;
	codes0f38[0xCC] = DasmX86_32_0f38CC;
	codes0f38[0xCD] = DasmX86_32_0f38CD;
	codes0f38[0xCE] = DasmX86_32_0f38CE;
	codes0f38[0xCF] = DasmX86_32_0f38CF;
	codes0f38[0xD0] = DasmX86_32_0f38D0;
	codes0f38[0xD1] = DasmX86_32_0f38D1;
	codes0f38[0xD2] = DasmX86_32_0f38D2;
	codes0f38[0xD3] = DasmX86_32_0f38D3;
	codes0f38[0xD4] = DasmX86_32_0f38D4;
	codes0f38[0xD5] = DasmX86_32_0f38D5;
	codes0f38[0xD6] = DasmX86_32_0f38D6;
	codes0f38[0xD7] = DasmX86_32_0f38D7;
	codes0f38[0xD8] = DasmX86_32_0f38D8;
	codes0f38[0xD9] = DasmX86_32_0f38D9;
	codes0f38[0xDA] = DasmX86_32_0f38DA;
	codes0f38[0xDB] = DasmX86_32_0f38DB;
	codes0f38[0xDC] = DasmX86_32_0f38DC;
	codes0f38[0xDD] = DasmX86_32_0f38DD;
	codes0f38[0xDE] = DasmX86_32_0f38DE;
	codes0f38[0xDF] = DasmX86_32_0f38DF;
	codes0f38[0xE0] = DasmX86_32_0f38E0;
	codes0f38[0xE1] = DasmX86_32_0f38E1;
	codes0f38[0xE2] = DasmX86_32_0f38E2;
	codes0f38[0xE3] = DasmX86_32_0f38E3;
	codes0f38[0xE4] = DasmX86_32_0f38E4;
	codes0f38[0xE5] = DasmX86_32_0f38E5;
	codes0f38[0xE6] = DasmX86_32_0f38E6;
	codes0f38[0xE7] = DasmX86_32_0f38E7;
	codes0f38[0xE8] = DasmX86_32_0f38E8;
	codes0f38[0xE9] = DasmX86_32_0f38E9;
	codes0f38[0xEA] = DasmX86_32_0f38EA;
	codes0f38[0xEB] = DasmX86_32_0f38EB;
	codes0f38[0xEC] = DasmX86_32_0f38EC;
	codes0f38[0xED] = DasmX86_32_0f38ED;
	codes0f38[0xEE] = DasmX86_32_0f38EE;
	codes0f38[0xEF] = DasmX86_32_0f38EF;
	codes0f38[0xF0] = DasmX86_32_0f38F0;
	codes0f38[0xF1] = DasmX86_32_0f38F1;
	codes0f38[0xF2] = DasmX86_32_0f38F2;
	codes0f38[0xF3] = DasmX86_32_0f38F3;
	codes0f38[0xF4] = DasmX86_32_0f38F4;
	codes0f38[0xF5] = DasmX86_32_0f38F5;
	codes0f38[0xF6] = DasmX86_32_0f38F6;
	codes0f38[0xF7] = DasmX86_32_0f38F7;
	codes0f38[0xF8] = DasmX86_32_0f38F8;
	codes0f38[0xF9] = DasmX86_32_0f38F9;
	codes0f38[0xFA] = DasmX86_32_0f38FA;
	codes0f38[0xFB] = DasmX86_32_0f38FB;
	codes0f38[0xFC] = DasmX86_32_0f38FC;
	codes0f38[0xFD] = DasmX86_32_0f38FD;
	codes0f38[0xFE] = DasmX86_32_0f38FE;
	codes0f38[0xFF] = DasmX86_32_0f38FF;

	codes0f3a[0x00] = DasmX86_32_0f3a00;
	codes0f3a[0x01] = DasmX86_32_0f3a01;
	codes0f3a[0x02] = DasmX86_32_0f3a02;
	codes0f3a[0x03] = DasmX86_32_0f3a03;
	codes0f3a[0x04] = DasmX86_32_0f3a04;
	codes0f3a[0x05] = DasmX86_32_0f3a05;
	codes0f3a[0x06] = DasmX86_32_0f3a06;
	codes0f3a[0x07] = DasmX86_32_0f3a07;
	codes0f3a[0x08] = DasmX86_32_0f3a08;
	codes0f3a[0x09] = DasmX86_32_0f3a09;
	codes0f3a[0x0A] = DasmX86_32_0f3a0A;
	codes0f3a[0x0B] = DasmX86_32_0f3a0B;
	codes0f3a[0x0C] = DasmX86_32_0f3a0C;
	codes0f3a[0x0D] = DasmX86_32_0f3a0D;
	codes0f3a[0x0E] = DasmX86_32_0f3a0E;
	codes0f3a[0x0F] = DasmX86_32_0f3a0F;
	codes0f3a[0x10] = DasmX86_32_0f3a10;
	codes0f3a[0x11] = DasmX86_32_0f3a11;
	codes0f3a[0x12] = DasmX86_32_0f3a12;
	codes0f3a[0x13] = DasmX86_32_0f3a13;
	codes0f3a[0x14] = DasmX86_32_0f3a14;
	codes0f3a[0x15] = DasmX86_32_0f3a15;
	codes0f3a[0x16] = DasmX86_32_0f3a16;
	codes0f3a[0x17] = DasmX86_32_0f3a17;
	codes0f3a[0x18] = DasmX86_32_0f3a18;
	codes0f3a[0x19] = DasmX86_32_0f3a19;
	codes0f3a[0x1A] = DasmX86_32_0f3a1A;
	codes0f3a[0x1B] = DasmX86_32_0f3a1B;
	codes0f3a[0x1C] = DasmX86_32_0f3a1C;
	codes0f3a[0x1D] = DasmX86_32_0f3a1D;
	codes0f3a[0x1E] = DasmX86_32_0f3a1E;
	codes0f3a[0x1F] = DasmX86_32_0f3a1F;
	codes0f3a[0x20] = DasmX86_32_0f3a20;
	codes0f3a[0x21] = DasmX86_32_0f3a21;
	codes0f3a[0x22] = DasmX86_32_0f3a22;
	codes0f3a[0x23] = DasmX86_32_0f3a23;
	codes0f3a[0x24] = DasmX86_32_0f3a24;
	codes0f3a[0x25] = DasmX86_32_0f3a25;
	codes0f3a[0x26] = DasmX86_32_0f3a26;
	codes0f3a[0x27] = DasmX86_32_0f3a27;
	codes0f3a[0x28] = DasmX86_32_0f3a28;
	codes0f3a[0x29] = DasmX86_32_0f3a29;
	codes0f3a[0x2A] = DasmX86_32_0f3a2A;
	codes0f3a[0x2B] = DasmX86_32_0f3a2B;
	codes0f3a[0x2C] = DasmX86_32_0f3a2C;
	codes0f3a[0x2D] = DasmX86_32_0f3a2D;
	codes0f3a[0x2E] = DasmX86_32_0f3a2E;
	codes0f3a[0x2F] = DasmX86_32_0f3a2F;
	codes0f3a[0x30] = DasmX86_32_0f3a30;
	codes0f3a[0x31] = DasmX86_32_0f3a31;
	codes0f3a[0x32] = DasmX86_32_0f3a32;
	codes0f3a[0x33] = DasmX86_32_0f3a33;
	codes0f3a[0x34] = DasmX86_32_0f3a34;
	codes0f3a[0x35] = DasmX86_32_0f3a35;
	codes0f3a[0x36] = DasmX86_32_0f3a36;
	codes0f3a[0x37] = DasmX86_32_0f3a37;
	codes0f3a[0x38] = DasmX86_32_0f3a38;
	codes0f3a[0x39] = DasmX86_32_0f3a39;
	codes0f3a[0x3A] = DasmX86_32_0f3a3A;
	codes0f3a[0x3B] = DasmX86_32_0f3a3B;
	codes0f3a[0x3C] = DasmX86_32_0f3a3C;
	codes0f3a[0x3D] = DasmX86_32_0f3a3D;
	codes0f3a[0x3E] = DasmX86_32_0f3a3E;
	codes0f3a[0x3F] = DasmX86_32_0f3a3F;
	codes0f3a[0x40] = DasmX86_32_0f3a40;
	codes0f3a[0x41] = DasmX86_32_0f3a41;
	codes0f3a[0x42] = DasmX86_32_0f3a42;
	codes0f3a[0x43] = DasmX86_32_0f3a43;
	codes0f3a[0x44] = DasmX86_32_0f3a44;
	codes0f3a[0x45] = DasmX86_32_0f3a45;
	codes0f3a[0x46] = DasmX86_32_0f3a46;
	codes0f3a[0x47] = DasmX86_32_0f3a47;
	codes0f3a[0x48] = DasmX86_32_0f3a48;
	codes0f3a[0x49] = DasmX86_32_0f3a49;
	codes0f3a[0x4A] = DasmX86_32_0f3a4A;
	codes0f3a[0x4B] = DasmX86_32_0f3a4B;
	codes0f3a[0x4C] = DasmX86_32_0f3a4C;
	codes0f3a[0x4D] = DasmX86_32_0f3a4D;
	codes0f3a[0x4E] = DasmX86_32_0f3a4E;
	codes0f3a[0x4F] = DasmX86_32_0f3a4F;
	codes0f3a[0x50] = DasmX86_32_0f3a50;
	codes0f3a[0x51] = DasmX86_32_0f3a51;
	codes0f3a[0x52] = DasmX86_32_0f3a52;
	codes0f3a[0x53] = DasmX86_32_0f3a53;
	codes0f3a[0x54] = DasmX86_32_0f3a54;
	codes0f3a[0x55] = DasmX86_32_0f3a55;
	codes0f3a[0x56] = DasmX86_32_0f3a56;
	codes0f3a[0x57] = DasmX86_32_0f3a57;
	codes0f3a[0x58] = DasmX86_32_0f3a58;
	codes0f3a[0x59] = DasmX86_32_0f3a59;
	codes0f3a[0x5A] = DasmX86_32_0f3a5A;
	codes0f3a[0x5B] = DasmX86_32_0f3a5B;
	codes0f3a[0x5C] = DasmX86_32_0f3a5C;
	codes0f3a[0x5D] = DasmX86_32_0f3a5D;
	codes0f3a[0x5E] = DasmX86_32_0f3a5E;
	codes0f3a[0x5F] = DasmX86_32_0f3a5F;
	codes0f3a[0x60] = DasmX86_32_0f3a60;
	codes0f3a[0x61] = DasmX86_32_0f3a61;
	codes0f3a[0x62] = DasmX86_32_0f3a62;
	codes0f3a[0x63] = DasmX86_32_0f3a63;
	codes0f3a[0x64] = DasmX86_32_0f3a64;
	codes0f3a[0x65] = DasmX86_32_0f3a65;
	codes0f3a[0x66] = DasmX86_32_0f3a66;
	codes0f3a[0x67] = DasmX86_32_0f3a67;
	codes0f3a[0x68] = DasmX86_32_0f3a68;
	codes0f3a[0x69] = DasmX86_32_0f3a69;
	codes0f3a[0x6A] = DasmX86_32_0f3a6A;
	codes0f3a[0x6B] = DasmX86_32_0f3a6B;
	codes0f3a[0x6C] = DasmX86_32_0f3a6C;
	codes0f3a[0x6D] = DasmX86_32_0f3a6D;
	codes0f3a[0x6E] = DasmX86_32_0f3a6E;
	codes0f3a[0x6F] = DasmX86_32_0f3a6F;
	codes0f3a[0x70] = DasmX86_32_0f3a70;
	codes0f3a[0x71] = DasmX86_32_0f3a71;
	codes0f3a[0x72] = DasmX86_32_0f3a72;
	codes0f3a[0x73] = DasmX86_32_0f3a73;
	codes0f3a[0x74] = DasmX86_32_0f3a74;
	codes0f3a[0x75] = DasmX86_32_0f3a75;
	codes0f3a[0x76] = DasmX86_32_0f3a76;
	codes0f3a[0x77] = DasmX86_32_0f3a77;
	codes0f3a[0x78] = DasmX86_32_0f3a78;
	codes0f3a[0x79] = DasmX86_32_0f3a79;
	codes0f3a[0x7A] = DasmX86_32_0f3a7A;
	codes0f3a[0x7B] = DasmX86_32_0f3a7B;
	codes0f3a[0x7C] = DasmX86_32_0f3a7C;
	codes0f3a[0x7D] = DasmX86_32_0f3a7D;
	codes0f3a[0x7E] = DasmX86_32_0f3a7E;
	codes0f3a[0x7F] = DasmX86_32_0f3a7F;
	codes0f3a[0x80] = DasmX86_32_0f3a80;
	codes0f3a[0x81] = DasmX86_32_0f3a81;
	codes0f3a[0x82] = DasmX86_32_0f3a82;
	codes0f3a[0x83] = DasmX86_32_0f3a83;
	codes0f3a[0x84] = DasmX86_32_0f3a84;
	codes0f3a[0x85] = DasmX86_32_0f3a85;
	codes0f3a[0x86] = DasmX86_32_0f3a86;
	codes0f3a[0x87] = DasmX86_32_0f3a87;
	codes0f3a[0x88] = DasmX86_32_0f3a88;
	codes0f3a[0x89] = DasmX86_32_0f3a89;
	codes0f3a[0x8A] = DasmX86_32_0f3a8A;
	codes0f3a[0x8B] = DasmX86_32_0f3a8B;
	codes0f3a[0x8C] = DasmX86_32_0f3a8C;
	codes0f3a[0x8D] = DasmX86_32_0f3a8D;
	codes0f3a[0x8E] = DasmX86_32_0f3a8E;
	codes0f3a[0x8F] = DasmX86_32_0f3a8F;
	codes0f3a[0x90] = DasmX86_32_0f3a90;
	codes0f3a[0x91] = DasmX86_32_0f3a91;
	codes0f3a[0x92] = DasmX86_32_0f3a92;
	codes0f3a[0x93] = DasmX86_32_0f3a93;
	codes0f3a[0x94] = DasmX86_32_0f3a94;
	codes0f3a[0x95] = DasmX86_32_0f3a95;
	codes0f3a[0x96] = DasmX86_32_0f3a96;
	codes0f3a[0x97] = DasmX86_32_0f3a97;
	codes0f3a[0x98] = DasmX86_32_0f3a98;
	codes0f3a[0x99] = DasmX86_32_0f3a99;
	codes0f3a[0x9A] = DasmX86_32_0f3a9A;
	codes0f3a[0x9B] = DasmX86_32_0f3a9B;
	codes0f3a[0x9C] = DasmX86_32_0f3a9C;
	codes0f3a[0x9D] = DasmX86_32_0f3a9D;
	codes0f3a[0x9E] = DasmX86_32_0f3a9E;
	codes0f3a[0x9F] = DasmX86_32_0f3a9F;
	codes0f3a[0xA0] = DasmX86_32_0f3aA0;
	codes0f3a[0xA1] = DasmX86_32_0f3aA1;
	codes0f3a[0xA2] = DasmX86_32_0f3aA2;
	codes0f3a[0xA3] = DasmX86_32_0f3aA3;
	codes0f3a[0xA4] = DasmX86_32_0f3aA4;
	codes0f3a[0xA5] = DasmX86_32_0f3aA5;
	codes0f3a[0xA6] = DasmX86_32_0f3aA6;
	codes0f3a[0xA7] = DasmX86_32_0f3aA7;
	codes0f3a[0xA8] = DasmX86_32_0f3aA8;
	codes0f3a[0xA9] = DasmX86_32_0f3aA9;
	codes0f3a[0xAA] = DasmX86_32_0f3aAA;
	codes0f3a[0xAB] = DasmX86_32_0f3aAB;
	codes0f3a[0xAC] = DasmX86_32_0f3aAC;
	codes0f3a[0xAD] = DasmX86_32_0f3aAD;
	codes0f3a[0xAE] = DasmX86_32_0f3aAE;
	codes0f3a[0xAF] = DasmX86_32_0f3aAF;
	codes0f3a[0xB0] = DasmX86_32_0f3aB0;
	codes0f3a[0xB1] = DasmX86_32_0f3aB1;
	codes0f3a[0xB2] = DasmX86_32_0f3aB2;
	codes0f3a[0xB3] = DasmX86_32_0f3aB3;
	codes0f3a[0xB4] = DasmX86_32_0f3aB4;
	codes0f3a[0xB5] = DasmX86_32_0f3aB5;
	codes0f3a[0xB6] = DasmX86_32_0f3aB6;
	codes0f3a[0xB7] = DasmX86_32_0f3aB7;
	codes0f3a[0xB8] = DasmX86_32_0f3aB8;
	codes0f3a[0xB9] = DasmX86_32_0f3aB9;
	codes0f3a[0xBA] = DasmX86_32_0f3aBA;
	codes0f3a[0xBB] = DasmX86_32_0f3aBB;
	codes0f3a[0xBC] = DasmX86_32_0f3aBC;
	codes0f3a[0xBD] = DasmX86_32_0f3aBD;
	codes0f3a[0xBE] = DasmX86_32_0f3aBE;
	codes0f3a[0xBF] = DasmX86_32_0f3aBF;
	codes0f3a[0xC0] = DasmX86_32_0f3aC0;
	codes0f3a[0xC1] = DasmX86_32_0f3aC1;
	codes0f3a[0xC2] = DasmX86_32_0f3aC2;
	codes0f3a[0xC3] = DasmX86_32_0f3aC3;
	codes0f3a[0xC4] = DasmX86_32_0f3aC4;
	codes0f3a[0xC5] = DasmX86_32_0f3aC5;
	codes0f3a[0xC6] = DasmX86_32_0f3aC6;
	codes0f3a[0xC7] = DasmX86_32_0f3aC7;
	codes0f3a[0xC8] = DasmX86_32_0f3aC8;
	codes0f3a[0xC9] = DasmX86_32_0f3aC9;
	codes0f3a[0xCA] = DasmX86_32_0f3aCA;
	codes0f3a[0xCB] = DasmX86_32_0f3aCB;
	codes0f3a[0xCC] = DasmX86_32_0f3aCC;
	codes0f3a[0xCD] = DasmX86_32_0f3aCD;
	codes0f3a[0xCE] = DasmX86_32_0f3aCE;
	codes0f3a[0xCF] = DasmX86_32_0f3aCF;
	codes0f3a[0xD0] = DasmX86_32_0f3aD0;
	codes0f3a[0xD1] = DasmX86_32_0f3aD1;
	codes0f3a[0xD2] = DasmX86_32_0f3aD2;
	codes0f3a[0xD3] = DasmX86_32_0f3aD3;
	codes0f3a[0xD4] = DasmX86_32_0f3aD4;
	codes0f3a[0xD5] = DasmX86_32_0f3aD5;
	codes0f3a[0xD6] = DasmX86_32_0f3aD6;
	codes0f3a[0xD7] = DasmX86_32_0f3aD7;
	codes0f3a[0xD8] = DasmX86_32_0f3aD8;
	codes0f3a[0xD9] = DasmX86_32_0f3aD9;
	codes0f3a[0xDA] = DasmX86_32_0f3aDA;
	codes0f3a[0xDB] = DasmX86_32_0f3aDB;
	codes0f3a[0xDC] = DasmX86_32_0f3aDC;
	codes0f3a[0xDD] = DasmX86_32_0f3aDD;
	codes0f3a[0xDE] = DasmX86_32_0f3aDE;
	codes0f3a[0xDF] = DasmX86_32_0f3aDF;
	codes0f3a[0xE0] = DasmX86_32_0f3aE0;
	codes0f3a[0xE1] = DasmX86_32_0f3aE1;
	codes0f3a[0xE2] = DasmX86_32_0f3aE2;
	codes0f3a[0xE3] = DasmX86_32_0f3aE3;
	codes0f3a[0xE4] = DasmX86_32_0f3aE4;
	codes0f3a[0xE5] = DasmX86_32_0f3aE5;
	codes0f3a[0xE6] = DasmX86_32_0f3aE6;
	codes0f3a[0xE7] = DasmX86_32_0f3aE7;
	codes0f3a[0xE8] = DasmX86_32_0f3aE8;
	codes0f3a[0xE9] = DasmX86_32_0f3aE9;
	codes0f3a[0xEA] = DasmX86_32_0f3aEA;
	codes0f3a[0xEB] = DasmX86_32_0f3aEB;
	codes0f3a[0xEC] = DasmX86_32_0f3aEC;
	codes0f3a[0xED] = DasmX86_32_0f3aED;
	codes0f3a[0xEE] = DasmX86_32_0f3aEE;
	codes0f3a[0xEF] = DasmX86_32_0f3aEF;
	codes0f3a[0xF0] = DasmX86_32_0f3aF0;
	codes0f3a[0xF1] = DasmX86_32_0f3aF1;
	codes0f3a[0xF2] = DasmX86_32_0f3aF2;
	codes0f3a[0xF3] = DasmX86_32_0f3aF3;
	codes0f3a[0xF4] = DasmX86_32_0f3aF4;
	codes0f3a[0xF5] = DasmX86_32_0f3aF5;
	codes0f3a[0xF6] = DasmX86_32_0f3aF6;
	codes0f3a[0xF7] = DasmX86_32_0f3aF7;
	codes0f3a[0xF8] = DasmX86_32_0f3aF8;
	codes0f3a[0xF9] = DasmX86_32_0f3aF9;
	codes0f3a[0xFA] = DasmX86_32_0f3aFA;
	codes0f3a[0xFB] = DasmX86_32_0f3aFB;
	codes0f3a[0xFC] = DasmX86_32_0f3aFC;
	codes0f3a[0xFD] = DasmX86_32_0f3aFD;
	codes0f3a[0xFE] = DasmX86_32_0f3aFE;
	codes0f3a[0xFF] = DasmX86_32_0f3aFF;
}

Manage::DasmX86_32::~DasmX86_32()
{
	MemFree(this->codes);
	MemFree(this->codes0f);
	MemFree(this->codes0f38);
	MemFree(this->codes0f3a);
}

Text::CStringNN Manage::DasmX86_32::GetHeader(Bool fullRegs) const
{
	if (fullRegs)
	{
		return CSTR(" Esp      Ebp      Eip      Eax      Edx      Ecx      Ebx      Esi      Edi      Code");
	}
	else
	{
		return CSTR(" Esp      Ebp      Eip      Code");
	}
}

Bool Manage::DasmX86_32::Disasm32(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, UInt32 *currEip, UInt32 *currEsp, UInt32 *currEbp, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs)
{
	UTF8Char sbuff[512];
	DasmX86_32_Sess sess;
	Text::StringBuilderUTF8 outStr;
	UOSInt initJmpCnt = jmpAddrs->GetCount();
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	MemCopyNO(&sess.regs, regs.Ptr(), sizeof(Manage::DasmX86_32::DasmX86_32_Regs));
	sess.regs.EIP = *currEip;
	sess.regs.ESP = *currEsp;
	sess.regs.EBP = *currEbp;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmX86_32::ET_NOT_END;
	sess.espOfst = 0;
	sess.lastStatus = 2;
	sess.thisStatus = 0;
	sess.codes = (void**)this->codes;
	sess.codes0f = (void**)this->codes0f;
	sess.stabesp = sess.regs.ESP;
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	*blockStart = sess.regs.EIP;

	while (true)
	{
		UInt8 buff[16];
		Bool ret;

		outStr.ClearStr();
		outStr.AppendHex32(sess.regs.ESP);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32(sess.regs.EBP);
		outStr.AppendC(UTF8STRC(" "));
		outStr.AppendHex32(sess.regs.EIP);
		outStr.AppendC(UTF8STRC(" "));
		if (fullRegs)
		{
			outStr.AppendHex32(sess.regs.EAX);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.EDX);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.ECX);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.EBX);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.ESI);
			outStr.AppendC(UTF8STRC(" "));
			outStr.AppendHex32(sess.regs.EDI);
			outStr.AppendC(UTF8STRC(" "));
		}
		sess.sbuff = sbuff;
		if (sess.memReader->ReadMemory(sess.regs.EIP, buff, 1) == 0)
		{
			ret = false;
		}
		else
		{
			ret = this->codes[buff[0]](sess);
		}
		if (!ret)
		{
			UOSInt buffSize;
			outStr.AppendC(UTF8STRC("Unknown opcode "));
			buffSize = sess.memReader->ReadMemory(sess.regs.EIP, buff, 16);
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
		if (sess.endType == Manage::DasmX86_32::ET_JMP && (UInt32)sess.retAddr >= *blockStart && (UInt32)sess.retAddr <= sess.regs.EIP)
		{
			UOSInt i;
			UInt32 minAddr = 0xffffffff;
			UInt32 jmpAddr;
			i = jmpAddrs->GetCount();
			while (i-- > initJmpCnt)
			{
				jmpAddr = jmpAddrs->GetItem(i);
				if (jmpAddr >= sess.regs.EIP && jmpAddr < minAddr)
				{
					minAddr = jmpAddr;
				}
			}
			if (minAddr - sess.regs.EIP > 0x1000)
			{
				*currEip = sess.retAddr;
				*currEsp = sess.regs.ESP;
				*currEbp = sess.regs.EBP;
				*blockEnd = sess.regs.EIP;
				MemCopyNO(regs.Ptr(), &sess.regs, sizeof(Manage::DasmX86_32::DasmX86_32_Regs));
				return false;
			}
			sess.regs.EIP = minAddr;
			sess.endType = Manage::DasmX86_32::ET_NOT_END;
		}
		else if (sess.endType != Manage::DasmX86_32::ET_NOT_END)
		{
			*currEip = sess.retAddr;
			*currEsp = sess.regs.ESP;
			*currEbp = sess.regs.EBP;
			*blockEnd = sess.regs.EIP;
			MemCopyNO(regs.Ptr(), &sess.regs, sizeof(Manage::DasmX86_32::DasmX86_32_Regs));
			return sess.endType != Manage::DasmX86_32::ET_EXIT;
		}
		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = 0;
	}
}

Bool Manage::DasmX86_32::Disasm32In(NN<Text::StringBuilderUTF8> outStr, Optional<Manage::AddressResolver> addrResol, UInt32 *currEip, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, NN<Manage::MemoryReader> memReader)
{
	UTF8Char sbuff[256];
	UInt32 initIP = *currEip;
	DasmX86_32_Sess sess;
	sess.callAddrs = callAddrs;
	sess.jmpAddrs = jmpAddrs;
	MemClear(&sess.regs, sizeof(Manage::DasmX86_32::DasmX86_32_Regs));
	sess.regs.EIP = *currEip;
//	sess.outStr = outStr;
	sess.endType = Manage::DasmX86_32::ET_NOT_END;
	sess.espOfst = 0;
	sess.lastStatus = 0;
	sess.thisStatus = 0x80000000;
	sess.codes = (void**)this->codes;
	sess.codes0f = (void**)this->codes0f;
	sess.addrResol = addrResol;
	sess.memReader = memReader;
	*blockStart = sess.regs.EIP;

	while (sess.memReader->ReadMemUInt8(sess.regs.EIP) == 0xe9)
	{
		outStr->AppendHex32(sess.regs.EIP);
		outStr->AppendC(UTF8STRC(" "));
		sess.sbuff = sbuff;
		Bool ret = this->codes[sess.memReader->ReadMemUInt8(sess.regs.EIP)](sess);
		if (!ret)
			break;
		outStr->AppendSlow(sbuff);
		sess.regs.EIP = sess.retAddr;
		*blockStart = sess.retAddr;
		sess.endType = Manage::DasmX86_32::ET_NOT_END;
	}

	IO::ConsoleWriter console;
	while (true)
	{
		UInt32 oriip = sess.regs.EIP;
		if (sess.regs.EIP == 0x50a210)
		{
			oriip = sess.regs.EIP;
		}

		outStr->AppendHex32(sess.regs.EIP);
		outStr->AppendC(UTF8STRC(" "));
		sess.sbuff = sbuff;
		Bool ret = this->codes[sess.memReader->ReadMemUInt8(sess.regs.EIP)](sess);
		UOSInt sbuffLen = Text::StrCharCnt(sbuff);
		console.Write(Text::CStringNN(sbuff, sbuffLen));
		outStr->AppendC(sbuff, sbuffLen);
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
		if (sess.endType != Manage::DasmX86_32::ET_NOT_END)
		{
			*currEip = sess.retAddr;
			*blockEnd = sess.regs.EIP;
			return true;
		}
		sess.lastStatus = sess.thisStatus;
		sess.thisStatus = 0x80000000;
	}
}

NN<Manage::Dasm::Dasm_Regs> Manage::DasmX86_32::CreateRegs() const
{
	return MemAllocNN(DasmX86_32_Regs);
}

void Manage::DasmX86_32::FreeRegs(NN<Dasm_Regs> regs) const
{
	MemFreeNN(regs);
}

NN<Manage::DasmX86_32::DasmX86_32_Sess> Manage::DasmX86_32::StartDasm(Optional<Manage::AddressResolver> addrResol, void *addr, NN<Manage::MemoryReader> memReader)
{
	NN<DasmX86_32_Sess> sess;
	sess = MemAllocNN(DasmX86_32_Sess);
	NEW_CLASS(sess->callAddrs, Data::ArrayListUInt32());
	NEW_CLASS(sess->jmpAddrs, Data::ArrayListUInt32());
	sess->regs.EIP = (UInt32)(OSInt)addr;
	sess->regs.ESP = 0;
	sess->regs.EBP = 0;
	sess->regs.EAX = 0;
	sess->regs.EBX = 0;
	sess->regs.ECX = 0;
	sess->regs.EDX = 0;
	sess->regs.ESI = 0;
	sess->regs.EDI = 0;
//	sess->outStr = 0;
	sess->endType = Manage::DasmX86_32::ET_NOT_END;
	sess->espOfst = 0;
	sess->lastStatus = 2;
	sess->thisStatus = 0x80000000;
	sess->codes = (void**)this->codes;
	sess->codes0f = (void**)this->codes0f;
	sess->stabesp = 0;
	sess->addrResol = addrResol;
	sess->memReader = memReader;
	return sess;
}

void Manage::DasmX86_32::EndDasm(NN<DasmX86_32_Sess> sess)
{
	DEL_CLASS(sess->callAddrs);
	DEL_CLASS(sess->jmpAddrs);
	MemFreeNN(sess);
}

UnsafeArrayOpt<UTF8Char> Manage::DasmX86_32::DasmNext(NN<DasmX86_32_Sess> sess, UnsafeArray<UTF8Char> buff)
{
	if (sess->endType != Manage::DasmX86_32::ET_NOT_END)
		return 0;
	sess->sbuff = buff;
	Bool ret = this->codes[sess->memReader->ReadMemUInt8(sess->regs.EIP)](sess);
	UnsafeArray<UTF8Char> sptr = sess->sbuff;
	if (!ret)
	{
		UInt8 cbuff[16];
		UOSInt buffSize;
		sptr = Text::StrConcatC(buff, UTF8STRC("Unknown opcode "));
		buffSize = sess->memReader->ReadMemory(sess->regs.EIP, cbuff, 16);
		if (buffSize > 0)
		{
			sptr = Text::StrHexBytes(sptr, cbuff, buffSize, ' ');
		}
		sess->endType = Manage::DasmX86_32::ET_INV_OP;
		return sptr;
	}
	if (sess->endType != Manage::DasmX86_32::ET_NOT_END)
	{
/*		*currEip = sess.retAddr;
		*currEsp = (Int32)sess.cesp;
		*currEbp = (Int32)sess.cebp;
		*blockEnd = (Int32)sess.regs.EIP;*/
		return sptr;
	}
	sess->lastStatus = sess->thisStatus;
	sess->thisStatus = sess->thisStatus & 0x80000000;
	return sptr;
}

OSInt Manage::DasmX86_32::SessGetCodeOffset(NN<DasmX86_32_Sess> sess)
{
	return (OSInt)sess->regs.EIP;
}

Manage::DasmX86_32::EndType Manage::DasmX86_32::SessGetEndType(NN<DasmX86_32_Sess> sess)
{
	return sess->endType;
}

Bool Manage::DasmX86_32::SessContJmp(NN<DasmX86_32_Sess> sess)
{
	if (sess->endType == Manage::DasmX86_32::ET_JMP)
	{
		sess->endType = Manage::DasmX86_32::ET_NOT_END;
		sess->regs.EIP = sess->retAddr;
		return true;
	}
	else
	{
		return false;
	}
}
