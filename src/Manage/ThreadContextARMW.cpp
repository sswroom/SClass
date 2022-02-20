#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/DasmARM.h"
#include "Manage/ThreadContextARM.h"
#include "Text/MyString.h"
#include <windows.h>

#if defined(CPU_ARM)
#define CONTEXT_TYPE CONTEXT


Manage::ThreadContextARM::ThreadContextARM(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(CONTEXT, 1);
	MemCopyNO(this->context, context, sizeof(CONTEXT));
}

Manage::ThreadContextARM::~ThreadContextARM()
{
	MemFree(this->context);
}

OSInt Manage::ThreadContextARM::GetRegisterCnt()
{
	OSInt cnt = 17;
	return cnt;
}

UTF8Char *Manage::ThreadContextARM::GetRegister(OSInt index, UTF8Char *buff, UInt8 *regVal, Int32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R0;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R0"));
	case 1:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R1;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R1"));
	case 2:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R2;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R2"));
	case 3:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R3;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R3"));
	case 4:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R4;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R4"));
	case 5:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R5;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R5"));
	case 6:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R6;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R6"));
	case 7:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R7;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R7"));
	case 8:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R8;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R8"));
	case 9:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R9;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R9"));
	case 10:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R10;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R10"));
	case 11:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R11;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R11"));
	case 12:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->R12;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R12"));
	case 13:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Sp;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Sp"));
	case 14:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Lr;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Lr"));
	case 15:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Pc;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Pc"));
	case 16:
		*(UInt32*)regVal = ((CONTEXT_TYPE*)this->context)->Cpsr;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Psr"));
	default:
		return 0;
	}
}

void Manage::ThreadContextARM::ToString(Text::StringBuilderUTF *sb)
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
			sptr = Text::StrConcatC(sptr, UTF8STRC(" = "));
			k = bitCnt >> 3;
			while (k-- > 0)
			{
				sptr = Text::StrHexByte(sptr, regBuff[k]);
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));
			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextARM::GetType()
{
	return Manage::ThreadContext::CT_ARM;
}

UInt32 Manage::ThreadContextARM::GetThreadId()
{
	return this->threadId;
}

UInt32 Manage::ThreadContextARM::GetProcessId()
{
	return this->procId;
}

OSInt Manage::ThreadContextARM::GetInstAddr()
{
	return (OSInt)((CONTEXT_TYPE*)this->context)->Pc;
}

OSInt Manage::ThreadContextARM::GetStackAddr()
{
	return (OSInt)((CONTEXT_TYPE*)this->context)->Sp;
}

OSInt Manage::ThreadContextARM::GetFrameAddr()
{
	return (OSInt)((CONTEXT_TYPE*)this->context)->Lr;
}

void Manage::ThreadContextARM::SetInstAddr(OSInt instAddr)
{
	((CONTEXT_TYPE*)this->context)->Pc = instAddr;
}

void Manage::ThreadContextARM::SetStackAddr(OSInt stackAddr)
{
	((CONTEXT_TYPE*)this->context)->Sp = stackAddr;
}

void Manage::ThreadContextARM::SetFrameAddr(OSInt frameAddr)
{
	((CONTEXT_TYPE*)this->context)->Lr = frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextARM::Clone()
{
	Manage::ThreadContextARM *ctx;
	NEW_CLASS(ctx, Manage::ThreadContextARM(this->procId, this->threadId, this->context));
	return ctx;
}

Bool Manage::ThreadContextARM::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmARM::DasmARM_Regs *r = (Manage::DasmARM::DasmARM_Regs *)regs;
	r->R0 = this->GetR0();
	r->R1 = this->GetR1();
	r->R2 = this->GetR2();
	r->R3 = this->GetR3();
	r->R4 = this->GetR4();
	r->R5 = this->GetR5();
	r->R6 = this->GetR6();
	r->R7 = this->GetR7();
	r->R8 = this->GetR8();
	r->R9 = this->GetR9();
	r->R10 = this->GetR10();
	r->FP = this->GetFP();
	r->IP = this->GetIP();
	r->SP = this->GetSP();
	r->PC = this->GetPC();
	r->LR = this->GetLR();
	r->CPSR = this->GetCPSR();
	return true;
}

Manage::Dasm *Manage::ThreadContextARM::CreateDasm()
{
	Manage::DasmARM *dasm;
	NEW_CLASS(dasm, Manage::DasmARM());
	return dasm;
}

UInt32 Manage::ThreadContextARM::GetR0()
{
	return ((CONTEXT_TYPE*)this->context)->R0;
}

UInt32 Manage::ThreadContextARM::GetR1()
{
	return ((CONTEXT_TYPE*)this->context)->R1;
}

UInt32 Manage::ThreadContextARM::GetR2()
{
	return ((CONTEXT_TYPE*)this->context)->R2;
}

UInt32 Manage::ThreadContextARM::GetR3()
{
	return ((CONTEXT_TYPE*)this->context)->R3;
}

UInt32 Manage::ThreadContextARM::GetR4()
{
	return ((CONTEXT_TYPE*)this->context)->R4;
}

UInt32 Manage::ThreadContextARM::GetR5()
{
	return ((CONTEXT_TYPE*)this->context)->R5;
}

UInt32 Manage::ThreadContextARM::GetR6()
{
	return ((CONTEXT_TYPE*)this->context)->R6;
}

UInt32 Manage::ThreadContextARM::GetR7()
{
	return ((CONTEXT_TYPE*)this->context)->R7;
}

UInt32 Manage::ThreadContextARM::GetR8()
{
	return ((CONTEXT_TYPE*)this->context)->R8;
}

UInt32 Manage::ThreadContextARM::GetR9()
{
	return ((CONTEXT_TYPE*)this->context)->R9;
}

UInt32 Manage::ThreadContextARM::GetR10()
{
	return ((CONTEXT_TYPE*)this->context)->R10;
}

UInt32 Manage::ThreadContextARM::GetFP()
{
	return ((CONTEXT_TYPE*)this->context)->R11;
}

UInt32 Manage::ThreadContextARM::GetIP()
{
	return ((CONTEXT_TYPE*)this->context)->R12;
}

UInt32 Manage::ThreadContextARM::GetSP()
{
	return ((CONTEXT_TYPE*)this->context)->Sp;
}

UInt32 Manage::ThreadContextARM::GetPC()
{
	return ((CONTEXT_TYPE*)this->context)->Pc;
}

UInt32 Manage::ThreadContextARM::GetLR()
{
	return ((CONTEXT_TYPE*)this->context)->Lr;
}

UInt32 Manage::ThreadContextARM::GetCPSR()
{
	return ((CONTEXT_TYPE*)this->context)->Cpsr;
}

void *Manage::ThreadContextARM::GetContext()
{
	return this->context;
}
#endif
