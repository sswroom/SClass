#ifndef _SM_NET_RAWCAPTURE
#define _SM_NET_RAWCAPTURE
#include "IO/Stream.h"
#include "IO/IFileSelector.h"
#include "Net/SocketMonitor.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Net
{
	class RAWCapture
	{
	public:
		typedef enum
		{
			CT_RAW,
			CT_IPV4,
			CT_UDPV4,
			CT_ICMPV4,

			CT_FIRST = CT_RAW,
			CT_LAST = CT_ICMPV4
		} CaptureType;

		typedef enum
		{
			FF_PCAP,
			FF_PCAPNG,

			FF_FIRST = FF_PCAP,
			FF_LAST = FF_PCAPNG
		} FileFormat;
	private:
		Net::SocketFactory *sockf;
		Net::SocketMonitor *socMon;
		IO::Stream *fs;
		FileFormat format;
		Sync::Mutex *mut;
		UInt64 packetCnt;
		UInt64 dataSize;
	
		static void __stdcall DataHandler(void *userData, const UInt8 *packetData, UOSInt packetSize);
	public:
		RAWCapture(Net::SocketFactory *sockf, UInt32 adapterIP, CaptureType type, FileFormat format, const UTF8Char *fileName, const UTF8Char *appName);
		~RAWCapture();

		Bool IsError();
		UInt64 GetPacketCnt();
		UInt64 GetDataSize();

		static Text::CString CaptureTypeGetName(CaptureType type);
		static Int32 CaptureTypeGetLinkType(CaptureType type);
		static Text::CString FileFormatGetName(FileFormat format);
		static Text::CString FileFormatGetExt(FileFormat format);
		static void AddFilters(IO::IFileSelector *selector);
	};
}
#endif
