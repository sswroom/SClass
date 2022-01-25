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
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF8 *sb);
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize);

			void AppendDevStatus(Text::StringBuilderUTF8 *sb, UInt16 status);
		};
	}
}
#endif
