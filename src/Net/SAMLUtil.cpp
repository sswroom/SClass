#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "IO/MemoryReadingStream.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Net/SAMLUtil.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Data/Compress/Inflater.h"

UOSInt Net::SAMLUtil::DecryptEncryptedKey(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff)
{
	NN<Text::String> nodeName;
	Crypto::Encrypt::RSACipher::Padding rsaPadding = Crypto::Encrypt::RSACipher::Padding::PKCS1;
	Bool algFound = false;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> avalue;
	UOSInt keySize = 0;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("e:EncryptionMethod")))
		{
			UOSInt i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("Algorithm")) && attr->value.SetTo(avalue))
				{
					if (avalue->Equals(UTF8STRC("http://www.w3.org/2001/04/xmlenc#rsa-oaep-mgf1p")))
					{
						algFound = true;
						rsaPadding = Crypto::Encrypt::RSACipher::Padding::PKCS1_OAEP;
						break;
					}
				}
			}
			reader->SkipElement();
		}
		else if (nodeName->Equals(UTF8STRC("KeyInfo")))
		{
			reader->SkipElement();
		}
		else if (nodeName->Equals(UTF8STRC("e:CipherData")))
		{
			if (!algFound)
			{
				sbResult->AppendC(UTF8STRC("Algorithm not found in EncryptedKey"));
				return 0;
			}
			Text::StringBuilderUTF8 sb;
			reader->ReadNodeText(sb);
			Text::TextBinEnc::Base64Enc b64;
			UOSInt dataSize = b64.CalcBinSize(sb.ToCString());
			if (dataSize != 256)
			{
				sbResult->AppendC(UTF8STRC("Length of e:CipherData not valid in EncryptedKey"));
				return 0;
			}
			UInt8 *data = MemAlloc(UInt8, dataSize);
			b64.DecodeBin(sb.ToCString(), data);
			keySize = ssl->Decrypt(key, keyBuff, Data::ByteArrayR(data, dataSize), rsaPadding);
			MemFree(data);
			if (keySize == 0)
			{
				sbResult->AppendC(UTF8STRC("Error in decrypting the EncryptedKey"));
				return 0;
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
	if (reader->GetErrorCode() != 0)
	{
		sbResult->AppendC(UTF8STRC("End of EncryptedKey not found"));
		return 0;
	}
	if (keySize == 0)
	{
		sbResult->AppendC(UTF8STRC("e:CipherData not found in EncryptedKey"));
		return 0;
	}
	return keySize;
}

UOSInt Net::SAMLUtil::ParseKeyInfo(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff)
{
	NN<Text::String> nodeName;
	UOSInt keySize = 0;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("e:EncryptedKey")))
		{
			keySize = DecryptEncryptedKey(ssl, key, reader, sbResult, keyBuff);
			if (keySize == 0)
			{
				return 0;
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
	if (reader->GetErrorCode() != 0)
	{
		sbResult->AppendC(UTF8STRC("End of EncryptedKey not found"));
		return 0;
	}
	if (keySize == 0)
	{
		sbResult->AppendC(UTF8STRC("e:CipherData not found in EncryptedKey"));
		return 0;
	}
	return keySize;
}

Bool Net::SAMLUtil::DecryptEncryptedData(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult)
{
	UInt8 keyBuff[128];
	UOSInt keySize = 0;
	UOSInt algKeySize = 0;
	Bool headingIV = false;
	MemClear(keyBuff, sizeof(keyBuff));
	Crypto::Encrypt::BlockCipher *cipher = 0;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> avalue;
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("xenc:EncryptionMethod")))
		{
			if (cipher != 0)
			{
				DEL_CLASS(cipher);
				sbResult->AppendC(UTF8STRC("xenc:EncryptionMethod already exists"));
				return false;
			}
			UOSInt i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				if (Text::String::OrEmpty(attr->name)->Equals(UTF8STRC("Algorithm")) && attr->value.SetTo(avalue))
				{
					if (avalue->Equals(UTF8STRC("http://www.w3.org/2001/04/xmlenc#aes256-cbc")))
					{
						NEW_CLASS(cipher, Crypto::Encrypt::AES256(keyBuff));
						cipher->SetChainMode(Crypto::Encrypt::ChainMode::CBC);
						algKeySize = 32;
						headingIV = true;
					}
					else
					{
						sbResult->AppendC(UTF8STRC("Algorithm not supported: "));
						sbResult->Append(avalue);
						break;
					}
				}
			}
			if (cipher == 0)
			{
				sbResult->AppendC(UTF8STRC("Algorithm not found in xenc:EncryptionMethod"));
				return false;
			}
			reader->SkipElement();
		}
		else if (nodeName->Equals(UTF8STRC("KeyInfo")))
		{
			keySize = ParseKeyInfo(ssl, key, reader, sbResult, keyBuff);
			if (keySize == 0)
			{
				SDEL_CLASS(cipher);
				return false;
			}
		}
		else if (nodeName->Equals(UTF8STRC("xenc:CipherData")))
		{
			if (cipher == 0)
			{
				sbResult->AppendC(UTF8STRC("xenc:EncryptionMethod not found before xenc:CipherData"));
				return false;
			}
			else if (keySize != algKeySize)
			{
				DEL_CLASS(cipher);
				sbResult->AppendC(UTF8STRC("Key size invalid"));
				return false;
			}
			((Crypto::Encrypt::AES256*)cipher)->SetKey(keyBuff);
			while (reader->NextElementName().SetTo(nodeName))
			{
				if (nodeName->Equals(UTF8STRC("xenc:CipherValue")))
				{
					
					Text::StringBuilderUTF8 sb;
					reader->ReadNodeText(sb);
					Text::TextBinEnc::Base64Enc b64;
					UOSInt dataSize = b64.CalcBinSize(sb.ToCString());
					if (headingIV)
					{
						if (dataSize < cipher->GetDecBlockSize())
						{
							sbResult->AppendC(UTF8STRC("xenc:CipherValue is too short to decrypt"));
							return false;
						}
					}
					UOSInt blkSize = cipher->GetDecBlockSize();
					UInt8 *data = MemAlloc(UInt8, dataSize);
					UInt8 *decData = MemAlloc(UInt8, dataSize + blkSize);
					UOSInt decSize;
					b64.DecodeBin(sb.ToCString(), data);
					if (headingIV)
					{
						cipher->SetIV(data);
						decSize = cipher->Decrypt(data + blkSize, dataSize - blkSize, decData);
					}
					else
					{
						decSize = cipher->Decrypt(data, dataSize, decData);
					}
					if (decData[decSize - 1] <= blkSize)
					{
						decSize -= decData[decSize - 1];
					}
					sbResult->AppendC(decData, decSize);
					MemFree(data);
					MemFree(decData);
					DEL_CLASS(cipher);
					return true;
				}
				else
				{
					reader->SkipElement();
				}
			}
			SDEL_CLASS(cipher);
			sbResult->AppendC(UTF8STRC("xenc:CipherData not found in EncryptedData"));
			return false;
		}
		else
		{
			reader->SkipElement();
		}
	}
	SDEL_CLASS(cipher);
	if (reader->GetErrorCode() != 0)
	{
		sbResult->AppendC(UTF8STRC("End of EncryptedData not found"));
		return false;
	}
	sbResult->AppendC(UTF8STRC("xenc:CipherData not found in EncryptedData"));
	return false;
}

Bool Net::SAMLUtil::DecryptAssertion(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult)
{
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("xenc:EncryptedData")))
		{
			return DecryptEncryptedData(ssl, key, reader, sbResult);
		}
		else
		{
			reader->SkipElement();
		}
	}
	if (reader->GetErrorCode() != 0)
	{
		sbResult->AppendC(UTF8STRC("End of EncryptedAssertion not found"));
		return false;
	}
	sbResult->AppendC(UTF8STRC("xenc:EncryptedData not found in EncryptedAssertion"));
	return false;
}

Bool Net::SAMLUtil::DecryptResponse(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult)
{
	NN<Text::String> nodeName;
	while (reader->NextElementName().SetTo(nodeName))
	{
		if (nodeName->Equals(UTF8STRC("EncryptedAssertion")))
		{
			return DecryptAssertion(ssl, key, reader, sbResult);
		}
		else
		{
			reader->SkipElement();
		}
	}
	if (reader->GetErrorCode() != 0)
	{
		sbResult->AppendC(UTF8STRC("End of Response not found"));
		return false;
	}
	sbResult->AppendC(UTF8STRC("Assertion not found in response"));
	return false;
}

Bool Net::SAMLUtil::DecryptResponse(NN<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, NN<Crypto::Cert::X509Key> key, Text::CStringNN responseXML, NN<Text::StringBuilderUTF8> sbResult)
{
	IO::MemoryReadingStream mstm(responseXML.v, responseXML.leng);
	Text::XMLReader reader(encFact, mstm, Text::XMLReader::PM_XML);
	NN<Text::String> nodeText;
	if (reader.NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("samlp:Response")))
		{
			return DecryptResponse(ssl, key, reader, sbResult);
		}
		else
		{
			sbResult->AppendC(UTF8STRC("Root node is not SAML Response"));
			return false;
		}
	}
	sbResult->AppendC(UTF8STRC("File is not valid XML"));
	return false;
}

Bool Net::SAMLUtil::DecodeRequest(Text::CStringNN requestB64, NN<Text::StringBuilderUTF8> sbResult)
{
	Text::TextBinEnc::Base64Enc b64;
	UOSInt decSize = b64.CalcBinSize(requestB64);
	if (decSize == 0)
		return false;
	UInt8 *decBuff = MemAlloc(UInt8, decSize);
	b64.DecodeBin(requestB64, decBuff);
	IO::MemoryStream mstm;
	Data::Compress::Inflater inf(mstm, false);
	Bool succ = inf.Write(Data::ByteArrayR(decBuff, decSize)) == decSize;
	MemFree(decBuff);
	if (succ)
	{
		sbResult->AppendC(mstm.GetBuff(), (UOSInt)mstm.GetLength());
	}
	return succ;
}
