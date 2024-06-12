#ifndef _SM_NET_LORAGWUTIL
#define _SM_NET_LORAGWUTIL
#include "Data/Timestamp.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class LoRaGWUtil
	{
	public:
		static void ParseGWMPMessage(NN<Text::StringBuilderUTF8> sb, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, Data::ByteArrayR msg);
		static void ParseUDPMessage(NN<Text::StringBuilderUTF8> sb, Bool toServer, Data::ByteArrayR msg);

		static UOSInt GenUpPayload(UInt8 *buff, Bool needConfirm, UInt32 devAddr, UInt32 fCnt, UInt8 fPort, const UInt8 *nwkSKey, const UInt8 *appSKey, const UInt8 *payload, UOSInt payloadLen);
		static void GenRxpkJSON(NN<Text::StringBuilderUTF8> sb, UInt32 freq, UInt32 chan, UInt32 rfch, UInt32 codrk, Int32 rssi, Int32 lsnr, const UInt8 *data, UOSInt dataSize);
		static void GenStatJSON(NN<Text::StringBuilderUTF8> sb, const Data::Timestamp &ts, UInt32 rxnb, UInt32 rxok, UInt32 rwfw, Double ackr, UInt32 dwnb, UInt32 txnb);
		static void GenStatJSON(NN<Text::StringBuilderUTF8> sb, const Data::Timestamp &ts, UInt32 rxnb, UInt32 rxok, UInt32 rwfw, Double ackr, UInt32 dwnb, UInt32 txnb, Double lat, Double lon, Int32 altitude);
	};
}
#endif