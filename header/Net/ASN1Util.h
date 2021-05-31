#ifndef _SM_NET_ASN1UTIL
#define _SM_NET_ASN1UTIL
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class ASN1Util
	{
	public:
		static UOSInt PDUParseLen(const UInt8 *pdu, UOSInt ofst, UOSInt pduSize, UInt32 *len); //return pduSize + 1 on error

		static const UInt8 *PDUParseSeq(const UInt8 *pdu, const UInt8 *pduEnd, UInt8 *type, const UInt8 **seqEnd);
		static const UInt8 *PDUParseUInt32(const UInt8 *pdu, const UInt8 *pduEnd, UInt32 *val);
		static const UInt8 *PDUParseString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb);
		static const UInt8 *PDUParseChoice(const UInt8 *pdu, const UInt8 *pduEnd, UInt32 *val);
	};
}
#endif
