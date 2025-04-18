#ifndef _SM_MANAGE_DASMARM64
#define _SM_MANAGE_DASMARM64
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmARM64 : public Dasm64
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

		struct DasmARM64_Regs : public Manage::Dasm::Dasm_Regs
		{
			union 
			{
				struct
				{
					UInt64 X0;
					UInt64 X1;
					UInt64 X2;
					UInt64 X3;
					UInt64 X4;
					UInt64 X5;
					UInt64 X6;
					UInt64 X7;
					UInt64 X8;
					UInt64 X9;
					UInt64 X10;
					UInt64 X11;
					UInt64 X12;
					UInt64 X13;
					UInt64 X14;
					UInt64 X15;
					UInt64 X16;
					UInt64 X17;
					UInt64 X18;
					UInt64 X19;
					UInt64 X20;
					UInt64 X21;
					UInt64 X22;
					UInt64 X23;
					UInt64 X24;
					UInt64 X25;
					UInt64 X26;
					UInt64 X27;
					UInt64 X28;
					UInt64 X29;
					UInt64 LR; //X30
					UInt64 SP; //X31
					UInt64 PC;
				};
				UInt64 indexes[33];
			};
		};

		typedef struct
		{
			DasmARM64_Regs regs;
			UInt8 *code;
			UInt16 codeSegm;
			Data::ArrayListUInt64 *callAddrs;
			Data::ArrayListUInt64 *jmpAddrs;
			UnsafeArray<UTF8Char> sbuff;
			UInt64 retAddr;
			Int32 thisStatus;
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			EndType endType;
			UInt8 *codeBuff;
			
			Optional<Manage::AddressResolver> addrResol;
			NN<Manage::MemoryReader> memReader;

			void **codeHdlrs;
			UTF8Char *outSPtr;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		} DasmARM64_Sess;

		typedef Bool (CALLBACKFUNC DasmARM64_Code)(NN<DasmARM64_Sess> sess);
	private:
		DasmARM64_Code *codes;
	public:
		DasmARM64();
		virtual ~DasmARM64();

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm64(NN<IO::Writer> writer, Optional<Manage::AddressResolver> addrResol, UInt64 *currInst, UInt64 *currStack, UInt64 *currFrame, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, NN<Manage::MemoryReader> memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		NN<DasmARM64_Sess> CreateSess(NN<DasmARM64_Regs> regs, UInt8 *code, UInt16 codeSegm);
		void DeleteSess(NN<DasmARM64_Sess> sess);

		Bool DasmNext(NN<DasmARM64_Sess> sess, UnsafeArray<UTF8Char> buff, OSInt *outBuffSize); //True = succ
	};
}

#endif
