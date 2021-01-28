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
			Data::ArrayListInt32 *callAddrs;
			Data::ArrayListInt32 *jmpAddrs;
			UTF8Char *sbuff;
			Int32 retAddr;
			Int32 thisStatus;
			Int32 endStatus; //0 = not end, 1 = jmp out, 2 = exit program, 3 = func return
			UInt16 endIP;
			EndType endType;
			UInt8 *codeBuff;
			
			Manage::AddressResolver *addrResol;
			Manage::IMemoryReader *memReader;

			void **codeHdlrs;
			void **codeTHdlrs;
			UTF8Char *outSPtr;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		} DasmARM_Sess;

		typedef Bool (__stdcall *DasmARM_Code)(DasmARM_Sess *sess);
	private:
		DasmARM_Code *codes;
		DasmARM_Code *codesT;
	public:
		DasmARM();
		virtual ~DasmARM();

		virtual const UTF8Char *GetHeader(Bool fullRegs);
		virtual Bool Disasm32(IO::IWriter *writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListInt32 *callAddrs, Data::ArrayListInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs); // true = succ
		virtual Dasm_Regs *CreateRegs();
		virtual void FreeRegs(Dasm_Regs *regs);

		DasmARM_Sess *CreateSess(DasmARM_Regs *regs, UInt8 *code, UInt16 codeSegm);
		void DeleteSess(DasmARM_Sess *sess);

		Bool DasmNext(DasmARM_Sess *sess, WChar *buff, OSInt *outBuffSize); //True = succ
	};
};

#endif
