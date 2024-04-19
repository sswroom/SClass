#ifndef _SM_NET_ASN1UTIL
#define _SM_NET_ASN1UTIL
#include "Data/ArrayList.h"
#include "IO/FileAnalyse/FrameDetail.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class ASN1Names;
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
			IT_ENUMERATED = 0x0a,
			IT_UTF8STRING = 0x0c,
			IT_NUMERICSTRING = 0x12,
			IT_PRINTABLESTRING = 0x13,
			IT_T61STRING = 0x14,
			IT_VIDEOTEXSTRING = 0x15,
			IT_IA5STRING = 0x16,
			IT_UTCTIME = 0x17,
			IT_GENERALIZEDTIME = 0x18,
			IT_UNIVERSALSTRING = 0x1c,
			IT_BMPSTRING = 0x1e,
			IT_SEQUENCE = 0x30,
			IT_SET = 0x31,
			IT_CHOICE_0 = 0x80,
			IT_CHOICE_1 = 0x81,
			IT_CHOICE_2 = 0x82,
			IT_CHOICE_3 = 0x83,
			IT_CHOICE_4 = 0x84,
			IT_CHOICE_5 = 0x85,
			IT_CHOICE_6 = 0x86,
			IT_CHOICE_7 = 0x87,
			IT_CHOICE_8 = 0x88,
			IT_CONTEXT_SPECIFIC_0 = 0xa0,
			IT_CONTEXT_SPECIFIC_1 = 0xa1,
			IT_CONTEXT_SPECIFIC_2 = 0xa2,
			IT_CONTEXT_SPECIFIC_3 = 0xa3,
			IT_CONTEXT_SPECIFIC_4 = 0xa4
		} ItemType;
	public:
		static UOSInt PDUParseLen(const UInt8 *pdu, UOSInt ofst, UOSInt pduSize, UInt32 *len); //return pduSize + 1 on error

		static const UInt8 *PDUParseSeq(const UInt8 *pdu, const UInt8 *pduEnd, UInt8 *type, const UInt8 **seqEnd);
		static const UInt8 *PDUParseUInt32(const UInt8 *pdu, const UInt8 *pduEnd, OutParam<UInt32> val);
		static const UInt8 *PDUParseString(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb);
		static const UInt8 *PDUParseChoice(const UInt8 *pdu, const UInt8 *pduEnd, OutParam<UInt32> val);

		static Bool PDUParseUTCTimeCont(const UInt8 *pdu, UOSInt len, NotNullPtr<Data::DateTime> dt);

		static Bool PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt level);
		static Bool PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt level, const UInt8 **pduNext);
		static Bool PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt level, const UInt8 **pduNext, Optional<Net::ASN1Names> names);

		static Bool PDUDSizeEnd(const UInt8 *pdu, const UInt8 *pduEnd, const UInt8 **pduNext);
		static const UInt8 *PDUGetItemRAW(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, OptOut<UOSInt> len, OutParam<UOSInt> itemOfst);
		static const UInt8 *PDUGetItem(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, OptOut<UOSInt> len, OptOut<ItemType> itemType);
		static ItemType PDUGetItemType(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path);
		static UOSInt PDUCountItem(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path);
		static Bool PDUIsValid(const UInt8 *pdu, const UInt8 *pduEnd);
		static void PDUAnalyse(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR buff, UOSInt pduOfst, UOSInt pduEndOfst, Optional<Net::ASN1Names> names);

		static OSInt OIDCompare(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static Bool OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
		static Bool OIDEqualsText(const UInt8 *oidPDU, UOSInt oidPDULen, const UTF8Char *oidText, UOSInt oidTextLen);
		static void OIDToString(const UInt8 *pdu, UOSInt pduSize, NotNullPtr<Text::StringBuilderUTF8> sb);
		static UOSInt OIDCalcPDUSize(const UTF8Char *oidText, UOSInt oidTextLen);
		static UOSInt OIDText2PDU(const UTF8Char *oidText, UOSInt oidTextLen, UInt8 *pduBuff);

		static void OIDToCPPCode(const UInt8 *oid, UOSInt oidLen, const UTF8Char *objectName, UOSInt nameLen, NotNullPtr<Text::StringBuilderUTF8> sb);

		static void BooleanToString(const UInt8 *data, UOSInt dataLen, NotNullPtr<Text::StringBuilderUTF8> sb);
		static void IntegerToString(const UInt8 *data, UOSInt dataLen, NotNullPtr<Text::StringBuilderUTF8> sb);
		static void UTCTimeToString(const UInt8 *data, UOSInt dataLen, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Text::CStringNN ItemTypeGetName(UInt8 itemType);
	private:
		static UInt32 Str2Digit(const UTF8Char *s);
	};
}
#endif
