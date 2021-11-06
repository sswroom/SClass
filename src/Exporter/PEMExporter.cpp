#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Exporter/PEMExporter.h"
#include "IO/FileStream.h"
#include "Net/ASN1Data.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"

Exporter::PEMExporter::PEMExporter()
{

}

Exporter::PEMExporter::~PEMExporter()
{

}

Int32 Exporter::PEMExporter::GetName()
{
	return *(Int32*)"PEME";
}

IO::FileExporter::SupportType Exporter::PEMExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ASN1Data)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::PEMExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	switch (index)
	{
	case 0:
		Text::StrConcat(nameBuff, (const UTF8Char*)"PEM file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.pem");
		return true;
	case 1:
		Text::StrConcat(nameBuff, (const UTF8Char*)"Cert file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.crt");
		return true;
	case 2:
		Text::StrConcat(nameBuff, (const UTF8Char*)"Key file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.key");
		return true;
	case 3:
		Text::StrConcat(nameBuff, (const UTF8Char*)"Cert Sign Request file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.csr");
		return true;
	}
	return false;
}

Bool Exporter::PEMExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ASN1Data)
	{
		return false;
	}
	Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		return false;
	}
	return ExportStream(stm, (Crypto::Cert::X509File*)asn1);
}

Bool Exporter::PEMExporter::ExportStream(IO::SeekableStream *stm, Crypto::Cert::X509File *x509)
{
	Text::TextBinEnc::Base64Enc b64;
	Text::StringBuilderUTF8 sb;
	switch (x509->GetFileType())
	{
	case Crypto::Cert::X509File::FileType::Cert:
		sb.Append((const UTF8Char*)"-----BEGIN CERTIFICATE-----\n");
		b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.Append((const UTF8Char*)"\n-----END CERTIFICATE-----\n");
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::CertRequest:
		sb.Append((const UTF8Char*)"-----BEGIN CERTIFICATE REQUEST-----\n");
		b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.Append((const UTF8Char*)"\n-----END CERTIFICATE REQUEST-----\n");
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::Jks:
		return false;
	case Crypto::Cert::X509File::FileType::Key:
		{
			Crypto::Cert::X509Key *key = (Crypto::Cert::X509Key*)x509;
			switch (key->GetKeyType())
			{
			case Crypto::Cert::X509Key::KeyType::RSA:
				sb.Append((const UTF8Char*)"-----BEGIN RSA PRIVATE KEY-----\n");
				b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
				sb.Append((const UTF8Char*)"\n-----END RSA PRIVATE KEY-----\n");
				return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
			case Crypto::Cert::X509Key::KeyType::DSA:
				sb.Append((const UTF8Char*)"-----BEGIN DSA PRIVATE KEY-----\n");
				b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
				sb.Append((const UTF8Char*)"\n-----END DSA PRIVATE KEY-----\n");
				return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
			case Crypto::Cert::X509Key::KeyType::ECDSA:
				sb.Append((const UTF8Char*)"-----BEGIN EC PRIVATE KEY-----\n");
				b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
				sb.Append((const UTF8Char*)"\n-----END EC PRIVATE KEY-----\n");
				return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
			case Crypto::Cert::X509Key::KeyType::RSAPublic:
			case Crypto::Cert::X509Key::KeyType::ED25519:
			case Crypto::Cert::X509Key::KeyType::Unknown:
			default:
				return false;
			}
		}
		return false;
	case Crypto::Cert::X509File::FileType::PrivateKey:
		sb.Append((const UTF8Char*)"-----BEGIN PRIVATE KEY-----\n");
		b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.Append((const UTF8Char*)"\n-----END PRIVATE KEY-----\n");
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::PublicKey:
		sb.Append((const UTF8Char*)"-----BEGIN PUBLIC KEY-----\n");
		b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.Append((const UTF8Char*)"\n-----END PUBLIC KEY-----\n");
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::PKCS7:
		sb.Append((const UTF8Char*)"-----BEGIN PKCS7-----\n");
		b64.EncodeBin(&sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.Append((const UTF8Char*)"\n-----END PKCS7-----\n");
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::PKCS12:
		break;
	}
	return false;
}

Bool Exporter::PEMExporter::ExportFile(const UTF8Char *fileName, Crypto::Cert::X509File *x509)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	Bool succ = ExportStream(fs, x509);
	DEL_CLASS(fs);
	return succ;
}
