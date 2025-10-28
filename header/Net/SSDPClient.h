#ifndef _SM_NET_SSDPCLIENT
#define _SM_NET_SSDPCLIENT
#include "Data/FastMapNN.h"
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
			NN<Text::String> usn;
			Optional<Text::String> userAgent;
		};

		struct SSDPDevice
		{
			Net::SocketUtil::AddressInfo addr;
			Data::ArrayListNN<SSDPService> services;
		};

		struct SSDPRoot
		{
			Optional<Text::String> udn;
			Optional<Text::String> friendlyName;
			Optional<Text::String> manufacturer;
			Optional<Text::String> manufacturerURL;
			Optional<Text::String> modelName;
			Optional<Text::String> modelNumber;
			Optional<Text::String> modelURL;
			Optional<Text::String> serialNumber;
			Optional<Text::String> presentationURL;
			Optional<Text::String> deviceType;
			Optional<Text::String> deviceURL;
		};
	private:
		NN<Net::UDPServer> udp;
		Optional<Text::String> userAgent;
		Sync::Mutex mut;
		Data::FastMapNN<UInt32, SSDPDevice> devMap;

		static void __stdcall OnPacketRecv(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);

		static void __stdcall SSDPServiceFree(NN<SSDPService> svc);
		static void __stdcall SSDPDeviceFree(NN<SSDPDevice> dev);
	public:
		SSDPClient(NN<Net::SocketFactory> sockf, Text::CString userAgent, NN<IO::LogTool> log);
		~SSDPClient();

		Bool IsError() const;
		Bool Scan();
		NN<const Data::ReadingListNN<SSDPDevice>> GetDevices(NN<Sync::MutexUsage> mutUsage) const;

		static NN<SSDPRoot> SSDPRootParse(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm);
		static void __stdcall SSDPRootFree(NN<SSDPRoot> root);
	};
}
#endif
