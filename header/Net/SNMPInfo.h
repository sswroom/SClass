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

		void PDUSeqGetDetail(const UInt8 *pdu, OSInt pduSize, OSInt level, Text::StringBuilderUTF *sb);
		OSInt PDUGetDetail(const UTF8Char *name, const UInt8 *pdu, OSInt pduSize, OSInt level, Text::StringBuilderUTF *sb);

		static void ValueToString(UInt8 type, const UInt8 *pduBuff, OSInt valLen, Text::StringBuilderUTF *sb);
	};
}
#endif
