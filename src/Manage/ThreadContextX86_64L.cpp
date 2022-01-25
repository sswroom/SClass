#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmX86_64.h"
#include "Manage/ThreadContextX86_64.h"

#if defined(__FreeBSD__)
#include <sys/ucontext.h>

Manage::ThreadContextX86_64::ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextX86_64::~ThreadContextX86_64()
{
	MemFree(this->context);
}

OSInt Manage::ThreadContextX86_64::GetRegisterCnt()
{
	OSInt cnt = 21;
	return cnt;
}

UTF8Char *Manage::ThreadContextX86_64::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rax;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RAX"));
	case 1:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rdx;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RDX"));
	case 2:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rcx;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RCX"));
	case 3:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rbx;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RBX"));
	case 4:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rsi;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RSI"));
	case 5:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rdi;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RDI"));
	case 6:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rsp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RSP"));
	case 7:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rbp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RBP"));
	case 8:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r8;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R8"));
	case 9:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r9;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R9"));
	case 10:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r10;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R10"));
	case 11:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r11;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R11"));
	case 12:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r12;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R12"));
	case 13:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r13;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R13"));
	case 14:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r14;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R14"));
	case 15:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_r15;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R15"));
	case 16:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_rip;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RIP"));
	case 17:
		*(UInt32*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_flags;
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 18:
		*(UInt16*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_cs;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 19:
		*(UInt16*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_fs;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 20:
		*(UInt16*)regVal = ((ucontext_t*)this->context)->uc_mcontext.mc_gs;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_64::ToString(Text::StringBuilderUTF8 *sb)
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

OSInt Manage::ThreadContextX86_64::GetInstAddr()
{
	return this->GetRIP();
}

OSInt Manage::ThreadContextX86_64::GetStackAddr()
{
	return this->GetRSP();
}

OSInt Manage::ThreadContextX86_64::GetFrameAddr()
{
	return this->GetRBP();
}

void Manage::ThreadContextX86_64::SetInstAddr(OSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.mc_rip = instAddr;
}

void Manage::ThreadContextX86_64::SetStackAddr(OSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.mc_rsp = stackAddr;
}

void Manage::ThreadContextX86_64::SetFrameAddr(OSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.mc_rbp = frameAddr;
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
	r->rax = ((ucontext_t*)this->context)->uc_mcontext.mc_rax;
	r->rdx = ((ucontext_t*)this->context)->uc_mcontext.mc_rdx;
	r->rcx = ((ucontext_t*)this->context)->uc_mcontext.mc_rcx;
	r->rbx = ((ucontext_t*)this->context)->uc_mcontext.mc_rbx;
	r->rsi = ((ucontext_t*)this->context)->uc_mcontext.mc_rsi;
	r->rdi = ((ucontext_t*)this->context)->uc_mcontext.mc_rdi;
	r->rsp = ((ucontext_t*)this->context)->uc_mcontext.mc_rsp;
	r->rbp = ((ucontext_t*)this->context)->uc_mcontext.mc_rbp;
	r->rip = ((ucontext_t*)this->context)->uc_mcontext.mc_rip;
	r->r8 = ((ucontext_t*)this->context)->uc_mcontext.mc_r8;
	r->r9 = ((ucontext_t*)this->context)->uc_mcontext.mc_r9;
	r->r10 = ((ucontext_t*)this->context)->uc_mcontext.mc_r10;
	r->r11 = ((ucontext_t*)this->context)->uc_mcontext.mc_r11;
	r->r12 = ((ucontext_t*)this->context)->uc_mcontext.mc_r12;
	r->r13 = ((ucontext_t*)this->context)->uc_mcontext.mc_r13;
	r->r14 = ((ucontext_t*)this->context)->uc_mcontext.mc_r14;
	r->r15 = ((ucontext_t*)this->context)->uc_mcontext.mc_r15;
	r->EFLAGS = ((ucontext_t*)this->context)->uc_mcontext.mc_flags;
	r->cs = ((ucontext_t*)this->context)->uc_mcontext.mc_cs;
	r->ss = ((ucontext_t*)this->context)->uc_mcontext.mc_ss;
	r->ds = ((ucontext_t*)this->context)->uc_mcontext.mc_ds;
	r->es = ((ucontext_t*)this->context)->uc_mcontext.mc_es;
	r->fs = ((ucontext_t*)this->context)->uc_mcontext.mc_fs;
	r->gs = ((ucontext_t*)this->context)->uc_mcontext.mc_gs;
	r->dr0 = 0;
	r->dr1 = 0;
	r->dr2 = 0;
	r->dr3 = 0;
	r->dr6 = 0;
	r->dr7 = 0;
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
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rax;
}

UInt64 Manage::ThreadContextX86_64::GetRDX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rdx;
}

UInt64 Manage::ThreadContextX86_64::GetRCX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rcx;
}

UInt64 Manage::ThreadContextX86_64::GetRBX()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rbx;
}

UInt64 Manage::ThreadContextX86_64::GetRSI()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rsi;
}

UInt64 Manage::ThreadContextX86_64::GetRDI()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rdi;
}

UInt64 Manage::ThreadContextX86_64::GetRSP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rsp;
}

UInt64 Manage::ThreadContextX86_64::GetRBP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rbp;
}

UInt64 Manage::ThreadContextX86_64::GetRIP()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_rip;
}

UInt64 Manage::ThreadContextX86_64::GetR8()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r8;
}

UInt64 Manage::ThreadContextX86_64::GetR9()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r9;
}

UInt64 Manage::ThreadContextX86_64::GetR10()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r10;
}

UInt64 Manage::ThreadContextX86_64::GetR11()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r11;
}

UInt64 Manage::ThreadContextX86_64::GetR12()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r12;
}

UInt64 Manage::ThreadContextX86_64::GetR13()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r13;
}

UInt64 Manage::ThreadContextX86_64::GetR14()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r14;
}

UInt64 Manage::ThreadContextX86_64::GetR15()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_r15;
}

UInt32 Manage::ThreadContextX86_64::GetEFLAGS()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_flags;
}

UInt16 Manage::ThreadContextX86_64::GetCS()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_cs;
}

UInt16 Manage::ThreadContextX86_64::GetSS()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_ss;
}

UInt16 Manage::ThreadContextX86_64::GetDS()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_ds;
}

UInt16 Manage::ThreadContextX86_64::GetES()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_es;
}

UInt16 Manage::ThreadContextX86_64::GetFS()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_fs;
}

UInt16 Manage::ThreadContextX86_64::GetGS()
{
	return ((ucontext_t*)this->context)->uc_mcontext.mc_gs;
}

UInt64 Manage::ThreadContextX86_64::GetDR0()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR1()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR2()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR3()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR6()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR7()
{
	return 0;
}
#elif defined(__APPLE__)
#include <sys/ucontext.h>

Manage::ThreadContextX86_64::ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextX86_64::~ThreadContextX86_64()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextX86_64::GetRegisterCnt()
{
	UOSInt cnt = 21;
	return cnt;
}

UTF8Char *Manage::ThreadContextX86_64::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rax;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RAX"));
	case 1:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rdx;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RDX"));
	case 2:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rcx;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RCX"));
	case 3:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rbx;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RBX"));
	case 4:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rsi;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RSI"));
	case 5:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rdi;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RDI"));
	case 6:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rsp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RSP"));
	case 7:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rbp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RBP"));
	case 8:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r8;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R8"));
	case 9:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r9;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R9"));
	case 10:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r10;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R10"));
	case 11:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r11;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R11"));
	case 12:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r12;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R12"));
	case 13:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r13;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R13"));
	case 14:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r14;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R14"));
	case 15:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r15;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R15"));
	case 16:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rip;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RIP"));
	case 17:
		*(UInt32*)regVal = (UInt32)(((ucontext_t*)this->context)->uc_mcontext->__ss.__rflags & 0xffffffffLL);
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 18:
		*(UInt16*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__cs;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 19:
		*(UInt16*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__fs;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 20:
		*(UInt16*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__gs;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_64::ToString(Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
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
	((ucontext_t*)this->context)->uc_mcontext->__ss.__rip = instAddr;
}

void Manage::ThreadContextX86_64::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext->__ss.__rsp = stackAddr;
}

void Manage::ThreadContextX86_64::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext->__ss.__rbp = frameAddr;
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
	r->rax = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rax;
	r->rdx = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rdx;
	r->rcx = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rcx;
	r->rbx = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rbx;
	r->rsi = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rsi;
	r->rdi = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rdi;
	r->rsp = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rsp;
	r->rbp = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rbp;
	r->rip = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rip;
	r->r8 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r8;
	r->r9 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r9;
	r->r10 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r10;
	r->r11 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r11;
	r->r12 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r12;
	r->r13 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r13;
	r->r14 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r14;
	r->r15 = ((ucontext_t*)this->context)->uc_mcontext->__ss.__r15;
	r->EFLAGS = ((ucontext_t*)this->context)->uc_mcontext->__ss.__rflags;
	r->cs = ((ucontext_t*)this->context)->uc_mcontext->__ss.__cs;
	r->ss = 0;
	r->ds = 0;
	r->es = 0;
	r->fs = ((ucontext_t*)this->context)->uc_mcontext->__ss.__fs;
	r->gs = ((ucontext_t*)this->context)->uc_mcontext->__ss.__gs;
	r->dr0 = 0;
	r->dr1 = 0;
	r->dr2 = 0;
	r->dr3 = 0;
	r->dr6 = 0;
	r->dr7 = 0;
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
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rax;
}

UInt64 Manage::ThreadContextX86_64::GetRDX()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rdx;
}

UInt64 Manage::ThreadContextX86_64::GetRCX()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rcx;
}

UInt64 Manage::ThreadContextX86_64::GetRBX()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rbx;
}

UInt64 Manage::ThreadContextX86_64::GetRSI()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rsi;
}

UInt64 Manage::ThreadContextX86_64::GetRDI()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rdi;
}

UInt64 Manage::ThreadContextX86_64::GetRSP()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rsp;
}

UInt64 Manage::ThreadContextX86_64::GetRBP()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rbp;
}

UInt64 Manage::ThreadContextX86_64::GetRIP()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__rip;
}

UInt64 Manage::ThreadContextX86_64::GetR8()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r8;
}

UInt64 Manage::ThreadContextX86_64::GetR9()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r9;
}

UInt64 Manage::ThreadContextX86_64::GetR10()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r10;
}

UInt64 Manage::ThreadContextX86_64::GetR11()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r11;
}

UInt64 Manage::ThreadContextX86_64::GetR12()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r12;
}

UInt64 Manage::ThreadContextX86_64::GetR13()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r13;
}

UInt64 Manage::ThreadContextX86_64::GetR14()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r14;
}

UInt64 Manage::ThreadContextX86_64::GetR15()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__r15;
}

UInt32 Manage::ThreadContextX86_64::GetEFLAGS()
{
	return (UInt32)(((ucontext_t*)this->context)->uc_mcontext->__ss.__rflags & 0xffffffffLL);
}

UInt16 Manage::ThreadContextX86_64::GetCS()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__cs;
}

UInt16 Manage::ThreadContextX86_64::GetSS()
{
	return 0;
}

UInt16 Manage::ThreadContextX86_64::GetDS()
{
	return 0;
}

UInt16 Manage::ThreadContextX86_64::GetES()
{
	return 0;
}

UInt16 Manage::ThreadContextX86_64::GetFS()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__fs;
}

UInt16 Manage::ThreadContextX86_64::GetGS()
{
	return ((ucontext_t*)this->context)->uc_mcontext->__ss.__gs;
}

UInt64 Manage::ThreadContextX86_64::GetDR0()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR1()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR2()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR3()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR6()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR7()
{
	return 0;
}
#else
#include <sys/ucontext.h>
#if defined(__sun__)
#include <sys/regset.h>
#endif

Manage::ThreadContextX86_64::ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextX86_64::~ThreadContextX86_64()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextX86_64::GetRegisterCnt()
{
	UOSInt cnt = 37;
	return cnt;
}

UTF8Char *Manage::ThreadContextX86_64::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount)
{
	switch (index)
	{
	case 0:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RAX];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RAX"));
	case 1:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RDX];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RDX"));
	case 2:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RCX];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RCX"));
	case 3:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBX];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RBX"));
	case 4:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSI];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RSI"));
	case 5:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RDI];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RDI"));
	case 6:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSP];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RSP"));
	case 7:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBP];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RBP"));
	case 8:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R8];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R8"));
	case 9:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R9];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R9"));
	case 10:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R10];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R10"));
	case 11:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R11];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R11"));
	case 12:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R12];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R12"));
	case 13:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R13];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R13"));
	case 14:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R14];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R14"));
	case 15:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R15];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("R15"));
	case 16:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RIP];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("RIP"));
#if defined(__sun__)
	case 17:
		*(Int64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RFL];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 18:
		*(Int16*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS] & 0xffff;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 19:
		*(Int16*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS] & 0xffff;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 20:
		*(Int16*)regVal = ((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS] & 0xffff;
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	case 21:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[0], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM0"));
	case 22:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[1], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM1"));
	case 23:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[2], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM2"));
	case 24:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[3], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM3"));
	case 25:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[4], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM4"));
	case 26:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[5], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM5"));
	case 27:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[6], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM6"));
	case 28:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[7], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM7"));
	case 29:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[8], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM8"));
	case 30:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[9], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM9"));
	case 31:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[10], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM10"));
	case 32:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[11], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM11"));
	case 33:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[12], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM12"));
	case 34:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[13], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM13"));
	case 35:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[14], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM14"));
	case 36:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[15], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM15"));
#else
	case 17:
		*(Int32*)regVal = (Int32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
		*regBitCount = 32;
		return Text::StrConcatC(buff, UTF8STRC("EFLAGS"));
	case 18:
		*(Int16*)regVal = (Int16)(((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] & 0xffff);
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("CS"));
	case 19:
		*(Int16*)regVal = (Int16)((((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] >> 32) & 0xffff);
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("FS"));
	case 20:
		*(Int16*)regVal = (Int16)((((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] >> 16) & 0xffff);
		*regBitCount = 16;
		return Text::StrConcatC(buff, UTF8STRC("GS"));
	case 21:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[0], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM0"));
	case 22:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[1], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM1"));
	case 23:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[2], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM2"));
	case 24:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[3], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM3"));
	case 25:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[4], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM4"));
	case 26:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[5], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM5"));
	case 27:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[6], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM6"));
	case 28:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[7], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM7"));
	case 29:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[8], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM8"));
	case 30:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[9], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM9"));
	case 31:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[10], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM10"));
	case 32:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[11], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM11"));
	case 33:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[12], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM12"));
	case 34:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[13], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM13"));
	case 35:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[14], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM14"));
	case 36:
		MemCopyNO(regVal, &((ucontext_t*)this->context)->__fpregs_mem._xmm[15], 16);
		*regBitCount = 128;
		return Text::StrConcatC(buff, UTF8STRC("XMM15"));
#endif
	default:
		return 0;
	}
}

void Manage::ThreadContextX86_64::ToString(Text::StringBuilderUTF8 *sb)
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
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RIP] = (OSInt)instAddr;
}

void Manage::ThreadContextX86_64::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSP] = (OSInt)stackAddr;
}

void Manage::ThreadContextX86_64::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBP] = (OSInt)frameAddr;
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
	r->rax = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RAX];
	r->rdx = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RDX];
	r->rcx = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RCX];
	r->rbx = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBX];
	r->rsi = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSI];
	r->rdi = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RDI];
	r->rsp = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSP];
	r->rbp = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBP];
	r->rip = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RIP];
	r->r8 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R8];
	r->r9 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R9];
	r->r10 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R10];
	r->r11 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R11];
	r->r12 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R12];
	r->r13 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R13];
	r->r14 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R14];
	r->r15 = (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R15];
	r->ss = 0;
	r->ds = 0;
	r->es = 0;
#if defined(__sun__)
	r->EFLAGS = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RFL];
	r->cs = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS] & 0xffff;
	r->fs = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS] & 0xffff;
	r->gs = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS] & 0xffff;
#else
	r->EFLAGS = (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
	r->cs = (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] & 0xffff;
	r->fs = (UInt16)(((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] >> 32) & 0xffff;
	r->gs = (UInt16)(((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] >> 16) & 0xffff;
#endif
	r->dr0 = 0;
	r->dr1 = 0;
	r->dr2 = 0;
	r->dr3 = 0;
	r->dr6 = 0;
	r->dr7 = 0;
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
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RAX];
}

UInt64 Manage::ThreadContextX86_64::GetRDX()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RDX];
}

UInt64 Manage::ThreadContextX86_64::GetRCX()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RCX];
}

UInt64 Manage::ThreadContextX86_64::GetRBX()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBX];
}

UInt64 Manage::ThreadContextX86_64::GetRSI()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSI];
}

UInt64 Manage::ThreadContextX86_64::GetRDI()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RDI];
}

UInt64 Manage::ThreadContextX86_64::GetRSP()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RSP];
}

UInt64 Manage::ThreadContextX86_64::GetRBP()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RBP];
}

UInt64 Manage::ThreadContextX86_64::GetRIP()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RIP];
}

UInt64 Manage::ThreadContextX86_64::GetR8()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R8];
}

UInt64 Manage::ThreadContextX86_64::GetR9()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R9];
}

UInt64 Manage::ThreadContextX86_64::GetR10()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R10];
}

UInt64 Manage::ThreadContextX86_64::GetR11()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R11];
}

UInt64 Manage::ThreadContextX86_64::GetR12()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R12];
}

UInt64 Manage::ThreadContextX86_64::GetR13()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R13];
}

UInt64 Manage::ThreadContextX86_64::GetR14()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R14];
}

UInt64 Manage::ThreadContextX86_64::GetR15()
{
	return (UInt64)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_R15];
}

UInt32 Manage::ThreadContextX86_64::GetEFLAGS()
{
#if defined(__sun__)
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_RFL];
#else
	return (UInt32)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_EFL];
#endif
}

UInt16 Manage::ThreadContextX86_64::GetCS()
{
#if defined(__sun__)
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CS] & 0xffff;
#else
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] & 0xffff;
#endif
}

UInt16 Manage::ThreadContextX86_64::GetSS()
{
	return 0;
}

UInt16 Manage::ThreadContextX86_64::GetDS()
{
	return 0;
}

UInt16 Manage::ThreadContextX86_64::GetES()
{
	return 0;
}

UInt16 Manage::ThreadContextX86_64::GetFS()
{
#if defined(__sun__)
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_FS] & 0xffff;
#else
	return (UInt16)(((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] >> 32) & 0xffff;
#endif
}

UInt16 Manage::ThreadContextX86_64::GetGS()
{
#if defined(__sun__)
	return (UInt16)((ucontext_t*)this->context)->uc_mcontext.gregs[REG_GS] & 0xffff;
#else
	return (UInt16)(((ucontext_t*)this->context)->uc_mcontext.gregs[REG_CSGSFS] >> 16) & 0xffff;
#endif
}

UInt64 Manage::ThreadContextX86_64::GetDR0()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR1()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR2()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR3()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR6()
{
	return 0;
}

UInt64 Manage::ThreadContextX86_64::GetDR7()
{
	return 0;
}
#endif
