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

		void PDUSeqGetDetail(const UInt8 *pdu, UOSInt pduSize, UOSInt level, Text::StringBuilderUTF8 *sb);
		UOSInt PDUGetDetail(Text::CString name, const UInt8 *pdu, UOSInt pduSize, UOSInt level, Text::StringBuilderUTF8 *sb);

		static void ValueToString(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, Text::StringBuilderUTF8 *sb);
	};
}
#endif
