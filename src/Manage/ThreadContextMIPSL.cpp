#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmMIPS.h"
#include "Manage/ThreadContextMIPS.h"
#include <sys/ucontext.h>

Manage::ThreadContextMIPS::ThreadContextMIPS(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextMIPS::~ThreadContextMIPS()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextMIPS::GetRegisterCnt()
{
	OSInt cnt = 17;
	return cnt;
}

UTF8Char *Manage::ThreadContextMIPS::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$zero");
	case 1:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$at");
	case 2:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$v0");
	case 3:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$v1");
	case 4:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$a0");
	case 5:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$a1");
	case 6:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$a2");
	case 7:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$a3");
	case 8:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t0");
	case 9:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t1");
	case 10:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t2");
	case 11:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t3");
	case 12:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t4");
	case 13:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t5");
	case 14:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t6");
	case 15:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t7");
	case 16:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s0");
	case 17:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s1");
	case 18:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s2");
	case 19:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s3");
	case 20:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s4");
	case 21:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s5");
	case 22:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s6");
	case 23:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$s7");
	case 24:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t8");
	case 25:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$t9");
	case 26:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$k0");
	case 27:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$k1");
	case 28:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$gp");
	case 29:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$sp");
	case 30:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$fp");
	case 31:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[index];
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$ra");
	case 32:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.pc;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"$pc");
	default:
		return 0;
	}
}

void Manage::ThreadContextMIPS::ToString(Text::StringBuilderUTF8 *sb)
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
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" = ");
			k = bitCnt >> 3;
			while (k-- > 0)
			{
				sptr = Text::StrHexByte(sptr, regBuff[k]);
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"\r\n");
			sb->AppendP(sbuff, sptr);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextMIPS::GetType()
{
	return Manage::ThreadContext::CT_MIPS;
}

UOSInt Manage::ThreadContextMIPS::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextMIPS::GetProcessId()
{
	return this->procId;
}

UOSInt Manage::ThreadContextMIPS::GetInstAddr()
{
	return (OSInt)((ucontext_t*)this->context)->uc_mcontext.pc;
}

UOSInt Manage::ThreadContextMIPS::GetStackAddr()
{
	return (OSInt)((ucontext_t*)this->context)->uc_mcontext.gregs[29];
}

UOSInt Manage::ThreadContextMIPS::GetFrameAddr()
{
	return (OSInt)((ucontext_t*)this->context)->uc_mcontext.gregs[30];
}

void Manage::ThreadContextMIPS::SetInstAddr(UOSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.pc = instAddr;
}

void Manage::ThreadContextMIPS::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[29] = stackAddr;
}

void Manage::ThreadContextMIPS::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[30] = frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextMIPS::Clone()
{
	Manage::ThreadContextMIPS *cont;
	NEW_CLASS(cont, Manage::ThreadContextMIPS(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextMIPS::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmMIPS::DasmMIPS_Regs *r = (Manage::DasmMIPS::DasmMIPS_Regs *)regs;
	r->zero = ((ucontext_t*)this->context)->uc_mcontext.gregs[0];
	r->at = ((ucontext_t*)this->context)->uc_mcontext.gregs[1];
	r->v0 = ((ucontext_t*)this->context)->uc_mcontext.gregs[2];
	r->v1 = ((ucontext_t*)this->context)->uc_mcontext.gregs[3];
	r->a0 = ((ucontext_t*)this->context)->uc_mcontext.gregs[4];
	r->a1 = ((ucontext_t*)this->context)->uc_mcontext.gregs[5];
	r->a2 = ((ucontext_t*)this->context)->uc_mcontext.gregs[6];
	r->a3 = ((ucontext_t*)this->context)->uc_mcontext.gregs[7];
	r->t0 = ((ucontext_t*)this->context)->uc_mcontext.gregs[8];
	r->t1 = ((ucontext_t*)this->context)->uc_mcontext.gregs[9];
	r->t2 = ((ucontext_t*)this->context)->uc_mcontext.gregs[10];
	r->t3 = ((ucontext_t*)this->context)->uc_mcontext.gregs[11];
	r->t4 = ((ucontext_t*)this->context)->uc_mcontext.gregs[12];
	r->t5 = ((ucontext_t*)this->context)->uc_mcontext.gregs[13];
	r->t6 = ((ucontext_t*)this->context)->uc_mcontext.gregs[14];
	r->t7 = ((ucontext_t*)this->context)->uc_mcontext.gregs[15];
	r->s0 = ((ucontext_t*)this->context)->uc_mcontext.gregs[16];
	r->s1 = ((ucontext_t*)this->context)->uc_mcontext.gregs[17];
	r->s2 = ((ucontext_t*)this->context)->uc_mcontext.gregs[18];
	r->s3 = ((ucontext_t*)this->context)->uc_mcontext.gregs[19];
	r->s4 = ((ucontext_t*)this->context)->uc_mcontext.gregs[20];
	r->s5 = ((ucontext_t*)this->context)->uc_mcontext.gregs[21];
	r->s6 = ((ucontext_t*)this->context)->uc_mcontext.gregs[22];
	r->s7 = ((ucontext_t*)this->context)->uc_mcontext.gregs[23];
	r->t8 = ((ucontext_t*)this->context)->uc_mcontext.gregs[24];
	r->t9 = ((ucontext_t*)this->context)->uc_mcontext.gregs[25];
	r->k0 = ((ucontext_t*)this->context)->uc_mcontext.gregs[26];
	r->k1 = ((ucontext_t*)this->context)->uc_mcontext.gregs[27];
	r->gp = ((ucontext_t*)this->context)->uc_mcontext.gregs[28];
	r->sp = ((ucontext_t*)this->context)->uc_mcontext.gregs[29];
	r->fp = ((ucontext_t*)this->context)->uc_mcontext.gregs[30];
	r->ra = ((ucontext_t*)this->context)->uc_mcontext.gregs[31];

	r->pc = ((ucontext_t*)this->context)->uc_mcontext.pc;
	r->hi = 0;
	r->lo = 0;
	return true;
}

Manage::Dasm *Manage::ThreadContextMIPS::CreateDasm()
{
	Manage::DasmMIPS *dasm;
	NEW_CLASS(dasm, Manage::DasmMIPS());
	return dasm;
}

void *Manage::ThreadContextMIPS::GetContext()
{
	return this->context;
}

