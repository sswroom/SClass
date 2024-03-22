#ifndef _SM_NET_TCPPORTSCANNER
#define _SM_NET_TCPPORTSCANNER
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Net/SocketFactory.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class TCPPortScanner
	{
	private:
		typedef void (__stdcall *PortUpdatedHandler)(AnyType userObj, UInt16 port);
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt8 *portList;
		Sync::Mutex portMut;
		Data::CallbackStorage<PortUpdatedHandler> hdlr;
		UOSInt threadCnt;
		Bool threadToStop;
		Sync::Event threadEvt;

		static UInt32 __stdcall ScanThread(AnyType userObj);
	public:
		TCPPortScanner(NotNullPtr<Net::SocketFactory> socf, UOSInt threadCnt, PortUpdatedHandler hdlr, AnyType userObj);
		~TCPPortScanner();

		void Start(Net::SocketUtil::AddressInfo *addr, UInt16 maxPort);
		Bool IsFinished();
		UOSInt GetAvailablePorts(Data::ArrayList<UInt16> *portList);
	};
}
#endif
