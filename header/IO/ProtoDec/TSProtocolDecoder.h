#ifndef _SM_IO_PROTODEC_TSPROTOCOLDECODER
#define _SM_IO_PROTODEC_TSPROTOCOLDECODER
#include "IO/ProtoDec/IProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class TSProtocolDecoder : public IO::ProtoDec::IProtocolDecoder
		{
		private:
			UInt8 *protoBuff;
		public:
			TSProtocolDecoder();
			virtual ~TSProtocolDecoder();

			virtual Text::CStringNN GetName() const;
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb);
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
