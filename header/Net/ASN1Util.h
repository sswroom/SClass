#ifndef _SM_NET_ASN1UTIL
#define _SM_NET_ASN1UTIL
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
		static UIntOS PDUParseLen(UnsafeArray<const UInt8> pdu, UIntOS ofst, UIntOS pduSize, OutParam<UInt32> len); //return pduSize + 1 on error

		static UnsafeArrayOpt<const UInt8> PDUParseSeq(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, OutParam<UInt8> type, OutParam<UnsafeArray<const UInt8>> seqEnd);
		static UnsafeArrayOpt<const UInt8> PDUParseUInt32(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, OutParam<UInt32> val);
		static UnsafeArrayOpt<const UInt8> PDUParseString(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb);
		static UnsafeArrayOpt<const UInt8> PDUParseChoice(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, OutParam<UInt32> val);

		static Bool PDUParseUTCTimeCont(Data::ByteArrayR pdu, NN<Data::DateTime> dt);

		static Bool PDUToString(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, UIntOS level);
		static Bool PDUToString(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, UIntOS level, OptOut<UnsafeArray<const UInt8>> pduNext);
		static Bool PDUToString(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, UIntOS level, OptOut<UnsafeArray<const UInt8>> pduNext, Optional<Net::ASN1Names> names);

		static Bool PDUDSizeEnd(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, OutParam<UnsafeArray<const UInt8>> pduNext);
		static UnsafeArrayOpt<const UInt8> PDUGetItemRAW(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, OptOut<UIntOS> len, OutParam<UIntOS> itemOfst);
		static UnsafeArrayOpt<const UInt8> PDUGetItem(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, OptOut<UIntOS> len, OptOut<ItemType> itemType);
		static ItemType PDUGetItemType(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path);
		static UIntOS PDUCountItem(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path);
		static Bool PDUIsValid(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd);
		static void PDUAnalyse(NN<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR buff, UIntOS pduOfst, UIntOS pduEndOfst, Optional<Net::ASN1Names> names);

		static IntOS OIDCompare(Data::ByteArrayR oid1, Data::ByteArrayR oid2);
		static Bool OIDStartsWith(UnsafeArray<const UInt8> oid1, UIntOS oid1Len, UnsafeArray<const UInt8> oid2, UIntOS oid2Len);
		static Bool OIDEqualsText(Data::ByteArrayR oidPDU, Text::CStringNN oidText);
		static void OIDToString(Data::ByteArrayR pdu, NN<Text::StringBuilderUTF8> sb);
		static UIntOS OIDCalcPDUSize(Text::CStringNN oidText);
		static UIntOS OIDText2PDU(Text::CStringNN oidText, UnsafeArray<UInt8> pduBuff);

		static void OIDToCPPCode(Data::ByteArrayR oid, Text::CStringNN objectName, NN<Text::StringBuilderUTF8> sb);

		static void BooleanToString(Data::ByteArrayR data, NN<Text::StringBuilderUTF8> sb);
		static void IntegerToString(Data::ByteArrayR data, NN<Text::StringBuilderUTF8> sb);
		static void UTCTimeToString(Data::ByteArrayR data, NN<Text::StringBuilderUTF8> sb);
		static Text::CStringNN ItemTypeGetName(UInt8 itemType);
	private:
		static UInt32 Str2Digit(UnsafeArray<const UTF8Char> s);
	};
}
#endif
