#ifndef _SM_NET_TCPPORTSCANNER
#define _SM_NET_TCPPORTSCANNER
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class TCPPortScanner
	{
	private:
		typedef void (__stdcall *PortUpdatedHandler)(void *userObj, UInt16 port);
	private:
		Net::SocketFactory *sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt8 *portList;
		Sync::Mutex *portMut;
		PortUpdatedHandler hdlr;
		void *hdlrObj;
		OSInt threadCnt;
		Bool threadToStop;
		Sync::Event *threadEvt;

		static UInt32 __stdcall ScanThread(void *userObj);
	public:
		TCPPortScanner(Net::SocketFactory *socf, OSInt threadCnt, PortUpdatedHandler hdlr, void *userObj);
		~TCPPortScanner();

		void Start(Net::SocketUtil::AddressInfo *addr, UInt16 maxPort);
		Bool IsFinished();
		OSInt GetAvailablePorts(Data::ArrayList<UInt16> *portList);
	};
};
#endif
