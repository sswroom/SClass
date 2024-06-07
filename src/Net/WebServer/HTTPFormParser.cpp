#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/FormEncode.h"
#include "IO/MemoryStream.h"
#include "IO/StreamReader.h"
#include "Net/WebServer/HTTPFormParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::HTTPFormParser::HTTPFormParser(Net::WebServer::IWebRequest *req, Int32 codePage)
{
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(this->strNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->strValues, Data::ArrayListStrUTF8());

	if (!req->GetHeaderC(sb, CSTR("Content-Type")))
	{
		return;
	}
	else if (Text::StrEqualsC(sb.ToString(), sb.GetLength(), UTF8STRC("application/x-www-form-urlencoded")))
	{
		UOSInt buffSize;
		const UInt8 *buff;
		UInt8 *tmpBuff = 0;
		UTF8Char *tmpBuff2 = 0;
		OSInt tmpBuffSize = 0;
		OSInt tmpBuffSize2 = 0;
		UnsafeArray<UTF8Char> sptr;
		OSInt size1;
		UOSInt size2;
		OSInt i;
		OSInt j;
		OSInt k;
		OSInt l;
		Text::Encoding enc(codePage);
		Crypto::Encrypt::FormEncode formEnc;

		buff = req->GetReqData(buffSize);
		if (buff == 0)
			return;
		i = 0;
		j = 0;
		while (true)
		{
			if (i == buffSize || buff[i] == '&')
			{
				k = j;
				while (k < i)
				{
					if (buff[k] == '=')
						break;
					k++;
				}
				if (k - j > tmpBuffSize)
				{
					if (tmpBuff)
						MemFree(tmpBuff);
					tmpBuffSize = k - j + 1024;
					tmpBuff = MemAlloc(UInt8, tmpBuffSize);
				}
				size1 = formEnc.Decrypt(&buff[j], (Int32)(k - j), tmpBuff);
				if (tmpBuffSize2 < size1 + 1)
				{
					if (tmpBuff2)
						MemFree(tmpBuff2);
					tmpBuffSize2 = size1 + 1025;
					tmpBuff2 = MemAlloc(UTF8Char, tmpBuffSize2);
				}
				sptr = enc.UTF8FromBytes(tmpBuff2, tmpBuff, size1, size2);
				l = this->strNames->SortedInsert(Text::StrCopyNewC(tmpBuff2, (UOSInt)(sptr - tmpBuff2)).Ptr());
				if (k < i)
				{
					if (i - k - 1 > tmpBuffSize)
					{
						if (tmpBuff)
							MemFree(tmpBuff);
						tmpBuffSize = i - k + 1023;
						tmpBuff = MemAlloc(UInt8, tmpBuffSize);
					}
					size1 = formEnc.Decrypt(&buff[k + 1], (Int32)(i - k - 1), tmpBuff);
					if (tmpBuffSize2 < size1 + 1)
					{
						if (tmpBuff2)
							MemFree(tmpBuff2);
						tmpBuffSize2 = size1 + 1025;
						tmpBuff2 = MemAlloc(UTF8Char, tmpBuffSize2);
					}
					sptr = enc.UTF8FromBytes(tmpBuff2, tmpBuff, size1, size2);
					this->strValues->Insert(l, Text::StrCopyNewC(tmpBuff2, (UOSInt)(sptr - tmpBuff2)).Ptr());
				}
				else
				{
					this->strValues->Insert(l, Text::StrCopyNewC(UTF8STRC("")).Ptr());
				}

				if (i == buffSize)
					break;
				j = i + 1;
			}
			i++;
		}
		if (tmpBuff)
			MemFree(tmpBuff);
		if (tmpBuff2)
			MemFree(tmpBuff2);
	}
	else if (Text::StrStartsWithC(sb.ToString(), sb.GetLength(), UTF8STRC("multipart/form-data")))
	{
		///////////////////////////////////////////////
	}
	else
	{
		return;
	}
}

Net::WebServer::HTTPFormParser::~HTTPFormParser()
{
	UOSInt i;
	UnsafeArray<const UTF8Char> s;
	i = this->strNames->GetCount();
	while (i-- > 0)
	{
		if (this->strNames->GetItem(i).SetTo(s)) Text::StrDelNew(s);
		if (this->strValues->GetItem(i).SetTo(s)) Text::StrDelNew(s);
	}
	DEL_CLASS(this->strNames);
	DEL_CLASS(this->strValues);
}

UOSInt Net::WebServer::HTTPFormParser::GetStrCount() const
{
	return this->strNames->GetCount();
}

UnsafeArrayOpt<const UTF8Char> Net::WebServer::HTTPFormParser::GetStrName(UOSInt index)
{
	return this->strNames->GetItem(index);
}

UnsafeArrayOpt<const UTF8Char> Net::WebServer::HTTPFormParser::GetStrValue(const UTF8Char *strName)
{
	OSInt i = this->strNames->SortedIndexOf(strName);
	if (i >= 0)
	{
		return this->strValues->GetItem(i);
	}
	return 0;
}
