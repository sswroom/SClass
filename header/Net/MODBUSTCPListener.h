#ifndef _SM_NET_MODBUSTCPLISTENER
#define _SM_NET_MODBUSTCPLISTENER
#include "Data/UInt32Map.h"
#include "IO/MODBUSDevSim.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class MODBUSTCPListener
	{
	private:
		Net::SocketFactory *sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		Sync::Mutex devMut;
		Data::UInt32Map<IO::MODBUSDevSim*> devMap;

		static void __stdcall OnClientConn(Socket *s, void *userObj);
		static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
	public:
		MODBUSTCPListener(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log);
		virtual ~MODBUSTCPListener();

		Bool IsError();

		void AddDevice(UInt8 addr, IO::MODBUSDevSim *dev);
		UOSInt GetDeviceCount();
		IO::MODBUSDevSim *GetDevice(UOSInt index);
		UInt32 GetDeviceAddr(UOSInt index);
	};
}
#endif