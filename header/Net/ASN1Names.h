#ifndef _SM_NET_ASN1NAMES
#define _SM_NET_ASN1NAMES
#include "Data/ArrayListNN.h"
#include "Net/ASN1Util.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class ASN1Names
	{
	private:
		typedef void (__stdcall *ContentFunc)(NotNullPtr<ASN1Names> names);

		enum class RuleCond
		{
			Any,
			TypeIsItemType,
			TypeIsTime,
			TypeIsString,
			RepeatIfTypeIs,
			LastOIDAndTypeIs
		};

		struct RuleContainer;
		struct NameRule
		{
			RuleCond cond;
			Net::ASN1Util::ItemType itemType;
			Text::CString condParam;
			Text::CStringNN name;
			ContentFunc contentFunc;
			Text::CStringNN *enumVals;
			UOSInt enumCnt;
		};

		struct RuleContainer
		{
			Data::ArrayListNN<NameRule> rules;
			RuleContainer *parent;
		};

		RuleCond currCond;
		Net::ASN1Util::ItemType currItemType;
		Text::CString currCondParam;
		Data::ArrayListNN<NameRule> rules;

		Data::ArrayList<UOSInt> readLev;
		UOSInt readIndex;
		RuleContainer *readContainer;
		UInt8 readLastOID[32];
		UOSInt readLastOIDLen;
	private:
		void AddRule(NotNullPtr<NameRule> rule);
		void FreeContainer(RuleContainer *container);
		void ClearRules(NotNullPtr<Data::ArrayListNN<NameRule>> rules);

		NotNullPtr<ASN1Names> AnyCond();
		NotNullPtr<ASN1Names> TypeIs(Net::ASN1Util::ItemType itemType);
		NotNullPtr<ASN1Names> TypeIsTime();
		NotNullPtr<ASN1Names> TypeIsString();
		NotNullPtr<ASN1Names> RepeatIfTypeIs(Net::ASN1Util::ItemType itemType);
		NotNullPtr<ASN1Names> LastOIDAndTypeIs(Text::CStringNN oidText, Net::ASN1Util::ItemType itemType);

		NotNullPtr<ASN1Names> Container(Text::CStringNN name, ContentFunc func);
		NotNullPtr<ASN1Names> NextValue(Text::CStringNN name);
		NotNullPtr<ASN1Names> Enum(Text::CStringNN name, Text::CStringNN *enumVals, UOSInt enumCnt);

		static void EmptyCont(NotNullPtr<ASN1Names> names);
		static void ValidityCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void ExtensionsCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void ExtensionCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void SubjectAltName(NotNullPtr<ASN1Names> names); //PKIX1Implicit88
		static void SubjectKeyIdentifier(NotNullPtr<ASN1Names> names); //PKIX1Implicit88
		static void AuthorityKeyIdentifier(NotNullPtr<ASN1Names> names); //PKIX1Implicit88
		static void AuthorityKeyIdentifierCont(NotNullPtr<ASN1Names> names); //PKIX1Implicit88
		static void GeneralNameCont(NotNullPtr<ASN1Names> names); //PKIX1Implicit88
		static void SubjectPublicKeyInfoCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void RSAPublicKey(NotNullPtr<ASN1Names> names); //PKCS-1
		static void RSAPublicKeyCont(NotNullPtr<ASN1Names> names); //PKCS-1
		static void RSAPrivateKey(NotNullPtr<ASN1Names> names); //PKCS-1
		static void RSAPrivateKeyCont(NotNullPtr<ASN1Names> names); //PKCS-1
		static void OtherPrimeInfos(NotNullPtr<ASN1Names> names); //PKCS-1
		static void CertificateCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void CertificationRequestCont(NotNullPtr<ASN1Names> names); //PKCS-10
		static void PFXCont(NotNullPtr<ASN1Names> names); //PKCS-12
		static void TBSCertificateCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void TBSCertificateVersion(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void TBSCertificateExtensions(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void RDNSequenceCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void RelativeDistinguishedNameCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void AttributeTypeAndValueCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void AlgorithmIdentifierCont(NotNullPtr<ASN1Names> names); //PKIX1Explicit88
		static void CertificationRequestInfoCont(NotNullPtr<ASN1Names> names); //PKCS-10
		static void AttributesCont(NotNullPtr<ASN1Names> names); //PKCS-10
		static void AttributeCont(NotNullPtr<ASN1Names> names); //PKCS-10
		static void PKCS7Data(NotNullPtr<ASN1Names> names); //PKCS-7
		static void PKCS7SignedData(NotNullPtr<ASN1Names> names); //PKCS-7
		static void PKCS7EnvelopedData(NotNullPtr<ASN1Names> names); //PKCS-7
		static void PKCS7SignedAndEnvelopedData(NotNullPtr<ASN1Names> names); //PKCS-7
		static void PKCS7DigestedData(NotNullPtr<ASN1Names> names); //PKCS-7
		static void PKCS7EncryptedData(NotNullPtr<ASN1Names> names); //PKCS-7
		static void PKCS7AuthenticatedData(NotNullPtr<ASN1Names> names); //PKCS-7
		static void ContentInfoCont(NotNullPtr<ASN1Names> names); //PKCS-7
		static void DigestInfoCont(NotNullPtr<ASN1Names> names); //PKCS-7
		static void MacDataCont(NotNullPtr<ASN1Names> names); //PKCS-12

		NotNullPtr<ASN1Names> Validity(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> Extensions(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> SubjectPublicKeyInfo(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> Name(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> AttributeTypeAndValue(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> TBSCertificate(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> AlgorithmIdentifier(Text::CStringNN name); //PKIX1Explicit88
		NotNullPtr<ASN1Names> CertificationRequestInfo(Text::CStringNN name); //PKCS-10
		NotNullPtr<ASN1Names> ContentInfo(Text::CStringNN name); //PKCS-7
		NotNullPtr<ASN1Names> DigestInfo(Text::CStringNN name); //PKCS-7
		NotNullPtr<ASN1Names> MacData(Text::CStringNN name); //PKCS-12
	public:
		ASN1Names();
		~ASN1Names();

		void ReadBegin();
		Text::CStringNN ReadName(Net::ASN1Util::ItemType itemType, UOSInt len, const UInt8 *buff);
		Text::CString ReadNameNoDef(Net::ASN1Util::ItemType itemType, UOSInt len, const UInt8 *buff);
		void ReadContainer();
		void ReadContainerEnd();

		NotNullPtr<ASN1Names> SetCertificate(); //PKIX1Explicit88
		NotNullPtr<ASN1Names> SetRSAPublicKey(); //PKCS-1
		NotNullPtr<ASN1Names> SetRSAPrivateKey(); //PKCS-1
		NotNullPtr<ASN1Names> SetCertificationRequest(); //PKCS-10
		NotNullPtr<ASN1Names> SetPFX(); //PKCS-12
	};
}
#endif
