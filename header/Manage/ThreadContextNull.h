#ifndef _SM_MANAGE_THREADCONTEXTNULL
#define _SM_MANAGE_THREADCONTEXTNULL
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextNull : public ThreadContext
	{
	private:
		void *context;
		UIntOS procId;
		UIntOS threadId;
	public:
		ThreadContextNull(UIntOS procId, UIntOS threadId, void *context);
		virtual ~ThreadContextNull();

		virtual UIntOS GetRegisterCnt() const;
		virtual UTF8Char *GetRegister(UIntOS index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount) const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		virtual ContextType GetType() const;
		virtual UIntOS GetThreadId() const;
		virtual UIntOS GetProcessId() const;
		virtual UIntOS GetInstAddr() const;
		virtual UIntOS GetStackAddr() const;
		virtual UIntOS GetFrameAddr() const;
		virtual void SetInstAddr(UIntOS instAddr);
		virtual void SetStackAddr(UIntOS stackAddr);
		virtual void SetFrameAddr(UIntOS frameAddr);
		virtual NN<ThreadContext> Clone() const;
		virtual Bool GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const;
		virtual Optional<Manage::Dasm> CreateDasm() const;
		void *GetContext() const;
	};
}
#endif
