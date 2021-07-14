#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/GoogleQRCode.h"
#include "Text/MyString.h"
#include "Text/TextEnc/FormEncoding.h"

Net::GoogleQRCode::GoogleQRCode(UOSInt width, UOSInt height, const UTF8Char *dataStr)
{
	this->width = width;
	this->height = height;
	this->dataStr = Text::StrCopyNew(dataStr);
}

Net::GoogleQRCode::~GoogleQRCode()
{
	Text::StrDelNew(this->dataStr);
}

void Net::GoogleQRCode::GetImageURL(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"https://chart.googleapis.com/chart?cht=qr&chs=");
	sb->AppendUOSInt(this->width);
	sb->AppendChar('x', 1);
	sb->AppendUOSInt(this->height);
	sb->Append((const UTF8Char*)"&chl=");
	Text::TextEnc::FormEncoding::FormEncode(sb, dataStr);
}
