#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmX86_64.h"
#include "Manage/ThreadContextX86_64.h"
#include <windows.h>

#define CONTEXT_TYPE CONTEXT


Manage::ThreadContextX86_64::ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(CONTEXT_TYPE, 1);
	MemCopyNO(this->context, context, sizeof(CONTEXT_TYPE));
}

Manage::ThreadContextX86_64::~ThreadContextX86_64()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextX86_64::GetRegisterCnt()
{
	UOSInt cnt = 46;
	return cnt;
}

UTF8Char *Manage::ThreadContextX86_64::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	CONTEXT_TYPE *ctx = (CONTEXT_TYPE*)this->context;
	switch (index)
	{
	case 0:
		*(UInt64*)regVal = ctx->Rax;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RAX");
	case 1:
		*(UInt64*)regVal = ctx->Rdx;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RDX");
	case 2:
		*(UInt64*)regVal = ctx->Rcx;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RCX");
	case 3:
		*(UInt64*)regVal = ctx->Rbx;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RBX");
	case 4:
		*(UInt64*)regVal = ctx->Rsi;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RSI");
	case 5:
		*(UInt64*)regVal = ctx->Rdi;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RDI");
	case 6:
		*(UInt64*)regVal = ctx->Rsp;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RSP");
	case 7:
		*(UInt64*)regVal = ctx->Rbp;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RBP");
	case 8:
		*(UInt64*)regVal = ctx->R8;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R8");
	case 9:
		*(UInt64*)regVal = ctx->R9;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R9");
	case 10:
		*(UInt64*)regVal = ctx->R10;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R10");
	case 11:
		*(UInt64*)regVal = ctx->R11;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R11");
	case 12:
		*(UInt64*)regVal = ctx->R12;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R12");
	case 13:
		*(UInt64*)regVal = ctx->R13;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R13");
	case 14:
		*(UInt64*)regVal = ctx->R14;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R14");
	case 15:
		*(UInt64*)regVal = ctx->R15;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"R15");
	case 16:
		*(UInt64*)regVal = ctx->Rip;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"RIP");
	case 17:
		*(UInt32*)regVal = ctx->EFlags;
		*regBitCount = 32;
		return Text::StrConcat(buff, (const UTF8Char*)"EFLAGS");
	case 18:
		*(UInt16*)regVal = ctx->SegCs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"CS");
	case 19:
		*(UInt16*)regVal = ctx->SegSs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"SS");
	case 20:
		*(UInt16*)regVal = ctx->SegDs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"DS");
	case 21:
		*(UInt16*)regVal = ctx->SegEs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"ES");
	case 22:
		*(UInt16*)regVal = ctx->SegFs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"FS");
	case 23:
		*(UInt16*)regVal = ctx->SegGs;
		*regBitCount = 16;
		return Text::StrConcat(buff, (const UTF8Char*)"GS");
	case 24:
		*(UInt64*)regVal = (UInt32)ctx->Dr0;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"DR0");
	case 25:
		*(UInt64*)regVal = ctx->Dr1;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"DR1");
	case 26:
		*(UInt64*)regVal = ctx->Dr2;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"DR2");
	case 27:
		*(UInt64*)regVal = ctx->Dr3;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"DR3");
	case 28:
		*(UInt64*)regVal = ctx->Dr6;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"DR6");
	case 29:
		*(UInt64*)regVal = ctx->Dr7;
		*regBitCount = 64;
		return Text::StrConcat(buff, (const UTF8Char*)"DR7");
	case 30:
		MemCopyNO(regVal, &ctx->Xmm0, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM0");
	case 31:
		MemCopyNO(regVal, &ctx->Xmm1, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM1");
	case 32:
		MemCopyNO(regVal, &ctx->Xmm2, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM2");
	case 33:
		MemCopyNO(regVal, &ctx->Xmm3, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM3");
	case 34:
		MemCopyNO(regVal, &ctx->Xmm4, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM4");
	case 35:
		MemCopyNO(regVal, &ctx->Xmm5, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM5");
	case 36:
		MemCopyNO(regVal, &ctx->Xmm6, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM6");
	case 37:
		MemCopyNO(regVal, &ctx->Xmm7, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM7");
	case 38:
		MemCopyNO(regVal, &ctx->Xmm8, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM8");
	case 39:
		MemCopyNO(regVal, &ctx->Xmm9, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM9");
	case 40:
		MemCopyNO(regVal, &ctx->Xmm10, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM10");
	case 41:
		MemCopyNO(regVal, &ctx->Xmm11, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM11");
	case 42:
		MemCopyNO(regVal, &ctx->Xmm12, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM12");
	case 43:
		MemCopyNO(regVal, &ctx->Xmm13, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM13");
	case 44:
		MemCopyNO(regVal, &ctx->Xmm14, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM14");
	case 45:
		MemCopyNO(regVal, &ctx->Xmm15, 16);
		*regBitCount = 128;
		return Text::StrConcat(buff, (const UTF8Char*)"XMM15");
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_64::ToString(Text::StringBuilderUTF *sb)
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
			switch (i)
			{
			case 3:
			case 7:
			case 11:
			case 15:
			case 17:
			case 23:
			case 29:
			case 33:
			case 37:
			case 41:
			case 45:
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

Manage::ThreadContext::ContextType Manage::ThreadContextX86_64::GetType()
{
	return Manage::ThreadContext::CT_X86_64;
}

UOSInt Manage::ThreadContextX86_64::GetThreadId()
{
	return this->threadId;
}

UOSInt Manage::ThreadContextX86_64::GetProcessId()
{
	return this->procId;
}

UOSInt Manage::ThreadContextX86_64::GetInstAddr()
{
	return this->GetRIP();
}

UOSInt Manage::ThreadContextX86_64::GetStackAddr()
{
	return this->GetRSP();
}

UOSInt Manage::ThreadContextX86_64::GetFrameAddr()
{
	return this->GetRBP();
}

void Manage::ThreadContextX86_64::SetInstAddr(UOSInt instAddr)
{
	((CONTEXT_TYPE*)this->context)->Rip = instAddr;
}

void Manage::ThreadContextX86_64::SetStackAddr(UOSInt stackAddr)
{
	((CONTEXT_TYPE*)this->context)->Rsp = stackAddr;
}

void Manage::ThreadContextX86_64::SetFrameAddr(UOSInt frameAddr)
{
	((CONTEXT_TYPE*)this->context)->Rbp = frameAddr;
}

Manage::ThreadContext *Manage::ThreadContextX86_64::Clone()
{
	Manage::ThreadContextX86_64 *ret;
	NEW_CLASS(ret, Manage::ThreadContextX86_64(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_64::GetRegs(Manage::Dasm::Dasm_Regs *regs)
{
	Manage::DasmX86_64::DasmX86_64_Regs *r = (Manage::DasmX86_64::DasmX86_64_Regs *)regs;
	r->rax = ((CONTEXT_TYPE*)this->context)->Rax;
	r->rdx = ((CONTEXT_TYPE*)this->context)->Rdx;
	r->rcx = ((CONTEXT_TYPE*)this->context)->Rcx;
	r->rbx = ((CONTEXT_TYPE*)this->context)->Rbx;
	r->rsi = ((CONTEXT_TYPE*)this->context)->Rsi;
	r->rdi = ((CONTEXT_TYPE*)this->context)->Rdi;
	r->rsp = ((CONTEXT_TYPE*)this->context)->Rsp;
	r->rbp = ((CONTEXT_TYPE*)this->context)->Rbp;
	r->rip = ((CONTEXT_TYPE*)this->context)->Rip;
	r->r8 = ((CONTEXT_TYPE*)this->context)->R8;
	r->r9 = ((CONTEXT_TYPE*)this->context)->R9;
	r->r10 = ((CONTEXT_TYPE*)this->context)->R10;
	r->r11 = ((CONTEXT_TYPE*)this->context)->R11;
	r->r12 = ((CONTEXT_TYPE*)this->context)->R12;
	r->r13 = ((CONTEXT_TYPE*)this->context)->R13;
	r->r14 = ((CONTEXT_TYPE*)this->context)->R14;
	r->r15 = ((CONTEXT_TYPE*)this->context)->R15;
	r->EFLAGS = ((CONTEXT_TYPE*)this->context)->EFlags;
	r->cs = ((CONTEXT_TYPE*)this->context)->SegCs;
	r->ss = ((CONTEXT_TYPE*)this->context)->SegSs;
	r->ds = ((CONTEXT_TYPE*)this->context)->SegDs;
	r->es = ((CONTEXT_TYPE*)this->context)->SegEs;
	r->fs = ((CONTEXT_TYPE*)this->context)->SegFs;
	r->gs = ((CONTEXT_TYPE*)this->context)->SegGs;
	r->dr0 = ((CONTEXT_TYPE*)this->context)->Dr0;
	r->dr1 = ((CONTEXT_TYPE*)this->context)->Dr1;
	r->dr2 = ((CONTEXT_TYPE*)this->context)->Dr2;
	r->dr3 = ((CONTEXT_TYPE*)this->context)->Dr3;
	r->dr6 = ((CONTEXT_TYPE*)this->context)->Dr6;
	r->dr7 = ((CONTEXT_TYPE*)this->context)->Dr7;
	return true;
}

Manage::Dasm *Manage::ThreadContextX86_64::CreateDasm()
{
	Manage::DasmX86_64 *dasm;
	NEW_CLASS(dasm, Manage::DasmX86_64());
	return dasm;
}

void *Manage::ThreadContextX86_64::GetContext()
{
	return this->context;
}

UInt64 Manage::ThreadContextX86_64::GetRAX()
{
	return ((CONTEXT_TYPE*)this->context)->Rax;
}

UInt64 Manage::ThreadContextX86_64::GetRDX()
{
	return ((CONTEXT_TYPE*)this->context)->Rdx;
}

UInt64 Manage::ThreadContextX86_64::GetRCX()
{
	return ((CONTEXT_TYPE*)this->context)->Rcx;
}

UInt64 Manage::ThreadContextX86_64::GetRBX()
{
	return ((CONTEXT_TYPE*)this->context)->Rbx;
}

UInt64 Manage::ThreadContextX86_64::GetRSI()
{
	return ((CONTEXT_TYPE*)this->context)->Rsi;
}

UInt64 Manage::ThreadContextX86_64::GetRDI()
{
	return ((CONTEXT_TYPE*)this->context)->Rdi;
}

UInt64 Manage::ThreadContextX86_64::GetRSP()
{
	return ((CONTEXT_TYPE*)this->context)->Rsp;
}

UInt64 Manage::ThreadContextX86_64::GetRBP()
{
	return ((CONTEXT_TYPE*)this->context)->Rbp;
}

UInt64 Manage::ThreadContextX86_64::GetRIP()
{
	return ((CONTEXT_TYPE*)this->context)->Rip;
}

UInt64 Manage::ThreadContextX86_64::GetR8()
{
	return ((CONTEXT_TYPE*)this->context)->R8;
}

UInt64 Manage::ThreadContextX86_64::GetR9()
{
	return ((CONTEXT_TYPE*)this->context)->R9;
}

UInt64 Manage::ThreadContextX86_64::GetR10()
{
	return ((CONTEXT_TYPE*)this->context)->R10;
}

UInt64 Manage::ThreadContextX86_64::GetR11()
{
	return ((CONTEXT_TYPE*)this->context)->R11;
}

UInt64 Manage::ThreadContextX86_64::GetR12()
{
	return ((CONTEXT_TYPE*)this->context)->R12;
}

UInt64 Manage::ThreadContextX86_64::GetR13()
{
	return ((CONTEXT_TYPE*)this->context)->R13;
}

UInt64 Manage::ThreadContextX86_64::GetR14()
{
	return ((CONTEXT_TYPE*)this->context)->R14;
}

UInt64 Manage::ThreadContextX86_64::GetR15()
{
	return ((CONTEXT_TYPE*)this->context)->R15;
}

UInt32 Manage::ThreadContextX86_64::GetEFLAGS()
{
	return ((CONTEXT_TYPE*)this->context)->EFlags;
}

UInt16 Manage::ThreadContextX86_64::GetCS()
{
	return ((CONTEXT_TYPE*)this->context)->SegCs;
}

UInt16 Manage::ThreadContextX86_64::GetSS()
{
	return ((CONTEXT_TYPE*)this->context)->SegSs;
}

UInt16 Manage::ThreadContextX86_64::GetDS()
{
	return ((CONTEXT_TYPE*)this->context)->SegDs;
}

UInt16 Manage::ThreadContextX86_64::GetES()
{
	return ((CONTEXT_TYPE*)this->context)->SegEs;
}

UInt16 Manage::ThreadContextX86_64::GetFS()
{
	return ((CONTEXT_TYPE*)this->context)->SegFs;
}

UInt16 Manage::ThreadContextX86_64::GetGS()
{
	return ((CONTEXT_TYPE*)this->context)->SegGs;
}

UInt64 Manage::ThreadContextX86_64::GetDR0()
{
	return ((CONTEXT_TYPE*)this->context)->Dr0;
}

UInt64 Manage::ThreadContextX86_64::GetDR1()
{
	return ((CONTEXT_TYPE*)this->context)->Dr1;
}

UInt64 Manage::ThreadContextX86_64::GetDR2()
{
	return ((CONTEXT_TYPE*)this->context)->Dr2;
}

UInt64 Manage::ThreadContextX86_64::GetDR3()
{
	return ((CONTEXT_TYPE*)this->context)->Dr3;
}

UInt64 Manage::ThreadContextX86_64::GetDR6()
{
	return ((CONTEXT_TYPE*)this->context)->Dr6;
}

UInt64 Manage::ThreadContextX86_64::GetDR7()
{
	return ((CONTEXT_TYPE*)this->context)->Dr7;
}
