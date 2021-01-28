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

WChar *Manage::ThreadContextX86_32::GetRegister(OSInt index, WChar *buff, UInt8 *regVal, Int32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EAX");
	case 1:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EDX");
	case 2:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"ECX");
	case 3:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EBX");
	case 4:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"ESI");
	case 5:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EDI");
	case 6:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"ESP");
	case 7:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EBP");
	case 8:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EIP");
	case 9:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
		*regBitCount = 32;
		return Text::StrConcat(buff, L"EFLAGS");
	case 10:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
		*regBitCount = 16;
		return Text::StrConcat(buff, L"CS");
	case 11:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
		*regBitCount = 16;
		return Text::StrConcat(buff, L"SS");
	case 12:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
		*regBitCount = 16;
		return Text::StrConcat(buff, L"DS");
	case 13:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
		*regBitCount = 16;
		return Text::StrConcat(buff, L"ES");
	case 14:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
		*regBitCount = 16;
		return Text::StrConcat(buff, L"FS");
	case 15:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];
		*regBitCount = 16;
		return Text::StrConcat(buff, L"GS");
	case 16:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, L"DR0");
	case 17:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, L"DR1");
	case 18:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, L"DR2");
	case 19:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, L"DR3");
	case 20:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, L"DR6");
	case 21:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, L"DR7");
	case 22:
		GetST0(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST0");
	case 23:
		GetST1(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST1");
	case 24:
		GetST2(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST2");
	case 25:
		GetST3(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST3");
	case 26:
		GetST4(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST4");
	case 27:
		GetST5(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST5");
	case 28:
		GetST6(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST6");
	case 29:
		GetST7(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, L"ST7");
	case 30:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
		*regBitCount = 16;
		return Text::StrConcat(buff, L"CTRL");
	case 31:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
		*regBitCount = 16;
		return Text::StrConcat(buff, L"STAT");
	case 32:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
		*regBitCount = 16;
		return Text::StrConcat(buff, L"TAG");
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_32::ToString(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	Int32 bitCnt;
	OSInt i = 0;
	OSInt j = this->GetRegisterCnt();
	OSInt k;

	while (i < j)
	{
		if ((sptr = this->GetRegister(i, sbuff, regBuff, &bitCnt)) != 0)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" = ");
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
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"\r\n");
				break;
			case 48:
				break;
			default:
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", ");
			}

			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_32::GetType()
{
	return Manage::ThreadContext::CT_X86_32;
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

void Manage::ThreadContextX86_32::SetInstAddr(OSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP] = (UInt32)instAddr;
}

void Manage::ThreadContextX86_32::SetStackAddr(OSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP] = (UInt32)stackAddr;
}

void Manage::ThreadContextX86_32::SetFrameAddr(OSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP] = (UInt32)frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextX86_32::Clone()
{
	Manage::ThreadContextX86_32 *ret;
	NEW_CLASS(ret, Manage::ThreadContextX86_32(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_32::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmX86_32::DasmX86_32_Regs *r = (Manage::DasmX86_32::DasmX86_32_Regs *)regs;
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

Manage::Dasm *Manage::ThreadContextX86_32::CreateDasm()
{
	Manage::DasmX86_32 *dasm;
	NEW_CLASS(dasm, Manage::DasmX86_32());
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

OSInt Manage::ThreadContextX86_32::GetRegisterCnt()
{
	OSInt cnt = 33;
	return cnt;
}

UTF8Char *Manage::ThreadContextX86_32::GetRegister(OSInt index, UTF8Char *buff, UInt8 *regVal, Int32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EAX];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EAX");
	case 1:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDX];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EDX");
	case 2:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ECX];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"ECX");
	case 3:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBX];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EBX");
	case 4:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESI];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"ESI");
	case 5:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EDI];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EDI");
	case 6:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"ESP");
	case 7:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EBP");
	case 8:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EIP");
	case 9:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EFLAGS");
	case 10:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS];
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"CS");
	case 11:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_SS];
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"SS");
	case 12:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_DS];
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"DS");
	case 13:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ES];
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"ES");
	case 14:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS];
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"FS");
	case 15:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS];
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"GS");
	case 16:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR0");
	case 17:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR1");
	case 18:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR2");
	case 19:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR3");
	case 20:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR6");
	case 21:
		*(UInt32*)regVal = 0;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"DR7");
	case 22:
		GetST0(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST0");
	case 23:
		GetST1(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST1");
	case 24:
		GetST2(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST2");
	case 25:
		GetST3(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST3");
	case 26:
		GetST4(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST4");
	case 27:
		GetST5(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST5");
	case 28:
		GetST6(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST6");
	case 29:
		GetST7(regVal);
		*regBitCount = 80;
		return Text::StrConcat(buff, (const UTF8Char*)"ST7");
	case 30:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.cw;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"CTRL");
	case 31:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.sw;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"STAT");
	case 32:
		*(UInt16*)regVal = (UInt16)((ucontext_t*)this->context)->__fpregs_mem.tag;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"TAG");
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_32::ToString(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	Int32 bitCnt;
	OSInt i = 0;
	OSInt j = this->GetRegisterCnt();
	OSInt k;

	while (i < j)
	{
		if ((sptr = this->GetRegister(i, sbuff, regBuff, &bitCnt)) != 0)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" = ");
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
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"\r\n");
				break;
			case 48:
				break;
			default:
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", ");
			}

			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextX86_32::GetType()
{
	return Manage::ThreadContext::CT_X86_32;
}

UOSInt Manage::ThreadContextX86_32::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextX86_32::GetProcessId()
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

void Manage::ThreadContextX86_32::SetInstAddr(OSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EIP] = (UInt32)instAddr;
}

void Manage::ThreadContextX86_32::SetStackAddr(OSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_ESP] = (UInt32)stackAddr;
}

void Manage::ThreadContextX86_32::SetFrameAddr(OSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EBP] = (UInt32)frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextX86_32::Clone()
{
	Manage::ThreadContextX86_32 *ret;
	NEW_CLASS(ret, Manage::ThreadContextX86_32(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_32::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmX86_32::DasmX86_32_Regs *r = (Manage::DasmX86_32::DasmX86_32_Regs *)regs;
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

Manage::Dasm *Manage::ThreadContextX86_32::CreateDasm()
{
	Manage::DasmX86_32 *dasm;
	NEW_CLASS(dasm, Manage::DasmX86_32());
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
#endif
