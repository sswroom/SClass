#ifndef _SM_NET_SNMPINFO
#define _SM_NET_SNMPINFO
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class SNMPInfo
	{
	public:
		SNMPInfo();
		~SNMPInfo();

		void PDUSeqGetDetail(const UInt8 *pdu, UOSInt pduSize, UOSInt level, Text::StringBuilderUTF *sb);
		UOSInt PDUGetDetail(const UTF8Char *name, const UInt8 *pdu, UOSInt pduSize, UOSInt level, Text::StringBuilderUTF *sb);

		static void ValueToString(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, Text::StringBuilderUTF *sb);
	};
}
#endif
