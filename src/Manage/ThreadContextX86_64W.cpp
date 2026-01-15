#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmX86_64.h"
#include "Manage/ThreadContextX86_64.h"
#include <windows.h>

#define CONTEXT_TYPE CONTEXT


Manage::ThreadContextX86_64::ThreadContextX86_64(UIntOS procId, UIntOS threadId, void *context)
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

UIntOS Manage::ThreadContextX86_64::GetRegisterCnt() const
{
	UIntOS cnt = 46;
	return cnt;
}

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextX86_64::GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const
{
	CONTEXT_TYPE *ctx = (CONTEXT_TYPE*)this->context;
	switch (index)
	{
	case 0:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rax;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RAX"));
	case 1:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rdx;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RDX"));
	case 2:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rcx;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RCX"));
	case 3:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rbx;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RBX"));
	case 4:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rsi;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RSI"));
	case 5:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rdi;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RDI"));
	case 6:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rsp;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RSP"));
	case 7:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rbp;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RBP"));
	case 8:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R8;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R8"));
	case 9:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R9;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R9"));
	case 10:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R10;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R10"));
	case 11:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R11;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R11"));
	case 12:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R12;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R12"));
	case 13:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R13;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R13"));
	case 14:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R14;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R14"));
	case 15:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->R15;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("R15"));
	case 16:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Rip;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("RIP"));
	case 17:
		UnsafeArray<UInt32>::ConvertFrom(regVal)[0] = ctx->EFlags;
		regBitCount.Set(32);
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 18:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = ctx->SegCs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 19:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = ctx->SegSs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("SS"));
	case 20:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = ctx->SegDs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("DS"));
	case 21:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = ctx->SegEs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("ES"));
	case 22:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = ctx->SegFs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 23:
		UnsafeArray<UInt16>::ConvertFrom(regVal)[0] = ctx->SegGs;
		regBitCount.Set(16);
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	case 24:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = (UInt32)ctx->Dr0;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("DR0"));
	case 25:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Dr1;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("DR1"));
	case 26:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Dr2;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("DR2"));
	case 27:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Dr3;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("DR3"));
	case 28:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Dr6;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("DR6"));
	case 29:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ctx->Dr7;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("DR7"));
	case 30:
		MemCopyNO(&regVal[0], &ctx->Xmm0, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM0"));
	case 31:
		MemCopyNO(&regVal[0], &ctx->Xmm1, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM1"));
	case 32:
		MemCopyNO(&regVal[0], &ctx->Xmm2, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM2"));
	case 33:
		MemCopyNO(&regVal[0], &ctx->Xmm3, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM3"));
	case 34:
		MemCopyNO(&regVal[0], &ctx->Xmm4, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM4"));
	case 35:
		MemCopyNO(&regVal[0], &ctx->Xmm5, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM5"));
	case 36:
		MemCopyNO(&regVal[0], &ctx->Xmm6, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM6"));
	case 37:
		MemCopyNO(&regVal[0], &ctx->Xmm7, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM7"));
	case 38:
		MemCopyNO(&regVal[0], &ctx->Xmm8, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM8"));
	case 39:
		MemCopyNO(&regVal[0], &ctx->Xmm9, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM9"));
	case 40:
		MemCopyNO(&regVal[0], &ctx->Xmm10, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM10"));
	case 41:
		MemCopyNO(&regVal[0], &ctx->Xmm11, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM11"));
	case 42:
		MemCopyNO(&regVal[0], &ctx->Xmm12, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM12"));
	case 43:
		MemCopyNO(&regVal[0], &ctx->Xmm13, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM13"));
	case 44:
		MemCopyNO(&regVal[0], &ctx->Xmm14, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM14"));
	case 45:
		MemCopyNO(&regVal[0], &ctx->Xmm15, 16);
		regBitCount.Set(128);
		return Text::StrConcatC(buff, UTF8STRC("XMM15"));
	default:
		return nullptr;
	}
}

void Manage::ThreadContextX86_64::ToString(NN<Text::StringBuilderUTF8> sb) const
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

Manage::ThreadContext::ContextType Manage::ThreadContextX86_64::GetType() const
{
	return Manage::ThreadContext::ContextType::X86_64;
}

UIntOS Manage::ThreadContextX86_64::GetThreadId() const
{
	return this->threadId;
}

UIntOS Manage::ThreadContextX86_64::GetProcessId() const
{
	return this->procId;
}

UIntOS Manage::ThreadContextX86_64::GetInstAddr() const
{
	return this->GetRIP();
}

UIntOS Manage::ThreadContextX86_64::GetStackAddr() const
{
	return this->GetRSP();
}

UIntOS Manage::ThreadContextX86_64::GetFrameAddr() const
{
	return this->GetRBP();
}

void Manage::ThreadContextX86_64::SetInstAddr(UIntOS instAddr)
{
	((CONTEXT_TYPE*)this->context)->Rip = instAddr;
}

void Manage::ThreadContextX86_64::SetStackAddr(UIntOS stackAddr)
{
	((CONTEXT_TYPE*)this->context)->Rsp = stackAddr;
}

void Manage::ThreadContextX86_64::SetFrameAddr(UIntOS frameAddr)
{
	((CONTEXT_TYPE*)this->context)->Rbp = frameAddr;
}

NN<Manage::ThreadContext> Manage::ThreadContextX86_64::Clone() const
{
	NN<Manage::ThreadContextX86_64> ret;
	NEW_CLASSNN(ret, Manage::ThreadContextX86_64(this->procId, this->threadId, this->context));
	return ret;
}

Bool Manage::ThreadContextX86_64::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	NN<Manage::DasmX86_64::DasmX86_64_Regs> r = NN<Manage::DasmX86_64::DasmX86_64_Regs>::ConvertFrom(regs);
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

Optional<Manage::Dasm> Manage::ThreadContextX86_64::CreateDasm() const
{
	Manage::DasmX86_64 *dasm;
	NEW_CLASS(dasm, Manage::DasmX86_64());
	return dasm;
}

void *Manage::ThreadContextX86_64::GetContext() const
{
	return this->context;
}

UInt64 Manage::ThreadContextX86_64::GetRAX() const
{
	return ((CONTEXT_TYPE*)this->context)->Rax;
}

UInt64 Manage::ThreadContextX86_64::GetRDX() const
{
	return ((CONTEXT_TYPE*)this->context)->Rdx;
}

UInt64 Manage::ThreadContextX86_64::GetRCX() const
{
	return ((CONTEXT_TYPE*)this->context)->Rcx;
}

UInt64 Manage::ThreadContextX86_64::GetRBX() const
{
	return ((CONTEXT_TYPE*)this->context)->Rbx;
}

UInt64 Manage::ThreadContextX86_64::GetRSI() const
{
	return ((CONTEXT_TYPE*)this->context)->Rsi;
}

UInt64 Manage::ThreadContextX86_64::GetRDI() const
{
	return ((CONTEXT_TYPE*)this->context)->Rdi;
}

UInt64 Manage::ThreadContextX86_64::GetRSP() const
{
	return ((CONTEXT_TYPE*)this->context)->Rsp;
}

UInt64 Manage::ThreadContextX86_64::GetRBP() const
{
	return ((CONTEXT_TYPE*)this->context)->Rbp;
}

UInt64 Manage::ThreadContextX86_64::GetRIP() const
{
	return ((CONTEXT_TYPE*)this->context)->Rip;
}

UInt64 Manage::ThreadContextX86_64::GetR8() const
{
	return ((CONTEXT_TYPE*)this->context)->R8;
}

UInt64 Manage::ThreadContextX86_64::GetR9() const
{
	return ((CONTEXT_TYPE*)this->context)->R9;
}

UInt64 Manage::ThreadContextX86_64::GetR10() const
{
	return ((CONTEXT_TYPE*)this->context)->R10;
}

UInt64 Manage::ThreadContextX86_64::GetR11() const
{
	return ((CONTEXT_TYPE*)this->context)->R11;
}

UInt64 Manage::ThreadContextX86_64::GetR12() const
{
	return ((CONTEXT_TYPE*)this->context)->R12;
}

UInt64 Manage::ThreadContextX86_64::GetR13() const
{
	return ((CONTEXT_TYPE*)this->context)->R13;
}

UInt64 Manage::ThreadContextX86_64::GetR14() const
{
	return ((CONTEXT_TYPE*)this->context)->R14;
}

UInt64 Manage::ThreadContextX86_64::GetR15() const
{
	return ((CONTEXT_TYPE*)this->context)->R15;
}

UInt32 Manage::ThreadContextX86_64::GetEFLAGS() const
{
	return ((CONTEXT_TYPE*)this->context)->EFlags;
}

UInt16 Manage::ThreadContextX86_64::GetCS() const
{
	return ((CONTEXT_TYPE*)this->context)->SegCs;
}

UInt16 Manage::ThreadContextX86_64::GetSS() const
{
	return ((CONTEXT_TYPE*)this->context)->SegSs;
}

UInt16 Manage::ThreadContextX86_64::GetDS() const
{
	return ((CONTEXT_TYPE*)this->context)->SegDs;
}

UInt16 Manage::ThreadContextX86_64::GetES() const
{
	return ((CONTEXT_TYPE*)this->context)->SegEs;
}

UInt16 Manage::ThreadContextX86_64::GetFS() const
{
	return ((CONTEXT_TYPE*)this->context)->SegFs;
}

UInt16 Manage::ThreadContextX86_64::GetGS() const
{
	return ((CONTEXT_TYPE*)this->context)->SegGs;
}

UInt64 Manage::ThreadContextX86_64::GetDR0() const
{
	return ((CONTEXT_TYPE*)this->context)->Dr0;
}

UInt64 Manage::ThreadContextX86_64::GetDR1() const
{
	return ((CONTEXT_TYPE*)this->context)->Dr1;
}

UInt64 Manage::ThreadContextX86_64::GetDR2() const
{
	return ((CONTEXT_TYPE*)this->context)->Dr2;
}

UInt64 Manage::ThreadContextX86_64::GetDR3() const
{
	return ((CONTEXT_TYPE*)this->context)->Dr3;
}

UInt64 Manage::ThreadContextX86_64::GetDR6() const
{
	return ((CONTEXT_TYPE*)this->context)->Dr6;
}

UInt64 Manage::ThreadContextX86_64::GetDR7() const
{
	return ((CONTEXT_TYPE*)this->context)->Dr7;
}
