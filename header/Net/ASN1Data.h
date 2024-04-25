#ifndef _SM_NET_ASN1DATA
#define _SM_NET_ASN1DATA
#include "Data/ByteBuffer.h"
#include "IO/ParsedObject.h"
#include "Net/ASN1Names.h"
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
		Data::ByteBuffer buff;

		ASN1Data(NN<Text::String> sourceName, Data::ByteArrayR buff);
		ASN1Data(Text::CStringNN sourceName, Data::ByteArrayR buff);
	public:
		virtual ~ASN1Data();

		virtual IO::ParserType GetParserType() const;
		virtual ASN1Type GetASN1Type() const = 0;
		virtual NN<ASN1Data> Clone() const = 0;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
		virtual NN<ASN1Names> CreateNames() const = 0;

		Bool ToASN1String(NN<Text::StringBuilderUTF8> sb) const;
		const UInt8 *GetASN1Buff() const;
		UOSInt GetASN1BuffSize() const;
		Data::ByteArrayR GetASN1Array() const;

		static void AppendInteger(NN<Text::StringBuilderUTF8> sb, const UInt8 *pdu, UOSInt len);
	};
}
#endif
