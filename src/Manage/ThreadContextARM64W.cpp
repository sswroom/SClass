#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/DasmARM64.h"
#include "Manage/ThreadContextARM64.h"
#include "Text/MyString.h"
#include <windows.h>

#if defined(CPU_ARM64)
#define CONTEXT_TYPE CONTEXT


Manage::ThreadContextARM64::ThreadContextARM64(UIntOS procId, UIntOS threadId, void *context)
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

UIntOS Manage::ThreadContextARM64::GetRegisterCnt() const
{
	IntOS cnt = 65;
	return cnt;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextARM64::GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UInt8 *regVal, UInt32 *regBitCount) const
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
	case 33:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[0];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q0"));
	case 34:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[1];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q1"));
	case 35:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[2];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q2"));
	case 36:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[3];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q3"));
	case 37:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[4];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q4"));
	case 38:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[5];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q5"));
	case 39:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[6];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q6"));
	case 40:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[7];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q7"));
	case 41:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[8];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q8"));
	case 42:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[9];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q9"));
	case 43:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[10];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q10"));
	case 44:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[11];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q11"));
	case 45:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[12];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q12"));
	case 46:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[13];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q13"));
	case 47:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[14];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q14"));
	case 48:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[15];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q15"));
	case 49:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[16];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q16"));
	case 50:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[17];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q17"));
	case 51:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[18];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q18"));
	case 52:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[19];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q19"));
	case 53:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[20];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q20"));
	case 54:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[21];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q21"));
	case 55:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[22];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q22"));
	case 56:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[23];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q23"));
	case 57:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[24];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q24"));
	case 58:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[25];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q25"));
	case 59:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[26];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q26"));
	case 60:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[27];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q27"));
	case 61:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[28];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q28"));
	case 62:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[29];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q29"));
	case 63:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[30];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q30"));
	case 64:
		*(ARM64_NT_NEON128*)regVal = ((CONTEXT_TYPE*)this->context)->V[31];
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("Q31"));
	default:
		return 0;
	}
}

void Manage::ThreadContextARM64::ToString(NN<Text::StringBuilderUTF8> sb) const
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
		if (this->GetRegister(i, sbuff, regBuff, &bitCnt).SetTo(sptr))
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

Manage::ThreadContext::ContextType Manage::ThreadContextARM64::GetType() const
{
	return Manage::ThreadContext::ContextType::ARM64;
}

UIntOS Manage::ThreadContextARM64::GetThreadId() const
{
	return this->threadId;
}

UIntOS Manage::ThreadContextARM64::GetProcessId() const
{
	return this->procId;
}

UIntOS Manage::ThreadContextARM64::GetInstAddr() const
{
	return (UIntOS)((CONTEXT_TYPE*)this->context)->Pc;
}

UIntOS Manage::ThreadContextARM64::GetStackAddr() const
{
	return (UIntOS)((CONTEXT_TYPE*)this->context)->Sp;
}

UIntOS Manage::ThreadContextARM64::GetFrameAddr() const
{
	return (UIntOS)((CONTEXT_TYPE*)this->context)->Lr;
}

void Manage::ThreadContextARM64::SetInstAddr(UIntOS instAddr)
{
	((CONTEXT_TYPE*)this->context)->Pc = instAddr;
}

void Manage::ThreadContextARM64::SetStackAddr(UIntOS stackAddr)
{
	((CONTEXT_TYPE*)this->context)->Sp = stackAddr;
}

void Manage::ThreadContextARM64::SetFrameAddr(UIntOS frameAddr)
{
	((CONTEXT_TYPE*)this->context)->Lr = frameAddr;
}

NN<Manage::ThreadContext> Manage::ThreadContextARM64::Clone() const
{
	NN<Manage::ThreadContextARM64> ctx;
	NEW_CLASSNN(ctx, Manage::ThreadContextARM64(this->procId, this->threadId, this->context));
	return ctx;
}

Bool Manage::ThreadContextARM64::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	NN<Manage::DasmARM64::DasmARM64_Regs> r = NN<Manage::DasmARM64::DasmARM64_Regs>::ConvertFrom(regs);
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
	r->X29 = ((CONTEXT_TYPE*)this->context)->Fp;
	r->SP = ((CONTEXT_TYPE*)this->context)->Sp;
	r->PC = ((CONTEXT_TYPE*)this->context)->Pc;
	r->LR = ((CONTEXT_TYPE*)this->context)->Lr;
	return true;
}

Optional<Manage::Dasm> Manage::ThreadContextARM64::CreateDasm() const
{
	Manage::DasmARM64 *dasm;
	NEW_CLASS(dasm, Manage::DasmARM64());
	return dasm;
}

void *Manage::ThreadContextARM64::GetContext() const
{
	return this->context;
}
#endif
