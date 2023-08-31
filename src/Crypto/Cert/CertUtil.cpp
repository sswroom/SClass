#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Data/ByteTool.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/ASN1Util.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/StringTool.h"

Bool Crypto::Cert::CertUtil::AppendNames(NotNullPtr<Net::ASN1PDUBuilder> builder, const CertNames *names)
{
	Bool found = false;
	builder->BeginSequence();
	if (names->countryName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.4.6"));
		builder->AppendPrintableString(names->countryName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->stateOrProvinceName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.4.8"));
		builder->AppendUTF8String(names->stateOrProvinceName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->localityName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.4.7"));
		builder->AppendUTF8String(names->localityName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->organizationName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.4.10"));
		builder->AppendUTF8String(names->organizationName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->organizationUnitName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.4.11"));
		builder->AppendUTF8String(names->organizationUnitName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->commonName)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.4.3"));
		builder->AppendUTF8String(names->commonName);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	if (names->emailAddress)
	{
		builder->BeginSet();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("1.2.840.113549.1.9.1"));
		builder->AppendIA5String(names->emailAddress);
		builder->EndLevel();
		builder->EndLevel();
		found = true;
	}
	builder->EndLevel();
	return found;
}

Bool Crypto::Cert::CertUtil::AppendPublicKey(NotNullPtr<Net::ASN1PDUBuilder> builder, NotNullPtr<Crypto::Cert::X509Key> key)
{
	if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA)
	{
		builder->BeginSequence();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("1.2.840.113549.1.1.1"));
		builder->AppendNull();
		builder->EndLevel();
		Crypto::Cert::X509Key *pubKey = key->CreatePublicKey();
		if (pubKey == 0)
		{
			return false;
		}
		builder->AppendBitString(0, pubKey->GetASN1Buff(), pubKey->GetASN1BuffSize());
		builder->EndLevel();
		DEL_CLASS(pubKey);
		return true;
	}
	else if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSAPublic)
	{
		builder->BeginSequence();
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("1.2.840.113549.1.1.1"));
		builder->AppendNull();
		builder->EndLevel();
		builder->AppendBitString(0, key->GetASN1Buff(), key->GetASN1BuffSize());
		builder->EndLevel();
		return true;
	}
	return false;
}

Bool Crypto::Cert::CertUtil::AppendExtensions(NotNullPtr<Net::ASN1PDUBuilder> builder, const CertExtensions *ext)
{
	Bool found = false;
	if (ext->subjectAltName && ext->subjectAltName->GetCount() > 0)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.29.17"));
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->BeginSequence();
		UOSInt i = 0;
		UOSInt j = ext->subjectAltName->GetCount();
		Text::String *s;
		Net::SocketUtil::AddressInfo addr;
		while (i < j)
		{
			s = ext->subjectAltName->GetItem(i);
			if (Net::SocketUtil::GetIPAddr(s->ToCString(), addr))
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
	if (ext->issuerAltName && ext->issuerAltName->GetCount() > 0)
	{
		if (!found)
		{
			builder->BeginSequence();
			found = true;
		}
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("2.5.29.18"));
		builder->BeginOther(Net::ASN1Util::IT_OCTET_STRING);
		builder->BeginSequence();
		UOSInt i = 0;
		UOSInt j = ext->issuerAltName->GetCount();
		Text::String *s;
		Net::SocketUtil::AddressInfo addr;
		while (i < j)
		{
			s = ext->issuerAltName->GetItem(i);
			if (Net::SocketUtil::GetIPAddr(s->ToCString(), addr))
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
			builder->AppendOIDString(UTF8STRC("2.5.29.19")); //basicConstraint
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
		builder->AppendOIDString(UTF8STRC("2.5.29.15")); //keyUsage
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
		builder->AppendOIDString(UTF8STRC("2.5.29.14"));
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
		builder->AppendOIDString(UTF8STRC("2.5.29.35"));
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

Bool Crypto::Cert::CertUtil::AppendSign(NotNullPtr<Net::ASN1PDUBuilder> builder, Net::SSLEngine *ssl, NotNullPtr<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType)
{
	UOSInt itemLen;
	UOSInt itemOfst;
	const UInt8 *item = builder->GetItemRAW("1", &itemLen, &itemOfst);
	if (item == 0)
	{
		return 0;
	}
	if (key->GetKeyType() == Crypto::Cert::X509Key::KeyType::RSA && hashType == Crypto::Hash::HashType::SHA256)
	{
		UInt8 signData[256];
		UOSInt signLen;
		if (!ssl->Signature(key, hashType, item, itemOfst + itemLen, signData, signLen))
		{
			return false;
		}
		builder->BeginSequence();
		builder->AppendOIDString(UTF8STRC("1.2.840.113549.1.1.11"));
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

Crypto::Cert::X509CertReq *Crypto::Cert::CertUtil::CertReqCreate(Net::SSLEngine *ssl, const CertNames *names, NotNullPtr<Crypto::Cert::X509Key> key, const CertExtensions *ext)
{
	Net::ASN1PDUBuilder builder;
	builder.BeginSequence();

	builder.BeginSequence();
	builder.AppendInt32(0);
	if (!AppendNames(builder, names)) return 0;
	if (!AppendPublicKey(builder, key)) return 0;
	builder.BeginOther(0xA0);
	if (ext)
	{
		builder.BeginSequence();
		builder.AppendOIDString(UTF8STRC("1.2.840.113549.1.9.14"));
		builder.BeginSet();
		AppendExtensions(builder, ext);
		builder.EndLevel();
		builder.EndLevel();
	}
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(builder, ssl, key, Crypto::Hash::HashType::SHA256)) return 0;
	builder.EndLevel();

	Text::StringBuilderUTF8 sb;
	sb.Append(names->commonName);
	sb.AppendC(UTF8STRC(".csr"));
	Crypto::Cert::X509CertReq *csr;
	NEW_CLASS(csr, Crypto::Cert::X509CertReq(sb.ToCString(), builder.GetArray()));
	return csr;
}

Crypto::Cert::X509Cert *Crypto::Cert::CertUtil::SelfSignedCertCreate(Net::SSLEngine *ssl, const CertNames *names, NotNullPtr<Crypto::Cert::X509Key> key, UOSInt validDays, const CertExtensions *ext)
{
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
	builder.AppendOIDString(UTF8STRC("1.2.840.113549.1.1.11"));
	builder.AppendNull();
	builder.EndLevel();

	if (!AppendNames(builder, names)) return 0;
	dt.SetCurrTimeUTC();
	builder.BeginSequence();
	builder.AppendUTCTime(dt);
	dt.AddDay((OSInt)validDays);
	builder.AppendUTCTime(dt);
	builder.EndLevel();

	if (!AppendNames(builder, names)) return 0;
	if (!AppendPublicKey(builder, key)) return 0;
	builder.BeginOther(Net::ASN1Util::IT_CONTEXT_SPECIFIC_3);
	if (ext)
	{
		AppendExtensions(builder, ext);
	}
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(builder, ssl, key, Crypto::Hash::HashType::SHA256)) return 0;
	builder.EndLevel();
	Text::StringBuilderUTF8 sb;
	sb.Append(names->commonName);
	sb.AppendC(UTF8STRC(".crt"));
	Crypto::Cert::X509Cert *cert;
	NEW_CLASS(cert, Crypto::Cert::X509Cert(sb.ToCString(), builder.GetArray()));
	return cert;
}

Crypto::Cert::X509Cert *Crypto::Cert::CertUtil::IssueCert(Net::SSLEngine *ssl, Crypto::Cert::X509Cert *caCert, NotNullPtr<Crypto::Cert::X509Key> caKey, UOSInt validDays, Crypto::Cert::X509CertReq *csr)
{
	UInt8 bSerial[20];
	if (caCert == 0)
	{
		return 0;
	}
	if (csr == 0)
	{
		return 0;
	}

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
	builder.AppendOIDString(UTF8STRC("1.2.840.113549.1.1.11"));
	builder.AppendNull();
	builder.EndLevel();

	MemClear(&names, sizeof(names));
	if (!caCert->GetSubjNames(&names))
	{
		return 0;
	}
	if (!AppendNames(builder, &names))
	{
		Crypto::Cert::CertNames::FreeNames(&names);
 		return 0;
	}
	Crypto::Cert::CertNames::FreeNames(&names);
	dt.SetCurrTimeUTC();
	builder.BeginSequence();
	builder.AppendUTCTime(dt);
	dt.AddDay((OSInt)validDays);
	builder.AppendUTCTime(dt);
	builder.EndLevel();

	MemClear(&names, sizeof(names));
	if (!csr->GetNames(&names))
	{
		return 0;
	}
	if (!AppendNames(builder, &names))
	{
		Crypto::Cert::CertNames::FreeNames(&names);
 		return 0;
	}
	sbFileName.Append(names.commonName);
	Crypto::Cert::CertNames::FreeNames(&names);
	NotNullPtr<Crypto::Cert::X509Key> pubKey;;
	if (!pubKey.Set(csr->GetNewPublicKey()))
	{
		return 0;
	}
	if (!AppendPublicKey(builder, pubKey))
	{
		pubKey.Delete();
		return 0;
	}
	pubKey.Delete();
	builder.BeginOther(Net::ASN1Util::IT_CONTEXT_SPECIFIC_3);
	Crypto::Cert::CertExtensions ext;
	MemClear(&ext, sizeof(ext));
	csr->GetExtensions(&ext);
	ext.useAuthKeyId = caKey->GetKeyId(BYTEARR(ext.authKeyId));
	ext.useSubjKeyId = csr->GetKeyId(BYTEARR(ext.subjKeyId));
	AppendExtensions(builder, &ext);
	Crypto::Cert::CertExtensions::FreeExtensions(&ext);
	builder.EndLevel();
	builder.EndLevel();

	if (!AppendSign(builder, ssl, caKey, Crypto::Hash::HashType::SHA256)) return 0;
	builder.EndLevel();
	sbFileName.AppendC(UTF8STRC(".crt"));
	Crypto::Cert::X509Cert *cert;
	NEW_CLASS(cert, Crypto::Cert::X509Cert(sbFileName.ToCString(), builder.GetArray()));
	return cert;
}

Crypto::Cert::X509Cert *Crypto::Cert::CertUtil::FindIssuer(NotNullPtr<Crypto::Cert::X509Cert> cert)
{
	UInt8 dataBuff[8192];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UInt8 keyId[20];
	
	Crypto::Cert::CertExtensions ext;
	MemClear(&ext, sizeof(ext));
	if (!cert->GetExtensions(&ext))
	{
		return 0;
	}
	if (!ext.useAuthKeyId)
	{
		Crypto::Cert::CertExtensions::FreeExtensions(&ext);
		return 0;
	}
	sptr = cert->GetSourceNameObj()->ConcatTo(sbuff);
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i == INVALID_INDEX)
	{
		Crypto::Cert::CertExtensions::FreeExtensions(&ext);
		return 0;
	}
	if (!cert->GetKeyId(BYTEARR(keyId)))
	{
		Crypto::Cert::CertExtensions::FreeExtensions(&ext);
		return 0;
	}
	if (BytesEquals(keyId, ext.authKeyId, 20))
	{
		Crypto::Cert::CertExtensions::FreeExtensions(&ext);
		return 0;
	}
	sptr = &sbuff[i + 1];
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	IO::Path::PathType pt;
	UInt64 fileSize;
	Crypto::Cert::X509File *x509;
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize)) != 0)
		{
			if (fileSize > 0 && fileSize <= sizeof(dataBuff))
			{
				if (IO::FileStream::LoadFile(CSTRP(sbuff, sptr2), dataBuff, sizeof(dataBuff)) == fileSize)
				{
					NotNullPtr<Text::String> s = Text::String::New(sbuff, (UOSInt)(sptr2 - sbuff));
					x509 = Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR(dataBuff, (UOSInt)fileSize), s);
					s->Release();
					if (x509)
					{
						if (x509->GetFileType() != Crypto::Cert::X509File::FileType::Cert)
						{
							DEL_CLASS(x509);
						}
						else
						{
							Crypto::Cert::X509Cert *srchCert = (Crypto::Cert::X509Cert*)x509;
							if (!srchCert->GetKeyId(BYTEARR(keyId)))
							{
								DEL_CLASS(srchCert);
							}
							else if (!BytesEquals(keyId, ext.authKeyId, 20))
							{
								DEL_CLASS(srchCert);
							}
							else
							{
								Crypto::Cert::CertExtensions::FreeExtensions(&ext);
								IO::Path::FindFileClose(sess);
								return srchCert;
							}
						}
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	Crypto::Cert::CertExtensions::FreeExtensions(&ext);
	return 0;
}
