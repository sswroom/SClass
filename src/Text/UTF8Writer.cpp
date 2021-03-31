#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SeekableStream.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Writer.h"

Text::UTF8Writer::UTF8Writer(IO::Stream *stm)
{
	this->stm = stm;
}

Text::UTF8Writer::~UTF8Writer()
{
}

Bool Text::UTF8Writer::Write(const UTF8Char *str, UOSInt nChar)
{
	return this->stm->Write(str, nChar) == nChar;
}

Bool Text::UTF8Writer::Write(const UTF8Char *str)
{
	UOSInt len = Text::StrCharCnt(str);
	return this->stm->Write(str, len) == len;
}

Bool Text::UTF8Writer::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	const UTF8Char crlf[2] = {13, 10};
	UOSInt ret = this->stm->Write(str, nChar);
	if (ret == nChar)
	{
		ret += this->stm->Write(crlf, 2);
	}
	return ret == nChar + 2;
}

Bool Text::UTF8Writer::WriteLine(const UTF8Char *str)
{
	const UTF8Char crlf[2] = {13, 10};
	UOSInt len = Text::StrCharCnt(str);
	UOSInt ret = this->stm->Write(str, len);
	if (ret == len)
	{
		ret += this->stm->Write(crlf, 2);
	}
	return ret == len + 2;
	
}

Bool Text::UTF8Writer::WriteW(const UTF16Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8Cnt(str, nChar);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 1);
	Text::StrUTF16_UTF8(utf8, str, (OSInt)nChar);
	UOSInt ret = this->stm->Write(utf8, utf8Len);
	MemFree(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteW(const UTF16Char *str)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8Cnt(str, -1);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 1);
	Text::StrUTF16_UTF8(utf8, str, -1);
	UOSInt ret = this->stm->Write(utf8, utf8Len);
	MemFree(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteLineW(const UTF16Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8Cnt(str, nChar);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 2);
	UTF8Char *sptr;
	sptr = Text::StrUTF16_UTF8(utf8, str, (OSInt)nChar);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(utf8, utf8Len + 2);
	MemFree(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteLineW(const UTF16Char *str)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8Cnt(str, -1);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 2);
	UTF8Char *sptr;
	sptr = Text::StrUTF16_UTF8(utf8, str, -1);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(utf8, utf8Len + 2);
	MemFree(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteW(const UTF32Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8Cnt(str, (OSInt)nChar);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 1);
	Text::StrUTF32_UTF8(utf8, str, nChar);
	UOSInt ret = this->stm->Write(utf8, utf8Len);
	MemFree(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteW(const UTF32Char *str)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8Cnt(str, -1);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 1);
	Text::StrUTF32_UTF8(utf8, str, -1);
	UOSInt ret = this->stm->Write(utf8, utf8Len);
	MemFree(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteLineW(const UTF32Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8Cnt(str, nChar);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 2);
	UTF8Char *sptr;
	sptr = Text::StrUTF32_UTF8(utf8, str, nChar);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(utf8, utf8Len + 2);
	MemFree(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteLineW(const UTF32Char *str)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8Cnt(str, -1);
	UTF8Char *utf8 = MemAlloc(UTF8Char, utf8Len + 2);
	UTF8Char *sptr;
	sptr = Text::StrUTF32_UTF8(utf8, str, -1);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(utf8, utf8Len + 2);
	MemFree(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteLine()
{
	const UTF8Char crlf[2] = {13, 10};
	return this->stm->Write(crlf, 2) == 2;
}

void Text::UTF8Writer::WriteSignature()
{
	if (this->stm->CanSeek())
	{
		if (((IO::SeekableStream*)this->stm)->GetPosition() == 0)
		{
			const UTF8Char buff[3] = {0xEF, 0xBB, 0xBF};
			this->stm->Write(buff, 3);
		}
	}
}

Int32 Text::UTF8Writer::Flush()
{
	return this->stm->Flush();
}

void Text::UTF8Writer::Close()
{
	this->stm->Close();
}
