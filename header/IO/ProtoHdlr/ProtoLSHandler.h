#ifndef _SM_IO_PROTOHDLR_PROTOLSHANDLER
#define _SM_IO_PROTOHDLR_PROTOLSHANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoLSHandler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;

		public:
			ProtoLSHandler(IO::IProtocolHandler::DataListener *listener);
			virtual ~ProtoLSHandler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);

			UInt16 CalCheck(const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
