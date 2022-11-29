#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmARM.h"
#include "Manage/ThreadContextARM.h"
#include <sys/ucontext.h>

Manage::ThreadContextARM::ThreadContextARM(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextARM::~ThreadContextARM()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextARM::GetRegisterCnt()
{
	UOSInt cnt = 17;
	return cnt;
}

UTF8Char *Manage::ThreadContextARM::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r0;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R0"));
	case 1:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r1;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R1"));
	case 2:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r2;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R2"));
	case 3:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r3;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R3"));
	case 4:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r4;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R4"));
	case 5:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r5;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R5"));
	case 6:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r6;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R6"));
	case 7:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r7;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R7"));
	case 8:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r8;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R8"));
	case 9:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r9;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R9"));
	case 10:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_r10;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("R10"));
	case 11:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_fp;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("FP (R11)"));
	case 12:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_ip;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("IP (R12)"));
	case 13:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_sp;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Sp"));
	case 14:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_lr;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Lr"));
	case 15:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_pc;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Pc"));
	case 16:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.arm_cpsr;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("Cpsr"));
	default:
		return 0;
	}
}

void Manage::ThreadContextARM::ToString(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UOSInt i = 0;
	UOSInt j = this->GetRegisterCnt();
	UOSInt k;

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
			sb->AppendP(sbuff, sptr);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextARM::GetType()
{
	return Manage::ThreadContext::ContextType::ARM;
}

UOSInt Manage::ThreadContextARM::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextARM::GetProcessId()
{
	return this->procId;
}

UOSInt Manage::ThreadContextARM::GetInstAddr()
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext.arm_pc;
}

UOSInt Manage::ThreadContextARM::GetStackAddr()
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext.arm_sp;
}

UOSInt Manage::ThreadContextARM::GetFrameAddr()
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext.arm_lr;
}

void Manage::ThreadContextARM::SetInstAddr(UOSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.arm_pc = instAddr;
}

void Manage::ThreadContextARM::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.arm_sp = stackAddr;
}

void Manage::ThreadContextARM::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.arm_lr = frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextARM::Clone()
{
	Manage::ThreadContextARM *cont;
	NEW_CLASS(cont, Manage::ThreadContextARM(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextARM::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmARM::DasmARM_Regs *r = (Manage::DasmARM::DasmARM_Regs *)regs;
	r->R0 = ((ucontext_t*)this->context)->uc_mcontext.arm_r0;
	r->R1 = ((ucontext_t*)this->context)->uc_mcontext.arm_r1;
	r->R2 = ((ucontext_t*)this->context)->uc_mcontext.arm_r2;
	r->R3 = ((ucontext_t*)this->context)->uc_mcontext.arm_r3;
	r->R4 = ((ucontext_t*)this->context)->uc_mcontext.arm_r4;
	r->R5 = ((ucontext_t*)this->context)->uc_mcontext.arm_r5;
	r->R6 = ((ucontext_t*)this->context)->uc_mcontext.arm_r6;
	r->R7 = ((ucontext_t*)this->context)->uc_mcontext.arm_r7;
	r->R8 = ((ucontext_t*)this->context)->uc_mcontext.arm_r8;
	r->R9 = ((ucontext_t*)this->context)->uc_mcontext.arm_r9;
	r->R10 = ((ucontext_t*)this->context)->uc_mcontext.arm_r10;
	r->FP = ((ucontext_t*)this->context)->uc_mcontext.arm_fp;
	r->IP = ((ucontext_t*)this->context)->uc_mcontext.arm_ip;
	r->SP = ((ucontext_t*)this->context)->uc_mcontext.arm_sp;
	r->SP = ((ucontext_t*)this->context)->uc_mcontext.arm_pc;
	r->LR = ((ucontext_t*)this->context)->uc_mcontext.arm_lr;
	r->CPSR = ((ucontext_t*)this->context)->uc_mcontext.arm_cpsr;
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
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r0;
}

UInt32 Manage::ThreadContextARM::GetR1()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r1;
}

UInt32 Manage::ThreadContextARM::GetR2()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r2;
}

UInt32 Manage::ThreadContextARM::GetR3()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r3;
}

UInt32 Manage::ThreadContextARM::GetR4()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r4;
}

UInt32 Manage::ThreadContextARM::GetR5()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r5;
}

UInt32 Manage::ThreadContextARM::GetR6()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r6;
}

UInt32 Manage::ThreadContextARM::GetR7()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r7;
}

UInt32 Manage::ThreadContextARM::GetR8()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r8;
}

UInt32 Manage::ThreadContextARM::GetR9()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r9;
}

UInt32 Manage::ThreadContextARM::GetR10()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_r10;
}

UInt32 Manage::ThreadContextARM::GetFP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_fp;
}

UInt32 Manage::ThreadContextARM::GetIP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_ip;
}

UInt32 Manage::ThreadContextARM::GetSP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_sp;
}

UInt32 Manage::ThreadContextARM::GetPC()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_pc;
}

UInt32 Manage::ThreadContextARM::GetLR()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_lr;
}

UInt32 Manage::ThreadContextARM::GetCPSR()
{
	return ((ucontext_t*)this->context)->uc_mcontext.arm_cpsr;
}

void *Manage::ThreadContextARM::GetContext()
{
	return this->context;
}

