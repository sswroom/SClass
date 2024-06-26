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

UOSInt Net::Email::EmailMessage::GetHeaderIndex(Text::CStringNN name)
{
	NN<Text::String> header;
	Data::ArrayIterator<NN<Text::String>> it = this->headerList.Iterator();
	UOSInt i = 0;
	while (it.HasNext())
	{
		header = it.Next();
		if (header->StartsWith(name) && header->v[name.leng] == ':' && header->v[name.leng + 1] == ' ')
		{
			return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

Bool Net::Email::EmailMessage::SetHeader(Text::CStringNN name, Text::CStringNN val)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(name);
	sb.AppendUTF8Char(':');
	sb.AppendUTF8Char(' ');
	sb.Append(val);
	UOSInt i = this->GetHeaderIndex(name);
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

Bool Net::Email::EmailMessage::AppendUTF8Header(NN<Text::StringBuilderUTF8> sb, Text::CStringNN val)
{
	Text::TextBinEnc::Base64Enc b64;
	sb->AppendC(UTF8STRC("=?UTF-8?B?"));
	b64.EncodeBin(sb, val.v.Ptr(), val.leng);
	sb->AppendC(UTF8STRC("?="));
	return true;
}

void Net::Email::EmailMessage::GenMultipart(NN<IO::Stream> stm, Text::CStringNN boundary)
{
	stm->Write(CSTR("--").ToByteArray());
	stm->Write(boundary.ToByteArray());
	stm->Write(CSTR("\r\nContent-Type: ").ToByteArray());
	stm->Write(this->contentType->ToByteArray());
	stm->Write(CSTR("\r\nContent-Transfer-Encoding: base64\r\n\r\n").ToByteArray());
	WriteB64Data(stm, this->content, this->contentLen);

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Attachment> att;
	Text::CStringNN mime;
	UOSInt k;
	Data::ArrayIterator<NN<Attachment>> it = this->attachments.Iterator();
	while (it.HasNext())
	{
		att = it.Next();
		stm->Write(CSTR("--").ToByteArray());
		stm->Write(boundary.ToByteArray());
		stm->Write(CSTR("\r\nContent-Type: ").ToByteArray());
		mime = Net::MIME::GetMIMEFromFileName(att->fileName->v, att->fileName->leng);
		stm->Write(mime.ToByteArray());
		stm->Write(CSTR("; name=\"").ToByteArray());
		stm->Write(att->fileName->ToByteArray());
		stm->Write(CSTR("\"\r\nContent-Description: ").ToByteArray());
		stm->Write(att->fileName->ToByteArray());
		stm->Write(CSTR("\r\nContent-Disposition: ").ToByteArray());
		if (att->isInline)
		{
			stm->Write(CSTR("inline;").ToByteArray());
			k = 21 + 7;
		}
		else
		{
			stm->Write(CSTR("attachment;").ToByteArray());
			k = 21 + 11;
		}
		if (k + 13 + att->fileName->leng > LINECHARCNT)
		{
			stm->Write(CSTR("\r\n\tfilename=\"").ToByteArray());
			stm->Write(att->fileName->ToByteArray());
			stm->Write(CSTR("\";").ToByteArray());
			k = 16 + att->fileName->leng;
		}
		else
		{
			stm->Write(CSTR(" filename=\"").ToByteArray());
			stm->Write(att->fileName->ToByteArray());
			stm->Write(CSTR("\";").ToByteArray());
			k += 13 + att->fileName->leng;
		}
		sptr = Text::StrUOSInt(sbuff, att->contentLen);
		if (k + 7 + (UOSInt)(sptr - sbuff) > LINECHARCNT)
		{
			stm->Write(CSTR("\r\n\tsize=").ToByteArray());
			stm->Write(CSTRP(sbuff, sptr).ToByteArray());
			stm->Write(CSTR(";").ToByteArray());
			k = 10 + (UOSInt)(sptr - sbuff);
		}
		else
		{
			stm->Write(CSTR(" size=").ToByteArray());
			stm->Write(CSTRP(sbuff, sptr).ToByteArray());
			stm->Write(CSTR(";").ToByteArray());
			k += 7 + (UOSInt)(sptr - sbuff);
		}
		if (k + 47 > LINECHARCNT)
		{
			stm->Write(CSTR("\r\n\tcreation-date=\"").ToByteArray());
			sptr = Net::WebUtil::Date2Str(sbuff, att->createTime);
			stm->Write(CSTRP(sbuff, sptr).ToByteArray());
			stm->Write(CSTR("\";").ToByteArray());
			k = 21 + (UOSInt)(sptr - sbuff);
		}
		else
		{
			stm->Write(CSTR(" creation-date=\"").ToByteArray());
			sptr = Net::WebUtil::Date2Str(sbuff, att->createTime);
			stm->Write(CSTRP(sbuff, sptr).ToByteArray());
			stm->Write(CSTR("\";").ToByteArray());
			k += 18 + (UOSInt)(sptr - sbuff);
		}
		if (k + 50 > LINECHARCNT)
		{
			stm->Write(CSTR("\r\n\tmodification-date=\"").ToByteArray());
			sptr = Net::WebUtil::Date2Str(sbuff, att->modifyTime);
			stm->Write(CSTRP(sbuff, sptr).ToByteArray());
			stm->Write(CSTR("\"\r\n").ToByteArray());
		}
		else
		{
			stm->Write(CSTR(" modification-date=\"").ToByteArray());
			sptr = Net::WebUtil::Date2Str(sbuff, att->modifyTime);
			stm->Write(CSTRP(sbuff, sptr).ToByteArray());
			stm->Write(CSTR("\"\r\n").ToByteArray());
			k = 21 + (UOSInt)(sptr - sbuff);
		}
		stm->Write(CSTR("Content-ID: <").ToByteArray());
		stm->Write(att->contentId->ToByteArray());
		stm->Write(CSTR(">\r\nContent-Transfer-Encoding: base64\r\n\r\n").ToByteArray());
		WriteB64Data(stm, att->content, att->contentLen);
	}
	stm->Write(CSTR("--").ToByteArray());
	stm->Write(boundary.ToByteArray());
	stm->Write(CSTR("--\r\n").ToByteArray());
}

void Net::Email::EmailMessage::WriteHeaders(NN<IO::Stream> stm)
{
	NN<Text::String> header;
	Data::ArrayIterator<NN<Text::String>> it = this->headerList.Iterator();
	while (it.HasNext())
	{
		header = it.Next();
		stm->Write(header->ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
	}
}

void Net::Email::EmailMessage::WriteContents(NN<IO::Stream> stm)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (this->attachments.GetCount() > 0)
	{
		sptr = GenBoundary(sbuff, this->content, this->contentLen);
		stm->Write(CSTR("Content-Type: multipart/mixed;\r\n\tboundary=\"").ToByteArray());
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("\"\r\n").ToByteArray());
		IO::MemoryStream mstm;
		GenMultipart(mstm, CSTRP(sbuff, sptr));
		stm->Write(CSTR("Content-Length: ").ToByteArray());
		sptr = Text::StrUInt64(sbuff, mstm.GetLength());
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(mstm.GetArray());
	}
	else
	{
		stm->Write(CSTR("Content-Type: ").ToByteArray());
		stm->Write(this->contentType->ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(CSTR("Content-Length: ").ToByteArray());
		sptr = Text::StrUOSInt(sbuff, this->contentLen);
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(Data::ByteArrayR(this->content, this->contentLen));
	}
}

UnsafeArray<UTF8Char> Net::Email::EmailMessage::GenBoundary(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> data, UOSInt dataLen)
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

void Net::Email::EmailMessage::WriteB64Data(NN<IO::Stream> stm, const UInt8 *data, UOSInt dataSize)
{
	Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::Normal, false);
	UTF8Char sbuff[80];
	UnsafeArray<UTF8Char> sptr;
	while (dataSize > 57)
	{
		sptr = b64.EncodeBin(sbuff, data, 57);
		sptr[0] = '\r';
		sptr[1] = '\n';
		sptr += 2;
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		data += 57;
		dataSize -= 57;
	}
	sptr = b64.EncodeBin(sbuff, data, dataSize);
	sptr[0] = '\r';
	sptr[1] = '\n';
	sptr += 2;
	stm->Write(CSTRP(sbuff, sptr).ToByteArray());
}

void Net::Email::EmailMessage::AttachmentFree(NN<Attachment> attachment)
{
	MemFree(attachment->content);
	attachment->fileName->Release();
	attachment->contentId->Release();
	MemFreeNN(attachment);
}

void Net::Email::EmailMessage::EmailAddressFree(NN<EmailAddress> addr)
{
	NN<Text::String> s;
	if (addr->name.SetTo(s))
		s->Release();
	addr->addr->Release();
	MemFreeNN(addr);
}

NN<Net::Email::EmailMessage::EmailAddress> Net::Email::EmailMessage::EmailAddressCreate(RecipientType type, Text::CString name, Text::CStringNN addr)
{
	NN<EmailAddress> ret = MemAllocNN(EmailAddress);
	ret->type = type;
	ret->name = Text::String::NewOrNull(name);
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
		this->AppendUTF8Header(sb, subject);
		this->SetHeader(CSTR("Subject"), sb.ToCString());
	}
	else
	{
		this->SetHeader(CSTR("Subject"), subject);
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
	MemCopyNO(this->content, content.v.Ptr(), content.leng);
	this->contentLen = content.leng;
	return true;
}

Bool Net::Email::EmailMessage::SetSentDate(NN<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::WebUtil::Date2Str(sbuff, dt);
	return this->SetHeader(CSTR("Date"), CSTRP(sbuff, sptr));
}

Bool Net::Email::EmailMessage::SetSentDate(Data::Timestamp ts)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::WebUtil::Date2Str(sbuff, ts);
	return this->SetHeader(CSTR("Date"), CSTRP(sbuff, sptr));
}

Bool Net::Email::EmailMessage::SetMessageId(Text::CStringNN msgId)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUTF8Char('<');
	sb.Append(msgId);
	sb.AppendUTF8Char('>');
	return this->SetHeader(CSTR("Message-ID"), sb.ToCString());
}

void Net::Email::EmailMessage::AddCustomHeader(Text::CStringNN name, Text::CStringNN value)
{
	if (Text::StringTool::IsNonASCII(value.v))
	{
		Text::StringBuilderUTF8 sb;
		this->AppendUTF8Header(sb, value);
		this->SetHeader(name, sb.ToCString());
	}
	else
	{
		this->SetHeader(name, value);
	}
}

Bool Net::Email::EmailMessage::SetFrom(Text::CString name, Text::CStringNN addr)
{
	Text::StringBuilderUTF8 sb;
	Text::CStringNN nnname;
	if (name.SetTo(nnname) && nnname.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(nnname.v))
		{
			this->AppendUTF8Header(sb, nnname);
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(nnname);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(CSTR("From"), sb.ToCString());
	this->fromAddr.FreeBy(EmailAddressFree);
	this->fromAddr = EmailAddressCreate(RecipientType::From, name, addr);
	return true;
}

Bool Net::Email::EmailMessage::AddTo(Text::CString name, Text::CStringNN addr)
{
	UOSInt i = this->GetHeaderIndex(CSTR("To"));
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	if (i != INVALID_INDEX && this->headerList.GetItem(i).SetTo(s))
	{
		sb.Append(s->ToCString().Substring(4));
		sb.AppendC(UTF8STRC(", "));
	}
	Text::CStringNN nnname;
	if (name.SetTo(nnname) && nnname.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(nnname.v))
		{
			this->AppendUTF8Header(sb, nnname);
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(nnname);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(CSTR("To"), sb.ToCString());
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
	UOSInt i = this->GetHeaderIndex(CSTR("Cc"));
	Text::StringBuilderUTF8 sb;
	NN<Text::String> s;
	if (i != INVALID_INDEX && this->headerList.GetItem(i).SetTo(s))
	{
		sb.AppendC(s->v + 4, s->leng - 4);
		sb.AppendC(UTF8STRC(", "));
	}
	Text::CStringNN nnname;
	if (name.SetTo(nnname) && nnname.leng > 0)
	{
		if (Text::StringTool::IsNonASCII(nnname.v))
		{
			this->AppendUTF8Header(sb, nnname);
		}
		else
		{
			sb.AppendUTF8Char('"');
			sb.Append(nnname);
			sb.AppendUTF8Char('"');
		}
		sb.AppendUTF8Char(' ');
	}
	sb.AppendUTF8Char('<');
	sb.Append(addr);
	sb.AppendUTF8Char('>');
	this->SetHeader(CSTR("Cc"), sb.ToCString());
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
	UnsafeArray<UTF8Char> sptr;
	NN<Attachment> attachment = MemAllocNN(Attachment);
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

NN<Net::Email::EmailMessage::Attachment> Net::Email::EmailMessage::AddAttachment(UnsafeArray<const UInt8> content, UOSInt contentLen, Text::CStringNN fileName)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Attachment> attachment = MemAllocNN(Attachment);
	attachment->contentLen = contentLen;
	attachment->content = MemAlloc(UInt8, attachment->contentLen);
	MemCopyNO(attachment->content, content.Ptr(), contentLen);
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

NN<const Data::ArrayListNN<Net::Email::EmailMessage::EmailAddress>> Net::Email::EmailMessage::GetRecpList()
{
	return this->recpList;
}

Bool Net::Email::EmailMessage::WriteToStream(NN<IO::Stream> stm)
{
	if (!this->CompletedMessage())
	{
		return false;
	}
	this->WriteHeaders(stm);
	NN<Net::SSLEngine> nnssl;
	NN<Crypto::Cert::X509Key> signKey;
	if (this->signCert && signKey.Set(this->signKey) && this->ssl.SetTo(nnssl))
	{
		IO::MemoryStream mstm;
		this->WriteContents(mstm);
		mstm.Write(CSTR("\r\n").ToByteArray());

		UInt8 signData[512];
		UOSInt signLen;
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		sptr = GenBoundary(sbuff, mstm.GetBuff(), (UOSInt)mstm.GetLength());
		stm->Write(CSTR("Content-Type: multipart/signed; protocol=\"application/pkcs7-signature\";\r\n micalg=sha-256; boundary=\"").ToByteArray());
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("\"\r\n\r\n").ToByteArray());
		stm->Write(CSTR("This is a cryptographically signed message in MIME format.\r\n").ToByteArray());
		stm->Write(CSTR("\r\n\r\n--").ToByteArray());
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(mstm.GetArray());
		stm->Write(CSTR("\r\n\r\n--").ToByteArray());
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("\r\n").ToByteArray());
		stm->Write(CSTR("Content-Type: application/pkcs7-signature; name=\"smime.p7s\"\r\n").ToByteArray());
		stm->Write(CSTR("Content-Transfer-Encoding: base64\r\n").ToByteArray());
		stm->Write(CSTR("Content-Disposition: attachment; filename=\"smime.p7s\"\r\n").ToByteArray());
		stm->Write(CSTR("Content-Description: S/MIME Cryptographic Signature\r\n\r\n").ToByteArray());

		UnsafeArrayOpt<const UInt8> data;
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
								builder.AppendSequence(data.Ptr(), dataSize);
								data = this->signCert->GetSerialNumber(dataSize);
								builder.AppendInteger(data.Ptr(), dataSize);
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
											builder.AppendSequence(data.Ptr(), dataSize);
											data = this->signCert->GetSerialNumber(dataSize);
											builder.AppendInteger(data.Ptr(), dataSize);
										builder.EndLevel();
									builder.EndLevel();
								builder.EndLevel();
								builder.BeginSequence();
									builder.AppendOIDString(CSTR("1.2.840.113549.1.9.16.2.11")); //id-aa-encrypKeyPref
									builder.BeginSet();
										builder.BeginContentSpecific(0);
											data = this->signCert->GetIssuerNamesSeq(dataSize);
											builder.AppendSequence(data.Ptr(), dataSize);
											data = this->signCert->GetSerialNumber(dataSize);
											builder.AppendInteger(data.Ptr(), dataSize);
										builder.EndLevel();
									builder.EndLevel();
								builder.EndLevel();
							builder.EndLevel();
							builder.BeginSequence();
								builder.AppendOIDString(CSTR("1.2.840.113549.1.1.1")); //rsaEncryption
								builder.AppendNull();
							builder.EndLevel();
							///////////////////////////////////////
							nnssl->Signature(signKey, Crypto::Hash::HashType::SHA256, mstm.GetArray(), signData, signLen);
							builder.AppendOctetString(signData, signLen);
						builder.EndLevel();
					builder.EndLevel();
				builder.EndLevel();
			builder.EndLevel();
		builder.EndLevel();
		
		WriteB64Data(stm, builder.GetBuff(), builder.GetBuffSize());
		stm->Write(CSTR("\r\n--").ToByteArray());
		stm->Write(CSTRP(sbuff, sptr).ToByteArray());
		stm->Write(CSTR("--").ToByteArray());
	}
	else
	{
		this->WriteContents(stm);
	}
	return true;
}

Bool Net::Email::EmailMessage::GenerateMessageID(NN<Text::StringBuilderUTF8> sb, Text::CStringNN mailFrom)
{
	sb->AppendHex64((UInt64)Data::DateTimeUtil::GetCurrTimeMillis());
	sb->AppendUTF8Char('.');
	UInt8 crcVal[4];
	Crypto::Hash::CRC32R crc;
	UOSInt i;
	i = Text::StrIndexOfCharC(mailFrom.v, mailFrom.leng, '@');
	crc.Calc((UInt8*)mailFrom.v.Ptr(), i);
	crc.GetValue((UInt8*)&crcVal);
	sb->AppendHex32(ReadMUInt32(crcVal));
	sb->AppendC(&mailFrom.v[i], mailFrom.leng - i);
	return true;
}
