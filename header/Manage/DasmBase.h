#ifndef _SM_MANAGE_DASM
#define _SM_MANAGE_DASM
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"
#include "IO/Writer.h"
#include "Manage/IMemoryReader.h"
#include "Text/CString.h"

namespace Manage
{
	class AddressResolver;
	class ThreadContext;

	class Dasm
	{
	public:
		typedef enum
		{
			RBD_16,
			RBD_32,
			RBD_64
		} RegBitDepth;
		
		struct Dasm_Regs
		{
		};

		struct DasmX86_16_Regs : public Manage::Dasm::Dasm_Regs
		{
			union
			{
				struct
				{
					union
					{
						Int32 EAX;
						UInt16 AX;
						struct
						{
							UInt8 AL;
							UInt8 AH;
						};
					};
					union
					{
						Int32 ECX;
						UInt16 CX;
						struct
						{
							UInt8 CL;
							UInt8 CH;
						};
					};
					union
					{
						Int32 EDX;
						UInt16 DX;
						struct
						{
							UInt8 DL;
							UInt8 DH;
						};
					};
					union
					{
						Int32 EBX;
						UInt16 BX;
						struct
						{
							UInt8 BL;
							UInt8 BH;
						};
					};
					union
					{
						Int32 ESP;
						UInt16 SP;
					};
					union
					{
						Int32 EBP;
						UInt16 BP;
					};
					union
					{
						Int32 ESI;
						UInt16 SI;
					};
					union
					{
						Int32 EDI;
						UInt16 DI;
					};
					union
					{
						Int32 EFLAGS;
						struct
						{
							int DF : 1;
							int SF : 1;
							int OF : 1;
							int IF : 1;
							int CF : 1;
						};
					};
					UInt16 IP;
					UInt16 CS;
					UInt16 DS;
					UInt16 ES;
					UInt16 SS;
				};
				Int32 indexes[11];
			};
		};

		struct DasmX86_32_Regs : public Manage::Dasm::Dasm_Regs
		{
			union
			{
				struct
				{
					UInt32 EAX;
					UInt32 ECX;
					UInt32 EDX;
					UInt32 EBX;
					UInt32 ESP;
					UInt32 EBP;
					UInt32 ESI;
					UInt32 EDI;
					UInt32 EIP;
				};
				struct
				{
					UInt32 regs[9];
				};
			};
			UInt32 EFLAGS;
			union
			{
				struct
				{
					UInt16 CS;
					UInt16 SS;
					UInt16 DS;
					UInt16 ES;
					UInt16 FS;
					UInt16 GS;
				};
				struct
				{
					UInt16 segm[6];
				};
			};

			union
			{
				struct
				{
					UInt32 DR0;
					UInt32 DR1;
					UInt32 DR2;
					UInt32 DR3;
					UInt32 DR6;
					UInt32 DR7;
				};
				struct
				{
					UInt32 dbgreg[6];
				};
			};
			struct
			{
				UInt8 floatBuff[80];
				UInt16 CTRL;
				UInt16 STAT;
				UInt16 TAG;
			};
		};

	public:
		virtual ~Dasm();

		virtual RegBitDepth GetRegBitDepth() = 0;
		virtual Text::CString GetHeader(Bool fullRegs) = 0;
		virtual Dasm_Regs *CreateRegs() = 0;
		virtual void FreeRegs(Dasm_Regs *regs) = 0;
	};

	class Dasm16 : public Dasm
	{
	public:
		virtual RegBitDepth GetRegBitDepth();
		virtual Bool Disasm16(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt16 *currInst, UInt16 *currStack, UInt16 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt16 *blockStart, UInt16 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs) = 0; // true = succ
	};

	class Dasm32 : public Dasm
	{
	public:
		virtual RegBitDepth GetRegBitDepth();
		virtual Bool Disasm32(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt32 *currInst, UInt32 *currStack, UInt32 *currFrame, Data::ArrayListUInt32 *callAddrs, Data::ArrayListUInt32 *jmpAddrs, UInt32 *blockStart, UInt32 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs) = 0; // true = succ
	};

	class Dasm64 : public Dasm
	{
	public:
		virtual RegBitDepth GetRegBitDepth();
		virtual Bool Disasm64(IO::Writer *writer, Manage::AddressResolver *addrResol, UInt64 *currInst, UInt64 *currStack, UInt64 *currFrame, Data::ArrayListUInt64 *callAddrs, Data::ArrayListUInt64 *jmpAddrs, UInt64 *blockStart, UInt64 *blockEnd, Manage::Dasm::Dasm_Regs *regs, Manage::IMemoryReader *memReader, Bool fullRegs) = 0; // true = succ
	};
}

#endif
