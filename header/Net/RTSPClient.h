#ifndef _SM_NET_RTSPCLIENT
#define _SM_NET_RTSPCLIENT
#include "AnyType.h"
#include "Net/RTPController.h"
#include "Net/RTPCliChannel.h"
#include "Net/SDPFile.h"
#include "Net/TCPClientFactory.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Net
{
	class RTSPClient : public RTPController
	{
	private:
		struct ClientData
		{
			Int32 useCnt;

			NN<Net::TCPClientFactory> clif;
			Optional<Net::TCPClient> cli;
			Sync::Mutex cliMut;
			NN<Text::String> host;
			UInt16 port;
			Int32 nextSeq;
			Data::Duration timeout;

			Sync::Event reqEvt;
			Sync::Mutex reqMut;
			UInt8 *reqReply;
			UIntOS reqReplySize;
			Int32 reqReplyStatus;
			Bool reqSuccess;
			Text::String *reqStrs;

			Bool threadRunning;
			Bool threadToStop;
		};
	private:
		ClientData *cliData;

		static UInt32 __stdcall ControlThread(AnyType userObj);
		Int32 NextRequest();
		Bool WaitForReply();
		Bool SendData(UnsafeArray<const UInt8> buff, UIntOS buffSize);

		RTSPClient(const RTSPClient *cli);
	public:
		RTSPClient(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port, Data::Duration timeout);
		~RTSPClient();


		Bool GetOptions(Text::CStringNN url, Data::ArrayListObj<const UTF8Char *> *options);
		Net::SDPFile *GetMediaInfo(Text::CStringNN url);

		static IO::ParsedObject *ParseURL(NN<Net::TCPClientFactory> clif, Text::CStringNN url, Data::Duration timeout, NN<IO::LogTool> log);

	private:
		UnsafeArrayOpt<UTF8Char> SetupRTP(UnsafeArray<UTF8Char> sessIdOut, Text::CStringNN url, NN<Net::RTPCliChannel> rtpChannel);

		Bool Play(Text::CStringNN url, Text::CStringNN sessId);
		Bool Close(Text::CStringNN url, Text::CStringNN sessId);

	public:
		virtual Bool Init(NN<Net::RTPCliChannel> rtpChannel);
		virtual Bool Play(NN<Net::RTPCliChannel> rtpChannel);
		virtual Bool KeepAlive(NN<Net::RTPCliChannel> rtpChannel);
		virtual Bool StopPlay(NN<Net::RTPCliChannel> rtpChannel);
		virtual Bool Deinit(NN<Net::RTPCliChannel> rtpChannel);

		virtual Net::RTPController *Clone() const;
	};
}
#endif
