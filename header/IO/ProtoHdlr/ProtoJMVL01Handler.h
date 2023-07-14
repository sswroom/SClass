#ifndef _SM_IO_PROTOHDLR_PROTOJMVL01HANDLER
#define _SM_IO_PROTOHDLR_PROTOJMVL01HANDLER
#include "Crypto/Hash/CRC16R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoJMVL01Handler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;
			UInt64 devId;
			Crypto::Hash::CRC16R crc;

		public:
			ProtoJMVL01Handler(IO::IProtocolHandler::DataListener *listener, UInt64 devId);
			virtual ~ProtoJMVL01Handler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
