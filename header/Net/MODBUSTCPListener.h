#ifndef _SM_NET_MODBUSTCPLISTENER
#define _SM_NET_MODBUSTCPLISTENER
#include "Data/FastMapNN.h"
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
		NN<Net::SocketFactory> sockf;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;
		UInt32 delay;
		Sync::Mutex devMut;
		Data::FastMapNN<UInt32, IO::MODBUSDevSim> devMap;

		static void __stdcall OnClientConn(NN<Socket> s, AnyType userObj);
		static void __stdcall OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
	public:
		MODBUSTCPListener(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log, Bool autoStart);
		virtual ~MODBUSTCPListener();

		Bool Start();
		Bool IsError();

		void AddDevice(UInt8 addr, NN<IO::MODBUSDevSim> dev);
		UOSInt GetDeviceCount() const;
		NN<IO::MODBUSDevSim> GetDeviceNoCheck(UOSInt index) const;
		Optional<IO::MODBUSDevSim> GetDevice(UOSInt index) const;
		UInt32 GetDeviceAddr(UOSInt index);
		UInt32 GetDelay();
		void SetDelay(UInt32 delay);
	};
}
#endif
