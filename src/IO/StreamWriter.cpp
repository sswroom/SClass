#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamWriter.h"
#include "IO/SeekableStream.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

IO::StreamWriter::StreamWriter(NN<IO::Stream> stm, Text::Encoding *enc) : enc(enc->GetEncCodePage())
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, this->buffSize = 256);
}

IO::StreamWriter::StreamWriter(NN<IO::Stream> stm, UInt32 codePage) : enc(codePage)
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, this->buffSize = 256);
}

IO::StreamWriter::~StreamWriter()
{
	MemFree(this->buff);
}

Bool IO::StreamWriter::Write(Text::CStringNN str)
{
	if (this->enc.GetEncCodePage() == 65001)
	{
		return this->stm->Write(str.v, str.leng) == str.leng;
	}
	else
	{
		UOSInt wnChar = Text::StrUTF8_WCharCntC(str.v, str.leng);
		WChar *ws = MemAlloc(WChar, wnChar + 1);
		Text::StrUTF8_WCharC(ws, str.v, str.leng, 0);

		UOSInt strSize = 3 * wnChar;
		if (strSize > buffSize)
		{
			while (strSize > buffSize)
			{
				buffSize <<= 1;
			}
			MemFree(this->buff);
			this->buff = MemAlloc(UInt8, this->buffSize);
		}
		UOSInt nChar = enc.WToBytesC(this->buff, ws, wnChar);
		MemFree(ws);
		return this->stm->Write(this->buff, nChar) == nChar;
	}
}

Bool IO::StreamWriter::WriteLine(Text::CStringNN str)
{
	if (this->enc.GetEncCodePage() == 65001)
	{
		const UTF8Char crlf[2] = {13, 10};
		UOSInt ret = this->stm->Write(str.v, str.leng);
		if (ret == str.leng)
		{
			ret += this->stm->Write(crlf, 2);
			return true;
		}
		return false;
	}
	else
	{
		UOSInt wnChar = Text::StrUTF8_WCharCntC(str.v, str.leng);
		WChar *ws = MemAlloc(WChar, wnChar + 2);
		Text::StrUTF8_WCharC(ws, str.v, str.leng, 0);
		ws[wnChar] = 13;
		ws[wnChar + 1] = 10;

		UOSInt strSize = 3 * (wnChar + 2);
		if (strSize > buffSize)
		{
			while (strSize > buffSize)
			{
				buffSize <<= 1;
			}
			MemFree(this->buff);
			this->buff = MemAlloc(UInt8, this->buffSize);
		}
		UOSInt nChar = enc.WToBytesC(this->buff, ws, wnChar + 2);
		MemFree(ws);
		return this->stm->Write(this->buff, nChar) == nChar;
	}
}

Bool IO::StreamWriter::WriteW(const WChar *str, UOSInt nChar)
{
	UOSInt strSize = 3 * nChar;
	if (strSize > buffSize)
	{
		while (strSize > buffSize)
		{
			buffSize <<= 1;
		}
		MemFree(this->buff);
		this->buff = MemAlloc(UInt8, this->buffSize);
	}
	nChar = enc.WToBytesC(this->buff, str, nChar);
	return this->stm->Write(this->buff, nChar) == nChar;
}

Bool IO::StreamWriter::WriteW(const WChar *str)
{
	return WriteW(str, Text::StrCharCnt(str));
}

Bool IO::StreamWriter::WriteLineW(const WChar *str, UOSInt nChar)
{
	UOSInt strSize = 3 * nChar + 2;
	if (strSize > buffSize)
	{
		while (strSize > buffSize)
		{
			buffSize <<= 1;
		}
		MemFree(this->buff);
		this->buff = MemAlloc(UInt8, this->buffSize);
	}
	nChar = enc.WToBytesC(this->buff, str, nChar);
	this->buff[nChar] = 0xd;
	this->buff[nChar + 1] = 0xa;
	return this->stm->Write(this->buff, nChar + 2) == nChar + 2;
}

Bool IO::StreamWriter::WriteLineW(const WChar *str)
{
	Bool ret = WriteLineW(str, Text::StrCharCnt(str));
	return ret;
}

Bool IO::StreamWriter::WriteLine()
{
	UTF8Char buff[2] = {13, 10};
	return this->stm->Write(buff, 2) == 2;
}

void IO::StreamWriter::WriteSignature()
{
	if (this->stm->CanSeek())
	{
		if (((IO::SeekableStream*)this->stm.Ptr())->GetPosition() == 0)
		{
			UInt32 cp = enc.GetEncCodePage();
			if (cp == 65001)
			{
				buff[0] = 0xef;
				buff[1] = 0xbb;
				buff[2] = 0xbf;
				this->stm->Write(buff, 3);
			}
			else if (cp == 1200)
			{
				buff[0] = 0xff;
				buff[1] = 0xfe;
				this->stm->Write(buff, 2);
			}
			else if (cp == 1201)
			{
				buff[0] = 0xfe;
				buff[1] = 0xff;
				this->stm->Write(buff, 2);
			}
		}
	}
}

void IO::StreamWriter::Close()
{
	this->stm->Close();
}
