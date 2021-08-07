#ifndef _SM_NET_PACKETANALYZERBLUETOOTH
#define _SM_NET_PACKETANALYZERBLUETOOTH
#include "IO/DeviceDB.h"
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class PacketAnalyzerBluetooth
	{
	public:
		static IO::DeviceDB::VendorInfo vendorList[];
	public:
		static void AppendBool(Text::StringBuilderUTF *sb, const Char *name, UInt8 v);
		static void AppendUInt(Text::StringBuilderUTF *sb, const Char *name, UOSInt v);
		static void AppendHex8(Text::StringBuilderUTF *sb, const Char *name, UInt8 v);
		static void AppendHex16(Text::StringBuilderUTF *sb, const Char *name, UInt16 v);
		static void AppendDirection(Text::StringBuilderUTF *sb, UInt32 dir);
		static void AppendHCIPacketType(Text::StringBuilderUTF *sb, UInt8 packetType);
		static void AppendParamLen(Text::StringBuilderUTF *sb, UInt8 len);
		static void AppendCmdOpcode(Text::StringBuilderUTF *sb, UInt16 cmd);
		static void AppendBDAddr(Text::StringBuilderUTF *sb, const Char *name, const UInt8 *mac, Bool randomAddr);
		static void AppendScanType(Text::StringBuilderUTF *sb, UInt8 scanType);
		static void AppendScanFilterPolicy(Text::StringBuilderUTF *sb, UInt8 scanType);
		static void AppendScanInterval(Text::StringBuilderUTF *sb, UInt16 scanInt);
		static void AppendScanWindow(Text::StringBuilderUTF *sb, UInt16 scanWind);
		static void AppendAddrType(Text::StringBuilderUTF *sb, const Char *name, UInt8 addrType);
		static void AppendHCIEvent(Text::StringBuilderUTF *sb, UInt8 evt);
		static void AppendLESubEvent(Text::StringBuilderUTF *sb, UInt8 subEvt);
		static void AppendAdvEvtType(Text::StringBuilderUTF *sb, UInt8 evtType);
		static void AppendRSSI(Text::StringBuilderUTF *sb, Int8 rssi);
		static void AppendAdvData(Text::StringBuilderUTF *sb, const UInt8 *packet, UOSInt packetSize);
		static void AppendLAP(Text::StringBuilderUTF *sb, UInt32 lap);
		static void AppendInquiryLen(Text::StringBuilderUTF *sb, UInt8 len);
		static void AppendPageScanMode(Text::StringBuilderUTF *sb, UInt8 mode);
		static void AppendClassOfDevice(Text::StringBuilderUTF *sb, UInt32 cls);
		static void AppendClockOffset(Text::StringBuilderUTF *sb, UInt16 ofst);
		static void AppendExAdvEvtType(Text::StringBuilderUTF *sb, UInt16 evtType);
		static void AppendPHYType(Text::StringBuilderUTF *sb, const Char *name, UInt8 phyType);
		static void AppendAdvSID(Text::StringBuilderUTF *sb, UInt8 sid);
		static void AppendTxPower(Text::StringBuilderUTF *sb, Int8 txPower);
		static void AppendPeriodicAdv(Text::StringBuilderUTF *sb, UInt16 interval);
		static void AppendUnknown(Text::StringBuilderUTF *sb, const UInt8 *packet, UOSInt packetSize);

		static Bool PacketGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);

		static const UTF8Char *CompanyGetName(UInt16 company);
		static const UTF8Char *CmdGetName(UInt16 cmd);
		static const UTF8Char *HCIPacketTypeGetName(UInt8 packetType);
		static const UTF8Char *HCIEventGetName(UInt8 evt);
		static const UTF8Char *LESubEventGetName(UInt8 subEvt);
	};
}
#endif
