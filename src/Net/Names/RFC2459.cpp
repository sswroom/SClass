#include "Stdafx.h"
#include "Net/Names/RFC2459.h"


void Net::Names::RFC2459::AuthorityInfoAccessSyntax(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AuthorityInfoAccessSyntax"), AuthorityInfoAccessSyntaxCont);
}

void Net::Names::RFC2459::AuthorityInfoAccessSyntaxCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AccessDescription"), AccessDescriptionCont);
}

void Net::Names::RFC2459::AccessDescriptionCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("accessMethod"));
	names->NextValue(CSTR("accessLocation"));
}