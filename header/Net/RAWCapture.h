#ifndef _SM_NET_RAWCAPTURE
#define _SM_NET_RAWCAPTURE
#include "IO/FileSelector.h"
#include "IO/PacketLogWriter.h"
#include "IO/Stream.h"
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
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SocketMonitor *socMon;
		IO::PacketLogWriter *writer;
		Sync::Mutex mut;
		UInt64 packetCnt;
		UInt64 dataSize;
	
		static void __stdcall DataHandler(void *userData, const UInt8 *packetData, UOSInt packetSize);
	public:
		RAWCapture(NotNullPtr<Net::SocketFactory> sockf, UInt32 adapterIP, CaptureType type, FileFormat format, Text::CStringNN fileName, Text::CString appName);
		~RAWCapture();

		Bool IsError();
		UInt64 GetPacketCnt();
		UInt64 GetDataSize();

		static Text::CStringNN CaptureTypeGetName(CaptureType type);
		static Int32 CaptureTypeGetLinkType(CaptureType type);
		static Text::CStringNN FileFormatGetName(FileFormat format);
		static Text::CStringNN FileFormatGetExt(FileFormat format);
		static void AddFilters(IO::FileSelector *selector);
	};
}
#endif
