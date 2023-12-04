#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "IO/MemoryReadingStream.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Net/SAMLUtil.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Data/Compress/Inflate.h"

UOSInt Net::SAMLUtil::DecryptEncryptedKey(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff)
{
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeName;
	Crypto::Encrypt::RSACipher::Padding rsaPadding = Crypto::Encrypt::RSACipher::Padding::PKCS1;
	Bool algFound = false;
	Text::XMLAttrib *attr;
	UOSInt keySize = 0;
	while (reader->ReadNext())
	{
		nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::Element)
		{
			nodeName = reader->GetNodeText();
			if (nodeName->Equals(UTF8STRC("e:EncryptionMethod")))
			{
				UOSInt i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("Algorithm")))
					{
						if (attr->value->Equals(UTF8STRC("http://www.w3.org/2001/04/xmlenc#rsa-oaep-mgf1p")))
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
				UOSInt dataSize = b64.CalcBinSize(sb.ToString(), sb.GetLength());
				if (dataSize != 256)
				{
					sbResult->AppendC(UTF8STRC("Length of e:CipherData not valid in EncryptedKey"));
					return 0;
				}
				UInt8 *data = MemAlloc(UInt8, dataSize);
				b64.DecodeBin(sb.ToString(), sb.GetLength(), data);
				keySize = ssl->Decrypt(key, keyBuff, data, dataSize, rsaPadding);
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
		else if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			if (keySize == 0)
			{
				sbResult->AppendC(UTF8STRC("e:CipherData not found in EncryptedKey"));
				return 0;
			}
			return keySize;
		}
	}
	sbResult->AppendC(UTF8STRC("End of EncryptedKey not found"));
	return 0;
}

UOSInt Net::SAMLUtil::ParseKeyInfo(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff)
{
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeName;
	UOSInt keySize = 0;
	while (reader->ReadNext())
	{
		nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::Element)
		{
			nodeName = reader->GetNodeText();
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
		else if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			if (keySize == 0)
			{
				sbResult->AppendC(UTF8STRC("e:CipherData not found in EncryptedKey"));
				return 0;
			}
			return keySize;
		}
	}
	sbResult->AppendC(UTF8STRC("End of EncryptedKey not found"));
	return 0;
}

Bool Net::SAMLUtil::DecryptEncryptedData(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult)
{
	UInt8 keyBuff[128];
	UOSInt keySize = 0;
	UOSInt algKeySize = 0;
	Bool headingIV = false;
	MemClear(keyBuff, sizeof(keyBuff));
	Crypto::Encrypt::BlockCipher *cipher = 0;
	Text::XMLAttrib *attr;
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeName;
	while (reader->ReadNext())
	{
		nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::Element)
		{
			nodeName = reader->GetNodeText();
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
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("Algorithm")))
					{
						if (attr->value->Equals(UTF8STRC("http://www.w3.org/2001/04/xmlenc#aes256-cbc")))
						{
							NEW_CLASS(cipher, Crypto::Encrypt::AES256(keyBuff));
							cipher->SetChainMode(Crypto::Encrypt::ChainMode::CBC);
							algKeySize = 32;
							headingIV = true;
						}
						else
						{
							sbResult->AppendC(UTF8STRC("Algorithm not supported: "));
							sbResult->Append(attr->value);
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
				while (reader->ReadNext())
				{
					nodeType = reader->GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::Element)
					{
						nodeName = reader->GetNodeText();
						if (nodeName->Equals(UTF8STRC("xenc:CipherValue")))
						{
							
							Text::StringBuilderUTF8 sb;
							reader->ReadNodeText(sb);
							Text::TextBinEnc::Base64Enc b64;
							UOSInt dataSize = b64.CalcBinSize(sb.ToString(), sb.GetLength());
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
							b64.DecodeBin(sb.ToString(), sb.GetLength(), data);
							if (headingIV)
							{
								cipher->SetIV(data);
								decSize = cipher->Decrypt(data + blkSize, dataSize - blkSize, decData, 0);
							}
							else
							{
								decSize = cipher->Decrypt(data, dataSize, decData, 0);
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
					else if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
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
		else if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			SDEL_CLASS(cipher);
			sbResult->AppendC(UTF8STRC("xenc:CipherData not found in EncryptedData"));
			return false;
		}
	}
	SDEL_CLASS(cipher);
	sbResult->AppendC(UTF8STRC("End of EncryptedData not found"));
	return false;

}

Bool Net::SAMLUtil::DecryptAssertion(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult)
{
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeName;
	while (reader->ReadNext())
	{
		nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::Element)
		{
			nodeName = reader->GetNodeText();
			if (nodeName->Equals(UTF8STRC("xenc:EncryptedData")))
			{
				return DecryptEncryptedData(ssl, key, reader, sbResult);
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			sbResult->AppendC(UTF8STRC("xenc:EncryptedData not found in EncryptedAssertion"));
			return false;
		}
	}
	sbResult->AppendC(UTF8STRC("End of EncryptedAssertion not found"));
	return false;
}

Bool Net::SAMLUtil::DecryptResponse(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult)
{
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeName;
	while (reader->ReadNext())
	{
		nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::Element)
		{
			nodeName = reader->GetNodeText();
			if (nodeName->Equals(UTF8STRC("EncryptedAssertion")))
			{
				return DecryptAssertion(ssl, key, reader, sbResult);
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			sbResult->AppendC(UTF8STRC("Assertion not found in response"));
			return false;
		}
	}
	sbResult->AppendC(UTF8STRC("End of Response not found"));
	return false;
}

Bool Net::SAMLUtil::DecryptResponse(NotNullPtr<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, NotNullPtr<Crypto::Cert::X509Key> key, Text::CString responseXML, NotNullPtr<Text::StringBuilderUTF8> sbResult)
{
	IO::MemoryReadingStream mstm(responseXML.v, responseXML.leng);
	Text::XMLReader reader(encFact, mstm, Text::XMLReader::PM_XML);
	Text::String *nodeText;
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			nodeText = reader.GetNodeText();
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
	}
	sbResult->AppendC(UTF8STRC("File is not valid XML"));
	return false;
}

Bool Net::SAMLUtil::DecodeRequest(Text::CString requestB64, NotNullPtr<Text::StringBuilderUTF8> sbResult)
{
	Text::TextBinEnc::Base64Enc b64;
	UOSInt decSize = b64.CalcBinSize(requestB64.v, requestB64.leng);
	if (decSize == 0)
		return false;
	UInt8 *decBuff = MemAlloc(UInt8, decSize);
	b64.DecodeBin(requestB64.v, requestB64.leng, decBuff);
	Data::Compress::Inflate inf(false);
	IO::MemoryStream mstm;
	IO::StmData::MemoryDataRef fd(decBuff, decSize);
	Bool succ = inf.Decompress(&mstm, fd);
	MemFree(decBuff);
	if (succ)
	{
		sbResult->AppendC(mstm.GetBuff(), (UOSInt)mstm.GetLength());
	}
	return succ;
}
