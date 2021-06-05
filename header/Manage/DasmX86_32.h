#ifndef _SM_MANAGE_DASMX86_32
#define _SM_MANAGE_DASMX86_32
#include "Data/ArrayListUInt32.h"
#include "Manage/AddressResolver.h"
#include "Manage/IMemoryReader.h"
#include "Manage/ThreadContextX86_32.h"
#include "Text/StringBuilderUTF.h"

namespace Manage
{
	class DasmX86_32 : public Dasm32
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

		typedef struct
		{
			DasmX86_32_Regs regs;
			Data::ArrayListUInt32 *callAddrs;
			Data::ArrayListUInt32 *jmpAddrs;
			//Text::StringBuilderW *outStr;
			UTF8Char *sbuff;
//			Bool isEnded;
			EndType endType;
			UInt32 espOfst;
			UInt32 retAddr;
			UInt32 lastStatus;
			UInt32 thisStatus; //bit0: prefix1, bit1: ignore esp change, bit2: prefix2, bit16: segment mod, bit31: ignore esp
			//prefix 2 1: 00 = no, 01 = 0x66, 02 = f2, 03 = f3
			Int32 segmId;
			
			Manage::AddressResolver *addrResol;
			Manage::IMemoryReader *memReader;

			UInt32 stabesp;
			void **codes;
			void **codes0f;
			void **codes0f38;
			void **codes0f3a;
		} DasmX86_32_Sess;

		typedef Bool (__stdcall *DasmX86_32_Code)(DasmX86_32_Sess *sess);
	private:
		DasmX86_32_Code *codes;
		DasmX86_32_Code *codes0f;
		DasmX86_32_Code *codes0f38;
		DasmX86_32_Code *codes0f3a;

//		AddressNameFunc nameFunc;
//		void *userObj;
	public:
		DasmX86_32();
		virtual ~DasmX86_32();

		virtual const UTF8Char *GetHeader(Bool fullRegs);
		virtual Bool Disasm32(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt32 *currEip, UInt32 *currEsp, UInt32 *currEbp, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs); // true = succ
		Bool Disasm32In(Text::StringBuilderUTF *outStr, Manage::AddressResolver *addrResol, UInt32 *currEip, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::IMemoryReader *memReader); // true = succ
		virtual Dasm_Regs *CreateRegs();
		virtual void FreeRegs(Dasm_Regs *regs);

		void *StartDasm(Manage::AddressResolver *addrResol, void *addr, Manage::IMemoryReader *memReader);
		void EndDasm(void *sess);
		UTF8Char *DasmNext(void *sess, UTF8Char *buff);
		OSInt SessGetCodeOffset(void *sess);
		EndType SessGetEndType(void *sess);
		Bool SessContJmp(void *sess);
	};
}

#endif
