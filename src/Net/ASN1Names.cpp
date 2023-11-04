#include "Stdafx.h"
#include "Net/ASN1Names.h"
#include "Net/Names/PKCS1.h"
#include "Net/Names/PKCS7.h"
#include "Net/Names/PKCS10.h"
#include "Net/Names/PKCS12.h"
#include "Net/Names/PKIX1Explicit88.h"

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
	Bool anyMatch = false;
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
		case RuleCond::TypeIsOpt:
			this->readIndex++;
			if (itemType == rule->itemType + Net::ASN1Util::IT_CHOICE_0 ||
				itemType == rule->itemType + Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
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
		case RuleCond::AllNotMatch:
			this->readIndex = 0;
			if (anyMatch)
				return rule->name;
			anyMatch = true;
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

NotNullPtr<Net::ASN1Names> Net::ASN1Names::TypeIsOpt(UInt8 index)
{
	this->currCond = RuleCond::TypeIsOpt;
	this->currItemType = (Net::ASN1Util::ItemType)index;
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

NotNullPtr<Net::ASN1Names> Net::ASN1Names::AllNotMatch()
{
	this->currCond = RuleCond::AllNotMatch;
	this->currItemType = Net::ASN1Util::IT_UNKNOWN;
	this->currCondParam = CSTR_NULL;
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

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetCertificate()
{
	Names::PKIX1Explicit88::Certificate(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetRSAPublicKey()
{
	Names::PKCS1::RSAPublicKey(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetRSAPrivateKey()
{
	Names::PKCS1::RSAPrivateKey(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetPKCS7ContentInfo()
{
	Names::PKCS7::ContentInfo(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetCertificationRequest()
{
	Names::PKCS10::CertificationRequest(*this);
	return *this;
}

NotNullPtr<Net::ASN1Names> Net::ASN1Names::SetPFX()
{
	Names::PKCS12::PFX(*this);
	return *this;
}
