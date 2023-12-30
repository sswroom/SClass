#ifndef _SM_NET_PACKETANALYZERBLUETOOTH
#define _SM_NET_PACKETANALYZERBLUETOOTH
#include "IO/DeviceDB.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class PacketAnalyzerBluetooth
	{
	public:
		static IO::DeviceDB::VendorInfo vendorList[];
	public:
		static void AddDirection(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 dir);
		static void AddHCIPacketType(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 packetType);
		static void AddParamLen(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len);
		static void AddCmdOpcode(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 cmd);
		static void AddBDAddr(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, const UInt8 *mac, Bool randomAddr);
		static void AddScanType(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType);
		static void AddScanFilterPolicy(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType);
		static void AddScanInterval(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanInt);
		static void AddScanWindow(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanWind);
		static void AddAddrType(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 addrType);
		static void AddHCIEvent(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evt);
		static void AddLESubEvent(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 subEvt);
		static void AddAdvEvtType(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evtType);
		static void AddRSSI(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 rssi);
		static void AddAdvData(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize);
		static void AddLAP(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 lap);
		static void AddInquiryLen(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len);
		static void AddPageScanMode(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 mode);
		static void AddClassOfDevice(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 cls);
		static void AddClockOffset(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 ofst);
		static void AddExAdvEvtType(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 evtType);
		static void AddPHYType(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 phyType);
		static void AddAdvSID(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 sid);
		static void AddTxPower(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 txPower);
		static void AddPeriodicAdv(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 interval);
		static void AddUnknown(NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize);

		static Bool PacketGetName(const UInt8 *packet, UOSInt packetSize, NotNullPtr<Text::StringBuilderUTF8> sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, NotNullPtr<Text::StringBuilderUTF8> sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, NotNullPtr<IO::FileAnalyse::FrameDetailHandler> frame);

		static Text::CString CompanyGetName(UInt16 company);
		static Text::CString CmdGetName(UInt16 cmd);
		static Text::CString HCIPacketTypeGetName(UInt8 packetType);
		static Text::CString HCIEventGetName(UInt8 evt);
		static Text::CString LESubEventGetName(UInt8 subEvt);
	};
}
#endif
