#ifndef _SM_NET_SNMPINFO
#define _SM_NET_SNMPINFO
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class SNMPInfo
	{
	public:
		SNMPInfo();
		~SNMPInfo();

		void PDUSeqGetDetail(Data::ByteArrayR pdu, UOSInt level, NN<Text::StringBuilderUTF8> sb);
		UOSInt PDUGetDetail(Text::CString name, Data::ByteArrayR pdu, UOSInt level, NN<Text::StringBuilderUTF8> sb);

		static void ValueToString(UInt8 type, Data::ByteArrayR pduBuff, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
