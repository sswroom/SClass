#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/MIME.h"
#include "Text/IMIMEObj.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::IMIMEObj::IMIMEObj(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
}

Text::IMIMEObj::~IMIMEObj()
{
}

IO::ParserType Text::IMIMEObj::GetParserType()
{
	return IO::ParserType::MIMEObject;
}

Text::IMIMEObj *Text::IMIMEObj::ParseFromData(IO::IStreamData *data, const UTF8Char *contentType, UOSInt typeLen)
{
	Text::IMIMEObj *obj;
	UOSInt buffSize;
	UInt8 *buff;
	if (data->GetDataSize() > 104857600)
	{
		return 0;
	}
	if (contentType == 0)
	{
		buffSize = (UOSInt)data->GetDataSize();
		buff = MemAlloc(UInt8, buffSize);
		data->GetRealData(0, buffSize, buff);
		NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(buff, buffSize, 0));
		MemFree(buff);
		return obj;
	}
	else if (Text::StrStartsWithC(contentType, typeLen, UTF8STRC("message/rfc822")))
	{
		obj = Text::MIMEObj::MailMessage::ParseFile(data);
		if (obj)
			return obj;
	}
	else if (Text::StrStartsWithC(contentType, typeLen, UTF8STRC("text/plain")))
	{
		UOSInt i = Text::StrIndexOf(contentType, (const UTF8Char*)"charset=");
		UInt32 codePage = 0;
		if (i != INVALID_INDEX && i > 0)
		{
			Text::StringBuilderUTF8 sb;
			UOSInt j;
			sb.Append(&contentType[i + 8]);
			j = Text::StrIndexOf(sb.ToString(), (const UTF8Char*)";");
			if (j != INVALID_INDEX)
			{
				sb.TrimToLength(j);
			}
			Text::EncodingFactory encFact;
			codePage = encFact.GetCodePage(sb.ToString());
		}

		buffSize = (UOSInt)data->GetDataSize();
		buff = MemAlloc(UInt8, buffSize);
		data->GetRealData(0, buffSize, buff);
		NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(buff, buffSize, codePage));
		MemFree(buff);
		return obj;
	}
	else if (Text::StrStartsWithC(contentType, typeLen, UTF8STRC("multipart/mixed;")) || Text::StrStartsWithC(contentType, typeLen, UTF8STRC("multipart/related;")) || Text::StrStartsWithC(contentType, typeLen, UTF8STRC("multipart/alternative;")))
	{
		obj = Text::MIMEObj::MultipartMIMEObj::ParseFile(contentType, data);
		if (obj)
			return obj;
	}
	buffSize = (UOSInt)data->GetDataSize();
	buff = MemAlloc(UInt8, buffSize);
	data->GetRealData(0, buffSize, buff);
	NEW_CLASS(obj, Text::MIMEObj::UnknownMIMEObj(buff, buffSize, contentType));
	MemFree(buff);
	return obj;
}

Text::IMIMEObj *Text::IMIMEObj::ParseFromFile(const UTF8Char *fileName)
{
	Text::CString contentType;
	IO::StmData::FileData *fd;
	Text::IMIMEObj *obj;
	UTF8Char sbuff[64];
	IO::Path::GetFileExt(sbuff, fileName);
	contentType = Net::MIME::GetMIMEFromExt(sbuff);
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	if (fd->GetDataSize() <= 0)
	{
		obj = 0;
	}
	else
	{
		obj = ParseFromData(fd, contentType.v, contentType.len);
	}
	DEL_CLASS(fd);
	return obj;
}
