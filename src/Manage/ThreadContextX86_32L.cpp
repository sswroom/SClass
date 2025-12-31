#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"

#if defined(__FreeBSD__)
#include "Manage/DasmX86_32.h"
#include "Manage/ThreadContextX86_32.h"
#include <sys/ucontext.h>

Manage::ThreadContextX86_32::ThreadContextX86_32(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextX86_32::~ThreadContextX86_32()
{
	MemFree(this->context);
}

OSInt Manage::ThreadContextX86_32::GetRegisterCnt()
{
	OSInt cnt = 33;
	return cnt;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextX86_32::GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<Int32> regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EAX"));
	case 1:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EDX"));
	case 2:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ECX"));
	case 3:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EBX"));
	case 4:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ESI"));
	case 5:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EDI"));
	case 6:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ESP"));
	case 7:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EBP"));
	case 8:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EIP"));
	case 9:
		*(UInt32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 10:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 11:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("SS"));
	case 12:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("DS"));
	case 13:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("ES"));
	case 14:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 15:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	case 16:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR0"));
	case 17:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR1"));
	case 18:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR2"));
	case 19:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR3"));
	case 20:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR6"));
	case 21:
		*(UInt32*)regVal.Ptr() = 0;
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
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CTRL"));
	case 31:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("STAT"));
	case 32:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("TAG"));
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_32::ToString(NN<Text::StringBuilderUTF> sb)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 regBuff[16];
	Int32 bitCnt;
	UOSInt i = 0;
	UOSInt j = this->GetRegisterCnt();
	UOSInt k;

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

			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_32::GetType()
{
	return Manage::ThreadContext::ContextType::X86_32;
}

UInt32 Manage::ThreadContextX86_32::GetThreadId()
{
	return this->threadId;
}

UInt32 Manage::ThreadContextX86_32::GetProcessId()
{
	return this->procId;
}

OSInt Manage::ThreadContextX86_32::GetInstAddr()
{
	return this->GetEIP();
}

OSInt Manage::ThreadContextX86_32::GetStackAddr()
{
	return this->GetESP();
}

OSInt Manage::ThreadContextX86_32::GetFrameAddr()
{
	return this->GetEBP();
}

void Manage::ThreadContextX86_32::SetInstAddr(UOSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP] = (UInt32)instAddr;
}

void Manage::ThreadContextX86_32::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP] = (UInt32)stackAddr;
}

void Manage::ThreadContextX86_32::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP] = (UInt32)frameAddr;
}

NN<Manage::ThreadContext> Manage::ThreadContextX86_32::Clone()
{
	NN<Manage::ThreadContextX86_32> ret;
	NEW_CLASSNN(ret, Manage::ThreadContextX86_32(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_32::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs)
{
	NN<Manage::DasmX86_32::DasmX86_32_Regs> r = NN<Manage::DasmX86_32::DasmX86_32_Regs>::ConvertFrom(regs);
	r->EAX = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
	r->EDX = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
	r->ECX = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
	r->EBX = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
	r->ESI = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
	r->EDI = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
	r->ESP = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
	r->EBP = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
	r->EIP = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];

	r->EFLAGS = ((ucontext_t*)this->context)->uc_flags;
	r->CS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
	r->SS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
	r->DS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
	r->ES = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
	r->FS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
	r->GS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];

	r->DR0 = 0;
	r->DR1 = 0;
	r->DR2 = 0;
	r->DR3 = 0;
	r->DR6 = 0;
	r->DR7 = 0;

	OSInt i = 8;
	while (i-- > 0)
	{
		MemCopyNO(&r->floatBuff[i * 10], &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[i], 10);
	}
	r->CTRL = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
	r->STAT = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
	r->TAG = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
	return true;
}

NN<Manage::Dasm> Manage::ThreadContextX86_32::CreateDasm()
{
	NN<Manage::DasmX86_32> dasm;
	NEW_CLASSNN(dasm, Manage::DasmX86_32());
	return dasm;
}

void *Manage::ThreadContextX86_32::GetContext()
{
	return this->context;
}

UInt32 Manage::ThreadContextX86_32::GetEAX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
}

UInt32 Manage::ThreadContextX86_32::GetEDX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
}

UInt32 Manage::ThreadContextX86_32::GetECX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
}

UInt32 Manage::ThreadContextX86_32::GetEBX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
}

UInt32 Manage::ThreadContextX86_32::GetESI()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
}

UInt32 Manage::ThreadContextX86_32::GetEDI()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
}

UInt32 Manage::ThreadContextX86_32::GetESP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
}

UInt32 Manage::ThreadContextX86_32::GetEBP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
}

UInt32 Manage::ThreadContextX86_32::GetEIP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];
}

UInt32 Manage::ThreadContextX86_32::GetEFLAGS()
{
	return ((ucontext_t*)this->context)->uc_flags;
}

UInt16 Manage::ThreadContextX86_32::GetCS()
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
}

UInt16 Manage::ThreadContextX86_32::GetSS()
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
}

UInt16 Manage::ThreadContextX86_32::GetDS()
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
}

UInt16 Manage::ThreadContextX86_32::GetES()
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
}

UInt16 Manage::ThreadContextX86_32::GetFS()
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
}

UInt16 Manage::ThreadContextX86_32::GetGS()
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];
}

UInt32 Manage::ThreadContextX86_32::GetDR0()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR1()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR2()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR3()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR6()
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR7()
{
	return 0;
}

void Manage::ThreadContextX86_32::GetST0(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 0) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST1(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 1) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST2(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 2) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST3(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 3) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST4(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 4) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST5(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 5) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST6(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[ (((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 6) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST7(UInt8 *val)
{
	MemCopyNO(val, &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 7) & 7)], 10);
}

UInt16 Manage::ThreadContextX86_32::GetCTRL()
{
	return (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
}

UInt16 Manage::ThreadContextX86_32::GetSTAT()
{
	return (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
}

UInt16 Manage::ThreadContextX86_32::GetTAG()
{
	return (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
}
#else
#include "Manage/DasmX86_32.h"
#include "Manage/ThreadContextX86_32.h"
#include <sys/ucontext.h>

Manage::ThreadContextX86_32::ThreadContextX86_32(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextX86_32::~ThreadContextX86_32()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextX86_32::GetRegisterCnt() const
{
	UOSInt cnt = 33;
	return cnt;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextX86_32::GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const
{
	switch (index)
	{
	case 0:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EAX"));
	case 1:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EDX"));
	case 2:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ECX"));
	case 3:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EBX"));
	case 4:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ESI"));
	case 5:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EDI"));
	case 6:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("ESP"));
	case 7:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EBP"));
	case 8:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EIP"));
	case 9:
		*(Int32*)regVal.Ptr() = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 10:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 11:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("SS"));
	case 12:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("DS"));
	case 13:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("ES"));
	case 14:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 15:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	case 16:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR0"));
	case 17:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR1"));
	case 18:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR2"));
	case 19:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR3"));
	case 20:
		*(UInt32*)regVal.Ptr() = 0;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("DR6"));
	case 21:
		*(UInt32*)regVal.Ptr() = 0;
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
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CTRL"));
	case 31:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("STAT"));
	case 32:
		*(UInt16*)regVal.Ptr() = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("TAG"));
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
	UOSInt i = 0;
	UOSInt j = this->GetRegisterCnt();
	UOSInt k;

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

UOSInt Manage::ThreadContextX86_32::GetThreadId() const
{
	return this->threadId;
}

UOSInt Manage::ThreadContextX86_32::GetProcessId() const
{
	return this->procId;
}

UOSInt Manage::ThreadContextX86_32::GetInstAddr() const
{
	return this->GetEIP();
}

UOSInt Manage::ThreadContextX86_32::GetStackAddr() const
{
	return this->GetESP();
}

UOSInt Manage::ThreadContextX86_32::GetFrameAddr() const
{
	return this->GetEBP();
}

void Manage::ThreadContextX86_32::SetInstAddr(UOSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP] = (Int32)instAddr;
}

void Manage::ThreadContextX86_32::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP] = (Int32)stackAddr;
}

void Manage::ThreadContextX86_32::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP] = (Int32)frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextX86_32::Clone() const
{
	Manage::ThreadContextX86_32 *ret;
	NEW_CLASS(ret, Manage::ThreadContextX86_32(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_32::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	NN<Manage::DasmX86_32::DasmX86_32_Regs> r = NN<Manage::DasmX86_32::DasmX86_32_Regs>::ConvertFrom(regs);
	r->EAX = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
	r->EDX = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
	r->ECX = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
	r->EBX = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
	r->ESI = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
	r->EDI = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
	r->ESP = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
	r->EBP = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
	r->EIP = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];

	r->EFLAGS = ((ucontext_t*)this->context)->uc_flags;
	r->CS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
	r->SS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
	r->DS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
	r->ES = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
	r->FS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
	r->GS = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];

	r->DR0 = 0;
	r->DR1 = 0;
	r->DR2 = 0;
	r->DR3 = 0;
	r->DR6 = 0;
	r->DR7 = 0;

	OSInt i = 8;
	while (i-- > 0)
	{
		MemCopyNO(&r->floatBuff[i * 10], &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[i], 10);
	}
	r->CTRL = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
	r->STAT = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
	r->TAG = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
	return true;
}

NN<Manage::Dasm> Manage::ThreadContextX86_32::CreateDasm() const
{
	NN<Manage::DasmX86_32> dasm;
	NEW_CLASSNN(dasm, Manage::DasmX86_32());
	return dasm;
}

void *Manage::ThreadContextX86_32::GetContext() const
{
	return this->context;
}

UInt32 Manage::ThreadContextX86_32::GetEAX() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
}

UInt32 Manage::ThreadContextX86_32::GetEDX() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
}

UInt32 Manage::ThreadContextX86_32::GetECX() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
}

UInt32 Manage::ThreadContextX86_32::GetEBX() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
}

UInt32 Manage::ThreadContextX86_32::GetESI() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
}

UInt32 Manage::ThreadContextX86_32::GetEDI() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
}

UInt32 Manage::ThreadContextX86_32::GetESP() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
}

UInt32 Manage::ThreadContextX86_32::GetEBP() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
}

UInt32 Manage::ThreadContextX86_32::GetEIP() const
{
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];
}

UInt32 Manage::ThreadContextX86_32::GetEFLAGS() const
{
	return ((ucontext_t*)this->context)->uc_flags;
}

UInt16 Manage::ThreadContextX86_32::GetCS() const
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
}

UInt16 Manage::ThreadContextX86_32::GetSS() const
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
}

UInt16 Manage::ThreadContextX86_32::GetDS() const
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
}

UInt16 Manage::ThreadContextX86_32::GetES() const
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
}

UInt16 Manage::ThreadContextX86_32::GetFS() const
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
}

UInt16 Manage::ThreadContextX86_32::GetGS() const
{
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];
}

UInt32 Manage::ThreadContextX86_32::GetDR0() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR1() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR2() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR3() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR6() const
{
	return 0;
}

UInt32 Manage::ThreadContextX86_32::GetDR7() const
{
	return 0;
}

void Manage::ThreadContextX86_32::GetST0(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 0) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST1(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 1) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST2(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 2) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST3(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 3) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST4(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 4) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST5(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 5) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST6(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[ (((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 6) & 7)], 10);
}

void Manage::ThreadContextX86_32::GetST7(UnsafeArray<UInt8> val) const
{
	MemCopyNO(val.Ptr(), &((ucontext_t*)this->context)->uc_mcontext.fpregs->_st[(((((ucontext_t*)this->context)->__fpregs_mem.sw >> 11) + 7) & 7)], 10);
}

UInt16 Manage::ThreadContextX86_32::GetCTRL() const
{
	return (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
}

UInt16 Manage::ThreadContextX86_32::GetSTAT() const
{
	return (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
}

UInt16 Manage::ThreadContextX86_32::GetTAG() const
{
	return (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
}
#endif
