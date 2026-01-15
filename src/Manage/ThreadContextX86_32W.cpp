#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmX86_32.h"
#include "Manage/ThreadContextX86_32.h"
#include <windows.h>

#if defined(CONTEXT_TYPE)
#undef CONTEXT_TYPE
#endif

#if defined(_WIN32_WCE) && !defined(_X86_)

UInt32 Manage::ThreadContextX86_32::GetEAX() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetEDX() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetECX() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetEBX() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetESI() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetEDI() const
{
	return 0;
}

#else
#if defined(_WIN64) // && MSC_VER > 0x1400
#define CONTEXT_TYPE WOW64_CONTEXT
#else
#ifdef _WIN64
#define CONTEXT_VX86 1
#endif
#define CONTEXT_TYPE CONTEXT
#endif


Manage::ThreadContextX86_32::ThreadContextX86_32(UIntOS procId, UIntOS threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(CONTEXT_TYPE, 1);
	MemCopyNO(this->context, context, sizeof(CONTEXT_TYPE));
	mmOfst = 0x20;
	xmmOfst = 0xa0;
}

Manage::ThreadContextX86_32::~ThreadContextX86_32()
{
	MemFree(this->context);
}

UIntOS Manage::ThreadContextX86_32::GetRegisterCnt() const
{
	UIntOS cnt = 33;
	if (this->mmOfst >= 0)
	{
		cnt += 8;
		if (this->xmmOfst >= 0)
		{
			cnt += 8;
		}
	}
	return cnt;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextX86_32::GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const
{
	switch (index)
	{
	case 0:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rax;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Eax;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EAX"));
	case 1:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rdx;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Edx;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EDX"));
	case 2:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rcx;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Ecx;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ECX"));
	case 3:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rbx;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Ebx;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EBX"));
	case 4:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rsi;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Esi;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ESI"));
	case 5:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rdi;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Edi;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EDI"));
	case 6:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rsp;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Esp;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ESP"));
	case 7:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rbp;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Ebp;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EBP"));
	case 8:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Rip;
#else
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->Eip;
#endif
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EIP"));
	case 9:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ((CONTEXT_TYPE*)this->context)->EFlags;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 10:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->SegCs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 11:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->SegSs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("SS"));
	case 12:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->SegDs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("DS"));
	case 13:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->SegEs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("ES"));
	case 14:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->SegFs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 15:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->SegGs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	case 16:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Dr0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR0"));
	case 17:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Dr1;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR1"));
	case 18:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Dr2;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR2"));
	case 19:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Dr3;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR3"));
	case 20:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Dr6;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR6"));
	case 21:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = (UInt32)((CONTEXT_TYPE*)this->context)->Dr7;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR7"));
	case 22:
		GetST0(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST0"));
	case 23:
		GetST1(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST1"));
	case 24:
		GetST2(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST2"));
	case 25:
		GetST3(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST3"));
	case 26:
		GetST4(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST4"));
	case 27:
		GetST5(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST5"));
	case 28:
		GetST6(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST6"));
	case 29:
		GetST7(regVal);
		regBitCount.Set(80);
		return Text::StrConcatC(buff, UTF8STRC("ST7"));
	case 30:
#ifdef CONTEXT_VX86
		
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.ControlWord;
#else
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.ControlWord;
#endif
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CTRL"));
	case 31:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.StatusWord;
#else
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord;
#endif
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("STAT"));
	case 32:
#ifdef CONTEXT_VX86
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.TagWord;
#else
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.TagWord;
#endif
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("TAG"));
	case 33:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[0];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM0"));
	case 34:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[1];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 16];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM1"));
	case 35:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[2];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 32];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM2"));
	case 36:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[3];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 48];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM3"));
	case 37:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[4];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 64];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM4"));
	case 38:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[5];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 80];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM5"));
	case 39:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[6];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 96];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM6"));
	case 40:
		if (this->mmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->Legacy[7];
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->mmOfst + 112];
#endif
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("MM7"));
	case 41:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm0;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 8];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM0"));
	case 42:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm1;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 16];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 24];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM1"));
	case 43:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm2;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 32];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 40];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM2"));
	case 44:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm3;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 48];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 56];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM3"));
	case 45:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm4;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 64];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 72];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM4"));
	case 46:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm5;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 80];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 88];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM5"));
	case 47:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm6;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 96];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 104];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM6"));
	case 48:
		if (this->xmmOfst < 0)
			return 0;
#ifdef CONTEXT_VX86
		*(M128A*)regVal = ((CONTEXT_TYPE*)this->context)->Xmm7;
#else
		UnsafeArray<Int64>::ConvertFrom(regVal)[0] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 112];
		*(Int64*)&regVal[8] = *(Int64*)&((CONTEXT_TYPE*)this->context)->ExtendedRegisters[this->xmmOfst + 120];
#endif
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM7"));
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_32::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UIntOS i = 0;
	UIntOS j = this->GetRegisterCnt();
	UIntOS k;

	while (i < j)
	{
		if (this->GetRegister(i, sbuff, regBuff, bitCnt).SetTo(sptr))
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(" = "));
			k = bitCnt >> 3;
			while (k-- > 0)
			{
				sptr = Text::StrHexByte(sptr, regBuff[k]);
			}
			switch (i)
			{
			case 3:
			case 8:
			case 9:
			case 15:
			case 21:
			case 25:
			case 29:
			case 32:
			case 36:
			case 40:
			case 44:
				sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
				break;
			case 48:
				break;
			default:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
			}

			sb->AppendP(sbuff, sptr);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_32::GetType() const
{
	return Manage::ThreadContext::ContextType::X86_32;
}

UIntOS Manage::ThreadContextX86_32::GetThreadId() const
{
	return this->threadId;
}

UIntOS Manage::ThreadContextX86_32::GetProcessId() const
{
	return this->procId;
}

UIntOS Manage::ThreadContextX86_32::GetInstAddr() const
{
	return this->GetEIP();
}

UIntOS Manage::ThreadContextX86_32::GetStackAddr() const
{
	return this->GetESP();
}

UIntOS Manage::ThreadContextX86_32::GetFrameAddr() const
{
	return this->GetEBP();
}

void Manage::ThreadContextX86_32::SetInstAddr(UIntOS instAddr)
{
#ifdef CONTEXT_VX86
	((CONTEXT_TYPE*)this->context)->Rip = instAddr;
#else
	((CONTEXT_TYPE*)this->context)->Eip = (UInt32)instAddr;
#endif
}

void Manage::ThreadContextX86_32::SetStackAddr(UIntOS stackAddr)
{
#ifdef CONTEXT_VX86
	((CONTEXT_TYPE*)this->context)->Rsp = stackAddr;
#else
	((CONTEXT_TYPE*)this->context)->Esp = (UInt32)stackAddr;
#endif
}

void Manage::ThreadContextX86_32::SetFrameAddr(UIntOS frameAddr)
{
#ifdef CONTEXT_VX86
	((CONTEXT_TYPE*)this->context)->Rbp = frameAddr;
#else
	((CONTEXT_TYPE*)this->context)->Ebp = (UInt32)frameAddr;
#endif
}

NN<Manage::ThreadContext> Manage::ThreadContextX86_32::Clone() const
{
	NN<Manage::ThreadContextX86_32> ret;
	NEW_CLASSNN(ret, Manage::ThreadContextX86_32(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_32::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	NN<Manage::DasmX86_32::DasmX86_32_Regs> r = NN<Manage::DasmX86_32::DasmX86_32_Regs>::ConvertFrom(regs);
#ifdef CONTEXT_VX86
	r->EAX = (UInt32)((CONTEXT_TYPE*)this->context)->Rax;
	r->EDX = (UInt32)((CONTEXT_TYPE*)this->context)->Rdx;
	r->ECX = (UInt32)((CONTEXT_TYPE*)this->context)->Rcx;
	r->EBX = (UInt32)((CONTEXT_TYPE*)this->context)->Rbx;
	r->ESI = (UInt32)((CONTEXT_TYPE*)this->context)->Rsi;
	r->EDI = (UInt32)((CONTEXT_TYPE*)this->context)->Rdi;
	r->ESP = (UInt32)((CONTEXT_TYPE*)this->context)->Rsp;
	r->EBP = (UInt32)((CONTEXT_TYPE*)this->context)->Rbp;
	r->EIP = (UInt32)((CONTEXT_TYPE*)this->context)->Rip;
#else
	r->EAX = ((CONTEXT_TYPE*)this->context)->Eax;
	r->EDX = ((CONTEXT_TYPE*)this->context)->Edx;
	r->ECX = ((CONTEXT_TYPE*)this->context)->Ecx;
	r->EBX = ((CONTEXT_TYPE*)this->context)->Ebx;
	r->ESI = ((CONTEXT_TYPE*)this->context)->Esi;
	r->EDI = ((CONTEXT_TYPE*)this->context)->Edi;
	r->ESP = ((CONTEXT_TYPE*)this->context)->Esp;
	r->EBP = ((CONTEXT_TYPE*)this->context)->Ebp;
	r->EIP = ((CONTEXT_TYPE*)this->context)->Eip;
#endif
	r->EFLAGS = ((CONTEXT_TYPE*)this->context)->EFlags;
	r->CS = (UInt16)((CONTEXT_TYPE*)this->context)->SegCs;
	r->SS = (UInt16)((CONTEXT_TYPE*)this->context)->SegSs;
	r->DS = (UInt16)((CONTEXT_TYPE*)this->context)->SegDs;
	r->ES = (UInt16)((CONTEXT_TYPE*)this->context)->SegEs;
	r->FS = (UInt16)((CONTEXT_TYPE*)this->context)->SegFs;
	r->GS = (UInt16)((CONTEXT_TYPE*)this->context)->SegGs;

#ifdef CONTEXT_VX86
	r->DR0 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr0;
	r->DR1 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr1;
	r->DR2 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr2;
	r->DR3 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr3;
	r->DR6 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr6;
	r->DR7 = (UInt32)((CONTEXT_TYPE*)this->context)->Dr7;
#else
	r->DR0 = ((CONTEXT_TYPE*)this->context)->Dr0;
	r->DR1 = ((CONTEXT_TYPE*)this->context)->Dr1;
	r->DR2 = ((CONTEXT_TYPE*)this->context)->Dr2;
	r->DR3 = ((CONTEXT_TYPE*)this->context)->Dr3;
	r->DR6 = ((CONTEXT_TYPE*)this->context)->Dr6;
	r->DR7 = ((CONTEXT_TYPE*)this->context)->Dr7;
#endif

#ifdef CONTEXT_VX86
	IntOS i = 8;
	while (i-- > 0)
	{
		MemCopyNO(&r->floatBuff[i * 10], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[i], 10);
	}
	r->CTRL = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.ControlWord;
	r->STAT = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.StatusWord;
	r->TAG = (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.TagWord;
#else
	MemCopyNO(r->floatBuff, ((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea, 80);
	r->CTRL = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.ControlWord;
	r->STAT = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord;
	r->TAG = (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.TagWord;
#endif
	return true;
}

Optional<Manage::Dasm> Manage::ThreadContextX86_32::CreateDasm() const
{
	Manage::DasmX86_32 *dasm;
	NEW_CLASS(dasm, Manage::DasmX86_32());
	return dasm;
}

void *Manage::ThreadContextX86_32::GetContext() const
{
	return this->context;
}

UInt32 Manage::ThreadContextX86_32::GetEAX() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rax;
#else
	return ((CONTEXT_TYPE*)this->context)->Eax;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEDX() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rdx;
#else
	return ((CONTEXT_TYPE*)this->context)->Edx;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetECX() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rcx;
#else
	return ((CONTEXT_TYPE*)this->context)->Ecx;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEBX() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rbx;
#else
	return ((CONTEXT_TYPE*)this->context)->Ebx;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetESI() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rsi;
#else
	return ((CONTEXT_TYPE*)this->context)->Esi;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEDI() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rdi;
#else
	return ((CONTEXT_TYPE*)this->context)->Edi;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetESP() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rsp;
#else
	return ((CONTEXT_TYPE*)this->context)->Esp;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEBP() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rbp;
#else
	return ((CONTEXT_TYPE*)this->context)->Ebp;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEIP() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Rip;
#else
	return ((CONTEXT_TYPE*)this->context)->Eip;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetEFLAGS() const
{
	return ((CONTEXT_TYPE*)this->context)->EFlags;
}

UInt16 Manage::ThreadContextX86_32::GetCS() const
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegCs;
}

UInt16 Manage::ThreadContextX86_32::GetSS() const
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegSs;
}

UInt16 Manage::ThreadContextX86_32::GetDS() const
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegDs;
}

UInt16 Manage::ThreadContextX86_32::GetES() const
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegEs;
}

UInt16 Manage::ThreadContextX86_32::GetFS() const
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegFs;
}

UInt16 Manage::ThreadContextX86_32::GetGS() const
{
	return (UInt16)((CONTEXT_TYPE*)this->context)->SegGs;
}

UInt32 Manage::ThreadContextX86_32::GetDR0() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr0;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr0;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR1() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr1;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr1;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR2() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr2;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr2;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR3() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr3;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr3;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR6() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr6;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr6;
#endif
}

UInt32 Manage::ThreadContextX86_32::GetDR7() const
{
#ifdef CONTEXT_VX86
	return (UInt32)((CONTEXT_TYPE*)this->context)->Dr7;
#else
	return ((CONTEXT_TYPE*)this->context)->Dr7;
#endif
}

void Manage::ThreadContextX86_32::GetST0(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 0) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 0) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST1(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 1) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 1) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST2(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 2) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 2) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST3(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 3) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 3) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST4(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 4) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 4) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST5(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 5) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 5) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST6(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 6) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 6) & 7)], 10);
#endif
}

void Manage::ThreadContextX86_32::GetST7(UnsafeArray<UInt8> val) const
{
#ifdef CONTEXT_VX86
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FltSave.FloatRegisters[(((((CONTEXT_TYPE*)this->context)->FltSave.StatusWord >> 11) + 7) & 7)], 10);
#else
	MemCopyNO(&val[0], &((CONTEXT_TYPE*)this->context)->FloatSave.RegisterArea[10 * (((((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord >> 11) + 7) & 7)], 10);
#endif
}

UInt16 Manage::ThreadContextX86_32::GetCTRL() const
{
#ifdef CONTEXT_VX86
	return (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.ControlWord;
#else
	return (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.ControlWord;
#endif
}

UInt16 Manage::ThreadContextX86_32::GetSTAT() const
{
#ifdef CONTEXT_VX86
	return (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.StatusWord;
#else
	return (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.StatusWord;
#endif
}

UInt16 Manage::ThreadContextX86_32::GetTAG() const
{
#ifdef CONTEXT_VX86
	return (UInt16)((CONTEXT_TYPE*)this->context)->FltSave.TagWord;
#else
	return (UInt16)((CONTEXT_TYPE*)this->context)->FloatSave.TagWord;
#endif
}
#endif