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

UnsafeArrayOpt<UTF8Char> Manage::ThreadContextARM64::GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const
{
#if defined(__APPLE__)
	switch (index)
	{
	case 0:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[0];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X0"));
	case 1:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[1];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X1"));
	case 2:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[2];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X2"));
	case 3:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[3];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X3"));
	case 4:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[4];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X4"));
	case 5:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[5];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X5"));
	case 6:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[6];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X6"));
	case 7:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[7];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X7"));
	case 8:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[8];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X8"));
	case 9:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[9];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X9"));
	case 10:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[10];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X10"));
	case 11:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[11];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X11"));
	case 12:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[12];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X12"));
	case 13:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[13];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X13"));
	case 14:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[14];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X14"));
	case 15:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[15];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X15"));
	case 16:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[16];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X16"));
	case 17:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[17];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X17"));
	case 18:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[18];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X18"));
	case 19:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[19];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X19"));
	case 20:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[20];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X20"));
	case 21:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[21];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X21"));
	case 22:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[22];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X22"));
	case 23:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[23];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X23"));
	case 24:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[24];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X24"));
	case 25:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[25];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X25"));
	case 26:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[26];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X26"));
	case 27:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[27];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X27"));
	case 28:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__x[28];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X28"));
	case 29:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__fp;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X29"));
	case 30:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__lr;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X30"));
	case 31:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__sp;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("SP"));
	case 32:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__pc;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("PC"));
	case 33:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext->__ss.__cpsr;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("PState"));
	default:
		return 0;
	}
#else
	switch (index)
	{
	case 0:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[0];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X0"));
	case 1:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[1];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X1"));
	case 2:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[2];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X2"));
	case 3:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[3];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X3"));
	case 4:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[4];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X4"));
	case 5:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[5];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X5"));
	case 6:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[6];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X6"));
	case 7:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[7];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X7"));
	case 8:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[8];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X8"));
	case 9:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[9];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X9"));
	case 10:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[10];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X10"));
	case 11:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[11];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X11"));
	case 12:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[12];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X12"));
	case 13:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[13];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X13"));
	case 14:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[14];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X14"));
	case 15:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[15];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X15"));
	case 16:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[16];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X16"));
	case 17:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[17];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X17"));
	case 18:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[18];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X18"));
	case 19:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[19];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X19"));
	case 20:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[20];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X20"));
	case 21:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[21];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X21"));
	case 22:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[22];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X22"));
	case 23:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[23];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X23"));
	case 24:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[24];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X24"));
	case 25:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[25];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X25"));
	case 26:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[26];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X26"));
	case 27:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[27];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X27"));
	case 28:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[28];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X28"));
	case 29:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[29];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X29"));
	case 30:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.regs[30];
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("X30"));
	case 31:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.sp;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("SP"));
	case 32:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.pc;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("PC"));
	case 33:
		UnsafeArray<UInt64>::ConvertFrom(regVal)[0] = ((ucontext_t*)this->context)->uc_mcontext.pstate;
		regBitCount.Set(64);
		return Text::StrConcatC(buff, UTF8STRC("PState"));
	default:
		return 0;
	}
#endif
}

void Manage::ThreadContextARM64::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 regBuff[16];
	UInt32 bitCnt;
	UOSInt i = 0;
	UOSInt j = this->GetRegisterCnt();
	UOSInt k;

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

NN<Manage::ThreadContext> Manage::ThreadContextARM64::Clone() const
{
	NN<Manage::ThreadContextARM64> cont;
	NEW_CLASSNN(cont, Manage::ThreadContextARM64(this->procId, this->threadId, this->context));
	return cont;
}

Bool Manage::ThreadContextARM64::GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const
{
	return false;
}

Optional<Manage::Dasm> Manage::ThreadContextARM64::CreateDasm() const
{
	return 0;
}

void *Manage::ThreadContextARM64::GetContext() const
{
	return this->context;
}

