#ifndef _SM_MANAGE_THREADCONTEXTX86_32
#define _SM_MANAGE_THREADCONTEXTX86_32
#include "Manage/ThreadContext.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class ThreadContextX86_32 : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
		OSInt mmOfst;
		OSInt xmmOfst;
	public:
		ThreadContextX86_32(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextX86_32();

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

		UInt32 GetEAX() const;
		UInt32 GetEDX() const;
		UInt32 GetECX() const;
		UInt32 GetEBX() const;
		UInt32 GetESI() const;
		UInt32 GetEDI() const;
		UInt32 GetESP() const;
		UInt32 GetEBP() const;
		UInt32 GetEIP() const;
		UInt32 GetEFLAGS() const;
		UInt16 GetCS() const;
		UInt16 GetSS() const;
		UInt16 GetDS() const;
		UInt16 GetES() const;
		UInt16 GetFS() const;
		UInt16 GetGS() const;
		UInt32 GetDR0() const;
		UInt32 GetDR1() const;
		UInt32 GetDR2() const;
		UInt32 GetDR3() const;
		UInt32 GetDR6() const;
		UInt32 GetDR7() const;
		void GetST0(UInt8 *val) const;
		void GetST1(UInt8 *val) const;
		void GetST2(UInt8 *val) const;
		void GetST3(UInt8 *val) const;
		void GetST4(UInt8 *val) const;
		void GetST5(UInt8 *val) const;
		void GetST6(UInt8 *val) const;
		void GetST7(UInt8 *val) const;
		UInt16 GetCTRL() const;
		UInt16 GetSTAT() const;
		UInt16 GetTAG() const;
	};
}
#endif
