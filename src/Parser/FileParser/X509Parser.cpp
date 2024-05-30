#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509CRL.h"
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Cert/X509PKCS7.h"
#include "Crypto/Cert/X509PKCS12.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Data/ByteBuffer.h"
#include "IO/MemoryReadingStream.h"
#include "Net/ASN1Util.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Base64Enc.h"

Parser::FileParser::X509Parser::X509Parser()
{
}

Parser::FileParser::X509Parser::~X509Parser()
{
}

Int32 Parser::FileParser::X509Parser::GetName()
{
	return *(Int32*)"X509";
}

void Parser::FileParser::X509Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ASN1Data)
	{
		selector->AddFilter(CSTR("*.crt"), CSTR("X.509 Certification File"));
		selector->AddFilter(CSTR("*.csr"), CSTR("X.509 Certification Request"));
		selector->AddFilter(CSTR("*.p7b"), CSTR("PKCS 7 Certification File"));
		selector->AddFilter(CSTR("*.p7s"), CSTR("PKCS 7 Signature File"));
		selector->AddFilter(CSTR("*.p12"), CSTR("PKCS 12 KeyStore File"));
		selector->AddFilter(CSTR("*.pfx"), CSTR("PKCS 12 KeyStore File"));
		selector->AddFilter(CSTR("*.pem"), CSTR("PEM File"));
		selector->AddFilter(CSTR("*.der"), CSTR("DER File"));
		selector->AddFilter(CSTR("*.cer"), CSTR("CER File"));
		selector->AddFilter(CSTR("*.req"), CSTR("PKCS 10 Request File"));
		selector->AddFilter(CSTR("*.crl"), CSTR("Certification Revocation List"));
	}
}

IO::ParserType Parser::FileParser::X509Parser::GetParserType()
{
	return IO::ParserType::ASN1Data;
}

Optional<IO::ParsedObject> Parser::FileParser::X509Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 buff[10240];
	UInt64 len = fd->GetDataSize();
	if (targetType != IO::ParserType::Unknown && targetType != IO::ParserType::ASN1Data)
	{
		return 0;
	}
	NN<Text::String> fileName = fd->GetFullFileName();
	if (len > sizeof(buff))
	{
		if (fileName->EndsWithICase(UTF8STRC(".CRL")) && len <= 10485760)
		{
			Data::ByteBuffer tmpBuff(len);
			if (fd->GetRealData(0, (UOSInt)len, tmpBuff) != len)
			{
				return 0;
			}
			return ParseBuff(tmpBuff, fileName);
		}
		else
		{
			return 0;
		}
	}

	fd->GetRealData(0, (UOSInt)len, BYTEARR(buff));
	return ParseBuff(BYTEARR(buff).WithSize(len), fd->GetFullFileName());
}

Optional<Crypto::Cert::X509File> Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR buff, NN<Text::String> fileName)
{
	Crypto::Cert::X509File *ret = 0;
	UInt8 dataBuff[10240];
	UnsafeArray<UTF8Char> sptr;
	UOSInt dataLen;
	if (buff[0] == 0xEF && buff[1] == 0xBB && buff[2] == 0xBF)
	{
		buff = buff.SubArray(3);
	}
	if (Text::StrStartsWithC(buff.Arr(), 5, UTF8STRC("-----")))
	{
		IO::MemoryReadingStream mstm(buff);
		Text::UTF8Reader reader(mstm);
		Text::StringBuilderUTF8 sb;
		Text::TextBinEnc::Base64Enc b64;
		Crypto::Cert::X509FileList *fileList = 0;
		Crypto::Cert::X509File *file = 0;
		while (reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
		{
			if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN CERTIFICATE-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END CERTIFICATE-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				NEW_CLASSNN(certFile, Crypto::Cert::X509Cert(fileName, Data::ByteArrayR(dataBuff, dataLen)));
				if (fileList)
				{
					fileList->AddFile(certFile);
				}
				else
				{
					file = certFile.Ptr();
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN RSA PRIVATE KEY-----")))
			{
				Bool enc = false;
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END RSA PRIVATE KEY-----")))
					{
						break;
					}
					else if (Text::StrStartsWithC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("Proc-Type:")))
					{
						enc = true;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				if (!enc)
				{
					dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
					NN<Crypto::Cert::X509Cert> certFile;
					if (certFile.Set((Crypto::Cert::X509Cert*)file))
					{
						NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
						file = 0;
					}
					if (fileList)
					{
						NN<Crypto::Cert::X509Key> subFile;
						NEW_CLASSNN(subFile, Crypto::Cert::X509Key(fileName, Data::ByteArrayR(dataBuff, dataLen), Crypto::Cert::X509File::KeyType::RSA));
						fileList->AddFile(subFile);
					}
					else
					{
						NEW_CLASS(file, Crypto::Cert::X509Key(fileName, Data::ByteArrayR(dataBuff, dataLen), Crypto::Cert::X509File::KeyType::RSA));
					}
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN DSA PRIVATE KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END DSA PRIVATE KEY-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509Key> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509Key(fileName, Data::ByteArrayR(dataBuff, dataLen), Crypto::Cert::X509File::KeyType::DSA));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509Key(fileName, Data::ByteArrayR(dataBuff, dataLen), Crypto::Cert::X509File::KeyType::DSA));
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN EC PRIVATE KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END EC PRIVATE KEY-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509Key> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509Key(fileName, Data::ByteArrayR(dataBuff, dataLen), Crypto::Cert::X509File::KeyType::ECDSA));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509Key(fileName, Data::ByteArrayR(dataBuff, dataLen), Crypto::Cert::X509File::KeyType::ECDSA));
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN PRIVATE KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END PRIVATE KEY-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509PrivKey> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509PrivKey(fileName, Data::ByteArrayR(dataBuff, dataLen)));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509PrivKey(fileName, Data::ByteArrayR(dataBuff, dataLen)));
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN PUBLIC KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END PUBLIC KEY-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509PubKey> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509PubKey(fileName, Data::ByteArrayR(dataBuff, dataLen)));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509PubKey(fileName, Data::ByteArrayR(dataBuff, dataLen)));
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN CERTIFICATE REQUEST-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END CERTIFICATE REQUEST-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509CertReq> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509CertReq(fileName, Data::ByteArrayR(dataBuff, dataLen)));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509CertReq(fileName, Data::ByteArrayR(dataBuff, dataLen)));
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN NEW CERTIFICATE REQUEST-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END NEW CERTIFICATE REQUEST-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509CertReq> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509CertReq(fileName, Data::ByteArrayR(dataBuff, dataLen)));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509CertReq(fileName, Data::ByteArrayR(dataBuff, dataLen)));
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN PKCS7-----")))
			{
				sb.ClearStr();
				while (true)
				{
					if (!reader.ReadLine(dataBuff, sizeof(dataBuff) - 1).SetTo(sptr))
					{
						SDEL_CLASS(fileList);
						SDEL_CLASS(file);
						return 0;
					}
					if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----END PKCS7-----")))
					{
						break;
					}
					else
					{
						sb.AppendP(dataBuff, sptr);
					}
				}
				dataLen = b64.DecodeBin(sb.ToCString(), dataBuff);
				NN<Crypto::Cert::X509Cert> certFile;
				if (certFile.Set((Crypto::Cert::X509Cert*)file))
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, certFile));
					file = 0;
				}
				if (fileList)
				{
					NN<Crypto::Cert::X509PKCS7> subFile;
					NEW_CLASSNN(subFile, Crypto::Cert::X509PKCS7(fileName, Data::ByteArrayR(dataBuff, dataLen)));
					fileList->AddFile(subFile);
				}
				else
				{
					NEW_CLASS(file, Crypto::Cert::X509PKCS7(fileName, Data::ByteArrayR(dataBuff, dataLen)));
				}
			}
		}
		if (fileList)
			return fileList;
		else
			return file;
	}
	else if (buff[0] == 0x30 && Net::ASN1Util::PDUIsValid(buff.Arr(), buff.ArrEnd()))
	{
		if (fileName->EndsWithICase(UTF8STRC(".P12")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS12(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".PFX")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS12(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".DER")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".CER")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".CRT")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".P7B")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS7(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".P7S")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS7(fileName, buff));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".CRL")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509CRL(fileName, buff));
		}
		else
		{
			return 0;
		}
	}
	return ret;
}

Optional<Crypto::Cert::X509File> Parser::FileParser::X509Parser::ToType(NN<IO::ParsedObject> pobj, Crypto::Cert::X509File::FileType ftype)
{
	NN<Net::ASN1Data> asn1;
	if (pobj->GetParserType() != IO::ParserType::ASN1Data)
	{
		pobj.Delete();
		return 0;
	}
	asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		pobj.Delete();
		return 0;
	}
	NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
	if (x509->GetFileType() == ftype)
	{
		return x509;
	}
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		NN<Crypto::Cert::X509Key> key = NN<Crypto::Cert::X509Key>::ConvertFrom(x509);
		if (ftype == Crypto::Cert::X509File::FileType::PrivateKey)
		{
			if (key->IsPrivateKey())
			{
				Optional<Crypto::Cert::X509PrivKey> pkey = Crypto::Cert::X509PrivKey::CreateFromKey(key);
				key.Delete();
				return pkey;
			}
		}
	}
	x509.Delete();
	return 0;
}

Crypto::Cert::X509File *Parser::FileParser::X509Parser::ParseBinary(Data::ByteArrayR buff)
{
	if (Crypto::Cert::X509File::IsCertificate(buff.Arr(), buff.ArrEnd(), "1"))
	{
		return NEW_CLASS_D(Crypto::Cert::X509Cert(CSTR("Certificate.crt"), buff));
	}
	return 0;
}
