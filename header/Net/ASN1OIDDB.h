#ifndef _SM_NET_ASN1OIDDB
#define _SM_NET_ASN1OIDDB
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class ASN1OIDDB
	{
	public:
		typedef struct
		{
			const Char *name;
			UOSInt len;
			const UInt8 oid[64];
		} OIDInfo;

	private:
		static OIDInfo oidList[];

	public:
		static void OIDToNameString(const UInt8 *pdu, UOSInt pduSize, NN<Text::StringBuilderUTF8> sb);
		static const OIDInfo *OIDGetEntry(const UInt8 *pdu, UOSInt pduSize);
	};
}
#endif
