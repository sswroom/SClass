#ifndef _SM_NET_PACKETANALYZERBLUETOOTH
#define _SM_NET_PACKETANALYZERBLUETOOTH
#include "IO/DeviceDB.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class PacketAnalyzerBluetooth
	{
	public:
		static IO::DeviceDB::VendorInfo vendorList[];
	public:
		static void AddDirection(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 dir);
		static void AddHCIPacketType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 packetType);
		static void AddParamLen(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 len);
		static void AddCmdOpcode(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 cmd);
		static void AddBDAddr(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, const UInt8 *mac, Bool randomAddr);
		static void AddScanType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 scanType);
		static void AddScanFilterPolicy(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 scanType);
		static void AddScanInterval(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 scanInt);
		static void AddScanWindow(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 scanWind);
		static void AddAddrType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, UInt8 addrType);
		static void AddHCIEvent(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 evt);
		static void AddLESubEvent(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 subEvt);
		static void AddAdvEvtType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 evtType);
		static void AddRSSI(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, Int8 rssi);
		static void AddAdvData(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize);
		static void AddLAP(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 lap);
		static void AddInquiryLen(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 len);
		static void AddPageScanMode(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 mode);
		static void AddClassOfDevice(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 cls);
		static void AddClockOffset(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 ofst);
		static void AddExAdvEvtType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 evtType);
		static void AddPHYType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, UInt8 phyType);
		static void AddAdvSID(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 sid);
		static void AddTxPower(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, Int8 txPower);
		static void AddPeriodicAdv(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 interval);
		static void AddUnknown(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize);

		static Bool PacketGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);

		static const UTF8Char *CompanyGetName(UInt16 company);
		static const UTF8Char *CmdGetName(UInt16 cmd);
		static const UTF8Char *HCIPacketTypeGetName(UInt8 packetType);
		static const UTF8Char *HCIEventGetName(UInt8 evt);
		static const UTF8Char *LESubEventGetName(UInt8 subEvt);
	};
}
#endif
