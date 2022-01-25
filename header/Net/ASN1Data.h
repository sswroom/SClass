#ifndef _SM_NET_ASN1DATA
#define _SM_NET_ASN1DATA
#include "IO/ParsedObject.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class ASN1Data : public IO::ParsedObject
	{
	public:
		enum class ASN1Type
		{
			X509
		};
	protected:
		UInt8 *buff;
		UOSInt buffSize;

		ASN1Data(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
		ASN1Data(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
	public:
		virtual ~ASN1Data();

		virtual IO::ParserType GetParserType();
		virtual ASN1Type GetASN1Type() = 0;
		virtual ASN1Data *Clone() = 0;
		virtual void ToString(Text::StringBuilderUTF8 *sb) = 0;

		Bool ToASN1String(Text::StringBuilderUTF8 *sb);
		const UInt8 *GetASN1Buff();
		UOSInt GetASN1BuffSize();

		static void AppendInteger(Text::StringBuilderUTF8 *sb, const UInt8 *pdu, UOSInt len);
	};
}
#endif
