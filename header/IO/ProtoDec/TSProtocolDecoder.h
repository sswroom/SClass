#ifndef _SM_IO_PROTODEC_TSPROTOCOLDECODER
#define _SM_IO_PROTODEC_TSPROTOCOLDECODER
#include "IO/ProtoDec/ProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class TSProtocolDecoder : public IO::ProtoDec::ProtocolDecoder
		{
		private:
			UnsafeArray<UInt8> protoBuff;
		public:
			TSProtocolDecoder();
			virtual ~TSProtocolDecoder();

			virtual Text::CStringNN GetName() const;
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UnsafeArray<UInt8> buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb);
			virtual Bool IsValid(UnsafeArray<UInt8> buff, UOSInt buffSize);
		};
	}
}
#endif
