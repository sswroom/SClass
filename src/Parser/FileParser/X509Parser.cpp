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

void Parser::FileParser::X509Parser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ASN1Data)
	{
		selector->AddFilter(CSTR("*.crt"), CSTR("X.509 Certification File"));
		selector->AddFilter(CSTR("*.csr"), CSTR("X.509 Certification Request"));
		selector->AddFilter(CSTR("*.p7b"), CSTR("PKCS 7 Certification File"));
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

IO::ParsedObject *Parser::FileParser::X509Parser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 buff[10240];
	UInt64 len = fd->GetDataSize();
	if (targetType != IO::ParserType::Unknown && targetType != IO::ParserType::ASN1Data)
	{
		return 0;
	}
	NotNullPtr<Text::String> fileName = fd->GetFullFileName();
	if (len > sizeof(buff))
	{
		if (fileName->EndsWithICase(UTF8STRC(".CRL")) && len <= 10485760)
		{
			UInt8 *tmpBuff = MemAlloc(UInt8, len);
			if (fd->GetRealData(0, (UOSInt)len, tmpBuff) != len)
			{
				MemFree(tmpBuff);
				return 0;
			}
			IO::ParsedObject *pobj = ParseBuff(tmpBuff, len, fileName);
			MemFree(tmpBuff);
			return pobj;
		}
		else
		{
			return 0;
		}
	}

	fd->GetRealData(0, (UOSInt)len, buff);
	return ParseBuff(buff, (UOSInt)len, fd->GetFullFileName());
}

Crypto::Cert::X509File *Parser::FileParser::X509Parser::ParseBuff(const UInt8 *buff, UOSInt buffSize, NotNullPtr<Text::String> fileName)
{
	Crypto::Cert::X509File *ret = 0;
	UInt8 dataBuff[10240];
	UTF8Char *sptr;
	UOSInt dataLen;
	if (buff[0] == 0xEF && buff[1] == 0xBB && buff[2] == 0xBF)
	{
		buff += 3;
		buffSize -= 3;
	}
	if (Text::StrStartsWithC(buff, 5, UTF8STRC("-----")))
	{
		IO::MemoryReadingStream mstm(buff, buffSize);
		Text::UTF8Reader reader(mstm);
		Text::StringBuilderUTF8 sb;
		Text::TextBinEnc::Base64Enc b64;
		Crypto::Cert::X509FileList *fileList = 0;
		Crypto::Cert::X509File *file = 0;
		while ((sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1)) != 0)
		{
			if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN CERTIFICATE-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509Cert(fileName, dataBuff, dataLen));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN RSA PRIVATE KEY-----")))
			{
				Bool enc = false;
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
					dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
					if (file)
					{
						NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
						file = 0;
					}
					NEW_CLASS(file, Crypto::Cert::X509Key(fileName, dataBuff, dataLen, Crypto::Cert::X509File::KeyType::RSA));
					if (fileList)
					{
						fileList->AddFile(file);
						file = 0;
					}
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN DSA PRIVATE KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509Key(fileName, dataBuff, dataLen, Crypto::Cert::X509File::KeyType::DSA));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN EC PRIVATE KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509Key(fileName, dataBuff, dataLen, Crypto::Cert::X509File::KeyType::ECDSA));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN PRIVATE KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509PrivKey(fileName, dataBuff, dataLen));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN PUBLIC KEY-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509PubKey(fileName, dataBuff, dataLen));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN CERTIFICATE REQUEST-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509CertReq(fileName, dataBuff, dataLen));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN NEW CERTIFICATE REQUEST-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509CertReq(fileName, dataBuff, dataLen));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
			else if (Text::StrEqualsC(dataBuff, (UOSInt)(sptr - dataBuff), UTF8STRC("-----BEGIN PKCS7-----")))
			{
				sb.ClearStr();
				while (true)
				{
					sptr = reader.ReadLine(dataBuff, sizeof(dataBuff) - 1);
					if (sptr == 0)
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
				dataLen = b64.DecodeBin(sb.ToString(), sb.GetLength(), dataBuff);
				if (file)
				{
					NEW_CLASS(fileList, Crypto::Cert::X509FileList(fileName, (Crypto::Cert::X509Cert*)file));
					file = 0;
				}
				NEW_CLASS(file, Crypto::Cert::X509PKCS7(fileName, dataBuff, dataLen));
				if (fileList)
				{
					fileList->AddFile(file);
					file = 0;
				}
			}
		}
		if (fileList)
			return fileList;
		else
			return file;
	}
	else if (buff[0] == 0x30)
	{
		if (fileName->EndsWithICase(UTF8STRC(".P12")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS12(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".PFX")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS12(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".DER")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".CER")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".CRT")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509Cert(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".P7B")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS7(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".P7S")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509PKCS7(fileName, buff, buffSize));
		}
		else if (fileName->EndsWithICase(UTF8STRC(".CRL")))
		{
			NEW_CLASS(ret, Crypto::Cert::X509CRL(fileName, buff, buffSize));
		}
		else
		{
			return 0;
		}
	}
	return ret;
}

Crypto::Cert::X509File *Parser::FileParser::X509Parser::ToType(IO::ParsedObject *pobj, Crypto::Cert::X509File::FileType ftype)
{
	Net::ASN1Data *asn1;
	if (pobj == 0)
	{
		return 0;
	}
	if (pobj->GetParserType() != IO::ParserType::ASN1Data)
	{
		DEL_CLASS(pobj);
		return 0;
	}
	asn1 = (Net::ASN1Data*)pobj;
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		DEL_CLASS(pobj);
		return 0;
	}
	Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
	if (x509->GetFileType() == ftype)
	{
		return x509;
	}
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		Crypto::Cert::X509Key *key = (Crypto::Cert::X509Key*)x509;
		if (ftype == Crypto::Cert::X509File::FileType::PrivateKey)
		{
			if (key->IsPrivateKey())
			{
				Crypto::Cert::X509PrivKey *pkey = Crypto::Cert::X509PrivKey::CreateFromKey(key);
				DEL_CLASS(key);
				return pkey;
			}
		}
	}
	DEL_CLASS(x509);
	return 0;
}

Crypto::Cert::X509File *Parser::FileParser::X509Parser::ParseBinary(const UInt8 *buff, UOSInt buffSize)
{
	if (Crypto::Cert::X509File::IsCertificate(buff, buff + buffSize, "1"))
	{
		return NEW_CLASS_D(Crypto::Cert::X509Cert(CSTR("Certificate.crt"), buff, buffSize));
	}
	return 0;
}
