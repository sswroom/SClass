#ifndef _SM_MANAGE_THREADCONTEXTNULL
#define _SM_MANAGE_THREADCONTEXTNULL
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextNull : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
	public:
		ThreadContextNull(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextNull();

		virtual UOSInt GetRegisterCnt() const;
		virtual UTF8Char *GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount) const;
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
