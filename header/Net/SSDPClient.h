#ifndef _SM_NET_SSDPCLIENT
#define _SM_NET_SSDPCLIENT
#include "Data/FastMap.h"
#include "Net/UDPServer.h"
#include "Sync/MutexUsage.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"

namespace Net
{
	class SSDPClient
	{
	public:
		struct SSDPService
		{
			Int64 time;
			Optional<Text::String> location;
			Optional<Text::String> opt;
			Optional<Text::String> server;
			Optional<Text::String> st;
			NotNullPtr<Text::String> usn;
			Optional<Text::String> userAgent;
		};

		struct SSDPDevice
		{
			Net::SocketUtil::AddressInfo addr;
			Data::ArrayList<SSDPService*> services;
		};

		struct SSDPRoot
		{
			Text::String *udn;
			Text::String *friendlyName;
			Text::String *manufacturer;
			Text::String *manufacturerURL;
			Text::String *modelName;
			Text::String *modelNumber;
			Text::String *modelURL;
			Text::String *serialNumber;
			Text::String *presentationURL;
			Text::String *deviceType;
			Text::String *deviceURL;
		};
	private:
		Net::UDPServer *udp;
		Optional<Text::String> userAgent;
		Sync::Mutex mut;
		Data::FastMap<UInt32, SSDPDevice *> devMap;

		static void __stdcall OnPacketRecv(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);

		void SSDPServiceFree(SSDPService *svc);
		void SSDPDeviceFree(SSDPDevice *dev);
	public:
		SSDPClient(NotNullPtr<Net::SocketFactory> sockf, Text::CString userAgent, NotNullPtr<IO::LogTool> log);
		~SSDPClient();

		Bool IsError() const;
		Bool Scan();
		const Data::ReadingList<SSDPDevice*> *GetDevices(NotNullPtr<Sync::MutexUsage> mutUsage) const;

		static SSDPRoot *SSDPRootParse(Text::EncodingFactory *encFact, NotNullPtr<IO::Stream> stm);
		static void SSDPRootFree(SSDPRoot *root);
	};
}
#endif
