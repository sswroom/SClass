#ifndef _SM_IO_FILEANALYSE_FRAMEDETAILHANDLER
#define _SM_IO_FILEANALYSE_FRAMEDETAILHANDLER
#include "Data/ByteArray.h"
#include "Text/CString.h"

namespace IO
{
	namespace FileAnalyse
	{
		class FrameDetailHandler
		{
		public:
			virtual ~FrameDetailHandler() {};

			virtual void AddHeader(Text::CStringNN header) = 0;
			virtual void AddField(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value) = 0;
			virtual void AddSubfield(UInt64 ofst, UInt64 size, Text::CStringNN name, Text::CString value) = 0;
			virtual void AddFieldSeperstor(UInt64 ofst, Text::CStringNN name) = 0;
			virtual void AddText(UInt64 ofst, Text::CStringNN name) = 0;
			virtual void AddSubframe(UInt64 ofst, UInt64 size) = 0;

			void AddBool(UIntOS frameOfst, Text::CStringNN name, UInt8 v);
			void AddInt64(UIntOS frameOfst, Text::CStringNN name, Int64 v);
			void AddInt64V(UIntOS frameOfst, UIntOS size, Text::CStringNN name, Int64 v);
			void AddUInt64(UIntOS frameOfst, Text::CStringNN name, UInt64 v);
			void AddUInt64V(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UInt64 v);
			void AddInt(UIntOS frameOfst, UIntOS size, Text::CStringNN name, IntOS v);
			void AddUInt(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UIntOS v);
			void AddBit(UIntOS frameOfst, Text::CStringNN name, UInt8 v, UIntOS bitNum);
			void AddUIntName(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UIntOS v, Text::CString vName);
			void AddUInt64Name(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UInt64 v, Text::CString vName);
			void AddFloat(UIntOS frameOfst, UIntOS size, Text::CStringNN name, Double v);
			void AddHex8(UIntOS frameOfst, Text::CStringNN name, UInt8 v);
			void AddHex16(UIntOS frameOfst, Text::CStringNN name, UInt16 v);
			void AddHex24(UIntOS frameOfst, Text::CStringNN name, UInt32 v);
			void AddHex32(UIntOS frameOfst, Text::CStringNN name, UInt32 v);
			void AddHex64(UIntOS frameOfst, Text::CStringNN name, UInt64 v);
			void AddHex64V(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UInt64 v);
			void AddHex8Name(UIntOS frameOfst, Text::CStringNN name, UInt8 v, Text::CString vName);
			void AddHex16Name(UIntOS frameOfst, Text::CStringNN name, UInt16 v, Text::CString vName);
			void AddHex32Name(UIntOS frameOfst, Text::CStringNN name, UInt32 v, Text::CString vName);
			void AddStrC(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff);
			void AddStrS(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff);
			void AddStrZ(UIntOS frameOfst, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff);
			void AddHexBuff(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UnsafeArray<const UInt8> vBuff, Bool multiLine);
			void AddHexBuff(UIntOS frameOfst, UIntOS size, Text::CStringNN name, UnsafeArray<const UInt8> vBuff, UTF8Char seperator, Bool multiLine);
			void AddHexBuff(UIntOS frameOfst, Text::CStringNN name, Data::ByteArrayR vBuff, Bool multiLine);
			void AddHexBuff(UIntOS frameOfst, Text::CStringNN name, Data::ByteArrayR vBuff, UTF8Char seperator, Bool multiLine);
			void AddIPv4(UIntOS frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> vBuff);
			void AddIPv6(UIntOS frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> vBuff);
			void AddMACAddr(UIntOS frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> macBuff, Bool showVendor);
			void AddNetBIOSName(UIntOS frameOfst, UIntOS size, Text::CStringNN name, Text::CStringNN nbName);

			void AddTextHexBuff(UIntOS frameOfst, UIntOS size, UnsafeArray<const UInt8> vBuff, Bool multiLine);
		};
	}
}
#endif
