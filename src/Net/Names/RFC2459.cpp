#include "Stdafx.h"
#include "Net/Names/RFC2459.h"


void __stdcall Net::Names::RFC2459::AuthorityInfoAccessSyntax(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AuthorityInfoAccessSyntax"), AuthorityInfoAccessSyntaxCont);
}

void __stdcall Net::Names::RFC2459::AuthorityInfoAccessSyntaxCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AccessDescription"), AccessDescriptionCont);
}

void __stdcall Net::Names::RFC2459::AccessDescriptionCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("accessMethod"));
	names->NextValue(CSTR("accessLocation"));
}
