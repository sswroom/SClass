#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/TextBinEnc/ANSITextBinEnc.h"
#include "Text/TextBinEnc/ASN1OIDBinEnc.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/CPPByteArrBinEnc.h"
#include "Text/TextBinEnc/CPPTextBinEnc.h"
#include "Text/TextBinEnc/FormEncoding.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"
#include "Text/TextBinEnc/Punycode.h"
#include "Text/TextBinEnc/QuotedPrintableEnc.h"
#include "Text/TextBinEnc/TextBinEncList.h"
#include "Text/TextBinEnc/UCS2TextBinEnc.h"
#include "Text/TextBinEnc/URIEncoding.h"
#include "Text/TextBinEnc/UTF8TextBinEnc.h"

Text::TextBinEnc::TextBinEncList::TextBinEncList()
{
	Text::TextBinEnc::ITextBinEnc *enc;
	NEW_CLASS(this->encList, Data::ArrayList<Text::TextBinEnc::ITextBinEnc*>());
	NEW_CLASS(enc, Text::TextBinEnc::Base64Enc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::ANSITextBinEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::UTF8TextBinEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::CPPByteArrBinEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::CPPTextBinEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::HexTextBinEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::QuotedPrintableEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::UCS2TextBinEnc());
	this->encList->Add(enc);
	NEW_CLASS(enc, Text::TextBinEnc::ASN1OIDBinEnc());
	this->encList->Add(enc);
}

Text::TextBinEnc::TextBinEncList::~TextBinEncList()
{
	UOSInt i;
	Text::TextBinEnc::ITextBinEnc *enc;
	i = this->encList->GetCount();
	while (i-- > 0)
	{
		enc = this->encList->GetItem(i);
		DEL_CLASS(enc);
	}
	DEL_CLASS(this->encList);
}

Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *Text::TextBinEnc::TextBinEncList::GetEncList()
{
	return this->encList;
}
