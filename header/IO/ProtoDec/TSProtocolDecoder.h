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

			virtual const UTF8Char *GetName();
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF *sb);
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
