#ifndef _SM_NET_RTSPCLIENT
#define _SM_NET_RTSPCLIENT
#include "Net/IRTPController.h"
#include "Net/RTPCliChannel.h"
#include "Net/SDPFile.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Net
{
	class RTSPClient : public IRTPController
	{
	private:
		struct ClientData
		{
			Int32 useCnt;

			NotNullPtr<Net::SocketFactory> sockf;
			Net::TCPClient *cli;
			Sync::Mutex cliMut;
			NotNullPtr<Text::String> host;
			UInt16 port;
			Int32 nextSeq;
			Data::Duration timeout;

			Sync::Event reqEvt;
			Sync::Mutex reqMut;
			UInt8 *reqReply;
			UOSInt reqReplySize;
			Int32 reqReplyStatus;
			Bool reqSuccess;
			Text::String *reqStrs;

			Bool threadRunning;
			Bool threadToStop;
		};
	private:
		ClientData *cliData;

		static UInt32 __stdcall ControlThread(void *userObj);
		Int32 NextRequest();
		Bool WaitForReply();
		Bool SendData(UInt8 *buff, UOSInt buffSize);

		RTSPClient(const RTSPClient *cli);
	public:
		RTSPClient(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Data::Duration timeout);
		~RTSPClient();


		Bool GetOptions(Text::CString url, Data::ArrayList<const UTF8Char *> *options);
		Net::SDPFile *GetMediaInfo(Text::CString url);

		static IO::ParsedObject *ParseURL(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN url, Data::Duration timeout, NotNullPtr<IO::LogTool> log);

	private:
		UTF8Char *SetupRTP(UTF8Char *sessIdOut, Text::CString url, Net::RTPCliChannel *rtpChannel);

		Bool Play(Text::CString url, Text::CString sessId);
		Bool Close(Text::CString url, Text::CString sessId);

	public:
		virtual Bool Init(Net::RTPCliChannel *rtpChannel);
		virtual Bool Play(Net::RTPCliChannel *rtpChannel);
		virtual Bool KeepAlive(Net::RTPCliChannel *rtpChannel);
		virtual Bool StopPlay(Net::RTPCliChannel *rtpChannel);
		virtual Bool Deinit(Net::RTPCliChannel *rtpChannel);

		virtual Net::IRTPController *Clone() const;
	};
};
#endif
