#ifndef _SM_NET_ASN1UNKNOWNDATA
#define _SM_NET_ASN1UNKNOWNDATA
#include "Net/ASN1Data.h"

namespace Net
{
	class ASN1UnknownData : public Net::ASN1Data
	{
	public:
		ASN1UnknownData(Text::CStringNN sourceName, Data::ByteArrayR buff);
		virtual ~ASN1UnknownData();

		virtual ASN1Type GetASN1Type() const;
		virtual NN<ASN1Data> Clone() const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		virtual NN<ASN1Names> CreateNames() const;
	};
}
#endif
