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
			virtual UIntOS ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UnsafeArray<UInt8> buff, UIntOS buffSize);
			virtual Bool GetProtocolDetail(UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Text::StringBuilderUTF8> sb);
			virtual Bool IsValid(UnsafeArray<UInt8> buff, UIntOS buffSize);

			UIntOS Unpack(UnsafeArray<UInt8> buff, UnsafeArray<const UInt8> proto, UIntOS protoSize);
			Bool ParseLocation(UnsafeArray<const UTF8Char> loc, UIntOS locSize, NN<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
