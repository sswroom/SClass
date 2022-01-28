#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/DasmARM64.h"
#include "Manage/ThreadContextARM64.h"
#include "Text/MyString.h"
#include <windows.h>

#if defined(CPU_ARM64)
#define CONTEXT_TYPE CONTEXT


Manage::ThreadContextARM64::ThreadContextARM64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(CONTEXT, 1);
	MemCopyNO(this->context, context, sizeof(CONTEXT));
}

Manage::ThreadContextARM64::~ThreadContextARM64()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextARM64::GetRegisterCnt()
{
	OSInt cnt = 17;
	return cnt;
}

UTF8Char *Manage::ThreadContextARM64::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X0;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X0"));
	case 1:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X1;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X1"));
	case 2:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X2;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X2"));
	case 3:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X3;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X3"));
	case 4:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X4;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X4"));
	case 5:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X5;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X5"));
	case 6:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X6;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X6"));
	case 7:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X7;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X7"));
	case 8:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X8;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X8"));
	case 9:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X9;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X9"));
	case 10:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X10;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X10"));
	case 11:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X11;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X11"));
	case 12:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X12;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X12"));
	case 13:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X13;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X13"));
	case 14:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X14;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X14"));
	case 15:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X15;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X15"));
	case 16:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X16;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X16"));
	case 17:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X17;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X17"));
	case 18:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X18;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X18"));
	case 19:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X19;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X19"));
	case 20:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X20;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X20"));
	case 21:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X21;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X21"));
	case 22:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X22;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X22"));
	case 23:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X24;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X23"));
	case 24:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X24;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X24"));
	case 25:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X25;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X25"));
	case 26:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X26;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X26"));
	case 27:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X27;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X27"));
	case 28:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->X28;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X28"));
	case 29:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->Sp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("Sp"));
	case 30:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->Lr;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("Lr"));
	case 31:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->Pc;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("Pc"));
	case 32:
		*(UInt64*)regVal = ((CONTEXT_TYPE*)this->context)->Fp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("Fp"));
	default:
		return 0;
	}
}

void Manage::ThreadContextARM64::ToString(Text::StringBuilderUTF8 *sb)
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
			sb->Append(sbuff);
		}

		i++;
	}
}

Manage::ThreadContext::ContextType Manage::ThreadContextARM64::GetType()
{
	return Manage::ThreadContext::CT_ARM64;
}

UOSInt Manage::ThreadContextARM64::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextARM64::GetProcessId()
{
	return this->procId;
}

UOSInt Manage::ThreadContextARM64::GetInstAddr()
{
	return (UOSInt)((CONTEXT_TYPE*)this->context)->Pc;
}

UOSInt Manage::ThreadContextARM64::GetStackAddr()
{
	return (UOSInt)((CONTEXT_TYPE*)this->context)->Sp;
}

UOSInt Manage::ThreadContextARM64::GetFrameAddr()
{
	return (UOSInt)((CONTEXT_TYPE*)this->context)->Lr;
}

void Manage::ThreadContextARM64::SetInstAddr(UOSInt instAddr)
{
	((CONTEXT_TYPE*)this->context)->Pc = instAddr;
}

void Manage::ThreadContextARM64::SetStackAddr(UOSInt stackAddr)
{
	((CONTEXT_TYPE*)this->context)->Sp = stackAddr;
}

void Manage::ThreadContextARM64::SetFrameAddr(UOSInt frameAddr)
{
	((CONTEXT_TYPE*)this->context)->Lr = frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextARM64::Clone()
{
	Manage::ThreadContextARM64 *ctx;
	NEW_CLASS(ctx, Manage::ThreadContextARM64(this->procId, this->threadId, this->context));
	return ctx;
}

Bool Manage::ThreadContextARM64::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmARM64::DasmARM64_Regs *r = (Manage::DasmARM64::DasmARM64_Regs *)regs;
	r->X0 = ((CONTEXT_TYPE*)this->context)->X0;
	r->X1 = ((CONTEXT_TYPE*)this->context)->X1;
	r->X2 = ((CONTEXT_TYPE*)this->context)->X2;
	r->X3 = ((CONTEXT_TYPE*)this->context)->X3;
	r->X4 = ((CONTEXT_TYPE*)this->context)->X4;
	r->X5 = ((CONTEXT_TYPE*)this->context)->X5;
	r->X6 = ((CONTEXT_TYPE*)this->context)->X6;
	r->X7 = ((CONTEXT_TYPE*)this->context)->X7;
	r->X8 = ((CONTEXT_TYPE*)this->context)->X8;
	r->X9 = ((CONTEXT_TYPE*)this->context)->X9;
	r->X10 = ((CONTEXT_TYPE*)this->context)->X10;
	r->X11 = ((CONTEXT_TYPE*)this->context)->X11;
	r->X12 = ((CONTEXT_TYPE*)this->context)->X12;
	r->X13 = ((CONTEXT_TYPE*)this->context)->X13;
	r->X14 = ((CONTEXT_TYPE*)this->context)->X14;
	r->X15 = ((CONTEXT_TYPE*)this->context)->X15;
	r->X16 = ((CONTEXT_TYPE*)this->context)->X16;
	r->X17 = ((CONTEXT_TYPE*)this->context)->X17;
	r->X18 = ((CONTEXT_TYPE*)this->context)->X18;
	r->X19 = ((CONTEXT_TYPE*)this->context)->X19;
	r->X20 = ((CONTEXT_TYPE*)this->context)->X20;
	r->X21 = ((CONTEXT_TYPE*)this->context)->X21;
	r->X22 = ((CONTEXT_TYPE*)this->context)->X22;
	r->X23 = ((CONTEXT_TYPE*)this->context)->X23;
	r->X24 = ((CONTEXT_TYPE*)this->context)->X24;
	r->X25 = ((CONTEXT_TYPE*)this->context)->X25;
	r->X26 = ((CONTEXT_TYPE*)this->context)->X26;
	r->X27 = ((CONTEXT_TYPE*)this->context)->X27;
	r->X28 = ((CONTEXT_TYPE*)this->context)->X28;
	r->FP = ((CONTEXT_TYPE*)this->context)->Fp;
	r->SP = ((CONTEXT_TYPE*)this->context)->Sp;
	r->PC = ((CONTEXT_TYPE*)this->context)->Pc;
	r->LR = ((CONTEXT_TYPE*)this->context)->Lr;
	return true;
}

Manage::Dasm *Manage::ThreadContextARM64::CreateDasm()
{
	Manage::DasmARM64 *dasm;
	NEW_CLASS(dasm, Manage::DasmARM64());
	return dasm;
}

void *Manage::ThreadContextARM64::GetContext()
{
	return this->context;
}
#endif
