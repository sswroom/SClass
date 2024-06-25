#ifndef _SM_MAP_REVERSEGEOCODERCLIENT
#define _SM_MAP_REVERSEGEOCODERCLIENT
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "IO/Writer.h"
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
		IO::ProtoHdlr::ProtoRevGeoHandler protocol;
		NN<Net::SocketFactory> sockf;
		IO::Writer *errWriter;
		NN<Text::String> host;
		UInt16 port;
		Sync::Mutex cliMut;
		Net::TCPClient *cli;
		Bool cliRunning;
		Bool cliToStop;
		Sync::Event monEvt;
		Bool monRunning;
		Bool monToStop;
		Data::DateTime lastKASent;
		Data::DateTime lastKARecv;

	private:
		static UInt32 __stdcall ClientThread(AnyType userObj);
		static UInt32 __stdcall MonThread(AnyType userObj);
	public:
		ReverseGeocoderClient(NN<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Map::IReverseGeocoder *revGeo, IO::Writer *errWriter);
		virtual ~ReverseGeocoderClient();

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif
