#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/GoogleQRCode.h"
#include "Text/MyString.h"
#include "Text/String.h"
#include "Text/TextBinEnc/FormEncoding.h"

Net::GoogleQRCode::GoogleQRCode(UIntOS width, UIntOS height, Text::CStringNN dataStr)
{
	this->width = width;
	this->height = height;
	this->dataStr = Text::String::New(dataStr);
}

Net::GoogleQRCode::~GoogleQRCode()
{
	this->dataStr->Release();
}

void Net::GoogleQRCode::GetImageURL(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("https://chart.googleapis.com/chart?cht=qr&chs="));
	sb->AppendUIntOS(this->width);
	sb->AppendUTF8Char('x');
	sb->AppendUIntOS(this->height);
	sb->AppendC(UTF8STRC("&chl="));
	Text::TextBinEnc::FormEncoding::FormEncode(sb, dataStr->ToCString());
}
