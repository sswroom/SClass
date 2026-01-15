#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Core/ByteTool_C.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/ASN1Util.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringTool.h"

Bool Crypto::Cert::CertUtil::AppendNames(NN<Net::ASN1PDUBuilder> builder, NN<const CertNames> names)
{
	Bool found = false;
	NN<Text::String> s;
	builder->BeginSequence();
	if (names->countryName.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.4.6"));
		builder->AppendPrintableString(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->stateOrProvinceName.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.4.8"));
		builder->AppendUTF8String(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->localityName.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.4.7"));
		builder->AppendUTF8String(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->organizationName.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.4.10"));
		builder->AppendUTF8String(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->organizationUnitName.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.4.11"));
		builder->AppendUTF8String(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->commonName.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.4.3"));
		builder->AppendUTF8String(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->emailAddress.SetTo(s))
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("1.2.840.113549.1.9.1"));
		builder->AppendIA5String(s);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	builder->EndLevel();
	return found;
}

Bool Crypto::Cert::CertUtil::AppendPublicKey(NN<Net::ASN1PDUBuilder> builder, NN<Crypto::Cert::X509Key> key)
{
	if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		builder->BeginSequence();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("1.2.840.113549.1.1.1"));
		builder->AppendNull();
		builder->EndLevel();
		NN<Crypto::Cert::X509Key> pubKey;
		if (!key->ExtractPublicKey().SetTo(pubKey))
		{
			return false;
		}
		builder->AppendBitString(0, pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		builder->EndLevel();
		pubKey.Delete();
		return true;
	}
	else if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSAPublic)
	{
		builder->BeginSequence();
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("1.2.840.113549.1.1.1"));
		builder->AppendNull();
		builder->EndLevel();
		builder->AppendBitString(0, key->GetASN1Buff(), key->GetASN1BuffSize());
		builder->EndLevel();
		return true;
	}
	return false;
}

Bool Crypto::Cert::CertUtil::AppendExtensions(NN<Net::ASN1PDUBuilder> builder, NN<const CertExtensions> ext)
{
	Bool found = false;
	NN<Data::ArrayListStringNN> strList;
	if (ext->subjectAltName.SetTo(strList) && strList->GetCount() > 0)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.29.17"));
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->BeginSequence();
		Data::ArrayIterator<NN<Text::String>> it = strList->Iterator();
		NN<Text::String> s;
		Net::SocketUtil::AddressInfo addr;
		while (it.HasNext())
		{
			s = it.Next();
			if (Net::SocketUtil::SetAddrInfo(addr, s->ToCString()))
			{
				if (addr.addrType == Net::AddrType::IPv4)
				{
					builder->AppendOther(0x87, addr.addr, 4);
				}
			}
			else if (Text::StringTool::IsEmailAddress(s->v))
			{
				builder->AppendOther(0x81, s->v, s->leng);
			}
			else
			{
				builder->AppendOther(0x82, s->v, s->leng);
			}
		}
		builder->EndLevel();
		builder->EndLevel();
		builder->EndLevel();
	}
	if (ext->issuerAltName.SetTo(strList) && strList->GetCount() > 0)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.29.18"));
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->BeginSequence();
		UOSInt i = 0;
		UOSInt j = strList->GetCount();
		NN<Text::String> s;
		Net::SocketUtil::AddressInfo addr;
		while (i < j)
		{
			s = strList->GetItemNoCheck(i);
			if (Net::SocketUtil::SetAddrInfo(addr, s->ToCString()))
			{
				if (addr.addrType == Net::AddrType::IPv4)
				{
					builder->AppendOther(0x87, addr.addr, 4);
				}
			}
			else if (Text::StringTool::IsEmailAddress(s->v))
			{
				builder->AppendOther(0x81, s->v, s->leng);
			}
			else
			{
				builder->AppendOther(0x82, s->v, s->leng);
			}
			i++;
		}
		builder->EndLevel();
		builder->EndLevel();
		builder->EndLevel();
	}
	if (ext->caCert || ext->digitalSign)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		if (ext->caCert)
		{
			builder->BeginSequence();
			builder->AppendOIDString(CSTR("2.5.29.19")); //basicConstraint
			builder->AppendBool(true); // Critical
			builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
			builder->BeginSequence();
			builder->AppendBool(true);
			if (ext->caCertPathLen == 0)
			{
				builder->AppendInt32(3);
			}
			else
			{
				builder->AppendInt32(ext->caCertPathLen);
			}
			builder->EndLevel();
			builder->EndLevel();
			builder->EndLevel();
		}

		UInt8 bitLeft = 1;
		UInt8 buff[2];
		buff[0] = 0;
		buff[1] = 0;
		if (ext->caCert)
		{
			buff[0] |= 6;
		}
		if (ext->digitalSign)
		{
			buff[0] |= 0x80;
		}
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.29.15")); //keyUsage
		builder->AppendBool(true); // Critical
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->AppendBitString(bitLeft, buff, 1);
		builder->EndLevel();
		builder->EndLevel();
	}
	if (ext->useSubjKeyId)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.29.14"));
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->AppendOctetString(ext->subjKeyId, 20);
		builder->EndLevel();
		builder->EndLevel();
	}
	if (ext->useAuthKeyId)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("2.5.29.35"));
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->BeginSequence();
		builder->AppendOther(0x80, ext->authKeyId, 20);
		builder->EndLevel();
		builder->EndLevel();
		builder->EndLevel();
	}
	if (found)
	{
		builder->EndLevel();
	}
	return true;
}

Bool Crypto::Cert::CertUtil::AppendSign(NN<Net::ASN1PDUBuilder> builder, NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType)
{
	UOSInt itemLen;
	UOSInt itemOfst;
	UnsafeArray<const UInt8> item;
	if (!builder->GetItemRAW("1", itemLen, itemOfst).SetTo(item))
	{
		return 0;
	}
	if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA && hashType == Crypto::Hash::HashType::SHA256)
	{
		UInt8 signData[1024];
		UOSInt signLen;
		if (!ssl->Signature(key, hashType, Data::ByteArrayR(item, itemOfst + itemLen), signData, signLen))
		{
			return false;
		}
		builder->BeginSequence();
		builder->AppendOIDString(CSTR("1.2.840.113549.1.1.11"));
		builder->AppendNull();
		builder->EndLevel();
		builder->AppendBitString(0, signData, signLen);
		return true;
	}
	else
	{
		return false;
	}
}

Optional<Crypto::Cert::X509CertReq> Crypto::Cert::CertUtil::CertReqCreate(NN<Net::SSLEngine> ssl, NN<const CertNames> names, NN<Crypto::Cert::X509Key> key, Optional<const CertExtensions> ext)
{
	NN<const CertExtensions> nnext;
	Net::ASN1PDUBuilder builder;
	builder.BeginSequence();

	builder.BeginSequence();
	builder.AppendInt32(0);
	if (!AppendNames(builder, names)) return nullptr;
	if (!AppendPublicKey(builder, key)) return nullptr;
	builder.BeginOther(0xA0);
	if (ext.SetTo(nnext))
	{
		builder.BeginSequence();
		builder.AppendOIDString(CSTR("1.2.840.113549.1.9.14"));
		builder.BeginSet();
		AppendExtensions(builder, nnext);
		builder.EndLevel();
		builder.EndLevel();
	}
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(builder, ssl, key, Crypto::Hash::HashType::SHA256)) return nullptr;
	builder.EndLevel();

	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(names->commonName);
	sb.AppendC(UTF8STRC(".csr"));
	NN<Crypto::Cert::X509CertReq> csr;
	NEW_CLASSNN(csr, Crypto::Cert::X509CertReq(sb.ToCString(), builder.GetArray()));
	return csr;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::CertUtil::SelfSignedCertCreate(NN<Net::SSLEngine> ssl, NN<const CertNames> names, NN<Crypto::Cert::X509Key> key, UOSInt validDays, Optional<const CertExtensions> ext)
{
	NN<const CertExtensions> nnext;
	Data::RandomBytesGenerator rndBytes;
	Net::ASN1PDUBuilder builder;
	Data::DateTime dt;
	UInt8 buff[20];
	builder.BeginSequence();

	builder.BeginSequence();
	builder.BeginOther(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0);
	builder.AppendInt32(2);
	builder.EndLevel();
	while (true)
	{
		rndBytes.NextBytes(buff, 20);
		if (buff[0] != 0)
		{
			break;
		}
	}
	builder.AppendOther(Net::ASN1Util::IT_INTEGER, buff, 20);

	builder.BeginSequence();
	builder.AppendOIDString(CSTR("1.2.840.113549.1.1.11"));
	builder.AppendNull();
	builder.EndLevel();

	if (!AppendNames(builder, names)) return nullptr;
	dt.SetCurrTimeUTC();
	builder.BeginSequence();
	builder.AppendUTCTime(dt);
	dt.AddDay((OSInt)validDays);
	builder.AppendUTCTime(dt);
	builder.EndLevel();

	if (!AppendNames(builder, names)) return nullptr;
	if (!AppendPublicKey(builder, key)) return nullptr;
	builder.BeginOther(Net::ASN1Util::IT_CONTEXT_SPECIFIC_3);
	if (ext.SetTo(nnext))
	{
		AppendExtensions(builder, nnext);
	}
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(builder, ssl, key, Crypto::Hash::HashType::SHA256)) return nullptr;
	builder.EndLevel();
	Text::StringBuilderUTF8 sb;
	sb.AppendOpt(names->commonName);
	sb.AppendC(UTF8STRC(".crt"));
	Crypto::Cert::X509Cert *cert;
	NEW_CLASS(cert, Crypto::Cert::X509Cert(sb.ToCString(), builder.GetArray()));
	return cert;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::CertUtil::IssueCert(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Cert> caCert, NN<Crypto::Cert::X509Key> caKey, UOSInt validDays, NN<Crypto::Cert::X509CertReq> csr)
{
	UInt8 bSerial[20];
	Text::StringBuilderUTF8 sbFileName;
	Crypto::Cert::CertNames names;
	Net::ASN1PDUBuilder builder;
	Data::DateTime dt;
	Data::RandomBytesGenerator rndBytes;
	builder.BeginSequence();

	builder.BeginSequence();
	builder.BeginOther(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0);
	builder.AppendInt32(2);
	builder.EndLevel();
	while (true)
	{
		rndBytes.NextBytes(bSerial, 20);
		if (bSerial[0] != 0)
		{
			break;
		}
	}
	builder.AppendOther(Net::ASN1Util::IT_INTEGER, bSerial, 20);

	builder.BeginSequence();
	builder.AppendOIDString(CSTR("1.2.840.113549.1.1.11"));
	builder.AppendNull();
	builder.EndLevel();

	MemClear(&names, sizeof(names));
	if (!caCert->GetSubjNames(names))
	{
		return nullptr;
	}
	if (!AppendNames(builder, names))
	{
		Crypto::Cert::CertNames::FreeNames(names);
 		return nullptr;
	}
	Crypto::Cert::CertNames::FreeNames(names);
	dt.SetCurrTimeUTC();
	builder.BeginSequence();
	builder.AppendUTCTime(dt);
	dt.AddDay((OSInt)validDays);
	builder.AppendUTCTime(dt);
	builder.EndLevel();

	MemClear(&names, sizeof(names));
	if (!csr->GetNames(names))
	{
		return nullptr;
	}
	if (!AppendNames(builder, names))
	{
		Crypto::Cert::CertNames::FreeNames(names);
 		return nullptr;
	}
	sbFileName.AppendOpt(names.commonName);
	Crypto::Cert::CertNames::FreeNames(names);
	NN<Crypto::Cert::X509Key> pubKey;;
	if (!csr->GetNewPublicKey().SetTo(pubKey))
	{
		return nullptr;
	}
	if (!AppendPublicKey(builder, pubKey))
	{
		pubKey.Delete();
		return nullptr;
	}
	pubKey.Delete();
	builder.BeginOther(Net::ASN1Util::IT_CONTEXT_SPECIFIC_3);
	Crypto::Cert::CertExtensions ext;
	MemClear(&ext, sizeof(ext));
	csr->GetExtensions(ext);
	ext.useAuthKeyId = caKey->GetKeyId(BYTEARR(ext.authKeyId));
	ext.useSubjKeyId = csr->GetKeyId(BYTEARR(ext.subjKeyId));
	AppendExtensions(builder, ext);
	Crypto::Cert::CertExtensions::FreeExtensions(ext);
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(builder, ssl, caKey, Crypto::Hash::HashType::SHA256)) return nullptr;
	builder.EndLevel();
	sbFileName.AppendC(UTF8STRC(".crt"));
	Crypto::Cert::X509Cert *cert;
	NEW_CLASS(cert, Crypto::Cert::X509Cert(sbFileName.ToCString(), builder.GetArray()));
	return cert;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::CertUtil::FindIssuer(NN<Crypto::Cert::X509Cert> cert)
{
	UInt8 dataBuff[8192];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UInt8 keyId[20];
	
	Crypto::Cert::CertExtensions ext;
	MemClear(&ext, sizeof(ext));
	if (!cert->GetExtensions(ext))
	{
		return nullptr;
	}
	if (!ext.useAuthKeyId)
	{
		Crypto::Cert::CertExtensions::FreeExtensions(ext);
		return nullptr;
	}
	sptr = cert->GetSourceNameObj()->ConcatTo(sbuff);
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i == INVALID_INDEX)
	{
		Crypto::Cert::CertExtensions::FreeExtensions(ext);
		return nullptr;
	}
	if (!cert->GetKeyId(BYTEARR(keyId)))
	{
		Crypto::Cert::CertExtensions::FreeExtensions(ext);
		return nullptr;
	}
	if (BytesEquals(keyId, ext.authKeyId, 20))
	{
		Crypto::Cert::CertExtensions::FreeExtensions(ext);
		return nullptr;
	}
	sptr = &sbuff[i + 1];
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	UInt64 fileSize;
	NN<Crypto::Cert::X509File> x509;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, fileSize).SetTo(sptr2))
		{
			if (fileSize > 0 && fileSize <= sizeof(dataBuff))
			{
				if (IO::FileStream::LoadFile(CSTRP(sbuff, sptr2), dataBuff, sizeof(dataBuff)) == fileSize)
				{
					NN<Text::String> s = Text::String::New(sbuff, (UOSInt)(sptr2 - sbuff));
					if (Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR(dataBuff, (UOSInt)fileSize), s).SetTo(x509))
					{
						s->Release();
						if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Cert)
						{
							x509.Delete();
						}
						else
						{
							NN<Crypto::Cert::X509Cert> srchCert = NN<Crypto::Cert::X509Cert>::ConvertFrom(x509);
							if (!srchCert->GetKeyId(BYTEARR(keyId)))
							{
								srchCert.Delete();
							}
							else if (!BytesEquals(keyId, ext.authKeyId, 20))
							{
								srchCert.Delete();
							}
							else
							{
								Crypto::Cert::CertExtensions::FreeExtensions(ext);
								IO::Path::FindFileClose(sess);
								return srchCert;
							}
						}
					}
					else
					{
						s->Release();
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	Crypto::Cert::CertExtensions::FreeExtensions(ext);
	return nullptr;
}
