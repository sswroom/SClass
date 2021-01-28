#ifndef _SM_IO_PROTOHDLR_PROTORODBINHANDLER
#define _SM_IO_PROTOHDLR_PROTORODBINHANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoRodBinHandler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;

		public:
			ProtoRodBinHandler(IO::IProtocolHandler::DataListener *listener);
			virtual ~ProtoRodBinHandler();

			virtual void *CreateStreamData(IO::Stream *stm);
			virtual void DeleteStreamData(IO::Stream *stm, void *stmData);
			virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, UInt8 *cmd, UOSInt cmdSize, void *stmData);

			UInt16 CalCheck(UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
