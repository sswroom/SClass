#ifndef _SM_IO_PROTODEC_PROTOCOLDECODER
#define _SM_IO_PROTODEC_PROTOCOLDECODER
#include "AnyType.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace ProtoDec
	{
		class ProtocolDecoder
		{
		public:
			typedef void (CALLBACKFUNC ProtocolInfo)(AnyType userObj, UInt64 fileOfst, UIntOS size, Text::CStringNN typeName);
		public:
			virtual ~ProtocolDecoder(){};

			virtual Text::CStringNN GetName() const = 0;
			virtual UIntOS ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UnsafeArray<UInt8> buff, UIntOS buffSize) = 0;
			virtual Bool GetProtocolDetail(UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool IsValid(UnsafeArray<UInt8> buff, UIntOS buffSize) = 0;
		};
	}
}
#endif
