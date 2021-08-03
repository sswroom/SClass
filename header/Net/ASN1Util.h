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

		static Bool PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, UOSInt level);

		static OSInt OIDCompare(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static Bool OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static void OIDToString(const UInt8 *pdu, UOSInt pduSize, Text::StringBuilderUTF *sb);
		static UOSInt OIDCalcPDUSize(const UTF8Char *oid);
		static UOSInt OIDText2PDU(const Char *oid, UInt8 *pduBuff);
		static UOSInt OIDUText2PDU(const UTF8Char *oid, UInt8 *pduBuff);

		static void OIDToCPPCode(const UInt8 *oid, UOSInt oidLen, const UTF8Char *objectName, Text::StringBuilderUTF *sb);
	private:
		static UInt32 Str2Digit(const UTF8Char *s);
	};
}
#endif
