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
					UInt64 FP;
					UInt64 SP;
					UInt64 PC;
					UInt64 LR;
				};
				Int32 indexes[17];
			};
		};

		typedef struct
		{
			DasmARM64_Regs regs;
			UInt8 *code;
			UInt16 codeSegm;
			Data::ArrayListUInt64 *callAddrs;
			Data::ArrayListUInt64 *jmpAddrs;
			UTF8Char *sbuff;
			UInt64 retAddr;
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
		} DasmARM64_Sess;

		typedef Bool (__stdcall *DasmARM64_Code)(DasmARM64_Sess *sess);
	private:
		DasmARM64_Code *codes;
		DasmARM64_Code *codesT;
	public:
		DasmARM64();
		virtual ~DasmARM64();

		virtual Text::CString GetHeader(Bool fullRegs);
		virtual Bool Disasm64(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt64 *currInst, UInt64 *currStack, UInt64 *currFrame, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs); // true = succ
		virtual Dasm_Regs *CreateRegs();
		virtual void FreeRegs(Dasm_Regs *regs);

		DasmARM64_Sess *CreateSess(DasmARM64_Regs *regs, UInt8 *code, UInt16 codeSegm);
		void DeleteSess(DasmARM64_Sess *sess);

		Bool DasmNext(DasmARM64_Sess *sess, UTF8Char *buff, OSInt *outBuffSize); //True = succ
	};
}

#endif
