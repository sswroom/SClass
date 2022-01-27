#ifndef _SM_NET_SSDPCLIENT
#define _SM_NET_SSDPCLIENT
#include "Data/UInt32Map.h"
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
			const UTF8Char *location;
			const UTF8Char *opt;
			const UTF8Char *server;
			const UTF8Char *st;
			const UTF8Char *usn;
			const UTF8Char *userAgent;
		};

		struct SSDPDevice
		{
			Net::SocketUtil::AddressInfo addr;
			Data::ArrayList<SSDPService*> *services;
		};

		struct SSDPRoot
		{
			const UTF8Char *udn;
			const UTF8Char *friendlyName;
			const UTF8Char *manufacturer;
			const UTF8Char *manufacturerURL;
			const UTF8Char *modelName;
			const UTF8Char *modelNumber;
			const UTF8Char *modelURL;
			const UTF8Char *serialNumber;
			const UTF8Char *presentationURL;
			const UTF8Char *deviceType;
			const UTF8Char *deviceURL;
		};
	private:
		Net::UDPServer *udp;
		Text::String *userAgent;
		Sync::Mutex *mut;
		Data::UInt32Map<SSDPDevice *> *devMap;

		static void __stdcall OnPacketRecv(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);

		void SSDPServiceFree(SSDPService *svc);
		void SSDPDeviceFree(SSDPDevice *dev);
	public:
		SSDPClient(Net::SocketFactory *sockf, const UTF8Char *userAgent);
		~SSDPClient();

		Bool IsError();
		Bool Scan();
		Data::ArrayList<SSDPDevice*> *GetDevices(Sync::MutexUsage *mutUsage);

		static SSDPRoot *SSDPRootParse(Text::EncodingFactory *encFact, IO::Stream *stm);
		static void SSDPRootFree(SSDPRoot *root);
	};
}
#endif
