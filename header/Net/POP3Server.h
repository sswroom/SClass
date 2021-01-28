#ifndef _SM_NET_POP3SERVER
#define _SM_NET_POP3SERVER
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/IMailController.h"
#include "Net/SocketFactory.h"
#include "Net/TCPServer.h"
#include "Net/TCPClientMgr.h"

namespace Net
{
	class POP3Server
	{
	public:
		typedef struct
		{
			UInt8 *buff;
			OSInt buffSize;
			Int32 userId;
			const Char *cliName;
			const Char *userName;
			Data::ArrayList<const Char *> *rcptTo;
			Bool dataMode;
			IO::MemoryStream *dataStm;
		} MailStatus;

		typedef WChar *(__stdcall *MailHandler)(WChar *queryId, void *userObj, Net::TCPClient *cli, MailStatus *mail);
	private:
		Net::SocketFactory *sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		IO::LogTool *log;
		const UTF8Char *greeting;

		Net::IMailController *mailCtrl;
		IO::FileStream *rawLog;

		static void __stdcall ConnHdlr(UInt32 *s, void *userObj);
		static void __stdcall ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
		OSInt WriteMessage(Net::TCPClient *cli, Bool success, const UTF8Char *msg);
		OSInt WriteRAW(Net::TCPClient *cli, const UTF8Char *msg);
		//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
		void ParseCmd(Net::TCPClient *cli, MailStatus *cliStatus, Char *cmd);
	public:
		POP3Server(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, const UTF8Char *greeting, Net::IMailController *mailCtrl);
		~POP3Server();

		Bool IsError();
		static UInt16 GetDefaultPort();
	};
};
#endif