#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SeekableStream.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Writer.h"

Text::UTF8Writer::UTF8Writer(NN<IO::Stream> stm)
{
	this->stm = stm;
}

Text::UTF8Writer::~UTF8Writer()
{
}

Bool Text::UTF8Writer::Write(Text::CStringNN str)
{
	return this->stm->Write(str.ToByteArray()) == str.leng;
}

Bool Text::UTF8Writer::WriteLine(Text::CStringNN str)
{
	const UTF8Char crlf[2] = {13, 10};
	UOSInt ret = this->stm->Write(str.ToByteArray());
	if (ret == str.leng)
	{
		ret += this->stm->Write(Data::ByteArrayR(crlf, 2));
	}
	return ret == str.leng + 2;
}

Bool Text::UTF8Writer::WriteW(const UTF16Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8CntC(str, nChar);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 1);
	Text::StrUTF16_UTF8C(utf8, str, nChar);
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len));
	MemFreeArr(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteW(const UTF16Char *str)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8Cnt(str);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 1);
	Text::StrUTF16_UTF8(utf8, str);
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len));
	MemFreeArr(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteLineW(const UTF16Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8CntC(str, nChar);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 2);
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUTF16_UTF8C(utf8, str, nChar);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len + 2));
	MemFreeArr(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteLineW(const UTF16Char *str)
{
	UOSInt utf8Len = Text::StrUTF16_UTF8Cnt(str);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 2);
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUTF16_UTF8(utf8, str);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len + 2));
	MemFreeArr(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteW(const UTF32Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8CntC(str, nChar);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 1);
	Text::StrUTF32_UTF8C(utf8, str, nChar);
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len));
	MemFreeArr(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteW(const UTF32Char *str)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8Cnt(str);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 1);
	Text::StrUTF32_UTF8(utf8, str);
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len));
	MemFreeArr(utf8);
	return ret == utf8Len;
}

Bool Text::UTF8Writer::WriteLineW(const UTF32Char *str, UOSInt nChar)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8CntC(str, nChar);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 2);
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUTF32_UTF8C(utf8, str, nChar);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len + 2));
	MemFreeArr(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteLineW(const UTF32Char *str)
{
	UOSInt utf8Len = Text::StrUTF32_UTF8Cnt(str);
	UnsafeArray<UTF8Char> utf8 = MemAllocArr(UTF8Char, utf8Len + 2);
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUTF32_UTF8(utf8, str);
	sptr[0] = 13;
	sptr[1] = 10;
	UOSInt ret = this->stm->Write(Data::ByteArrayR(utf8, utf8Len + 2));
	MemFreeArr(utf8);
	return ret == utf8Len + 2;
}

Bool Text::UTF8Writer::WriteLine()
{
	const UTF8Char crlf[2] = {13, 10};
	return this->stm->Write(Data::ByteArrayR(crlf, 2)) == 2;
}

void Text::UTF8Writer::WriteSignature()
{
	if (this->stm->CanSeek())
	{
		if (((IO::SeekableStream*)this->stm.Ptr())->GetPosition() == 0)
		{
			const UTF8Char buff[3] = {0xEF, 0xBB, 0xBF};
			this->stm->Write(Data::ByteArrayR(buff, 3));
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
