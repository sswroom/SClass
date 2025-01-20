#ifndef _SM_IO_PROTODEC_JTT808PROTOCOLDECODER
#define _SM_IO_PROTODEC_JTT808PROTOCOLDECODER
#include "AnyType.h"
#include "IO/ProtoDec/ProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class JTT808ProtocolDecoder : public IO::ProtoDec::ProtocolDecoder
		{
		public:
			JTT808ProtocolDecoder();
			virtual ~JTT808ProtocolDecoder();

			virtual Text::CStringNN GetName() const;
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UnsafeArray<UInt8> buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb);
			virtual Bool IsValid(UnsafeArray<UInt8> buff, UOSInt buffSize);

			UOSInt Unpack(UnsafeArray<UInt8> buff, UnsafeArray<const UInt8> proto, UOSInt protoSize);
			Bool ParseLocation(UnsafeArray<const UTF8Char> loc, UOSInt locSize, NN<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
