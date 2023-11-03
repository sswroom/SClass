#include "Stdafx.h"
#include "Net/ASN1Names.h"

void Net::ASN1Names::AddRule(NotNullPtr<NameRule> rule)
{
	if (this->readContainer)
		this->readContainer->rules.Add(rule);
	else
		this->rules.Add(rule);
	this->AnyCond();
}

void Net::ASN1Names::FreeContainer(RuleContainer *container)
{
	ClearRules(container->rules);
	DEL_CLASS(container);
}
	
void Net::ASN1Names::ClearRules(NotNullPtr<Data::ArrayListNN<NameRule>> rules)
{
	NameRule *rule;
	UOSInt i = rules->GetCount();
	while (i-- > 0)
	{
		rule = rules->GetItem(i);
		MemFree(rule);
	}
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::AnyCond()
{
	this->currCond = RuleCond::Any;
	this->currItemType = Net::ASN1Util::IT_UNKNOWN;
	this->currCondParam = CSTR_NULL;
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::TypeIs(Net::ASN1Util::ItemType itemType)
{
	this->currCond = RuleCond::TypeIsItemType;
	this->currItemType = itemType;
	this->currCondParam = CSTR_NULL;
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::TypeIsTime()
{
	this->currCond = RuleCond::TypeIsTime;
	this->currItemType = Net::ASN1Util::IT_UNKNOWN;
	this->currCondParam = CSTR_NULL;
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::TypeIsString()
{
	this->currCond = RuleCond::TypeIsString;
	this->currItemType = Net::ASN1Util::IT_UNKNOWN;
	this->currCondParam = CSTR_NULL;
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::RepeatIfTypeIs(Net::ASN1Util::ItemType itemType)
{
	this->currCond = RuleCond::RepeatIfTypeIs;
	this->currItemType = itemType;
	this->currCondParam = CSTR_NULL;
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::LastOIDAndTypeIs(Text::CStringNN oidText, Net::ASN1Util::ItemType itemType)
{
	this->currCond = RuleCond::LastOIDAndTypeIs;
	this->currItemType = itemType;
	this->currCondParam = oidText;
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::Container(Text::CStringNN name, ContentFunc func)
{
	NotNullPtr<NameRule> rule = MemAllocNN(NameRule, 1);
	rule->cond = this->currCond;
	rule->itemType = this->currItemType;
	rule->condParam = this->currCondParam;
	rule->name = name;
	rule->contentFunc = func;
	rule->enumCnt = 0;
	rule->enumVals = 0;
	this->AddRule(rule);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::NextValue(Text::CStringNN name)
{
	NotNullPtr<NameRule> rule = MemAllocNN(NameRule, 1);
	rule->cond = this->currCond;
	rule->itemType = this->currItemType;
	rule->condParam = this->currCondParam;
	rule->name = name;
	rule->contentFunc = 0;
	rule->enumCnt = 0;
	rule->enumVals = 0;
	this->AddRule(rule);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::Enum(Text::CStringNN name, Text::CStringNN *enumVals, UOSInt enumCnt)
{
	NotNullPtr<NameRule> rule = MemAllocNN(NameRule, 1);
	rule->cond = this->currCond;
	rule->itemType = this->currItemType;
	rule->condParam = this->currCondParam;
	rule->name = name;
	rule->contentFunc = 0;
	rule->enumCnt = enumCnt;
	rule->enumVals = enumVals;
	this->AddRule(rule);
	return *this;
}

void Net::ASN1Names::EmptyCont(NotNullPtr<ASN1Names> names)
{
}

void Net::ASN1Names::ValidityCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIsTime()->NextValue(CSTR("notBefore"));
	names->TypeIsTime()->NextValue(CSTR("notAfter"));
}

void Net::ASN1Names::ExtensionsCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Extension"), ExtensionCont);
}

void Net::ASN1Names::ExtensionCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("extnID"));
	names->TypeIs(Net::ASN1Util::IT_BOOLEAN)->NextValue(CSTR("critical"));
	names->LastOIDAndTypeIs(CSTR("2.5.29.17"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("subjectAltName"), SubjectAltName);
	names->LastOIDAndTypeIs(CSTR("2.5.29.14"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("subjectKeyIdentifier"), SubjectKeyIdentifier);
	names->LastOIDAndTypeIs(CSTR("2.5.29.35"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("authorityKeyIdentifier"), AuthorityKeyIdentifier);
	names->NextValue(CSTR("extnValue"));//////////////////////////////
}

void Net::ASN1Names::SubjectAltName(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("GeneralName"), GeneralNameCont);
}

void Net::ASN1Names::SubjectKeyIdentifier(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("SubjectKeyIdentifier"));
}

void Net::ASN1Names::AuthorityKeyIdentifier(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AuthorityKeyIdentifier"), AuthorityKeyIdentifierCont);
}

void Net::ASN1Names::AuthorityKeyIdentifierCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("keyIdentifier"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_1)->NextValue(CSTR("authorityCertIssuer"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_2)->NextValue(CSTR("authorityCertSerialNumber"));
}

void Net::ASN1Names::GeneralNameCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("otherName"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_1)->NextValue(CSTR("rfc822Name"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_2)->NextValue(CSTR("dNSName"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_3)->NextValue(CSTR("x400Address"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_4)->NextValue(CSTR("directoryName"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_5)->NextValue(CSTR("ediPartyName"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_6)->NextValue(CSTR("uniformResourceIdentifier"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_7)->NextValue(CSTR("iPAddress"));
	names->TypeIs(Net::ASN1Util::IT_CHOICE_8)->NextValue(CSTR("registeredID"));
}

void Net::ASN1Names::SubjectPublicKeyInfoCont(NotNullPtr<ASN1Names> names)
{
	names->AlgorithmIdentifier(CSTR("algorithm"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.1.1"), Net::ASN1Util::IT_BIT_STRING)->Container(CSTR("subjectPublicKey"), RSAPublicKey);
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("subjectPublicKey"));
}

void Net::ASN1Names::RSAPublicKey(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("RSAPublicKey"), RSAPublicKeyCont);
}

void Net::ASN1Names::RSAPublicKeyCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("modulus"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("publicExponent"));
}

void Net::ASN1Names::RSAPrivateKey(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("RSAPrivateKey"), RSAPrivateKeyCont);
}

void Net::ASN1Names::RSAPrivateKeyCont(NotNullPtr<ASN1Names> names)
{
	static Text::CStringNN Version[] = {CSTR("two-prime"), CSTR("multi")};
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->Enum(CSTR("Version"), Version, 2);
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("modulus"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("publicExponent"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("privateExponent"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("prime1"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("prime2"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("exponent1"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("exponent2"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("coefficient"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("otherPrimeInfos"), OtherPrimeInfos);
}

void Net::ASN1Names::OtherPrimeInfos(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("prime"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("exponent"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("coefficient"));
}

void Net::ASN1Names::CertificateList(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertificateList"), CertificateListCont);
}

void Net::ASN1Names::CertificateListCont(NotNullPtr<ASN1Names> names)
{
	names->TBSCertList(CSTR("tbsCertList"));
	names->AlgorithmIdentifier(CSTR("signatureAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("signature"));
}

void Net::ASN1Names::Certificate(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Certificate"), CertificateCont);
}

void Net::ASN1Names::CertificateCont(NotNullPtr<ASN1Names> names)
{
	names->TBSCertificate(CSTR("tbsCertificate"));
	names->AlgorithmIdentifier(CSTR("signatureAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("signature"));
}

void Net::ASN1Names::CertificationRequestCont(NotNullPtr<ASN1Names> names)
{
	names->CertificationRequestInfo(CSTR("certificationRequestInfo"));
	names->AlgorithmIdentifier(CSTR("signatureAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("signature"));
}

void Net::ASN1Names::PFXCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("Version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("authSafe"), AuthenticatedSafeContentInfoCont);
	names->MacData(CSTR("macData"));
}

void Net::ASN1Names::TBSCertListCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("Version"));
	names->AlgorithmIdentifier(CSTR("signature"));
	names->AddName(CSTR("issuer"));
	names->TypeIsTime()->NextValue(CSTR("thisUpdate"));
	names->TypeIsTime()->NextValue(CSTR("nextUpdate"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("revokedCertificates"), RevokedCertificates);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("crlExtensions"), Extensions);
}

void Net::ASN1Names::TBSCertificateCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("version"), TBSCertificateVersion);
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("serialNumber"));
	names->AlgorithmIdentifier(CSTR("signature"));
	names->AddName(CSTR("issuer"));
	names->Validity(CSTR("validity"));
	names->AddName(CSTR("subject"));
	names->SubjectPublicKeyInfo(CSTR("subjectPublicKeyInfo"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->NextValue(CSTR("issuerUniqueID"));/////////////////////
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_2)->NextValue(CSTR("subjectUniqueID"));//////////////////////
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_3)->Container(CSTR("extensions"), Extensions);
}

void Net::ASN1Names::TBSCertificateVersion(NotNullPtr<ASN1Names> names)
{
	static Text::CStringNN Version[] = {CSTR("v1"), CSTR("v2"), CSTR("v3")};
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->Enum(CSTR("Version"), Version, 3);
}

void Net::ASN1Names::RevokedCertificates(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("revokedCertificate"), RevokedCertificateCont);
}

void Net::ASN1Names::RevokedCertificateCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("userCertificate"));
	names->TypeIsTime()->NextValue(CSTR("revocationDate"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("crlEntryExtensions"), ExtensionsCont);
}

void Net::ASN1Names::Extensions(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Extensions"), ExtensionsCont);
}

void Net::ASN1Names::RDNSequenceCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("rdnSequence"), RelativeDistinguishedNameCont);
}

void Net::ASN1Names::RelativeDistinguishedNameCont(NotNullPtr<ASN1Names> names)
{
	names->AttributeTypeAndValue(CSTR("AttributeTypeAndValue"));
}

void Net::ASN1Names::AttributeTypeAndValueCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("type"));
	names->NextValue(CSTR("value"));
}

void Net::ASN1Names::AlgorithmIdentifierCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("algorithm"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.1.6"), Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("parameters"), RC2_CBC_Param);
	names->NextValue(CSTR("parameters"));
}

void Net::ASN1Names::RC2_CBC_Param(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("iv"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("effectiveBits"));
}

void Net::ASN1Names::CertificationRequestInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->AddName(CSTR("subject"));
	names->SubjectPublicKeyInfo(CSTR("subjectPKInfo"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("attributes"), AttributesCont);
}

void Net::ASN1Names::AttributesCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Attribute"), AttributeCont);
}

void Net::ASN1Names::AttributeCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("attrId"));
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.1"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeEmailAddress);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.2"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeUnstructuredName);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.3"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeContentType);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.4"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeMessageDigest);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.5"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeSigningTime);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.6"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeCounterSignature);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.7"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeChallengePassword);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.8"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeUnstructuredAddress);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.9"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeExtendedCertificateAttributes);

	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.15"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeSMIMECapabilities);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.16.2.11"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeSMIMEEncryptionKeyPreference);

	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.20"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeFriendlyName);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.21"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeLocalKeyId);
	names->LastOIDAndTypeIs(CSTR("1.3.6.1.4.1.311.16.4"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeOutlookExpress);
	
	names->TypeIs(Net::ASN1Util::IT_SET)->NextValue(CSTR("attrValues"));
}

void Net::ASN1Names::DigestAlgorithmIdentifiers(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("DigestAlgorithmIdentifier"), AlgorithmIdentifierCont);
}

void Net::ASN1Names::SignerInfos(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SignerInfo"), SignerInfoCont);
}

void Net::ASN1Names::SignerInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signerIdentifier"), IssuerAndSerialNumberCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_2)->Container(CSTR("signerIdentifier"), SubjectKeyIdentifier);
  	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("digestAlgorithm"), AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("authenticatedAttributes"), AttributesCont);
  	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("digestEncryptionAlgorithm"), AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("encryptedDigest"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("unauthenticatedAttributes"), AttributesCont);
}

void Net::ASN1Names::IssuerAndSerialNumberCont(NotNullPtr<ASN1Names> names)
{
	names->AddName(CSTR("issuer"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("serialNumber"));
}

void Net::ASN1Names::CertificateSet(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("certificate"), CertificateCont);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("extendedCertificate"));//, ExtendedCertificate);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_1)->NextValue(CSTR("attributeCertificate"));//, AttributeCertificate);
}

void Net::ASN1Names::CertificateRevocationLists(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->NextValue(CSTR("CertificateRevocationLists"));//, CertificateListCont);
}

void Net::ASN1Names::OriginatorInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certificates"), CertificateSet);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("crls"), CertificateRevocationLists);
}

void Net::ASN1Names::RecipientInfos(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("keyTransportRecipientInfo"), KeyTransportRecipientInfoCont);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("keyAgreementRecipientInfo"));//, KeyAgreementRecipientInfo);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_1)->NextValue(CSTR("keyEncryptionKeyRecipientInfo"));//, KeyEncryptionKeyRecipientInfo);
}

void Net::ASN1Names::KeyTransportRecipientInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->//TypeIs(Net::ASN1Util::IT_INTEGER)->
		NextValue(CSTR("recipientIdentifier")); //RecipientIdentifier
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("keyEncryptionAlgorithm"), AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("encryptedKey"));
}

void Net::ASN1Names::PKCS7Data(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("data"));
}

void Net::ASN1Names::PKCS7SignedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7SignedDataCont);
}

void Net::ASN1Names::PKCS7SignedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("digestAlgorithms"), DigestAlgorithmIdentifiers);
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("contentInfo"), ContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certificates"), CertificateSet);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("crls"), CertificateRevocationLists);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("signerInfos"), SignerInfos);
}

void Net::ASN1Names::PKCS7EnvelopedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("enveloped-data"), PKCS7EnvelopedDataCont);
}

void Net::ASN1Names::PKCS7EnvelopedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("originatorInfo"), OriginatorInfoCont);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("recipientInfos"), RecipientInfos);
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encryptedContentInfo"), ContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("unprotectedAttributes"), AttributesCont);
}

void Net::ASN1Names::PKCS7SignedAndEnvelopedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7SignedAndEnvelopedDataCont);
}

void Net::ASN1Names::PKCS7SignedAndEnvelopedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SET)->NextValue(CSTR("recipientInfos"));
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("digestAlgorithms"), DigestAlgorithmIdentifiers);
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("contentInfo"), ContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certificates"), CertificateSet);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("crls"), CertificateRevocationLists);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("signerInfos"), SignerInfos);
}

void Net::ASN1Names::PKCS7DigestedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7DigestedDataCont);
}

void Net::ASN1Names::PKCS7DigestedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7DigestedDataCont);
}

void Net::ASN1Names::PKCS7EncryptedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encrypted-data"), PKCS7EncryptedDataCont);
}

void Net::ASN1Names::PKCS7EncryptedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encryptedContentInfo"), EncryptedContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("unprotectedAttributes"), AttributesCont);
}

void Net::ASN1Names::EncryptedContentInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("contentType"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("contentEncryptionAlgorithm"), AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("encryptedContent"));
}

void Net::ASN1Names::PKCS7AuthenticatedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7AuthenticatedData);
}

void Net::ASN1Names::PrivateKeyInfo(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PrivateKeyInfo"), PrivateKeyInfoCont);
}

void Net::ASN1Names::PrivateKeyInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("privateKeyAlgorithm"), AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("privateKey"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("attributes"), AttributesCont);
}

void Net::ASN1Names::EncryptedPrivateKeyInfo(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("EncryptedPrivateKeyInfo"), EncryptedPrivateKeyInfoCont);
}

void Net::ASN1Names::EncryptedPrivateKeyInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encryptionAlgorithm"), AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("encryptedData"));
}

void Net::ASN1Names::AuthSafeContentInfo(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("ContentInfo"), AuthSafeContentInfoCont);
}

void Net::ASN1Names::AuthSafeContentInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("content-type"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), SafeContentsData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7SignedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7EnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.4"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7SignedAndEnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.5"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7DigestedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7EncryptedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.16.1.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7AuthenticatedData);
	names->NextValue(CSTR("pkcs7-content")); ////////////////////////
}

void Net::ASN1Names::SafeContentsData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("data"), SafeContents);
}

void Net::ASN1Names::SafeContents(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SafeContents"), SafeContentsCont);
}

void Net::ASN1Names::SafeContentsCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SafeBag"), SafeBagCont);
}

void Net::ASN1Names::SafeBagCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("bagId"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("keyBag"), PrivateKeyInfo);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs8ShroudedKeyBag"), EncryptedPrivateKeyInfo);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certBag"), CertBag);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.4"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("crlBag"), CRLBag);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.5"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("secretBag"), SecretBag);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("safeContentsBag"), SafeContents);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("bagAttributes"), PKCS12Attributes);
}

void Net::ASN1Names::CertBag(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertBag"), CertBagCont);
}

void Net::ASN1Names::CertBagCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("certId"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.22.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certValue"), X509Certificate);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.22.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certValue"), SdsiCertificate);
}

void Net::ASN1Names::X509Certificate(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("x509Certificate"), Certificate);
}

void Net::ASN1Names::SdsiCertificate(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_IA5STRING)->NextValue(CSTR("sdsiCertificate"));
}

void Net::ASN1Names::CRLBag(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CRLBag"), CRLBagCont);
}

void Net::ASN1Names::CRLBagCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("crlId"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.23.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("crlValue"), X509CRL);
}

void Net::ASN1Names::X509CRL(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("x509CRL"), CertificateList);
}

void Net::ASN1Names::SecretBag(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SecretBag"), SecretBagCont);
}

void Net::ASN1Names::SecretBagCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("secretTypeId"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->NextValue(CSTR("secretValue"));
}

void Net::ASN1Names::PKCS12Attributes(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PKCS12Attribute"), AttributeCont);
}

void Net::ASN1Names::AttributeContentType(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("contentType"));
}

void Net::ASN1Names::AttributeMessageDigest(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("messageDigest"));
}

void Net::ASN1Names::AttributeSigningTime(NotNullPtr<ASN1Names> names)
{
	names->TypeIsTime()->NextValue(CSTR("signingTime"));
}

void Net::ASN1Names::AttributeFriendlyName(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_BMPSTRING)->NextValue(CSTR("friendlyName"));
}

void Net::ASN1Names::AttributeSMIMECapabilities(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("smimeCapabilities"), SMIMECapabilitiesCont);
}

void Net::ASN1Names::AttributeSMIMEEncryptionKeyPreference(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("issuerAndSerialNumber"), IssuerAndSerialNumberCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->NextValue(CSTR("receipentKeyId"));//, RecipientKeyIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_2)->NextValue(CSTR("subjectAltKeyIdentifier"));//, SubjectKeyIdentifierCont);
}

void Net::ASN1Names::AttributeLocalKeyId(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("localKeyId"));
}

void Net::ASN1Names::AttributeOutlookExpress(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("issuerAndSerialNumber"), IssuerAndSerialNumberCont);
}

void Net::ASN1Names::SMIMECapabilitiesCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SMIMECapability"), SMIMECapabilityCont);
}

void Net::ASN1Names::SMIMECapabilityCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("algorithm"));
	names->NextValue(CSTR("parameters"));
}

void Net::ASN1Names::AuthenticatedSafe(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AuthenticatedSafe"), AuthSafeContentInfo);
}

void Net::ASN1Names::AuthenticatedSafeData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("data"), AuthenticatedSafe);
}

void Net::ASN1Names::AuthenticatedSafeEnvelopedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7EnvelopedDataCont);
}

void Net::ASN1Names::AuthenticatedSafeEncryptedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encrypted-data"), PKCS7EncryptedDataCont);
}

void Net::ASN1Names::AuthenticatedSafeContentInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("content-type"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedSafeData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedSafeEnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedSafeEncryptedData);
	names->NextValue(CSTR("pkcs7-content")); ////////////////////////
}

void Net::ASN1Names::ContentInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("content-type"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7Data);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7SignedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7EnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.4"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7SignedAndEnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.5"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7DigestedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7EncryptedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.16.1.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7AuthenticatedData);
	names->NextValue(CSTR("pkcs7-content")); ////////////////////////
}

void Net::ASN1Names::DigestInfoCont(NotNullPtr<ASN1Names> names)
{
	names->AlgorithmIdentifier(CSTR("digestAlgorithm"));
	names->NextValue(CSTR("digest")); ////////////////////////
}

void Net::ASN1Names::MacDataCont(NotNullPtr<ASN1Names> names)
{
	names->DigestInfo(CSTR("mac"));
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("macSalt"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("iterations"));
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::Validity(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, ValidityCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::AddExtensions(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, ExtensionsCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SubjectPublicKeyInfo(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, SubjectPublicKeyInfoCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::AddName(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, RDNSequenceCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::AttributeTypeAndValue(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, AttributeTypeAndValueCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::TBSCertList(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, TBSCertListCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::TBSCertificate(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, TBSCertificateCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::AlgorithmIdentifier(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, AlgorithmIdentifierCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::CertificationRequestInfo(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, CertificationRequestInfoCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::ContentInfo(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, ContentInfoCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::DigestInfo(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, DigestInfoCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::MacData(Text::CStringNN name)
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, MacDataCont);
}

Net::ASN1Names::ASN1Names()
{
	this->readContainer = 0;
	this->ReadBegin();
	this->AnyCond();
}

Net::ASN1Names::~ASN1Names()
{
	RuleContainer *container;
	while (this->readContainer)
	{
		container = this->readContainer;
		this->readContainer = container->parent;
		FreeContainer(container);
	}
	this->ClearRules(this->rules);
}

void Net::ASN1Names::ReadBegin()
{
	this->readLev.Clear();
	this->readIndex = 0;
	this->readContainer = 0;
	this->readLastOIDLen = 0;
}

Text::CStringNN Net::ASN1Names::ReadName(Net::ASN1Util::ItemType itemType, UOSInt len, const UInt8 *buff)
{
	Text::CString name = ReadNameNoDef(itemType, len, buff);
	if (name.v)
		return Text::CStringNN(name.v, name.leng);
	return Net::ASN1Util::ItemTypeGetName(itemType);
}

Text::CString Net::ASN1Names::ReadNameNoDef(Net::ASN1Util::ItemType itemType, UOSInt len, const UInt8 *buff)
{
	if (itemType == Net::ASN1Util::IT_OID)
	{
		MemCopyNO(this->readLastOID, buff, len);
		this->readLastOIDLen = len;
	}
	if (this->readIndex == INVALID_INDEX)
	{
		return CSTR_NULL;
	}
	NameRule *rule;
	while (true)
	{
		if (this->readContainer)
		{
			rule = this->readContainer->rules.GetItem(this->readIndex);
		}
		else
		{
			rule = this->rules.GetItem(this->readIndex);
		}
		if (rule == 0)
		{
			return CSTR_NULL;
		}
		switch (rule->cond)
		{
		default:
		case RuleCond::Any:
			this->readIndex++;
			return rule->name;
		case RuleCond::TypeIsItemType:
			this->readIndex++;
			if (rule->itemType == itemType)
				return rule->name;
			break;
		case RuleCond::TypeIsTime:
			this->readIndex++;
			if (itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME)
				return rule->name;
			break;
		case RuleCond::TypeIsString:
			this->readIndex++;
			if (itemType == Net::ASN1Util::IT_BMPSTRING ||
				itemType == Net::ASN1Util::IT_UTF8STRING ||
				itemType == Net::ASN1Util::IT_UNIVERSALSTRING ||
				itemType == Net::ASN1Util::IT_PRINTABLESTRING ||
				itemType == Net::ASN1Util::IT_T61STRING)
				return rule->name;
			break;
		case RuleCond::LastOIDAndTypeIs:
			this->readIndex++;
			if (itemType == rule->itemType && Net::ASN1Util::OIDEqualsText(this->readLastOID, this->readLastOIDLen, rule->condParam.v, rule->condParam.leng))
				return rule->name;
			break;
		case RuleCond::RepeatIfTypeIs:
			if (itemType == rule->itemType)
				return rule->name;
			this->readIndex++;
			break;
		}
	}
}

void Net::ASN1Names::ReadContainer()
{
	NameRule *rule;
	if (this->readIndex == INVALID_INDEX)
	{
		this->readLev.Add(INVALID_INDEX);
	}
	else
	{
		if (this->readContainer)
		{
			if (this->readIndex == 0)
				rule = this->readContainer->rules.GetItem(0);
			else
				rule = this->readContainer->rules.GetItem(this->readIndex - 1);
		}
		else
		{
			if (this->readIndex == 0)
				rule = this->rules.GetItem(0);
			else
				rule = this->rules.GetItem(this->readIndex - 1);
		}
		this->readLev.Add(this->readIndex);
		if (rule == 0 || rule->contentFunc == 0)
		{
			this->readIndex = INVALID_INDEX;
		}
		else
		{
			RuleContainer *container;
			this->readIndex = 0;
			NEW_CLASS(container, RuleContainer());
			container->parent = this->readContainer;
			this->readContainer = container;
			rule->contentFunc(*this);
		}
	}
}

void Net::ASN1Names::ReadContainerEnd()
{
	if (this->readIndex == INVALID_INDEX)
	{
		this->readIndex = this->readLev.Pop();
	}
	else
	{
		this->readIndex = this->readLev.Pop();
		if (this->readContainer)
		{
			RuleContainer *container = this->readContainer;
			this->readContainer = this->readContainer->parent;
			FreeContainer(container);
		}
	}
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetCertificate()
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Certificate"), CertificateCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetRSAPublicKey()
{
	RSAPublicKey(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetRSAPrivateKey()
{
	RSAPrivateKey(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetPKCS7ContentInfo()
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("ContentInfo"), ContentInfoCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetCertificationRequest()
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertificationRequest"), CertificationRequestCont);
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetPFX()
{
	return this->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PFX"), PFXCont);
}
