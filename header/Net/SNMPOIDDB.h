#ifndef _SM_NET_SNMPOIDDB
#define _SM_NET_SNMPOIDDB
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class SNMPOIDDB
	{
	public:
		typedef struct
		{
			const Char *name;
			OSInt len;
			const UInt8 oid[64];
		} OIDInfo;

	private:
		static OIDInfo oidList[];

	public:
		static void OIDToNameString(const UInt8 *pdu, OSInt pduSize, Text::StringBuilderUTF *sb);
		static OIDInfo *OIDGetEntry(const UInt8 *pdu, OSInt pduSize);
	};
}
#endif
