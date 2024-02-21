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
			TypeIsOpt,
			RepeatIfTypeIs,
			LastOIDAndTypeIs,
			AllNotMatch
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

	public:
		ASN1Names();
		~ASN1Names();

		void ReadBegin();
		Text::CStringNN ReadName(Net::ASN1Util::ItemType itemType, UOSInt len, const UInt8 *buff);
		Text::CString ReadNameNoDef(Net::ASN1Util::ItemType itemType, UOSInt len, const UInt8 *buff);
		void ReadContainerBegin();
		void ReadContainerEnd();

		NotNullPtr<ASN1Names> AnyCond();
		NotNullPtr<ASN1Names> TypeIs(Net::ASN1Util::ItemType itemType);
		NotNullPtr<ASN1Names> TypeIsTime();
		NotNullPtr<ASN1Names> TypeIsString();
		NotNullPtr<ASN1Names> TypeIsOpt(UInt8 index);
		NotNullPtr<ASN1Names> RepeatIfTypeIs(Net::ASN1Util::ItemType itemType);
		NotNullPtr<ASN1Names> LastOIDAndTypeIs(Text::CStringNN oidText, Net::ASN1Util::ItemType itemType);
		NotNullPtr<ASN1Names> AllNotMatch();

		NotNullPtr<ASN1Names> Container(Text::CStringNN name, ContentFunc func);
		NotNullPtr<ASN1Names> NextValue(Text::CStringNN name);
		NotNullPtr<ASN1Names> Enum(Text::CStringNN name, Text::CStringNN *enumVals, UOSInt enumCnt);

		NotNullPtr<ASN1Names> SetCertificate(); //PKIX1Explicit88
		NotNullPtr<ASN1Names> SetRSAPublicKey(); //PKCS-1
		NotNullPtr<ASN1Names> SetRSAPrivateKey(); //PKCS-1
		NotNullPtr<ASN1Names> SetPKCS7ContentInfo(); //PKCS-7
		NotNullPtr<ASN1Names> SetCertificationRequest(); //PKCS-10
		NotNullPtr<ASN1Names> SetPFX(); //PKCS-12
	};
}
#endif
