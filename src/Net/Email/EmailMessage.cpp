#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/SHA1.h"
#include "Crypto/Hash/SHA256.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/MIME.h"
#include "Net/WebUtil.h"
#include "Net/Email/EmailMessage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"
#include "Text/TextBinEnc/Base64Enc.h"

#define LINECHARCNT 77

UOSInt Net::Email::EmailMessage::GetHeaderIndex(const UTF8Char *name, UOSInt nameLen)
{
	NotNullPtr<Text::String> header;
	Data::ArrayIterator<NotNullPtr<Text::String>> it = this->headerList.Iterator();
	UOSInt i = 0;
	while (it.HasNext())
	{
		header = it.Next();
		if (header->StartsWith(name, nameLen) && header->v[nameLen] == ':' && header->v[nameLen + 1] == ' ')
		{
			return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

Bool Net::Email::EmailMessage::SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val, UOSInt valLen)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(name, nameLen);
	sb.AppendUTF8Char(':');
	sb.AppendUTF8Char(' ');
	sb.AppendC(val, valLen);
	UOSInt i = this->GetHeaderIndex(name, nameLen);
	if (i == INVALID_INDEX)
	{
		this->headerList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
	}
	else
	{
		OPTSTR_DEL(this->headerList.GetItem(i));
		this->headerList.SetItem(i, Text::String::New(sb.ToString(), sb.GetLength()));
	}
	return true;
}

Bool Net::Email::EmailMessage::AppendUTF8Header(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *val, UOSInt valLen)
{
	Text::TextBinEnc::Base64Enc b64;
	sb->AppendC(UTF8STRC("=?UTF-8?B?"));
	b64.EncodeBin(sb, val, valLen);
	sb->AppendC(UTF8STRC("?="));
	return true;
}

void Net::Email::EmailMessage::GenMultipart(NotNullPtr<IO::Stream> stm, Text::CString boundary)
{
	stm->Write(UTF8STRC("--"));
	stm->Write(boundary.v, boundary.leng);
	stm->Write(UTF8STRC("\r\nContent-Type: "));
	stm->Write(this->contentType->v, this->contentType->leng);
	stm->Write(UTF8STRC("\r\nContent-Transfer-Encoding: base64\r\n\r\n"));
	WriteB64Data(stm, this->content, this->contentLen);

	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<Attachment> att;
	Text::CString mime;
	UOSInt k;
	Data::ArrayIterator<NotNullPtr<Attachment>> it = this->attachments.Iterator();
	while (it.HasNext())
	{
		att = it.Next();
		stm->Write(UTF8STRC("--"));
		stm->Write(boundary.v, boundary.leng);
		stm->Write(UTF8STRC("\r\nContent-Type: "));
		mime = Net::MIME::GetMIMEFromFileName(att->fileName->v, att->fileName->leng);
		stm->Write(mime.v, mime.leng);
		stm->Write(UTF8STRC("; name=\""));
		stm->Write(att->fileName->v, att->fileName->leng);
		stm->Write(UTF8STRC("\"\r\nContent-Description: "));
		stm->Write(att->fileName->v, att->fileName->leng);
		stm->Write(UTF8STRC("\r\nContent-Disposition: "));
		if (att->isInline)
		{
			stm->Write(UTF8STRC("inline;"));
			k = 21 + 7;
		}
		else
		{
			stm->Write(UTF8STRC("attachment;"));
			k = 21 + 11;
		}
		if (k + 13 + att->fileName->leng > LINECHARCNT)
		{
			stm->Write(UTF8STRC("\r\n\tfilename=\""));
			stm->Write(att->fileName->v, att->fileName->leng);
			stm->Write(UTF8STRC("\";"));
			k = 16 + att->fileName->leng;
		}
		else
		{
			stm->Write(UTF8STRC(" filename=\""));
			stm->Write(att->fileName->v, att->fileName->leng);
			stm->Write(UTF8STRC("\";"));
			k += 13 + att->fileName->leng;
		}
		sptr = Text::StrUOSInt(sbuff, att->contentLen);
		if (k + 7 + (UOSInt)(sptr - sbuff) > LINECHARCNT)
		{
			stm->Write(UTF8STRC("\r\n\tsize="));
			stm->Write(sbuff, (UOSInt)(sptr - sbuff));
			stm->Write(UTF8STRC(";"));
			k = 10 + (UOSInt)(sptr - sbuff);
		}
		else
		{
			stm->Write(UTF8STRC(" size="));
			stm->Write(sbuff, (UOSInt)(sptr - sbuff));
			stm->Write(UTF8STRC(";"));
			k += 7 + (UOSInt)(sptr - sbuff);
		}
		if (k + 47 > LINECHARCNT)
		{
			stm->Write(UTF8STRC("\r\n\tcreation-date=\""));
			sptr = Net::WebUtil::Date2Str(sbuff, att->createTime);
			stm->Write(sbuff, (UOSInt)(sptr - sbuff));
			stm->Write(UTF8STRC("\";"));
			k = 21 + (UOSInt)(sptr - sbuff);
		}
		else
		{
			stm->Write(UTF8STRC(" creation-date=\""));
			sptr = Net::WebUtil::Date2Str(sbuff, att->createTime);
			stm->Write(sbuff, (UOSInt)(sptr - sbuff));
			stm->Write(UTF8STRC("\";"));
			k += 18 + (UOSInt)(sptr - sbuff);
		}
		if (k + 50 > LINECHARCNT)
		{
			stm->Write(UTF8STRC("\r\n\tmodification-date=\""));
			sptr = Net::WebUtil::Date2Str(sbuff, att->modifyTime);
			stm->Write(sbuff, (UOSInt)(sptr - sbuff));
			stm->Write(UTF8STRC("\"\r\n"));
		}
		else
		{
			stm->Write(UTF8STRC(" modification-date=\""));
			sptr = Net::WebUtil::Date2Str(sbuff, att->modifyTime);
			stm->Write(sbuff, (UOSInt)(sptr - sbuff));
			stm->Write(UTF8STRC("\"\r\n"));
			k = 21 + (UOSInt)(sptr - sbuff);
		}
		stm->Write(UTF8STRC("Content-ID: <"));
		stm->Write(att->contentId->v, att->contentId->leng);
		stm->Write(UTF8STRC(">\r\nContent-Transfer-Encoding: base64\r\n\r\n"));
		WriteB64Data(stm, att->content, att->contentLen);
	}
	stm->Write(UTF8STRC("--"));
	stm->Write(boundary.v, boundary.leng);
	stm->Write(UTF8STRC("--\r\n"));
}

void Net::Email::EmailMessage::WriteHeaders(NotNullPtr<IO::Stream> stm)
{
	NotNullPtr<Text::String> header;
	Data::ArrayIterator<NotNullPtr<Text::String>> it = this->headerList.Iterator();
	while (it.HasNext())
	{
		header = it.Next();
		stm->Write(header->v, header->leng);
		stm->Write((const UInt8*)"\r\n", 2);
	}
}

void Net::Email::EmailMessage::WriteContents(NotNullPtr<IO::Stream> stm)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (this->attachments.GetCount() > 0)
	{
		sptr = GenBoundary(sbuff, this->content, this->contentLen);
		stm->Write(UTF8STRC("Content-Type: multipart/mixed;\r\n\tboundary=\""));
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write(UTF8STRC("\"\r\n"));
		IO::MemoryStream mstm;
		GenMultipart(mstm, CSTRP(sbuff, sptr));
		stm->Write(UTF8STRC("Content-Length: "));
		sptr = Text::StrUInt64(sbuff, mstm.GetLength());
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write((const UInt8*)"\r\n", 2);
		stm->Write((const UInt8*)"\r\n", 2);
		stm->Write(mstm.GetBuff(), (UOSInt)mstm.GetLength());
	}
	else
	{
		stm->Write(UTF8STRC("Content-Type: "));
		stm->Write(this->contentType->v, this->contentType->leng);
		stm->Write((const UInt8*)"\r\n", 2);
		stm->Write(UTF8STRC("Content-Length: "));
		sptr = Text::StrUOSInt(sbuff, this->contentLen);
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write((const UInt8*)"\r\n", 2);
		stm->Write((const UInt8*)"\r\n", 2);
		stm->Write(this->content, this->contentLen);
	}
}

UTF8Char *Net::Email::EmailMessage::GenBoundary(UTF8Char *sbuff, const UInt8 *data, UOSInt dataLen)
{
	Int64 ts = Data::DateTimeUtil::GetCurrTimeMillis();
	Crypto::Hash::SHA1 sha1;
	UInt8 sha1Val[20];
	sha1.Calc((const UInt8*)&ts, sizeof(ts));
	sha1.Calc(data, dataLen);
	sha1.GetValue(sha1Val);
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
	return b64.EncodeBin(sbuff, sha1Val, 20);
}

void Net::Email::EmailMessage::WriteB64Data(NotNullPtr<IO::Stream> stm, const UInt8 *data, UOSInt dataSize)
{
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::Normal, false);
	UTF8Char sbuff[80];
	UTF8Char *sptr;
	while (dataSize > 57)
	{
		sptr = b64.EncodeBin(sbuff, data, 57);
		sptr[0] = '\r';
		sptr[1] = '\n';
		sptr += 2;
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		data += 57;
		dataSize -= 57;
	}
	sptr = b64.EncodeBin(sbuff, data, dataSize);
	sptr[0] = '\r';
	sptr[1] = '\n';
	sptr += 2;
	stm->Write(sbuff, (UOSInt)(sptr - sbuff));
}

void Net::Email::EmailMessage::AttachmentFree(NotNullPtr<Attachment> attachment)
{
	MemFree(attachment->content);
	attachment->fileName->Release();
	attachment->contentId->Release();
	MemFreeNN(attachment);
}

void Net::Email::EmailMessage::EmailAddressFree(NotNullPtr<EmailAddress> addr)
{
	NotNullPtr<Text::String> s;
	if (addr->name.SetTo(s))
		s->Release();
	addr->addr->Release();
	MemFreeNN(addr);
}

NotNullPtr<Net::Email::EmailMessage::EmailAddress> Net::Email::EmailMessage::EmailAddressCreate(RecipientType type, Text::CString name, Text::CStringNN addr)
{
	NotNullPtr<EmailAddress> ret = MemAllocNN(EmailAddress, 1);
	ret->type = type;
	if (name.leng > 0)
		ret->name = Text::String::New(name);
	else
		ret->name = 0;
	ret->addr = Text::String::New(addr);
	return ret;
}

Net::Email::EmailMessage::EmailMessage()
{
	this->fromAddr = 0;
	this->content = 0;
	this->contentLen = 0;
	this->contentType = 0;
	this->signCert = 0;
	this->signKey = 0;
}

Net::Email::EmailMessage::~EmailMessage()
{
	this->fromAddr.FreeBy(EmailAddressFree);
	this->recpList.FreeAll(EmailAddressFree);
	this->headerList.FreeAll();
	SDEL_STRING(this->contentType);
	if (this->content)
	{
		MemFree(this->content);
	}
	this->attachments.FreeAll(AttachmentFree);
	SDEL_CLASS(this->signCert);
	SDEL_CLASS(this->signKey);
}

Bool Net::Email::EmailMessage::SetSubject(Text::CStringNN subject)
{
	if (Text::StringTool::IsNonASCII(subject.v))
	{
		Text::StringBuilderUTF8 sb;
		this->AppendUTF8Header(sb, subject.v, subject.leng);
		this->SetHeader(UTF8STRC("Subject"), sb.ToString(), sb.GetLength());
	}
	else
	{
		this->SetHeader(UTF8STRC("Subject"), subject.v, subject.leng);
	}
	return true;
}

Bool Net::Email::EmailMessage::SetContent(Text::CStringNN content, Text::CStringNN contentType)
{
	SDEL_STRING(this->contentType);
	this->contentType = Text::String::New(contentType).Ptr();
	if (this->content)
		MemFree(this->content);
	this->content = MemAlloc(UInt8, content.leng);
	MemCopyNO(this->content, content.v, content.leng);
	this->contentLen = content.leng;
	return true;
}

Bool Net::Email::EmailMessage::SetSentDate(NotNullPtr<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = Net::WebUtil::Date2Str(sbuff, dt);
	return this->SetHeader(UTF8STRC("Date"), sbuff, (UOSInt)(sptr - sbuff));
}

Bool Net::Email::EmailMessage::SetMessageId(Text::CString msgId)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('<');
	sb.Append(msgId);
	sb.AppendUTF8Char('>');
	return this->SetHeader(UTF8STRC("Message-ID"), sb.ToString(), sb.GetLength());
}

void Net::Email::EmailMessage::AddCustomHeader(Text::CStringNN name, Text::CStringNN value)
{
	if (Text::StringTool::IsNonASCII(value.v))
	{
		Text::StringBuilderUTF8 sb;
		this->AppendUTF8Header(sb, value.v, value.leng);
		this->SetHeader(name.v, name.leng, sb.ToString(), sb.GetLength());
	}
	else
	{
		this->SetHeader(name.v, name.leng, value.v, value.leng);
	}
}

Bool Net::Email::EmailMessage::SetFrom(Text::CString name, Text::CStringNN addr)
{
	Text::StringBuilderUTF8 sb;
	if (name.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(name.v))
		{
			this->AppendUTF8Header(sb, name.v, name.leng);			
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(name);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(UTF8STRC("From"), sb.ToString(), sb.GetLength());
	this->fromAddr.FreeBy(EmailAddressFree);
	this->fromAddr = EmailAddressCreate(RecipientType::From, name, addr);
	return true;
}

Bool Net::Email::EmailMessage::AddTo(Text::CString name, Text::CStringNN addr)
{
	UOSInt i = this->GetHeaderIndex(UTF8STRC("To"));
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Text::String> s;
	if (i != INVALID_INDEX && this->headerList.GetItem(i).SetTo(s))
	{
		sb.Append(s->ToCString().Substring(4));
		sb.AppendC(UTF8STRC(", "));
	}
	if (name.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(name.v))
		{
			this->AppendUTF8Header(sb, name.v, name.leng);			
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(name);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(UTF8STRC("To"), sb.ToString(), sb.GetLength());
	this->recpList.Add(EmailAddressCreate(RecipientType::To, name, addr));
	return true;
}

Bool Net::Email::EmailMessage::AddToList(Text::CStringNN addrs)
{
	Bool succ;
	UOSInt i;
	Text::PString sarr[2];
	Text::StringBuilderUTF8 sb;
	sb.Append(addrs);
	sarr[1] = sb;
	succ = true;
	while (true)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
		if (!Text::StringTool::IsEmailAddress(sarr[0].v))
		{
			succ = false;
		}
		else
		{
			succ = succ && this->AddTo(CSTR_NULL, sarr[0].ToCString());
		}
		if (i == 1)
			break;
	}
	return succ;
}

Bool Net::Email::EmailMessage::AddCc(Text::CString name, Text::CStringNN addr)
{
	UOSInt i = this->GetHeaderIndex(UTF8STRC("Cc"));
	Text::StringBuilderUTF8 sb;
	NotNullPtr<Text::String> s;
	if (i != INVALID_INDEX && this->headerList.GetItem(i).SetTo(s))
	{
		sb.AppendC(s->v + 4, s->leng - 4);
		sb.AppendC(UTF8STRC(", "));
	}
	if (name.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(name.v))
		{
			this->AppendUTF8Header(sb, name.v, name.leng);
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(name);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(UTF8STRC("Cc"), sb.ToString(), sb.GetLength());
	this->recpList.Add(EmailAddressCreate(RecipientType::Cc, name, addr));
	return true;
}

Bool Net::Email::EmailMessage::AddBcc(Text::CStringNN addr)
{
	this->recpList.Add(EmailAddressCreate(RecipientType::Bcc, CSTR_NULL, addr));
	return true;
}

Optional<Net::Email::EmailMessage::Attachment> Net::Email::EmailMessage::AddAttachment(Text::CStringNN fileName)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return 0;
	}
	UInt64 len = fs.GetLength();
	if (len > 104857600)
	{
		return 0;
	}
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<Attachment> attachment = MemAllocNN(Attachment, 1);
	attachment->contentLen = (UOSInt)len;
	attachment->content = MemAlloc(UInt8, attachment->contentLen);
	if (fs.Read(Data::ByteArray(attachment->content, attachment->contentLen)) != attachment->contentLen)
	{
		MemFree(attachment->content);
		MemFreeNN(attachment);
		return 0;
	}
	attachment->createTime.SetValue(0, 0);
	attachment->modifyTime.SetValue(0, 0);
	fs.GetFileTimes(&attachment->createTime, 0, &attachment->modifyTime);
	attachment->fileName = Text::String::New(fileName.Substring(fileName.LastIndexOf(IO::Path::PATH_SEPERATOR) + 1));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("attach")), this->attachments.GetCount() + 1);
	attachment->contentId = Text::String::NewP(sbuff, sptr);
	attachment->isInline = false;
	this->attachments.Add(attachment);
	return attachment;
}

NotNullPtr<Net::Email::EmailMessage::Attachment> Net::Email::EmailMessage::AddAttachment(const UInt8 *content, UOSInt contentLen, Text::CString fileName)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	NotNullPtr<Attachment> attachment = MemAllocNN(Attachment, 1);
	attachment->contentLen = contentLen;
	attachment->content = MemAlloc(UInt8, attachment->contentLen);
	MemCopyNO(attachment->content, content, contentLen);
	attachment->createTime.SetCurrTimeUTC();
	attachment->modifyTime.SetValue(attachment->createTime);
	attachment->fileName = Text::String::New(fileName.Substring(fileName.LastIndexOf(IO::Path::PATH_SEPERATOR) + 1));
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("attach")), this->attachments.GetCount() + 1);
	attachment->contentId = Text::String::NewP(sbuff, sptr);
	attachment->isInline = false;
	this->attachments.Add(attachment);
	return attachment;
}

Bool Net::Email::EmailMessage::AddSignature(Optional<Net::SSLEngine> ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509Key *key)
{
	SDEL_CLASS(this->signCert);
	SDEL_CLASS(this->signKey);
	this->ssl = ssl;
	this->signCert = cert;
	this->signKey = key;
	return cert != 0 && key != 0;
}

Bool Net::Email::EmailMessage::CompletedMessage()
{
	if (this->fromAddr.IsNull() || this->recpList.GetCount() == 0 || this->contentLen == 0)
	{
		return false;
	}
	return true;
}

Optional<Net::Email::EmailMessage::EmailAddress> Net::Email::EmailMessage::GetFrom()
{
	return this->fromAddr;
}

NotNullPtr<const Data::ArrayListNN<Net::Email::EmailMessage::EmailAddress>> Net::Email::EmailMessage::GetRecpList()
{
	return this->recpList;
}

Bool Net::Email::EmailMessage::WriteToStream(NotNullPtr<IO::Stream> stm)
{
	if (!this->CompletedMessage())
	{
		return false;
	}
	this->WriteHeaders(stm);
	NotNullPtr<Net::SSLEngine> nnssl;
	NotNullPtr<Crypto::Cert::X509Key> signKey;
	if (this->signCert && signKey.Set(this->signKey) && this->ssl.SetTo(nnssl))
	{
		IO::MemoryStream mstm;
		this->WriteContents(mstm);
		mstm.Write(UTF8STRC("\r\n"));

		UInt8 signData[512];
		UOSInt signLen;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		sptr = GenBoundary(sbuff, mstm.GetBuff(), (UOSInt)mstm.GetLength());
		stm->Write(UTF8STRC("Content-Type: multipart/signed; protocol=\"application/pkcs7-signature\";\r\n micalg=sha-256; boundary=\""));
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write(UTF8STRC("\"\r\n\r\n"));
		stm->Write(UTF8STRC("This is a cryptographically signed message in MIME format.\r\n"));
		stm->Write(UTF8STRC("\r\n\r\n--"));
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write(UTF8STRC("\r\n"));
		stm->Write(mstm.GetBuff(), (UOSInt)mstm.GetLength());
		stm->Write(UTF8STRC("\r\n\r\n--"));
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write(UTF8STRC("\r\n"));
		stm->Write(UTF8STRC("Content-Type: application/pkcs7-signature; name=\"smime.p7s\"\r\n"));
		stm->Write(UTF8STRC("Content-Transfer-Encoding: base64\r\n"));
		stm->Write(UTF8STRC("Content-Disposition: attachment; filename=\"smime.p7s\"\r\n"));
		stm->Write(UTF8STRC("Content-Description: S/MIME Cryptographic Signature\r\n\r\n"));

		const UInt8 *data;
		UOSInt dataSize;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Net::ASN1PDUBuilder builder;
		builder.BeginSequence();
			builder.AppendOIDString(CSTR("1.2.840.113549.1.7.2")); //signedData
			builder.BeginContentSpecific(0);
				builder.BeginSequence();
					builder.AppendInt32(1);
					builder.BeginSet();
						builder.BeginSequence();
							builder.AppendOIDString(CSTR("2.16.840.1.101.3.4.2.1")); //id-sha256
							builder.AppendNull();
						builder.EndLevel();
					builder.EndLevel();
					builder.BeginSequence();
						builder.AppendOIDString(CSTR("1.2.840.113549.1.7.1")); //data
					builder.EndLevel();
					builder.AppendContentSpecific(0, this->signCert->GetASN1Buff(), this->signCert->GetASN1BuffSize());
					builder.BeginSet();
						builder.BeginSequence();
							builder.AppendInt32(1);
							builder.BeginSequence();
								data = this->signCert->GetIssuerNamesSeq(dataSize);
								builder.AppendSequence(data, dataSize);
								data = this->signCert->GetSerialNumber(dataSize);
								builder.AppendInteger(data, dataSize);
							builder.EndLevel();
							builder.BeginSequence();
								builder.AppendOIDString(CSTR("2.16.840.1.101.3.4.2.1")); //id-sha256
								builder.AppendNull();
							builder.EndLevel();
							builder.BeginContentSpecific(0);
								builder.BeginSequence();
									builder.AppendOIDString(CSTR("1.2.840.113549.1.9.3")); //contentType
									builder.BeginSet();
										builder.AppendOIDString(CSTR("1.2.840.113549.1.7.1")); //data
									builder.EndLevel();
								builder.EndLevel();
								builder.BeginSequence();
									builder.AppendOIDString(CSTR("1.2.840.113549.1.9.5")); //signing-time
									builder.BeginSet();
										builder.AppendUTCTime(dt);
									builder.EndLevel();
								builder.EndLevel();
								{
									Crypto::Hash::SHA256 sha;
									sha.Calc(mstm.GetBuff(), (UOSInt)mstm.GetLength());
									sha.GetValue(signData);

									builder.BeginSequence();
										builder.AppendOIDString(CSTR("1.2.840.113549.1.9.4")); //messageDigest
										builder.BeginSet();
											builder.AppendOctetString(signData, 32);
										builder.EndLevel();
									builder.EndLevel();
								}
								builder.BeginSequence();
									builder.AppendOIDString(CSTR("1.2.840.113549.1.9.15")); //smimeCapabilities
									builder.BeginSet();
										builder.BeginSequence();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("2.16.840.1.101.3.4.1.42")); //aes256-CBC
											builder.EndLevel();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("2.16.840.1.101.3.4.1.2")); //aes128-CBC
											builder.EndLevel();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("1.2.840.113549.3.7")); //des-ede3-cbc
											builder.EndLevel();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("1.2.840.113549.3.2")); //rc2CBC
												builder.AppendInt32(128);
											builder.EndLevel();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("1.2.840.113549.3.2")); //rc2CBC
												builder.AppendInt32(64);
											builder.EndLevel();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("1.3.14.3.2.7")); //desCBC
											builder.EndLevel();
											builder.BeginSequence();
												builder.AppendOIDString(CSTR("1.2.840.113549.3.2")); //rc2CBC
												builder.AppendInt32(40);
											builder.EndLevel();
										builder.EndLevel();
									builder.EndLevel();
								builder.EndLevel();
								builder.BeginSequence();
									builder.AppendOIDString(CSTR("1.3.6.1.4.1.311.16.4")); //outlookExpress
									builder.BeginSet();
										builder.BeginSequence();
											data = this->signCert->GetIssuerNamesSeq(dataSize);
											builder.AppendSequence(data, dataSize);
											data = this->signCert->GetSerialNumber(dataSize);
											builder.AppendInteger(data, dataSize);
										builder.EndLevel();
									builder.EndLevel();
								builder.EndLevel();
								builder.BeginSequence();
									builder.AppendOIDString(CSTR("1.2.840.113549.1.9.16.2.11")); //id-aa-encrypKeyPref
									builder.BeginSet();
										builder.BeginContentSpecific(0);
											data = this->signCert->GetIssuerNamesSeq(dataSize);
											builder.AppendSequence(data, dataSize);
											data = this->signCert->GetSerialNumber(dataSize);
											builder.AppendInteger(data, dataSize);
										builder.EndLevel();
									builder.EndLevel();
								builder.EndLevel();
							builder.EndLevel();
							builder.BeginSequence();
								builder.AppendOIDString(CSTR("1.2.840.113549.1.1.1")); //rsaEncryption
								builder.AppendNull();
							builder.EndLevel();
							///////////////////////////////////////
							nnssl->Signature(signKey, Crypto::Hash::HashType::SHA256, mstm.GetBuff(), (UOSInt)mstm.GetLength(), signData, signLen);
							builder.AppendOctetString(signData, signLen);
						builder.EndLevel();
					builder.EndLevel();
				builder.EndLevel();
			builder.EndLevel();
		builder.EndLevel();
		
		WriteB64Data(stm, builder.GetBuff(), builder.GetBuffSize());
		stm->Write(UTF8STRC("\r\n--"));
		stm->Write(sbuff, (UOSInt)(sptr - sbuff));
		stm->Write(UTF8STRC("--"));
	}
	else
	{
		this->WriteContents(stm);
	}
	return true;
}

Bool Net::Email::EmailMessage::GenerateMessageID(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString mailFrom)
{
	sb->AppendHex64((UInt64)Data::DateTimeUtil::GetCurrTimeMillis());
	sb->AppendUTF8Char('.');
	UInt8 crcVal[4];
	Crypto::Hash::CRC32R crc;
	UOSInt i;
	i = Text::StrIndexOfCharC(mailFrom.v, mailFrom.leng, '@');
	crc.Calc((UInt8*)mailFrom.v, i);
	crc.GetValue((UInt8*)&crcVal);
	sb->AppendHex32(ReadMUInt32(crcVal));
	sb->AppendC(&mailFrom.v[i], mailFrom.leng - i);
	return true;
}
