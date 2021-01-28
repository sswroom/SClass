#ifndef _SM_IO_PROTODEC_TK109PROTOCOLDECODER
#define _SM_IO_PROTODEC_TK109PROTOCOLDECODER
#include "IO/ProtoDec/IProtocolDecoder.h"

namespace IO
{
	namespace ProtoDec
	{
		class TK109ProtocolDecoder : public IO::ProtoDec::IProtocolDecoder
		{
		public:
			TK109ProtocolDecoder();
			virtual ~TK109ProtocolDecoder();

			virtual const UTF8Char *GetName();
			virtual OSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, Int64 fileOfst, UInt8 *buff, OSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, OSInt buffSize, Text::StringBuilderUTF *sb);
			virtual Bool IsValid(UInt8 *buff, OSInt buffSize);

			void AppendDevStatus(Text::StringBuilderUTF *sb, UInt16 status);
		};
	}
}
#endif
