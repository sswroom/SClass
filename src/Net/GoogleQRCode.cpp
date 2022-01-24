#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/GoogleQRCode.h"
#include "Text/MyString.h"
#include "Text/String.h"
#include "Text/TextBinEnc/FormEncoding.h"

Net::GoogleQRCode::GoogleQRCode(UOSInt width, UOSInt height, const UTF8Char *dataStr)
{
	this->width = width;
	this->height = height;
	this->dataStr = Text::String::NewNotNull(dataStr);
}

Net::GoogleQRCode::~GoogleQRCode()
{
	this->dataStr->Release();
}

void Net::GoogleQRCode::GetImageURL(Text::StringBuilderUTF *sb)
{
	sb->AppendC(UTF8STRC("https://chart.googleapis.com/chart?cht=qr&chs="));
	sb->AppendUOSInt(this->width);
	sb->AppendChar('x', 1);
	sb->AppendUOSInt(this->height);
	sb->AppendC(UTF8STRC("&chl="));
	Text::TextBinEnc::FormEncoding::FormEncode(sb, dataStr->v, dataStr->leng);
}
