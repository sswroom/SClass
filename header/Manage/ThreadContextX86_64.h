#ifndef _SM_MANAGE_THREADCONTEXTX86_64
#define _SM_MANAGE_THREADCONTEXTX86_64
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextX86_64 : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
		OSInt mmOfst;
		OSInt xmmOfst;
	public:
		ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextX86_64();

		virtual UOSInt GetRegisterCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		virtual ContextType GetType() const;
		virtual UOSInt GetThreadId() const;
		virtual UOSInt GetProcessId() const;
		virtual UOSInt GetInstAddr() const;
		virtual UOSInt GetStackAddr() const;
		virtual UOSInt GetFrameAddr() const;
		virtual void SetInstAddr(UOSInt instAddr);
		virtual void SetStackAddr(UOSInt stackAddr);
		virtual void SetFrameAddr(UOSInt frameAddr);
		virtual NN<ThreadContext> Clone() const;
		virtual Bool GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const;
		virtual Optional<Manage::Dasm> CreateDasm() const;
		void *GetContext() const;

		UInt64 GetRAX() const;
		UInt64 GetRDX() const;
		UInt64 GetRCX() const;
		UInt64 GetRBX() const;
		UInt64 GetRSI() const;
		UInt64 GetRDI() const;
		UInt64 GetRSP() const;
		UInt64 GetRBP() const;
		UInt64 GetRIP() const;
		UInt64 GetR8() const;
		UInt64 GetR9() const;
		UInt64 GetR10() const;
		UInt64 GetR11() const;
		UInt64 GetR12() const;
		UInt64 GetR13() const;
		UInt64 GetR14() const;
		UInt64 GetR15() const;
		UInt32 GetEFLAGS() const;
		UInt16 GetCS() const;
		UInt16 GetSS() const;
		UInt16 GetDS() const;
		UInt16 GetES() const;
		UInt16 GetFS() const;
		UInt16 GetGS() const;
		UInt64 GetDR0() const;
		UInt64 GetDR1() const;
		UInt64 GetDR2() const;
		UInt64 GetDR3() const;
		UInt64 GetDR6() const;
		UInt64 GetDR7() const;
	};
}
#endif
