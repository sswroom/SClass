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
		typedef void (CALLBACKFUNC PortUpdatedHandler)(AnyType userObj, UInt16 port);
	private:
		NN<Net::SocketFactory> sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt8 *portList;
		Sync::Mutex portMut;
		Data::CallbackStorage<PortUpdatedHandler> hdlr;
		UIntOS threadCnt;
		Bool threadToStop;
		Sync::Event threadEvt;

		static UInt32 __stdcall ScanThread(AnyType userObj);
	public:
		TCPPortScanner(NN<Net::SocketFactory> socf, UIntOS threadCnt, PortUpdatedHandler hdlr, AnyType userObj);
		~TCPPortScanner();

		void Start(Net::SocketUtil::AddressInfo *addr, UInt16 maxPort);
		Bool IsFinished();
		UIntOS GetAvailablePorts(NN<Data::ArrayListNative<UInt16>> portList);
	};
}
#endif
