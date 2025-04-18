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

			void AddBool(UOSInt frameOfst, Text::CStringNN name, UInt8 v);
			void AddInt64(UOSInt frameOfst, Text::CStringNN name, Int64 v);
			void AddInt64V(UOSInt frameOfst, UOSInt size, Text::CStringNN name, Int64 v);
			void AddUInt64(UOSInt frameOfst, Text::CStringNN name, UInt64 v);
			void AddUInt64V(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UInt64 v);
			void AddInt(UOSInt frameOfst, UOSInt size, Text::CStringNN name, OSInt v);
			void AddUInt(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UOSInt v);
			void AddBit(UOSInt frameOfst, Text::CStringNN name, UInt8 v, UOSInt bitNum);
			void AddUIntName(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UOSInt v, Text::CString vName);
			void AddUInt64Name(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UInt64 v, Text::CString vName);
			void AddFloat(UOSInt frameOfst, UOSInt size, Text::CStringNN name, Double v);
			void AddHex8(UOSInt frameOfst, Text::CStringNN name, UInt8 v);
			void AddHex16(UOSInt frameOfst, Text::CStringNN name, UInt16 v);
			void AddHex24(UOSInt frameOfst, Text::CStringNN name, UInt32 v);
			void AddHex32(UOSInt frameOfst, Text::CStringNN name, UInt32 v);
			void AddHex64(UOSInt frameOfst, Text::CStringNN name, UInt64 v);
			void AddHex64V(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UInt64 v);
			void AddHex8Name(UOSInt frameOfst, Text::CStringNN name, UInt8 v, Text::CString vName);
			void AddHex16Name(UOSInt frameOfst, Text::CStringNN name, UInt16 v, Text::CString vName);
			void AddHex32Name(UOSInt frameOfst, Text::CStringNN name, UInt32 v, Text::CString vName);
			void AddStrC(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff);
			void AddStrS(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff);
			void AddStrZ(UOSInt frameOfst, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff);
			void AddHexBuff(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UInt8> vBuff, Bool multiLine);
			void AddHexBuff(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UInt8> vBuff, UTF8Char seperator, Bool multiLine);
			void AddHexBuff(UOSInt frameOfst, Text::CStringNN name, Data::ByteArrayR vBuff, Bool multiLine);
			void AddHexBuff(UOSInt frameOfst, Text::CStringNN name, Data::ByteArrayR vBuff, UTF8Char seperator, Bool multiLine);
			void AddIPv4(UOSInt frameOfst, Text::CStringNN name, const UInt8 *vBuff);
			void AddIPv6(UOSInt frameOfst, Text::CStringNN name, const UInt8 *vBuff);
			void AddMACAddr(UOSInt frameOfst, Text::CStringNN name, const UInt8 *macBuff, Bool showVendor);
			void AddNetBIOSName(UOSInt frameOfst, UOSInt size, Text::CStringNN name, Text::CStringNN nbName);

			void AddTextHexBuff(UOSInt frameOfst, UOSInt size, UnsafeArray<const UInt8> vBuff, Bool multiLine);
		};
	}
}
#endif
