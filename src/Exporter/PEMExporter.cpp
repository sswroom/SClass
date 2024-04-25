#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
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

IO::FileExporter::SupportType Exporter::PEMExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ASN1Data)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
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
		Text::StrConcatC(nameBuff, UTF8STRC("PEM file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.pem"));
		return true;
	case 1:
		Text::StrConcatC(nameBuff, UTF8STRC("Cert file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.crt"));
		return true;
	case 2:
		Text::StrConcatC(nameBuff, UTF8STRC("Key file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.key"));
		return true;
	case 3:
		Text::StrConcatC(nameBuff, UTF8STRC("Cert Sign Request file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.csr"));
		return true;
	}
	return false;
}

Bool Exporter::PEMExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::ASN1Data)
	{
		return false;
	}
	NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		return false;
	}
	return ExportStream(stm, NN<Crypto::Cert::X509File>::ConvertFrom(asn1));
}

Bool Exporter::PEMExporter::ExportStream(NN<IO::SeekableStream> stm, NN<Crypto::Cert::X509File> x509)
{
	Text::TextBinEnc::Base64Enc b64;
	Text::StringBuilderUTF8 sb;
	switch (x509->GetFileType())
	{
	case Crypto::Cert::X509File::FileType::Cert:
		sb.AppendC(UTF8STRC("-----BEGIN CERTIFICATE-----\n"));
		b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.AppendC(UTF8STRC("\n-----END CERTIFICATE-----\n"));
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::CertRequest:
		sb.AppendC(UTF8STRC("-----BEGIN CERTIFICATE REQUEST-----\n"));
		b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.AppendC(UTF8STRC("\n-----END CERTIFICATE REQUEST-----\n"));
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::Key:
		{
			NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
			switch (key->GetKeyType())
			{
			case Crypto::Cert::X509Key::KeyType::RSA:
				sb.AppendC(UTF8STRC("-----BEGIN RSA PRIVATE KEY-----\n"));
				b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
				sb.AppendC(UTF8STRC("\n-----END RSA PRIVATE KEY-----\n"));
				return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
			case Crypto::Cert::X509Key::KeyType::DSA:
				sb.AppendC(UTF8STRC("-----BEGIN DSA PRIVATE KEY-----\n"));
				b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
				sb.AppendC(UTF8STRC("\n-----END DSA PRIVATE KEY-----\n"));
				return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
			case Crypto::Cert::X509Key::KeyType::ECDSA:
				sb.AppendC(UTF8STRC("-----BEGIN EC PRIVATE KEY-----\n"));
				b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
				sb.AppendC(UTF8STRC("\n-----END EC PRIVATE KEY-----\n"));
				return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
			case Crypto::Cert::X509Key::KeyType::ECPublic:
			case Crypto::Cert::X509Key::KeyType::RSAPublic:
			case Crypto::Cert::X509Key::KeyType::ED25519:
			case Crypto::Cert::X509Key::KeyType::Unknown:
			default:
				return false;
			}
		}
		return false;
	case Crypto::Cert::X509File::FileType::PrivateKey:
		sb.AppendC(UTF8STRC("-----BEGIN PRIVATE KEY-----\n"));
		b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.AppendC(UTF8STRC("\n-----END PRIVATE KEY-----\n"));
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::PublicKey:
		sb.AppendC(UTF8STRC("-----BEGIN PUBLIC KEY-----\n"));
		b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.AppendC(UTF8STRC("\n-----END PUBLIC KEY-----\n"));
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::PKCS7:
		sb.AppendC(UTF8STRC("-----BEGIN PKCS7-----\n"));
		b64.EncodeBin(sb, x509->GetASN1Buff(), x509->GetASN1BuffSize(), Text::LineBreakType::LF, 64);
		sb.AppendC(UTF8STRC("\n-----END PKCS7-----\n"));
		return stm->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
	case Crypto::Cert::X509File::FileType::FileList:
	{
		NN<Crypto::Cert::X509FileList> fileList = NN<Crypto::Cert::X509FileList>::ConvertFrom(x509);
		NN<Crypto::Cert::X509File> file;
		UOSInt i = 0;
		UOSInt j = fileList->GetFileCount();
		while (i < j)
		{
			if (fileList->GetFile(i).SetTo(file))
				if (!ExportStream(stm, file))
					return false;
			i++;
		}
		return true;
	}
	case Crypto::Cert::X509File::FileType::PKCS12:
	case Crypto::Cert::X509File::FileType::CRL:
		break;
	}
	return false;
}

Bool Exporter::PEMExporter::ExportFile(Text::CStringNN fileName, NN<Crypto::Cert::X509File> x509)
{
	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Bool succ = ExportStream(fs, x509);
	return succ;
}
