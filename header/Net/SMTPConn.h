#ifndef _SM_NET_SMTPCONN
#define _SM_NET_SMTPCONN
#include "Data/DateTime.h"
#include "IO/IWriter.h"
#include "IO/Stream.h"
#include "IO/StreamWriter.h"
#include "Net/TCPClient.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class SMTPConn
	{
	private:
		Net::TCPClient *cli;
		IO::StreamWriter *writer;
		Bool threadToStop;
		Bool threadRunning;
		Bool threadStarted;
		Int32 codePage;
		Bool logged;
		Bool statusChg;
		Int32 lastStatus;
		WChar *msgRet;
		Sync::Event *evt;
		IO::IWriter *logWriter;
		Int32 initCode;

		static UInt32 __stdcall SMTPThread(void *userObj);
		Int32 WaitForResult();
	public:
		SMTPConn(const WChar *host, UInt16 port, Net::SocketFactory *sockf, Int32 codePage, IO::IWriter *logWriter);
		~SMTPConn();

		Bool IsError();

		Bool SendHelo(const WChar *cliName);
		Bool SendEHlo(const WChar *cliName);
		Bool SendMailFrom(const WChar *fromEmail);
		Bool SendRcptTo(const WChar *toEmail);
		Bool SendQuit();
	};
};
#endif
