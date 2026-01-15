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
			UIntOS len;
			const UInt8 oid[64];
		} OIDInfo;

	private:
		static OIDInfo oidList[];

	public:
		static void OIDToNameString(Data::ByteArrayR pdu, NN<Text::StringBuilderUTF8> sb);
		static Optional<const OIDInfo> OIDGetEntry(Data::ByteArrayR pdu);
	};
}
#endif
