#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509EPrivKey.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/JKSParser.h"
#include "Parser/FileParser/X509Parser.h"

#include <stdio.h>

Parser::FileParser::JKSParser::JKSParser()
{
}

Parser::FileParser::JKSParser::~JKSParser()
{
}

Int32 Parser::FileParser::JKSParser::GetName()
{
	return *(Int32*)"JKSP";
}

void Parser::FileParser::JKSParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SectorData)
	{
		selector->AddFilter(CSTR("*.jks"), CSTR("Java KeyStore"));
	}
}

IO::ParserType Parser::FileParser::JKSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::JKSParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt64 fileSize = fd->GetDataSize();
	UInt32 magic = ReadMUInt32(&hdr[0]);
	if (magic != 0xFEEDFEED && magic != 0xCECECECE)
	{
		return 0;
	}
	UInt32 version = ReadMUInt32(&hdr[4]);
	if (version > 2)
	{
		return 0;
	}
	UInt8 buff[256];
	Data::ByteBuffer cerBuff(0);
	Text::StringBuilderUTF8 sb;
	IO::VirtualPackageFile *pkg;
	NEW_CLASS(pkg, IO::VirtualPackageFileFast(fd->GetFullFileName()));
	UInt32 cnt = ReadMUInt32(&hdr[8]);
	UInt64 ofst = 12;
	UOSInt readSize;
	UOSInt i = 0;
	while (i < cnt && ofst < fileSize)
	{
		readSize = fd->GetRealData(ofst, 256, BYTEARR(buff));
		if (readSize < 25)
		{
			printf("JKS: readSize < 25\r\n");
			break;
		}
		UInt32 tag = ReadMUInt32(buff);
		if (tag == 1)
		{
			UOSInt aliasLen = ReadMUInt16(&buff[4]);
			Int64 ts = ReadMInt64(&buff[6 + aliasLen]);
			UInt32 certLen = ReadMUInt32(&buff[14 + aliasLen]);
			sb.ClearStr();
			sb.AppendC(&buff[6], aliasLen);
			sb.AppendC(UTF8STRC(".key"));
			if (cerBuff.GetSize() < certLen)
			{
				cerBuff.ChangeSizeAndClear(certLen);
			}
			NN<Text::String> s = Text::String::New(sb.ToCString());
			fd->GetRealData(ofst + 18 + aliasLen, certLen, cerBuff);
			NN<Crypto::Cert::X509File> file;
			NEW_CLASSNN(file, Crypto::Cert::X509EPrivKey(s, cerBuff.WithSize(certLen)));
			ofst += 18 + aliasLen + certLen;
			readSize = fd->GetRealData(ofst, 256, BYTEARR(buff));
			UOSInt chainCnt = ReadMUInt32(buff);
			ofst += 4;
			pkg->AddObject(file, Text::CStringNN(sb.v, sb.leng - 4), Data::Timestamp(ts, 0), 0, 0, 0);
			s->Release();
			if (chainCnt > 0)
			{
				sb.RemoveChars(3);
				sb.Append(CSTR("crt"));
				s = Text::String::New(sb.ToCString());
				UOSInt certTypeLen = ReadMUInt16(&buff[4]);
				certLen = ReadMUInt32(&buff[6 + certTypeLen]);
				if (cerBuff.GetSize() < certLen)
				{
					cerBuff.ChangeSizeAndClear(certLen);
				}
				fd->GetRealData(ofst + 6 + certTypeLen, certLen, cerBuff);
				NEW_CLASSNN(file, Crypto::Cert::X509Cert(s, cerBuff.WithSize(certLen)));
				ofst += ofst + 6 + certTypeLen + certLen;
				if (chainCnt == 1)
				{
					pkg->AddObject(file, Text::CStringNN(sb.v, sb.leng - 4), Data::Timestamp(ts, 0), 0, 0, 0);
				}
				else
				{
					NN<Crypto::Cert::X509FileList> files;
					NEW_CLASSNN(files, Crypto::Cert::X509FileList(s, NN<Crypto::Cert::X509Cert>::ConvertFrom(file)));
					UOSInt i = 1;
					while (i < chainCnt)
					{
						readSize = fd->GetRealData(ofst, 256, BYTEARR(buff));
						certTypeLen = ReadMUInt16(&buff[0]);
						certLen = ReadMUInt32(&buff[2 + certTypeLen]);
						if (cerBuff.GetSize() < certLen)
						{
							cerBuff.ChangeSizeAndClear(certLen);
						}
						fd->GetRealData(ofst + 6 + certTypeLen, certLen, cerBuff);
						NEW_CLASSNN(file, Crypto::Cert::X509Cert(s, cerBuff.WithSize(certLen)));
						files->AddFile(file);
						ofst += ofst + 6 + certTypeLen + certLen;
						i++;
					}
					pkg->AddObject(files, Text::CStringNN(sb.v, sb.leng - 4), Data::Timestamp(ts, 0), 0, 0, 0);
				}
				s->Release();
			}
		}
		else if (tag == 2)
		{
			UOSInt aliasLen = ReadMUInt16(&buff[4]);
			Int64 ts = ReadMInt64(&buff[6 + aliasLen]);
			UOSInt certTypeLen = ReadMUInt16(&buff[14 + aliasLen]);
			UInt32 certLen = ReadMUInt32(&buff[16 + aliasLen + certTypeLen]);
			if (certTypeLen != 5 || !Text::StrEqualsC(&buff[16 + aliasLen], certTypeLen, UTF8STRC("X.509")))
			{
				printf("JKS: Unknown cert Type\r\n");
				break;
			}
			sb.ClearStr();
			sb.AppendC(&buff[6], aliasLen);
			sb.AppendC(UTF8STRC(".cer"));
			if (cerBuff.GetSize() < certLen)
			{
				cerBuff.ChangeSizeAndClear(certLen);
			}
			NN<Text::String> s = Text::String::New(sb.ToCString());
			fd->GetRealData(ofst + 20 + aliasLen + certTypeLen, certLen, cerBuff);
			NN<Crypto::Cert::X509Cert> cert;
			if (Optional<Crypto::Cert::X509Cert>::ConvertFrom(Parser::FileParser::X509Parser::ParseBuff(cerBuff.WithSize(certLen), s)).SetTo(cert))
			{
				pkg->AddObject(cert, Text::CStringNN(&buff[6], aliasLen), Data::Timestamp(ts, 0), 0, 0, 0);
			}
			else
			{
				printf("JKS: Error in parsing cert file\r\n");
			}
			s->Release();
			ofst += 20 + aliasLen + certTypeLen + certLen;
		}
		else if (tag == 3)
		{
			printf("JKS: Unsupported tag: %d, ofst = %lld\r\n", tag, ofst);
			break;
		}
		else
		{
			printf("JKS: Unsupported tag: %d, ofst = %lld\r\n", tag, ofst);
			break;
		}

		i++;
	}
	return pkg;
}
