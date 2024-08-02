#ifndef _SM_NET_FTPCONN
#define _SM_NET_FTPCONN
#include "AnyType.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/StreamWriter.h"
#include "Net/TCPClient.h"
#include "Net/TCPClientFactory.h"

namespace Net
{
	class FTPConn
	{
	private:
		NN<Net::TCPClient> cli;
		NN<IO::StreamWriter> writer;
		Bool threadToStop;
		Bool threadRunning;
		UInt32 codePage;
		Bool logged;
		Bool statusChg;
		Int32 lastStatus;
		UnsafeArrayOpt<UTF8Char> msgRet;
		Sync::Event evt;

		static UInt32 __stdcall FTPThread(AnyType userObj);
		Int32 WaitForResult();
	public:
		FTPConn(Text::CStringNN host, UInt16 port, NN<Net::TCPClientFactory> clif, UInt32 codePage, Data::Duration timeout);
		~FTPConn();

		Bool IsLogged();

		Bool SendUser(UnsafeArray<const UTF8Char> userName);
		Bool SendPassword(UnsafeArray<const UTF8Char> password);
		Bool ChangeDirectory(UnsafeArray<const UTF8Char> dir);
		Bool MakeDirectory(UnsafeArray<const UTF8Char> dir);
		Bool RemoveDirectory(UnsafeArray<const UTF8Char> dir);
		Bool GetFileSize(UnsafeArray<const UTF8Char> fileName, OutParam<UInt64> fileSize);
		Bool GetFileModTime(UnsafeArray<const UTF8Char> fileName, NN<Data::DateTime> modTime);
		Bool ToBinaryType();
		Bool ToASCIIType();
		Bool ToEBCDICType();
		Bool ChangePassiveMode(OutParam<UInt32> ip, OutParam<UInt16> port);
		Bool ChangeActiveMode(UInt32 ip, UInt16 port);
		Bool ResumeTransferPos(UInt64 pos);
		Bool GetFile(UnsafeArray<const UTF8Char> fileName);
		Bool RenameFile(UnsafeArray<const UTF8Char> fromFile, UnsafeArray<const UTF8Char> toFile);
	};
}
#endif
