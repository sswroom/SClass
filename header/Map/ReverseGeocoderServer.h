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
			void *cliData;
		} ClientStatus;

	private:
		Net::TCPServerController *ctrl;
		IO::ProtoHdlr::ProtoRevGeoHandler *protocol;

		Sync::Mutex *reqMut;
		Sync::Event *reqEvt;
		UTF8Char *reqBuff;
		UOSInt reqBuffSize;
		Int32 reqLat;
		Int32 reqLon;
		UInt32 reqLCID;
		Bool reqResult;

		Net::TCPClient *GetLatestClient(OSInt retryCnt);
	public:
		ReverseGeocoderServer(Net::SocketFactory *sockf, IO::LogTool *log, UInt16 port);
		virtual ~ReverseGeocoderServer();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);

		virtual void *NewConn(Net::TCPClient *cli);
		virtual void EndConn(Net::TCPClient *cli, void *cliObj);
		virtual UOSInt ReceivedData(Net::TCPClient *cli, void *cliObj, UInt8 *buff, UOSInt buffSize); //Return buff size unprocessed

		virtual void DataParsed(IO::Stream *stm, void *cliObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *cliObj, const UInt8 *buff, UOSInt buffSize);
		Bool IsError();
	};
}
#endif
