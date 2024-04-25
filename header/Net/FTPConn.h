#ifndef _SM_NET_FTPCONN
#define _SM_NET_FTPCONN
#include "AnyType.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/StreamWriter.h"
#include "Net/TCPClient.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class FTPConn
	{
	private:
		Net::TCPClient cli;
		IO::StreamWriter *writer;
		Bool threadToStop;
		Bool threadRunning;
		UInt32 codePage;
		Bool logged;
		Bool statusChg;
		Int32 lastStatus;
		UTF8Char *msgRet;
		Sync::Event *evt;

		static UInt32 __stdcall FTPThread(AnyType userObj);
		Int32 WaitForResult();
	public:
		FTPConn(Text::CStringNN host, UInt16 port, NN<Net::SocketFactory> sockf, UInt32 codePage, Data::Duration timeout);
		~FTPConn();

		Bool IsLogged();

		Bool SendUser(const UTF8Char *userName);
		Bool SendPassword(const UTF8Char *password);
		Bool ChangeDirectory(const UTF8Char *dir);
		Bool MakeDirectory(const UTF8Char *dir);
		Bool RemoveDirectory(const UTF8Char *dir);
		Bool GetFileSize(const UTF8Char *fileName, UInt64 *fileSize);
		Bool GetFileModTime(const UTF8Char *fileName, Data::DateTime *modTime);
		Bool ToBinaryType();
		Bool ToASCIIType();
		Bool ToEBCDICType();
		Bool ChangePassiveMode(UInt32 *ip, UInt16 *port);
		Bool ChangeActiveMode(UInt32 ip, UInt16 port);
		Bool ResumeTransferPos(UInt64 pos);
		Bool GetFile(const UTF8Char *fileName);
		Bool RenameFile(const UTF8Char *fromFile, const UTF8Char *toFile);
	};
}
#endif
