#ifndef _SM_MANAGE_THREADCONTEXTARM64
#define _SM_MANAGE_THREADCONTEXTARM64
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextARM64 : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
	public:
		ThreadContextARM64(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextARM64();

		virtual UOSInt GetRegisterCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UInt8 *regVal, UInt32 *regBitCount) const;
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
	};
}
#endif
