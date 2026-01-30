#ifndef _SM_MAP_REVERSEGEOCODERCLIENT
#define _SM_MAP_REVERSEGEOCODERCLIENT
#include "DB/DBTool.h"
#include "IO/Writer.h"
#include "IO/LogTool.h"
#include "IO/ProtoHdlr/ProtoRevGeoHandler.h"
#include "Map/ReverseGeocoder.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"

namespace Map
{
	class ReverseGeocoderClient : public IO::ProtocolHandler::DataListener
	{
	private:
		NN<Map::ReverseGeocoder> revGeo;
		IO::ProtoHdlr::ProtoRevGeoHandler protocol;
		NN<Net::SocketFactory> sockf;
		NN<IO::Writer> errWriter;
		NN<Text::String> host;
		UInt16 port;
		Sync::Mutex cliMut;
		Optional<Net::TCPClient> cli;
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
		ReverseGeocoderClient(NN<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, NN<Map::ReverseGeocoder> revGeo, NN<IO::Writer> errWriter);
		virtual ~ReverseGeocoderClient();

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UIntOS buffSize);
	};
}
#endif
