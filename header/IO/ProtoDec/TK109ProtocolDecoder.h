#ifndef _SM_IO_PROTODEC_TK109PROTOCOLDECODER
#define _SM_IO_PROTODEC_TK109PROTOCOLDECODER
#include "AnyType.h"
#include "IO/ProtoDec/ProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class TK109ProtocolDecoder : public IO::ProtoDec::ProtocolDecoder
		{
		public:
			TK109ProtocolDecoder();
			virtual ~TK109ProtocolDecoder();

			virtual Text::CStringNN GetName() const;
			virtual UIntOS ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UnsafeArray<UInt8> buff, UIntOS buffSize);
			virtual Bool GetProtocolDetail(UnsafeArray<UInt8> buff, UIntOS buffSize, NN<Text::StringBuilderUTF8> sb);
			virtual Bool IsValid(UnsafeArray<UInt8> buff, UIntOS buffSize);

			void AppendDevStatus(NN<Text::StringBuilderUTF8> sb, UInt16 status);
		};
	}
}
#endif
