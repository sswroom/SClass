#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/MIME.h"
#include "Text/IMIMEObj.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::IMIMEObj::IMIMEObj(Text::CString sourceName) : IO::ParsedObject(sourceName)
{
}

Text::IMIMEObj::~IMIMEObj()
{
}

IO::ParserType Text::IMIMEObj::GetParserType() const
{
	return IO::ParserType::MIMEObject;
}

Text::IMIMEObj *Text::IMIMEObj::ParseFromData(NotNullPtr<IO::StreamData> data, Text::CString contentType)
{
	Text::IMIMEObj *obj;
	UOSInt buffSize;
	if (data->GetDataSize() > 104857600)
	{
		return 0;
	}
	if (contentType.leng == 0)
	{
		buffSize = (UOSInt)data->GetDataSize();
		Data::ByteBuffer buff(buffSize);
		data->GetRealData(0, buffSize, buff);
		NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(buff.Ptr(), buffSize, 0));
		return obj;
	}
	else if (contentType.StartsWith(UTF8STRC("message/rfc822")))
	{
		obj = Text::MIMEObj::MailMessage::ParseFile(data);
		if (obj)
			return obj;
	}
	else if (contentType.StartsWith(UTF8STRC("text/plain")))
	{
		UOSInt i = contentType.IndexOf(UTF8STRC("charset="));
		UInt32 codePage = 0;
		if (i != INVALID_INDEX && i > 0)
		{
			Text::StringBuilderUTF8 sb;
			UOSInt j;
			sb.AppendC(&contentType.v[i + 8], contentType.leng - i - 8);
			j = sb.IndexOf(';');
			if (j != INVALID_INDEX)
			{
				sb.TrimToLength(j);
			}
			Text::EncodingFactory encFact;
			codePage = encFact.GetCodePage(sb.ToCString());
		}

		buffSize = (UOSInt)data->GetDataSize();
		Data::ByteBuffer buff(buffSize);
		data->GetRealData(0, buffSize, buff);
		NEW_CLASS(obj, Text::MIMEObj::TextMIMEObj(buff.Ptr(), buffSize, codePage));
		return obj;
	}
	else if (contentType.StartsWith(UTF8STRC("multipart/mixed;")) ||
			 contentType.StartsWith(UTF8STRC("multipart/related;")) ||
			 contentType.StartsWith(UTF8STRC("multipart/signed;")) ||
			 contentType.StartsWith(UTF8STRC("multipart/alternative;")))
	{
		obj = Text::MIMEObj::MultipartMIMEObj::ParseFile(contentType, data);
		if (obj)
			return obj;
	}
	buffSize = (UOSInt)data->GetDataSize();
	Data::ByteBuffer buff(buffSize);
	data->GetRealData(0, buffSize, buff);
	NEW_CLASS(obj, Text::MIMEObj::UnknownMIMEObj(buff.Ptr(), buffSize, contentType));
	return obj;
}

Text::IMIMEObj *Text::IMIMEObj::ParseFromFile(Text::CString fileName)
{
	Text::CString contentType;
	Text::IMIMEObj *obj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = IO::Path::GetFileExt(sbuff, fileName.v, fileName.leng);
	contentType = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr));
	IO::StmData::FileData fd(fileName, false);
	if (fd.GetDataSize() <= 0)
	{
		obj = 0;
	}
	else
	{
		obj = ParseFromData(fd, contentType);
	}
	return obj;
}
