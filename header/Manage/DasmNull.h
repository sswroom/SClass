#ifndef _SM_MANAGE_DASMNULL
#define _SM_MANAGE_DASMNULL
#include "Data/ArrayListInt32.h"
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmNull : public Dasm32
	{
	public:
		typedef enum
		{
			ET_NOT_END,
			ET_FUNC_RET,
			ET_JMP,
			ET_INV_OP,
			ET_EXIT
		} EndType;

	public:
		DasmNull();
		virtual ~DasmNull();

		virtual Text::CStringNN GetHeader(Bool fullRegs);
		virtual Bool Disasm32(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListInt32 *callAddrs, Data::ArrayListInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::MemoryReader *memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs();
		virtual void FreeRegs(NN<Dasm_Regs> regs);
	};
};

#endif
