#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Manage/DasmARM.h"
#include "Manage/ThreadContextARM64.h"
#include <sys/ucontext.h>

Manage::ThreadContextARM64::ThreadContextARM64(UOSInt procId, UOSInt threadId, void *context)
{
	this->procId = procId;
	this->threadId = threadId;
	this->context = MemAlloc(ucontext_t, 1);
	MemCopyNO(this->context, context, sizeof(ucontext_t));
}

Manage::ThreadContextARM64::~ThreadContextARM64()
{
	MemFree(this->context);
}

UOSInt Manage::ThreadContextARM64::GetRegisterCnt() const
{
	UOSInt cnt = 34;
	return cnt;
}

UTF8Char *Manage::ThreadContextARM64::GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount) const
{
#if defined(__APPLE__)
	switch (index)
	{
	case 0:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[0];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X0"));
	case 1:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[1];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X1"));
	case 2:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[2];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X2"));
	case 3:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[3];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X3"));
	case 4:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[4];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X4"));
	case 5:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[5];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X5"));
	case 6:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[6];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X6"));
	case 7:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[7];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X7"));
	case 8:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[8];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X8"));
	case 9:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[9];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X9"));
	case 10:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[10];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X10"));
	case 11:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[11];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X11"));
	case 12:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[12];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X12"));
	case 13:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[13];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X13"));
	case 14:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[14];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X14"));
	case 15:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[15];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X15"));
	case 16:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[16];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X16"));
	case 17:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[17];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X17"));
	case 18:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[18];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X18"));
	case 19:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[19];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X19"));
	case 20:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[20];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X20"));
	case 21:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[21];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X21"));
	case 22:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[22];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X22"));
	case 23:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[23];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X23"));
	case 24:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[24];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X24"));
	case 25:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[25];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X25"));
	case 26:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[26];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X26"));
	case 27:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[27];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X27"));
	case 28:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[28];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X28"));
	case 29:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__fp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X29"));
	case 30:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__lr;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X30"));
	case 31:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__sp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("SP"));
	case 32:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__pc;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("PC"));
	case 33:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext->__ss.__cpsr;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("PState"));
	default:
		return 0;
	}
#else
	switch (index)
	{
	case 0:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[0];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X0"));
	case 1:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[1];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X1"));
	case 2:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[2];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X2"));
	case 3:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[3];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X3"));
	case 4:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[4];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X4"));
	case 5:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[5];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X5"));
	case 6:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[6];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X6"));
	case 7:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[7];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X7"));
	case 8:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[8];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X8"));
	case 9:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[9];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X9"));
	case 10:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[10];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X10"));
	case 11:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[11];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X11"));
	case 12:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[12];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X12"));
	case 13:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[13];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X13"));
	case 14:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[14];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X14"));
	case 15:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[15];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X15"));
	case 16:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[16];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X16"));
	case 17:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[17];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X17"));
	case 18:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[18];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X18"));
	case 19:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[19];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X19"));
	case 20:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[20];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X20"));
	case 21:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[21];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X21"));
	case 22:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[22];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X22"));
	case 23:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[23];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X23"));
	case 24:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[24];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X24"));
	case 25:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[25];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X25"));
	case 26:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[26];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X26"));
	case 27:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[27];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X27"));
	case 28:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[28];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X28"));
	case 29:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[29];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X29"));
	case 30:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.regs[30];
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("X30"));
	case 31:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.sp;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("SP"));
	case 32:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.pc;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("PC"));
	case 33:
		*(UInt64*)regVal = ((ucontext_t*)this->context)->uc_mcontext.pstate;
		*regBitCount = 64;
		return Text::StrConcatC(buff, UTF8STRC("PState"));
	default:
		return 0;
	}
#endif
}

void Manage::ThreadContextARM64::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
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

Manage::ThreadContext::ContextType Manage::ThreadContextARM64::GetType() const
{
	return Manage::ThreadContext::ContextType::ARM64;
}

UOSInt Manage::ThreadContextARM64::GetThreadId() const
{
	return this->threadId;
}

UOSInt Manage::ThreadContextARM64::GetProcessId() const
{
	return this->procId;
}

#if defined(__APPLE__)
UOSInt Manage::ThreadContextARM64::GetInstAddr() const
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext->__ss.__pc;
}

UOSInt Manage::ThreadContextARM64::GetStackAddr() const
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext->__ss.__sp;
}

UOSInt Manage::ThreadContextARM64::GetFrameAddr() const
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext->__ss.__fp;
}

void Manage::ThreadContextARM64::SetInstAddr(UOSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext->__ss.__pc = instAddr;
}

void Manage::ThreadContextARM64::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext->__ss.__pc = stackAddr;
}

void Manage::ThreadContextARM64::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext->__ss.__fp = frameAddr;
}
#else
UOSInt Manage::ThreadContextARM64::GetInstAddr() const
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext.pc;
}

UOSInt Manage::ThreadContextARM64::GetStackAddr() const
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext.sp;
}

UOSInt Manage::ThreadContextARM64::GetFrameAddr() const
{
	return (UOSInt)((ucontext_t*)this->context)->uc_mcontext.pstate;
}

void Manage::ThreadContextARM64::SetInstAddr(UOSInt instAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.pc = instAddr;
}

void Manage::ThreadContextARM64::SetStackAddr(UOSInt stackAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.sp = stackAddr;
}

void Manage::ThreadContextARM64::SetFrameAddr(UOSInt frameAddr)
{
	((ucontext_t*)this->context)->uc_mcontext.pstate = frameAddr;
}
#endif

Manage::ThreadContext *Manage::ThreadContextARM64::Clone() const
{
	Manage::ThreadContextARM64 *cont;
	NEW_CLASS(cont, Manage::ThreadContextARM64(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextARM64::GetRegs(Manage::Dasm::Dasm_Regs *regs) const
{
	return false;
}

Manage::Dasm *Manage::ThreadContextARM64::CreateDasm() const
{
	return 0;
}

void *Manage::ThreadContextARM64::GetContext() const
{
	return this->context;
}

