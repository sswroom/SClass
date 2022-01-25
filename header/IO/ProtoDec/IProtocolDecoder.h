#ifndef _SM_IO_PROTODEC_IPROTOCOLDECODER
#define _SM_IO_PROTODEC_IPROTOCOLDECODER
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace ProtoDec
	{
		class IProtocolDecoder
		{
		public:
			typedef void (__stdcall *ProtocolInfo)(void *userObj, UInt64 fileOfst, UOSInt size, const UTF8Char *typeName);
		public:
			virtual ~IProtocolDecoder(){};

			virtual const UTF8Char *GetName() = 0;
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize) = 0;
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, Text::StringBuilderUTF8 *sb) = 0;
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize) = 0;
		};
	}
}
#endif
