#ifndef _SM_MAP_REVERSEGEOCODERCLIENT
#define _SM_MAP_REVERSEGEOCODERCLIENT
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "IO/IWriter.h"
#include "IO/LogTool.h"
#include "IO/ProtoHdlr/ProtoRevGeoHandler.h"
#include "Map/IReverseGeocoder.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"

namespace Map
{
	class ReverseGeocoderClient : public IO::IProtocolHandler::DataListener
	{
	private:
		Map::IReverseGeocoder *revGeo;
		IO::ProtoHdlr::ProtoRevGeoHandler *protocol;
		Net::SocketFactory *sockf;
		IO::IWriter *errWriter;
		const UTF8Char *host;
		UInt16 port;
		Sync::Mutex *cliMut;
		Net::TCPClient *cli;
		Bool cliRunning;
		Bool cliToStop;
		Sync::Event *monEvt;
		Bool monRunning;
		Bool monToStop;
		Data::DateTime *lastKASent;
		Data::DateTime *lastKARecv;

	private:
		static UInt32 __stdcall ClientThread(void *userObj);
		static UInt32 __stdcall MonThread(void *userObj);
	public:
		ReverseGeocoderClient(Net::SocketFactory *sockf, const UTF8Char *host, UInt16 port, Map::IReverseGeocoder *revGeo, IO::IWriter *errWriter);
		virtual ~ReverseGeocoderClient();

		virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
