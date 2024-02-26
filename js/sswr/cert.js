import {ASN1ItemType, ASN1Util, ASN1Names, ASN1PDUBuilder} from "./certutil.js";
import * as data from "./data.js";
import * as hash from "./hash.js";
import * as text from "./text.js";

export const ASN1Type = {
	X509: 0
}

export const X509FileType = {
	Cert: 0,
	Key: 1,
	CertRequest: 2,
	PrivateKey: 3,
	PublicKey: 4,
	PKCS7: 5,
	PKCS12: 6,
	CRL: 7,
	FileList: 8
}

export const KeyType = {
	Unknown: 0,
	RSA: 1,
	DSA: 2,
	ECDSA: 3,
	ED25519: 4,
	RSAPublic: 5,
	ECPublic: 6
}

export const CertValidStatus = {
	Valid: "Valid",
	SelfSigned: "SelfSigned",
	SignatureInvalid: "SignatureInvalid",
	Revoked: "Revoked",
	FileFormatInvalid: "FileFormatInvalid",
	UnknownIssuer: "UnknownIssuer",
	Expired: "Expired",
	UnsupportedAlgorithm: "UnsupportedAlgorithm"
}

export const AlgType = {
	Unknown: "Unknown",
	MD2WithRSAEncryption: "MD2WithRSAEncryption",
	MD5WithRSAEncryption: "MD5WithRSAEncryption",
	SHA1WithRSAEncryption: "SHA1WithRSAEncryption",
	SHA256WithRSAEncryption: "SHA256WithRSAEncryption",
	SHA384WithRSAEncryption: "SHA384WithRSAEncryption",
	SHA512WithRSAEncryption: "SHA512WithRSAEncryption",
	SHA224WithRSAEncryption: "SHA224WithRSAEncryption",
	ECDSAWithSHA256: "ECDSAWithSHA256",
	ECDSAWithSHA384: "ECDSAWithSHA384"
}

export const ECName = {
	Unknown: 0,
	secp256r1: 1,
	secp384r1: 2,
	secp521r1: 3
}

export class ASN1Data extends data.ParsedObject
{
	constructor(sourceName, objType, buff)
	{
		super(sourceName, objType);
		this.reader = new data.ByteReader(buff);
	}

	toASN1String()
	{
		let names = this.createNames();
		let lines = [];
		ASN1Util.pduToString(this.reader, 0, this.reader.getLength(), lines, 0, names);
		return lines.join("\r\n");
	}

	getASN1Buff()
	{
		return this.reader;
	}

	static appendInteger(arr, reader, ofst, len)
	{
		if (len == 1)
		{
			arr.push(reader.readUInt8(ofst).toString());
		}
		else if (len == 2)
		{
			arr.push(reader.readInt16(ofst, false).toString());
		}
		else if (len == 3)
		{
			arr.push(reader.readInt24(ofst, false).toString());
		}
		else if (len == 4)
		{
			arr.push(reader.readInt32(ofst, false).toString());
		}
		else if (len == 8)
		{
			arr.push(reader.readInt64(ofst, false).toString());
		}
		else
		{
			arr.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(ofst, len)), ' ', null));
		}
	}
}

export class X509File extends ASN1Data
{
	constructor(sourceName, objType, buff)
	{
		super(sourceName, objType, buff);
	}

	getASN1Type()
	{
		return ASN1Type.X509;
	}

	getCertCount()
	{
		return 0;
	}

	getCertName(index)
	{
		return null;
	}

	getNewCert(index)
	{
		return null;
	}

	toShortString()
	{
		return fileTypeGetName(this.getFileType())+": "+this.toShortName();
	}

	isSignatureKey(key)
	{
		let data = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1");
		let signature = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
		if (data == 0 || signature == 0 || signature.itemType != ASN1ItemType.BIT_STRING)
		{
			return false;
		}
		let signOfst = signature.rawOfst + signature.hdrLen;
		let signSize = signature.contLen;
		if (this.reader.readUInt8(signOfst) != 0)
			return false;
		signOfst++;
		signSize--;
		if (!key.signatureVerify(hash.HashType.SHA256, this.reader.getArrayBuffer(data.rawOfst, data.hdrLen + data.contLen), this.reader.getArrayBuffer(signOfst, signOfst + signSize)))
		{
			return false;
		}
		return true;
	}

	getSignedInfo()
	{
		let payload = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1");
		if (payload == null)
		{
			return null;
		}
		let itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
		let algType;
		if (itemPDU == null || itemPDU.itemType != ASN1ItemType.SEQUENCE || (algType = X509File.algorithmIdentifierGet(this.reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen)) == AlgType.Unknown)
		{
			return null;
		}
		if ((itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3")) == null || itemPDU.itemType != ASN1ItemType.BIT_STRING)
		{
			return null;
		}
		return {payload: this.reader.getArrayBuffer(payload.rawOfst, payload.hdrLen + payload.contLen),
			signature: this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen),
			algType: algType};
	}

	static nameGetByOID(reader, startOfst, endOfst, oidText)
	{
		let itemPDU;
		let oidPDU;
		let strPDU;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		let i = 0;
		while (i < cnt)
		{
			i++;
	
			let path = i.toString()+".1";
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path)) != null)
			{
				if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					oidPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
					if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID && ASN1Util.oidEqualsText(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen), oidText))
					{
						strPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "2");
						if (strPDU)
						{
							if (strPDU.itemType == ASN1ItemType.BMPSTRING)
							{
								return reader.readUTF16(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen >> 1, false);
							}
							else
							{
								return reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
						}
					}
				}
			}
		}
		return null;
	}

	static nameGetCN(reader, startOfst, endOfst)
	{
		return X509File.nameGetByOID(reader, startOfst, endOfst, "2.5.4.3");
	}

	static keyGetLeng(reader, startOfst, endOfst, keyType)
	{
		let keyPDU;
		switch (keyType)
		{
		case KeyType.RSA:
			keyPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
			if (keyPDU && keyPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let cnt = ASN1Util.pduCountItem(reader, keyPDU.rawOfst + keyPDU.hdrLen, keyPDU.rawOfst + keyPDU.hdrLen + keyPDU.contLen, null);
				if (cnt > 4)
				{
					let modulus = ASN1Util.pduGetItem(reader, keyPDU.rawOfst + keyPDU.hdrLen, keyPDU.rawOfst + keyPDU.hdrLen + keyPDU.contLen, "2");
					let privateExponent = ASN1Util.pduGetItem(reader, keyPDU.rawOfst + keyPDU.hdrLen, keyPDU.rawOfst + keyPDU.hdrLen + keyPDU.contLen, "4");
					if (modulus && privateExponent)
					{
						return (modulus.contLen - 1) << 3;
					}
				}
			}
			return 0;
		case KeyType.RSAPublic:
			if (reader.readUInt8(startOfst) == 0)
			{
				startOfst++;
			}
			keyPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
			if (keyPDU && keyPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let modulus = ASN1Util.pduGetItem(reader, keyPDU.rawOfst + keyPDU.hdrLen, keyPDU.rawOfst + keyPDU.hdrLen + keyPDU.contLen, "1");
				if (modulus)
				{
					return (modulus.contLen - 1) << 3;
				}
			}
			return 0;
		case KeyType.ECPublic:
			return 0;
		case KeyType.DSA:
		case KeyType.ECDSA:
		case KeyType.ED25519:
		case KeyType.Unknown:
		default:
			return 0;
		}
	}
}

export class X509Cert extends X509File
{
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getSubjectCN()
	{
		let tmpBuff;
		if (ASN1Util.pduGetItemType(this.reader, 0, this.reader.getLength(), "1.1.1") == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.6");
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.5");
		}
		if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
		{
			return X509File.nameGetCN(this.reader, tmpBuff.rawOfst + tmpBuff.hdrLen, tmpBuff.rawOfst + tmpBuff.hdrLen + tmpBuff.contLen);
		}
		else
		{
			return null;
		}
	}

	getIssuerCN()
	{
		let tmpBuff;
		if (ASN1Util.pduGetItemType(this.reader, 0, this.reader.getLength(), "1.1.1") == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4");
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
		}
		if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
		{
			return X509File.nameGetCN(this.reader, tmpBuff.rawOfst + tmpBuff.hdrLen, tmpBuff.rawOfst + tmpBuff.hdrLen + tmpBuff.contLen);
		}
		else
		{
			return null;
		}
	}

	setDefaultSourceName()
	{
		let cn = this.getSubjectCN();
		if (cn)
		{
			this.sourceName = cn+".crt";
		}
	}
	
	getFileType()
	{
		return X509FileType.Cert;	
	}

	toShortName()
	{
		return this.getSubjectCN();
	}

	getCertCount()
	{
		return 1;
	}

	getCertName(index)
	{
		if (index != 0)
			return null;
		return this.getSubjectCN();
	}

	getNewCert(index)
	{
		if (index != 0)
			return null;
		return this.clone();
	}

	isValid()
	{
	/*
		if (trustStore == 0)
	{
		trustStore = ssl->GetTrustStore();
	}
	Text::StringBuilderUTF8 sb;
	if (!this->GetIssuerCN(sb))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Data::DateTime dt;
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis();
	if (!this->GetNotBefore(dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() > currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
	}
	if (!this->GetNotAfter(dt))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	if (dt.ToTicks() < currTime)
	{
		return Crypto::Cert::X509File::ValidStatus::Expired;
	}
	SignedInfo signedInfo;
	if (!this->GetSignedInfo(signedInfo))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Crypto::Hash::HashType hashType = GetAlgHash(signedInfo.algType);
	if (hashType == Crypto::Hash::HashType::Unknown)
	{
		return Crypto::Cert::X509File::ValidStatus::UnsupportedAlgorithm;
	}

	Crypto::Cert::X509Cert *issuer = trustStore->GetCertByCN(sb.ToCString());
	if (issuer == 0)
	{
		if (!this->IsSelfSigned())
		{
			return Crypto::Cert::X509File::ValidStatus::UnknownIssuer;
		}
		NotNullPtr<Crypto::Cert::X509Key> key;
		if (!key.Set(this->GetNewPublicKey()))
		{
			return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
		}
		Bool signValid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
		key.Delete();
		if (signValid)
		{
			return Crypto::Cert::X509File::ValidStatus::SelfSigned;
		}
		else
		{
			return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
		}
	}

	NotNullPtr<Crypto::Cert::X509Key> key;
	if (!key.Set(issuer->GetNewPublicKey()))
	{
		return Crypto::Cert::X509File::ValidStatus::FileFormatInvalid;
	}
	Bool signValid = ssl->SignatureVerify(key, hashType, signedInfo.payload, signedInfo.payloadSize, signedInfo.signature, signedInfo.signSize);
	key.Delete();
	if (!signValid)
	{
		return Crypto::Cert::X509File::ValidStatus::SignatureInvalid;
	}

	Data::ArrayList<Text::CString> crlDistributionPoints;
	this->GetCRLDistributionPoints(&crlDistributionPoints);
	//////////////////////////
	// CRL
	return Crypto::Cert::X509File::ValidStatus::Valid;*/	
	}

	clone()
	{
		return new X509Cert(this.sourceName, this.reader.getArrayBuffer());
	}

	createX509Cert()
	{
		return new X509Cert(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		///////////////////////////////////////////////
	}

	createNames()
	{
		return new ASN1Names().setCertificate();
	}

	getIssuerNames()
	{
		/////////////////////////////////////////////
	}

	getSubjNames()
	{
		/////////////////////////////////////////////
	}
	getExtensions()
	{
		/////////////////////////////////////////////
	}

	getNewPublicKey()
	{
		/////////////////////////////////////////////
	}

	getKeyId()
	{
		/////////////////////////////////////////////
	}

	getNotBefore()
	{
		/////////////////////////////////////////////
	}

	getNotAfter()
	{
		/////////////////////////////////////////////
	}

	domainValid(domain)
	{
		/////////////////////////////////////////////
	}

	isSelfSigned()
	{
		/////////////////////////////////////////////
	}

	getCRLDistributionPoints()
	{
		/////////////////////////////////////////////
	}

	getIssuerNamesSeq()
	{
		/////////////////////////////////////////////
	}

	getSerialNumber()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return null;
		}
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.2");
			if (pdu && pdu.itemType == ASN1ItemType.INTEGER)
			{
				return this.reader.getArrayBuffer(pdu.rawOfst + pdu.hdrLen, pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
			if (pdu && pdu.itemType == ASN1ItemType.INTEGER)
			{
				return this.reader.getArrayBuffer(pdu.rawOfst + pdu.hdrLen, pdu.contLen);
			}
		}
		return 0;
	}
}

export class X509Key extends X509File
{
	constructor(sourceName, buff, keyType)
	{
		super(sourceName, "application/x-pem-file", buff);
		this.keyType = keyType;
	}

	getFileType()
	{
		return X509FileType.Key;
	}

	toShortName()
	{
		return keyTypeGetName(this.keyType)+" "+this.getKeySizeBits()+" bits";
	}

	isValid()
	{
		if (this.keyType == KeyType.Unknown)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		return CertValidStatus.Valid;
	}
	
	clone()
	{
		return new X509Key(this.sourceName, this.reader.getArrayBuffer(0, this.reader.getLength()), this.keyType);
	}

	toString()
	{
		let strs = [];
		let buff;
		if (this.keyType == KeyType.RSA)
		{
			buff = this.getRSAModulus();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Modulus = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAPublicExponent();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Public Exponent = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAPrivateExponent();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Private Exponent = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAPrime1();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Prime1 = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAPrime2();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Prime2 = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAExponent1();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Exponent1 = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAExponent2();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Exponent2 = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSACoefficient();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Coefficient = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
		}
		else if (this.keyType == KeyType.RSAPublic)
		{
			buff = this.getRSAModulus();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Modulus = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getRSAPublicExponent();
			if (buff)
			{
				strs.push(this.sourceName+".RSA.Public Exponent = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
		}
		else if (this.keyType == KeyType.ECPublic)
		{
			let ecName = this.getECName();
			strs.push(this.sourceName+".EC.Name = "+ecNameGetName(ecName));
	
			buff = this.getECPublic();
			if (buff)
			{
				strs.push(this.sourceName+".EC.Public = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
		}
		else if (this.keyType == KeyType.ECDSA)
		{
			let ecName = this.getECName();
			strs.push(this.sourceName+".EC.Name = "+ecNameGetName(ecName));
	
			buff = this.getECPrivate();
			if (buff)
			{
				strs.push(this.sourceName+".EC.Private = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
			buff = this.getECPublic();
			if (buff)
			{
				strs.push(this.sourceName+".EC.Public = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
			}
		}
	
		buff = this.getKeyId();
		if (buff)
		{
			strs.push(this.sourceName+".KeyId = "+text.u8Arr2Hex(new Uint8Array(buff), ' ', null));
		}
		return strs.join("\r\n");
	}

	createNames()
	{
		let names = new ASN1Names();
		switch (this.keyType)
		{
		case KeyType.RSA:
			return names.setRSAPrivateKey();
		case KeyType.RSAPublic:
			return names.setRSAPublicKey();
		default:
		case KeyType.DSA:
		case KeyType.ECDSA:
		case KeyType.ECPublic:
		case KeyType.ED25519:
		case KeyType.Unknown:
			return names;
		}
	}

	getKeyType()
	{
		return this.keyType;
	}

	getKeySizeBits()
	{
		return X509File.keyGetLeng(this.reader, 0, this.reader.getLength(), this.keyType);
	}

	isPrivateKey()
	{
		switch (this.keyType)
		{
		case KeyType.DSA:
		case KeyType.ECDSA:
		case KeyType.ED25519:
		case KeyType.RSA:
			return true;
		case KeyType.RSAPublic:
		case KeyType.ECPublic:
		case KeyType.Unknown:
		default:
			return false;
		}
	}

	createPublicKey()
	{
		if (this.keyType == KeyType.RSAPublic)
		{
			return this.clone();
		}
		else if (this.keyType == KeyType.RSA)
		{
			let builder = new ASN1PDUBuilder();
			let buff;
			builder.beginSequence();
			if ((buff = this.getRSAModulus()) == null) return null;
			builder.appendOther(ASN1ItemType.INTEGER, buff);
			if ((buff = this.getRSAPublicExponent()) == null) return null;
			builder.appendOther(ASN1ItemType.INTEGER, buff);
			builder.endLevel();
			return new X509Key(this.sourceName, builder.getArrayBuffer(), KeyType.RSAPublic);
		}
		else if (this.keyType == KeyType.ECPublic)
		{
			return this.clone();
		}
		else
		{
			return null;
		}		
	}

	getKeyId()
	{
		let pubKey = this.createPublicKey();
		if (pubKey)
		{
			let sha1 = new hash.SHA1();
			sha1.calc(pubKey.getASN1Buff().getArrayBuffer());
			return sha1.getValue();
		}
		return null;
	}

	getRSAModulus()
	{
		let len = null;
		if (this.keyType == KeyType.RSA)
		{
			len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
		}
		else if (this.keyType == KeyType.RSAPublic)
		{
			len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1");
		}
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAPublicExponent()
	{
		let len = null;
		if (this.keyType == KeyType.RSA)
		{
			len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
		}
		else if (this.keyType == KeyType.RSAPublic)
		{
			len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
		}
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAPrivateExponent()
	{
		if (this.keyType != KeyType.RSA) return 0;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.4");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAPrime1()
	{
		if (this.keyType != KeyType.RSA) return 0;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.5");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAPrime2()
	{
		if (this.keyType != KeyType.RSA) return 0;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.6");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAExponent1()
	{
		if (this.keyType != KeyType.RSA) return 0;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.7");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAExponent2()
	{
		if (this.keyType != KeyType.RSA) return 0;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.8");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSACoefficient()
	{
		if (this.keyType != KeyType.RSA) return 0;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.9");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getECPrivate()
	{
		if (this.keyType == KeyType.ECDSA)
		{
			let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
			if (len)
			{
				return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
			}
			return null;
		}
		else
		{
			return null;
		}
	}

	getECPublic()
	{
		let itemPDU;
		if (this.keyType == KeyType.ECPublic)
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
			if (itemPDU != null && itemPDU.itemType == ASN1ItemType.BIT_STRING)
			{
				return this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1);
			}
			return 0;
		}
		else if (this.keyType == KeyType.ECDSA)
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
			if (itemPDU != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				itemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
				if (itemPDU != null && itemPDU.itemType == ASN1ItemType.BIT_STRING)
				{
					return this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1);
				}
				return 0;
			}
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.4");
			if (itemPDU != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				itemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
				if (itemPDU != null && itemPDU.itemType == ASN1ItemType.BIT_STRING)
				{
					return this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1);
				}
				return 0;
			}
			return 0;
		}
		else
		{
			return 0;
		}
	}

	getECName()
	{
		if (this.keyType == KeyType.ECPublic)
		{
			let itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.2");
			if (itemPDU != null && itemPDU.itemType == ASN1ItemType.OID)
			{
				return ecNameFromOID(this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen));
			}
		}
		else if (this.keyType == KeyType.ECDSA)
		{
			let itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
			if (itemPDU != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				itemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
				if (itemPDU != null && itemPDU.itemType == ASN1ItemType.OID)
				{
					return ecNameFromOID(this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen));
				}
			}
		}
		return ECName.Unknown;		
	}
}

export function fileTypeGetName(fileType)
{
	switch (fileType)
	{
	case X509FileType.Cert:
		return "Cert";
	case X509FileType.CertRequest:
		return "CertReq";
	case X509FileType.Key:
		return "Key";
	case X509FileType.PrivateKey:
		return "PrivateKey";
	case X509FileType.PublicKey:
		return "PublicKey";
	case X509FileType.PKCS7:
		return "PKCS7";
	case X509FileType.PKCS12:
		return "PKCS12";
	case X509FileType.CRL:
		return "CRL";
	case X509FileType.FileList:
		return "FileList";
	default:
		return "Unknown";
	}
}

export function keyTypeGetName(keyType)
{
	switch (keyType)
	{
	case KeyType.RSA:
		return "RSA";
	case KeyType.DSA:
		return "DSA";
	case KeyType.ECDSA:
		return "ECDSA";
	case KeyType.ED25519:
		return "ED25519";
	case KeyType.RSAPublic:
		return "RSAPublic";
	case KeyType.ECPublic:
		return "ECPublic";
	case KeyType.Unknown:
	default:
		return "Unknown";
	}
}

export function ecNameGetName(ecName)
{
	switch (ecName)
	{
	case ECName.secp256r1:
		return "secp256r1";
	case ECName.secp384r1:
		return "secp384r1";
	case ECName.secp521r1:
		return "secp521r1";
	case ECName.Unknown:
	default:
		return "Unknown";
	}
}

export function ecNameGetOID(ecName)
{
	switch (ecName)
	{
	case ECName.secp256r1:
		return "1.2.840.10045.3.1.7";
	case ECName.secp384r1:
		return "1.3.132.0.34";
	case ECName.secp521r1:
		return "1.3.132.0.35";
	case ECName.Unknown:
	default:
		return "1.3.132.0.34";
	}
}

export function ecNameFromOID(buff)
{
	let arr = new Uint8Array(buff);
	if (ASN1Util.oidEqualsText(arr, "1.2.840.10045.3.1.7"))
	{
		return ECName.secp256r1;
	}
	else if (ASN1Util.oidEqualsText(arr, "1.3.132.0.34"))
	{
		return ECName.secp384r1;
	}
	else if (ASN1Util.oidEqualsText(arr, "1.3.132.0.35"))
	{
		return ECName.secp521r1;
	}
	return ECName.Unknown;
}