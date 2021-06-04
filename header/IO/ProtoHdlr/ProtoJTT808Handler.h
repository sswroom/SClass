#ifndef _SM_IO_PROTOHDLR_PROTOJTT808HANDLER
#define _SM_IO_PROTOHDLR_PROTOJTT808HANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoJTT808Handler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;
			UInt64 devId;

		public:
			ProtoJTT808Handler(IO::IProtocolHandler::DataListener *listener, UInt64 devId);
			virtual ~ProtoJTT808Handler();

			virtual void *CreateStreamData(IO::Stream *stm);
			virtual void DeleteStreamData(IO::Stream *stm, void *stmData);
			virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);

			const UInt8 *GetPacketContent(const UInt8 *packet, UOSInt *contSize);
		};
	}
}
#endif
