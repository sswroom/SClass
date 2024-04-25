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
		static void AddDirection(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 dir);
		static void AddHCIPacketType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 packetType);
		static void AddParamLen(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len);
		static void AddCmdOpcode(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 cmd);
		static void AddBDAddr(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, const UInt8 *mac, Bool randomAddr);
		static void AddScanType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType);
		static void AddScanFilterPolicy(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType);
		static void AddScanInterval(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanInt);
		static void AddScanWindow(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanWind);
		static void AddAddrType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 addrType);
		static void AddHCIEvent(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evt);
		static void AddLESubEvent(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 subEvt);
		static void AddAdvEvtType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evtType);
		static void AddRSSI(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 rssi);
		static void AddAdvData(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize);
		static void AddLAP(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 lap);
		static void AddInquiryLen(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len);
		static void AddPageScanMode(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 mode);
		static void AddClassOfDevice(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 cls);
		static void AddClockOffset(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 ofst);
		static void AddExAdvEvtType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 evtType);
		static void AddPHYType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 phyType);
		static void AddAdvSID(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 sid);
		static void AddTxPower(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 txPower);
		static void AddPeriodicAdv(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 interval);
		static void AddUnknown(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize);

		static Bool PacketGetName(const UInt8 *packet, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);

		static Text::CString CompanyGetName(UInt16 company);
		static Text::CString CmdGetName(UInt16 cmd);
		static Text::CString HCIPacketTypeGetName(UInt8 packetType);
		static Text::CString HCIEventGetName(UInt8 evt);
		static Text::CString LESubEventGetName(UInt8 subEvt);
	};
}
#endif
