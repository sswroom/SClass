#ifndef _SM_IO_PROTODEC_TK109PROTOCOLDECODER
#define _SM_IO_PROTODEC_TK109PROTOCOLDECODER
#include "AnyType.h"
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

			virtual Text::CStringNN GetName() const;
			virtual UOSInt ParseProtocol(ProtocolInfo hdlr, AnyType userObj, UInt64 fileOfst, UInt8 *buff, UOSInt buffSize);
			virtual Bool GetProtocolDetail(UInt8 *buff, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb);
			virtual Bool IsValid(UInt8 *buff, UOSInt buffSize);

			void AppendDevStatus(NN<Text::StringBuilderUTF8> sb, UInt16 status);
		};
	}
}
#endif
