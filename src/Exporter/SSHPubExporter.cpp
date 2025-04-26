#include "Stdafx.h"
#include "Crypto/Cert/SSHPubKey.h"
#include "Crypto/Cert/X509Key.h"
#include "Exporter/SSHPubExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/TextBinEnc/Base64Enc.h"

Exporter::SSHPubExporter::SSHPubExporter()
{

}

Exporter::SSHPubExporter::~SSHPubExporter()
{

}

Int32 Exporter::SSHPubExporter::GetName()
{
	return *(Int32*)"SSHP";
}

IO::FileExporter::SupportType Exporter::SSHPubExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
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
	NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
		UnsafeArray<const UInt8> modulus;
		UOSInt modulusSize;
		UnsafeArray<const UInt8> publicExponent;
		UOSInt publicExponentSize;
		if (key->GetRSAModulus(modulusSize).SetTo(modulus) && key->GetRSAPublicExponent(publicExponentSize).SetTo(publicExponent))
		{
			return IO::FileExporter::SupportType::NormalStream;
		}
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::SSHPubExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Pub file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.pub"));
		return true;
	}
	return false;
}

Bool Exporter::SSHPubExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
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
	NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
		UnsafeArray<const UInt8> modulus;
		UOSInt modulusSize;
		UnsafeArray<const UInt8> publicExponent;
		UOSInt publicExponentSize;
		NN<Crypto::Cert::SSHPubKey> skey;
		if (key->GetRSAModulus(modulusSize).SetTo(modulus) && key->GetRSAPublicExponent(publicExponentSize).SetTo(publicExponent))
		{
			Bool succ;
			Text::StringBuilderUTF8 sb;
			UOSInt i;
			Text::CStringNN name;
			skey = Crypto::Cert::SSHPubKey::CreateRSAPublicKey(fileName, Data::ByteArrayR(modulus, modulusSize), Data::ByteArrayR(publicExponent, publicExponentSize));
			sb.Append(CSTR("ssh-rsa "));
			Text::TextBinEnc::Base64Enc b64;
			b64.EncodeBin(sb, skey->GetArr(), skey->GetSize());
			sb.AppendUTF8Char(' ');
			i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
			name = fileName.Substring(i + 1);
			i = name.LastIndexOf('.');
			if (i == INVALID_INDEX)
			{
				sb.Append(name);
			}
			else
			{
				sb.AppendC(name.v, i);
			}
			sb.AppendUTF8Char('\n');
			{
				IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				succ = (fs.WriteCont(sb.v, sb.leng) == sb.leng);
			}
			skey.Delete();
			return succ;
		}
	}
	return false;
}
