#ifndef _SM_MAP_REVERSEGEOCODERSERVER
#define _SM_MAP_REVERSEGEOCODERSERVER
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "IO/ProtoHdlr/ProtoRevGeoHandler.h"
#include "Map/IReverseGeocoder.h"
#include "Net/TCPServerController.h"
#include "Sync/Mutex.h"

namespace Map
{
	class ReverseGeocoderServer : public Map::IReverseGeocoder, public Net::TCPServerController::TCPServerHandler, public IO::ProtoHdlr::ProtoRevGeoHandler::DataListener
	{
	private:
		typedef struct
		{
			Int64 lastReqTime;
			AnyType cliData;
		} ClientStatus;

	private:
		Net::TCPServerController *ctrl;
		IO::ProtoHdlr::ProtoRevGeoHandler protocol;

		Sync::Mutex reqMut;
		Sync::Event reqEvt;
		UTF8Char *reqBuff;
		UOSInt reqBuffSize;
		Int32 reqLat;
		Int32 reqLon;
		UInt32 reqLCID;
		Bool reqResult;

		Net::TCPClient *GetLatestClient(UOSInt retryCnt);
	public:
		ReverseGeocoderServer(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::LogTool> log, UInt16 port);
		virtual ~ReverseGeocoderServer();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);

		virtual AnyType NewConn(NotNullPtr<Net::TCPClient> cli);
		virtual void EndConn(NotNullPtr<Net::TCPClient> cli, AnyType cliObj);
		virtual UOSInt ReceivedData(NotNullPtr<Net::TCPClient> cli, AnyType cliObj, const Data::ByteArrayR &buff); //Return buff size unprocessed

		virtual void DataParsed(NotNullPtr<IO::Stream> stm, AnyType cliObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(NotNullPtr<IO::Stream> stm, AnyType cliObj, const UInt8 *buff, UOSInt buffSize);
		Bool IsError();
	};
}
#endif
