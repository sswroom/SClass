#include "Stdafx.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA256.h"
#include "Exporter/PPKExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/TextBinEnc/Base64Enc.h"

Exporter::PPKExporter::PPKExporter()
{

}

Exporter::PPKExporter::~PPKExporter()
{

}

Int32 Exporter::PPKExporter::GetName()
{
	return *(Int32*)"PPKE";
}

IO::FileExporter::SupportType Exporter::PPKExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
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
		if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSA)
		{
			return IO::FileExporter::SupportType::NormalStream;
		}
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::PPKExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("PuTTY Private Key file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.ppk"));
		return true;
	}
	return false;
}

Bool Exporter::PPKExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
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
		if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSA)
		{
			UnsafeArray<const UInt8> modulus;
			UOSInt modulusSize;
			UnsafeArray<const UInt8> publicExponent;
			UOSInt publicExponentSize;
			UnsafeArray<const UInt8> privateExponent;
			UOSInt privateExponentSize;
			UnsafeArray<const UInt8> prime1;
			UOSInt prime1Size;
			UnsafeArray<const UInt8> prime2;
			UOSInt prime2Size;
			UnsafeArray<const UInt8> coefficient;
			UOSInt coefficientSize;
			UInt8 hash[32];
			if (key->GetRSAModulus(modulusSize).SetTo(modulus) &&
				key->GetRSAPublicExponent(publicExponentSize).SetTo(publicExponent) &&
				key->GetRSAPrivateExponent(privateExponentSize).SetTo(privateExponent) &&
				key->GetRSAPrime1(prime1Size).SetTo(prime1) &&
				key->GetRSAPrime2(prime2Size).SetTo(prime2) &&
				key->GetRSACoefficient(coefficientSize).SetTo(coefficient))
			{
				Crypto::Hash::SHA256 sha256;
				Crypto::Hash::HMAC hmac(sha256, UTF8STRC(""));
				Bool succ;
				Text::StringBuilderUTF8 sb;
				UOSInt i;
				Text::CStringNN name;
				sb.Append(CSTR("PuTTY-User-Key-File-3: ssh-rsa\n"));
				sb.Append(CSTR("Encryption: none\n"));
				sb.Append(CSTR("Comment: "));
				WriteMUInt32(hash, 7);
				hmac.Calc(hash, 4);
				hmac.Calc(UTF8STRC("ssh-rsa"));
				WriteMUInt32(hash, 4);
				hmac.Calc(hash, 4);
				hmac.Calc(UTF8STRC("none"));
				i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
				name = fileName.Substring(i + 1);
				i = name.LastIndexOf('.');
				if (i == INVALID_INDEX)
				{
					sb.Append(name);
					WriteMUInt32(hash, name.leng);
					hmac.Calc(hash, 4);
					hmac.Calc(name.v, name.leng);
				}
				else
				{
					sb.AppendC(name.v, i);
					WriteMUInt32(hash, i);
					hmac.Calc(hash, 4);
					hmac.Calc(name.v, i);
				}
				sb.AppendUTF8Char('\n');
				Text::TextBinEnc::Base64Enc b64;
				UnsafeArray<UInt8> buff;
				UOSInt buffSize = 11 + modulusSize + 4 + publicExponentSize + 4;
				buff = MemAllocArr(UInt8, buffSize);
				WriteMUInt32(&buff[0], 7);
				MemCopyNO(&buff[4], "ssh-rsa", 7);
				i = 11;
				WriteMUInt32(&buff[i], publicExponentSize);
				MemCopyNO(&buff[i + 4], publicExponent.Ptr(), publicExponentSize);
				i += 4 + publicExponentSize;
				WriteMUInt32(&buff[i], modulusSize);
				MemCopyNO(&buff[i + 4], modulus.Ptr(), modulusSize);
				i += 4 + modulusSize;
				sb.Append(CSTR("Public-Lines: "));
				sb.AppendUOSInt(i / 48 + ((i % 48)?1:0));
				sb.AppendUTF8Char('\n');
				b64.EncodeBin(sb, buff, i, Text::LineBreakType::LF, 64);
				WriteMUInt32(hash, i);
				hmac.Calc(hash, 4);
				hmac.Calc(buff, i);
				sb.AppendUTF8Char('\n');
				MemFreeArr(buff);

				buffSize = privateExponentSize + 4 + prime1Size + 4 + prime2Size + 4 + coefficientSize + 4;
				buff = MemAllocArr(UInt8, buffSize);
				i = 0;
				WriteMUInt32(&buff[i], privateExponentSize);
				MemCopyNO(&buff[i + 4], privateExponent.Ptr(), privateExponentSize);
				i += 4 + privateExponentSize;
				WriteMUInt32(&buff[i], prime1Size);
				MemCopyNO(&buff[i + 4], prime1.Ptr(), prime1Size);
				i += 4 + prime1Size;
				WriteMUInt32(&buff[i], prime2Size);
				MemCopyNO(&buff[i + 4], prime2.Ptr(), prime2Size);
				i += 4 + prime2Size;
				WriteMUInt32(&buff[i], coefficientSize);
				MemCopyNO(&buff[i + 4], coefficient.Ptr(), coefficientSize);
				i += 4 + coefficientSize;
				sb.Append(CSTR("Private-Lines: "));
				sb.AppendUOSInt(i / 48 + ((i % 48)?1:0));
				sb.AppendUTF8Char('\n');
				b64.EncodeBin(sb, buff, i, Text::LineBreakType::LF, 64);
				WriteMUInt32(hash, i);
				hmac.Calc(hash, 4);
				hmac.Calc(buff, i);
				sb.AppendUTF8Char('\n');
				MemFreeArr(buff);
				
				hmac.GetValue(hash);
				sb.Append(CSTR("Private-MAC: "));
				sb.AppendHexBuffLC(hash, 32, 0, Text::LineBreakType::None);
				sb.AppendUTF8Char('\n');
				{
					IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					succ = (fs.WriteCont(sb.v, sb.leng) == sb.leng);
				}
				return succ;
			}
		}
	}
	return false;
}
