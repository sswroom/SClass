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
		UnsafeArrayOpt<UTF8Char> reqBuff;
		UOSInt reqBuffSize;
		Int32 reqLat;
		Int32 reqLon;
		UInt32 reqLCID;
		Bool reqResult;

		Net::TCPClient *GetLatestClient(UOSInt retryCnt);
	public:
		ReverseGeocoderServer(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port);
		virtual ~ReverseGeocoderServer();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);

		virtual AnyType NewConn(NN<Net::TCPClient> cli);
		virtual void EndConn(NN<Net::TCPClient> cli, AnyType cliObj);
		virtual UOSInt ReceivedData(NN<Net::TCPClient> cli, AnyType cliObj, const Data::ByteArrayR &buff); //Return buff size unprocessed

		virtual void DataParsed(NN<IO::Stream> stm, AnyType cliObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType cliObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		Bool IsError();
	};
}
#endif
