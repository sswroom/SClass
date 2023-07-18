#ifndef _SM_NET_MODBUSTCPLISTENER
#define _SM_NET_MODBUSTCPLISTENER
#include "Data/FastMap.h"
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
		NotNullPtr<Net::SocketFactory> sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		UInt32 delay;
		Sync::Mutex devMut;
		Data::FastMap<UInt32, IO::MODBUSDevSim*> devMap;

		static void __stdcall OnClientConn(Socket *s, void *userObj);
		static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);
	public:
		MODBUSTCPListener(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, IO::LogTool *log, Bool autoStart);
		virtual ~MODBUSTCPListener();

		Bool Start();
		Bool IsError();

		void AddDevice(UInt8 addr, IO::MODBUSDevSim *dev);
		UOSInt GetDeviceCount();
		IO::MODBUSDevSim *GetDevice(UOSInt index);
		UInt32 GetDeviceAddr(UOSInt index);
		UInt32 GetDelay();
		void SetDelay(UInt32 delay);
	};
}
#endif
