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
			virtual OSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, Int64 fileOfst, UInt8 *buff, OSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, OSInt buffSize, Text::StringBuilderUTF *sb);
			virtual Bool IsValid(UInt8 *buff, OSInt buffSize);

			OSInt Unpack(UInt8 *buff, const UInt8 *proto, OSInt protoSize);
			Bool ParseLocation(const UTF8Char *loc, OSInt locSize, Text::StringBuilderUTF *sb);
		};
	}
}
#endif
