#ifndef _SM_IO_PROTOCOLHANDLER
#define _SM_IO_PROTOCOLHANDLER
#include "AnyType.h"
#include "IO/ProtocolParser.h"

namespace IO
{
	class ProtocolHandler : public ProtocolParser
	{
	public:
		class DataListener
		{
		public:
			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize) = 0;
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize) = 0;
		};

	public:
		virtual ~ProtocolHandler(){};
		virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData) = 0;
	};
}
#endif
