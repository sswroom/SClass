#ifndef _SM_IO_PROTODEC_IPROTOCOLDECODER
#define _SM_IO_PROTODEC_IPROTOCOLDECODER
#include "AnyType.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace ProtoDec
	{
		class IProtocolDecoder
		{
		public:
			typedef void (CALLBACKFUNC ProtocolInfo)(AnyType userObj, UInt64 fileOfst, UOSInt size, Text::CStringNN typeName);
		public:
			virtual ~IProtocolDecoder(){};

			virtual Text::CStringNN GetName() const = 0;
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize) = 0;
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize) = 0;
		};
	}
}
#endif
