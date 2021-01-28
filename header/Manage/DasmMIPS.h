#ifndef _SM_MANAGE_DASMMIPS
#define _SM_MANAGE_DASMMIPS
#include "Data/ArrayListInt32.h"
#include "Manage/DasmBase.h"

namespace Manage
{
	class DasmMIPS : public Dasm32
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

		struct DasmMIPS_Regs : public Dasm_Regs
		{
			union
			{
				struct
				{
					UInt32 zero;
					UInt32 at;
					UInt32 v0;
					UInt32 v1;
					UInt32 a0;
					UInt32 a1;
					UInt32 a2;
					UInt32 a3;
					UInt32 t0;
					UInt32 t1;
					UInt32 t2;
					UInt32 t3;
					UInt32 t4;
					UInt32 t5;
					UInt32 t6;
					UInt32 t7;
					UInt32 s0;
					UInt32 s1;
					UInt32 s2;
					UInt32 s3;
					UInt32 s4;
					UInt32 s5;
					UInt32 s6;
					UInt32 s7;
					UInt32 t8;
					UInt32 t9;
					UInt32 k0;
					UInt32 k1;
					UInt32 gp;
					UInt32 sp;
					UInt32 fp;
					UInt32 ra;

					UInt32 pc;
					UInt32 hi;
					UInt32 lo;
				};
				Int32 regs[35];
			};
		};

		typedef struct
		{
			DasmMIPS_Regs regs;
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
			
			Manage::AddressResolver *addrResol;
			Manage::IMemoryReader *memReader;

			void **codeHdlrs;
			void **code_0Hdlrs;
			UTF8Char *outSPtr;
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
		} DasmMIPS_Sess;

		typedef Bool (__stdcall *DasmMIPS_Code)(DasmMIPS_Sess *sess);
	private:
		DasmMIPS_Code *codes;
		DasmMIPS_Code *codes_0;
	public:
		DasmMIPS();
		virtual ~DasmMIPS();

		virtual const UTF8Char *GetHeader(Bool fullRegs);
		virtual Bool Disasm32(IO::IWriter *writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListInt32 *callAddrs, Data::ArrayListInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs); // true = succ
		virtual Dasm_Regs *CreateRegs();
		virtual void FreeRegs(Dasm_Regs *regs);

		DasmMIPS_Sess *CreateSess(DasmMIPS_Regs *regs, UInt8 *code, UInt16 codeSegm);
		void DeleteSess(DasmMIPS_Sess *sess);

		Bool DasmNext(DasmMIPS_Sess *sess, UTF8Char *buff, OSInt *outBuffSize); //True = succ
	};
};

#endif
