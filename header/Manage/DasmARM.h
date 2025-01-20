#ifndef _SM_MANAGE_DASMARM
#define _SM_MANAGE_DASMARM
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmARM : public Dasm32
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

		struct DasmARM_Regs : public Manage::Dasm::Dasm_Regs
		{
			union 
			{
				struct
				{
					UInt32 R0;
					UInt32 R1;
					UInt32 R2;
					UInt32 R3;
					UInt32 R4;
					UInt32 R5;
					UInt32 R6;
					UInt32 R7;
					UInt32 R8;
					UInt32 R9;
					UInt32 R10;
					UInt32 FP;
					UInt32 IP;
					UInt32 SP;
					UInt32 PC;
					UInt32 LR;
					UInt32 CPSR;
				};
				Int32 indexes[17];
			};
		};

		typedef struct
		{
			DasmARM_Regs regs;
			UInt8 *code;
			UInt16 codeSegm;
			Data::ArrayListUInt32 *callAddrs;
			Data::ArrayListUInt32 *jmpAddrs;
			UnsafeArray<UTF8Char> sbuff;
			UInt32 retAddr;
			Int32 thisStatus;
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			EndType endType;
			UInt8 *codeBuff;
			
			Manage::AddressResolver *addrResol;
			Manage::MemoryReader *memReader;

			void **codeHdlrs;
			void **codeTHdlrs;
			UTF8Char *outSPtr;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		} DasmARM_Sess;

		typedef Bool (CALLBACKFUNC DasmARM_Code)(NN<DasmARM_Sess> sess);
	private:
		DasmARM_Code *codes;
		DasmARM_Code *codesT;
	public:
		DasmARM();
		virtual ~DasmARM();

		virtual Text::CStringNN GetHeader(Bool fullRegs) const;
		virtual Bool Disasm32(NN<IO::Writer> writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, NN<Manage::Dasm::Dasm_Regs> regs, Manage::MemoryReader *memReader, Bool fullRegs); // true = succ
		virtual NN<Dasm_Regs> CreateRegs() const;
		virtual void FreeRegs(NN<Dasm_Regs> regs) const;

		NN<DasmARM_Sess> CreateSess(NN<DasmARM_Regs> regs, UInt8 *code, UInt16 codeSegm);
		void DeleteSess(NN<DasmARM_Sess> sess);

		Bool DasmNext(NN<DasmARM_Sess> sess, UTF8Char *buff, UOSInt *outBuffSize); //True = succ
	};
}

#endif
