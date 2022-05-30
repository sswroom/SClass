#ifndef _SM_NET_ASN1UTIL
#define _SM_NET_ASN1UTIL
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class ASN1Util
	{
	public:
		typedef enum
		{
			IT_UNKNOWN = 0,
			IT_BOOLEAN = 0x01,
			IT_INTEGER = 0x02,
			IT_BIT_STRING = 0x03,
			IT_OCTET_STRING = 0x04,
			IT_NULL = 0x05,
			IT_OID = 0x06,
			IT_UTF8STRING = 0x0c,
			IT_NUMERICSTRING = 0x12,
			IT_PRINTABLESTRING = 0x13,
			IT_T61STRING = 0x14,
			IT_VIDEOTEXSTRING = 0x15,
			IT_IA5STRING = 0x16,
			IT_UTCTIME = 0x17,
			IT_UNIVERSALSTRING = 0x1c,
			IT_BMPSTRING = 0x1e,
			IT_SEQUENCE = 0x30,
			IT_SET = 0x31,
			IT_CONTEXT_SPECIFIC_0 = 0xa0,
			IT_CONTEXT_SPECIFIC_1 = 0xa1,
			IT_CONTEXT_SPECIFIC_2 = 0xa2,
			IT_CONTEXT_SPECIFIC_3 = 0xa3
		} ItemType;
	public:
		static UOSInt PDUParseLen(const UInt8 *pdu, UOSInt ofst, UOSInt pduSize, UInt32 *len); //return pduSize + 1 on error

		static const UInt8 *PDUParseSeq(const UInt8 *pdu, const UInt8 *pduEnd, UInt8 *type, const UInt8 **seqEnd);
		static const UInt8 *PDUParseUInt32(const UInt8 *pdu, const UInt8 *pduEnd, UInt32 *val);
		static const UInt8 *PDUParseString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb);
		static const UInt8 *PDUParseChoice(const UInt8 *pdu, const UInt8 *pduEnd, UInt32 *val);

		static Bool PDUParseUTCTimeCont(const UInt8 *pdu, UOSInt len, Data::DateTime *dt);

		static Bool PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, UOSInt level);
		static Bool PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, UOSInt level, const UInt8 **pduNext);

		static Bool PDUDSizeEnd(const UInt8 *pdu, const UInt8 *pduEnd, const UInt8 **pduNext);
		static const UInt8 *PDUGetItemRAW(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, UOSInt *len, UOSInt *itemOfst);
		static const UInt8 *PDUGetItem(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, UOSInt *len, ItemType *itemType);
		static ItemType PDUGetItemType(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path);
		static UOSInt PDUCountItem(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path);
		static Bool PDUIsValid(const UInt8 *pdu, const UInt8 *pduEnd);

		static OSInt OIDCompare(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static Bool OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static Bool OIDEqualsText(const UInt8 *oidPDU, UOSInt oidPDULen, const UTF8Char *oidText, UOSInt oidTextLen);
		static void OIDToString(const UInt8 *pdu, UOSInt pduSize, Text::StringBuilderUTF8 *sb);
		static UOSInt OIDCalcPDUSize(const UTF8Char *oidText, UOSInt oidTextLen);
		static UOSInt OIDText2PDU(const UTF8Char *oidText, UOSInt oidTextLen, UInt8 *pduBuff);

		static void OIDToCPPCode(const UInt8 *oid, UOSInt oidLen, const UTF8Char *objectName, UOSInt nameLen, Text::StringBuilderUTF8 *sb);

		static void BooleanToString(const UInt8 *data, UOSInt dataLen, Text::StringBuilderUTF8 *sb);
		static void IntegerToString(const UInt8 *data, UOSInt dataLen, Text::StringBuilderUTF8 *sb);
	private:
		static UInt32 Str2Digit(const UTF8Char *s);
	};
}
#endif
