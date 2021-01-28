#ifndef _SM_IO_PROTODEC_IPROTOCOLDECODER
#define _SM_IO_PROTODEC_IPROTOCOLDECODER
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace ProtoDec
	{
		class IProtocolDecoder
		{
		public:
			typedef void (__stdcall *ProtocolInfo)(void *userObj, Int64 fileOfst, OSInt size, const UTF8Char *typeName);
		public:
			virtual ~IProtocolDecoder(){};

			virtual const UTF8Char *GetName() = 0;
			virtual OSInt ParseProtocol(ProtocolInfo hdlr, void *userObj, Int64 fileOfst, UInt8 *buff, OSInt buffSize) = 0;
			virtual Bool GetProtocolDetail(UInt8 *buff, OSInt buffSize, Text::StringBuilderUTF *sb) = 0;
			virtual Bool IsValid(UInt8 *buff, OSInt buffSize) = 0;
		};
	}
}
#endif
