#ifndef _SM_IO_IPROTOCOLHANDLER
#define _SM_IO_IPROTOCOLHANDLER
#include "AnyType.h"
#include "IO/IProtocolParser.h"

namespace IO
{
	class IProtocolHandler : public IProtocolParser
	{
	public:
		class DataListener
		{
		public:
			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize) = 0;
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize) = 0;
		};

	public:
		virtual ~IProtocolHandler(){};
		virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData) = 0;
	};
}
#endif
