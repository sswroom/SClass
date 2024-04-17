#ifndef _SM_IO_DEVICE_SIM7000SOCKETFACTORY
#define _SM_IO_DEVICE_SIM7000SOCKETFACTORY
#include "Data/SyncLinkedList.h"
#include "IO/Device/SIM7000.h"
#include "Net/SocketFactory.h"
#include "Net/SocketUtil.h"

namespace IO
{
	namespace Device
	{
		class SIM7000SocketFactory : public Net::SocketFactory
		{
		private:
			enum class SocketState
			{
				Empty,
				TCP_Unopened,
				TCP_Opened,
				UDP_Unopened,
				UDP_Opened
			};

			struct DataPacket
			{
				UInt32 remoteIP;
				UInt16 remotePort;
				UOSInt dataSize;
				UInt8 data[1];
			};
			

			struct SocketStatus
			{
				SocketState state;
				UInt32 udpRIP;
				UInt16 udpRPort;
				Sync::Mutex dataMut;
				Data::SyncLinkedList dataList;
				Sync::Event dataEvt;
			};
			
		private:
			IO::Device::SIM7000 *modem;
			Bool needRelease;
			Text::String *apn;
			SocketStatus status[8];

			static void __stdcall OnReceiveData(AnyType userObj, UOSInt index, UInt32 remoteIP, UInt16 remotePort, const UInt8 *buff, UOSInt buffSize);
			void CloseAllSockets();
		public:
			SIM7000SocketFactory(IO::Device::SIM7000 *modem, Bool needRelease);
			virtual ~SIM7000SocketFactory();

			void SetAPN(Text::CString apn);
			Text::String *GetAPN();
			void Init();
			Bool NetworkStart();
			Bool NetworkEnd();

			virtual Socket *CreateTCPSocketv4(); ////////////////////////
			virtual Socket *CreateTCPSocketv6(); ////////////////////////
			virtual Socket *CreateUDPSocketv4();
			virtual Socket *CreateUDPSocketv6(); ////////////////////////

			virtual Socket *CreateICMPIPv4Socket(UInt32 ip);
			virtual Socket *CreateUDPRAWv4Socket(UInt32 ip);
			virtual Socket *CreateRAWIPv4Socket(UInt32 ip);
			virtual Socket *CreateARPSocket();
			virtual Socket *CreateRAWSocket();

			virtual void DestroySocket(Socket *socket);
			virtual Bool SocketIsInvalid(Socket *socket);
			virtual Bool SocketBindv4(Socket *socket, UInt32 ip, UInt16 port);
			virtual Bool SocketBind(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port); ////////////////////////
			virtual Bool SocketBindRAWIf(Socket *socket, UOSInt ifIndex); ////////////////////////
			virtual Bool SocketListen(Socket *socket); ////////////////////////
			virtual Socket *SocketAccept(Socket *socket); ////////////////////////
			virtual Int32 SocketGetLastError(); ////////////////////////
			virtual Bool GetRemoteAddr(Socket *socket, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port); ////////////////////////
			virtual Bool GetLocalAddr(Socket *socket, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port); ////////////////////////
			virtual OSInt SocketGetFD(Socket *socket);
			virtual Bool SocketWait(Socket *socket, Data::Duration dur);

			virtual void SetDontLinger(Socket *socket, Bool val); ////////////////////////
			virtual void SetLinger(Socket *socket, UInt32 ms); ////////////////////////
			virtual void SetRecvBuffSize(Socket *socket, Int32 buffSize); ////////////////////////
			virtual void SetNoDelay(Socket *socket, Bool val);
			virtual void SetRecvTimeout(Socket *socket, Data::Duration timeout); ////////////////////////
			virtual void SetReuseAddr(Socket *socket, Bool val); ////////////////////////
			virtual void SetIPv4TTL(Socket *socket, Int32 ttl);
			virtual void SetBroadcast(Socket *socket, Bool val);
			virtual void AddIPMembership(Socket *socket, UInt32 ip); ////////////////////////

			virtual UOSInt SendData(Socket *socket, const UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et); ////////////////////////
			virtual UOSInt ReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et); ////////////////////////
			virtual void *BeginReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, OptOut<ErrorType> et); ////////////////////////
			virtual UOSInt EndReceiveData(void *reqData, Bool toWait, OutParam<Bool> incomplete); ////////////////////////
			virtual void CancelReceiveData(void *reqData); ////////////////////////

			virtual UOSInt UDPReceive(Socket *socket, UInt8 *buff, UOSInt buffSize, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et); ////////////////////////
			virtual UOSInt SendTo(Socket *socket, const UInt8 *buff, UOSInt buffSize, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port);
			virtual UOSInt SendToIF(Socket *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName);

			virtual Bool IcmpSendEcho2(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt32 *respTime_us, UInt32 *ttl);

			virtual Bool Connect(Socket *socket, UInt32 ip, UInt16 port, Data::Duration timeout); ////////////////////////
			virtual Bool Connect(Socket *socket, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout); ////////////////////////
			virtual void ShutdownSend(Socket *socket); ////////////////////////
			virtual void ShutdownSocket(Socket *socket); ////////////////////////

			virtual Bool SocketGetReadBuff(Socket *socket, UInt32 *size); ////////////////////////

			virtual Bool DNSResolveIPDef(const Char *host, NotNullPtr<Net::SocketUtil::AddressInfo> addr);
			virtual Bool GetDefDNS(NotNullPtr<Net::SocketUtil::AddressInfo> addr);
			virtual UOSInt GetDNSList(Data::ArrayList<UInt32> *dnsList);
			virtual Bool LoadHosts(Net::DNSHandler *dnsHdlr);

			virtual Bool ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4);

			virtual UOSInt GetConnInfoList(NotNullPtr<Data::ArrayListNN<Net::ConnectionInfo>> connInfoList);
			virtual Bool GetIPInfo(NotNullPtr<IPInfo> info); //////////////////////////////////
			virtual Bool GetTCPInfo(NotNullPtr<TCPInfo> info); //////////////////////////////////
			virtual Bool GetUDPInfo(NotNullPtr<UDPInfo> info); //////////////////////////////////
			virtual UOSInt QueryPortInfos(NotNullPtr<Data::ArrayListNN<PortInfo>> portInfoList, ProtocolType protoType, UInt16 procId);
			virtual void FreePortInfos(NotNullPtr<Data::ArrayListNN<PortInfo>> portInfoList);
			virtual UOSInt QueryPortInfos2(NotNullPtr<Data::ArrayListNN<PortInfo3>> portInfoList, ProtocolType protoType, UInt16 procId);
			virtual void FreePortInfos2(NotNullPtr<Data::ArrayListNN<PortInfo3>> portInfoList);
		};
	}
}
#endif
