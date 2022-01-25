#ifndef _SM_IO_PROTODEC_JTT808PROTOCOLDECODER
#define _SM_IO_PROTODEC_JTT808PROTOCOLDECODER
#include "IO/ProtoDec/IProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class JTT808ProtocolDecoder : public IO::ProtoDec::IProtocolDecoder
		{
		public:
			JTT808ProtocolDecoder();
			virtual ~JTT808ProtocolDecoder();

			virtual const UTF8Char *GetName();
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF8 *sb);
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize);

			UOSInt Unpack(UInt8 *buff, const UInt8 *proto, UOSInt protoSize);
			Bool ParseLocation(const UTF8Char *loc, UOSInt locSize, Text::StringBuilderUTF8 *sb);
		};
	}
}
#endif
