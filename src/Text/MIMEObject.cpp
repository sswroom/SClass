#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/MIME.h"
#include "Text/MIMEObject.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MailMessage.h"
#include "Text/MIMEObj/MultipartMIMEObj.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::MIMEObject::MIMEObject(Text::CStringNN sourceName) : IO::ParsedObject(sourceName)
{
}

Text::MIMEObject::~MIMEObject()
{
}

IO::ParserType Text::MIMEObject::GetParserType() const
{
	return IO::ParserType::MIMEObject;
}

Optional<Text::MIMEObject> Text::MIMEObject::ParseFromData(NN<IO::StreamData> data, Text::CStringNN contentType)
{
	NN<Text::MIMEObject> obj;
	UIntOS buffSize;
	if (data->GetDataSize() > 104857600)
	{
		return nullptr;
	}
	if (contentType.leng == 0)
	{
		buffSize = (UIntOS)data->GetDataSize();
		Data::ByteBuffer buff(buffSize);
		data->GetRealData(0, buffSize, buff);
		NEW_CLASSNN(obj, Text::MIMEObj::TextMIMEObj(buff.Arr(), buffSize, 0));
		return obj;
	}
	else if (contentType.StartsWith(UTF8STRC("message/rfc822")))
	{
		if (Optional<MIMEObject>::ConvertFrom(Text::MIMEObj::MailMessage::ParseFile(data)).SetTo(obj))
			return obj;
	}
	else if (contentType.StartsWith(UTF8STRC("text/plain")))
	{
		UIntOS i = contentType.IndexOf(UTF8STRC("charset="));
		UInt32 codePage = 0;
		if (i != INVALID_INDEX && i > 0)
		{
			Text::StringBuilderUTF8 sb;
			UIntOS j;
			sb.AppendC(&contentType.v[i + 8], contentType.leng - i - 8);
			j = sb.IndexOf(';');
			if (j != INVALID_INDEX)
			{
				sb.TrimToLength(j);
			}
			Text::EncodingFactory encFact;
			codePage = encFact.GetCodePage(sb.ToCString());
		}

		buffSize = (UIntOS)data->GetDataSize();
		Data::ByteBuffer buff(buffSize);
		data->GetRealData(0, buffSize, buff);
		NEW_CLASSNN(obj, Text::MIMEObj::TextMIMEObj(buff.Arr(), buffSize, codePage));
		return obj;
	}
	else if (contentType.StartsWith(UTF8STRC("multipart/mixed;")) ||
			 contentType.StartsWith(UTF8STRC("multipart/related;")) ||
			 contentType.StartsWith(UTF8STRC("multipart/signed;")) ||
			 contentType.StartsWith(UTF8STRC("multipart/alternative;")))
	{
		if (Optional<MIMEObject>::ConvertFrom(Text::MIMEObj::MultipartMIMEObj::ParseFile(contentType, data)).SetTo(obj))
			return obj;
	}
	buffSize = (UIntOS)data->GetDataSize();
	Data::ByteBuffer buff(buffSize);
	data->GetRealData(0, buffSize, buff);
	NEW_CLASSNN(obj, Text::MIMEObj::UnknownMIMEObj(buff.Arr(), buffSize, contentType));
	return obj;
}

Optional<Text::MIMEObject> Text::MIMEObject::ParseFromFile(Text::CStringNN fileName)
{
	Text::CStringNN contentType;
	Optional<Text::MIMEObject> obj;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetFileExt(sbuff, fileName.v, fileName.leng);
	contentType = Net::MIME::GetMIMEFromExt(CSTRP(sbuff, sptr));
	IO::StmData::FileData fd(fileName, false);
	if (fd.GetDataSize() <= 0)
	{
		obj = nullptr;
	}
	else
	{
		obj = ParseFromData(fd, contentType);
	}
	return obj;
}
