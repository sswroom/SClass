import {ASN1ItemType, ASN1Util, ASN1Names, ASN1PDUBuilder} from "./certutil.js";
import * as data from "./data.js";
import * as hash from "./hash.js";
import * as net from "./net.js";
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

export const ContentDataType = {
	Unknown: 0,
	AuthenticatedSafe: 1
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

	static isSigned(reader, startOfst, endOfst, path)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt < 3)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".2") != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".3") != ASN1ItemType.BIT_STRING)
		{
			return false;
		}
		return true;
	}

	static appendSigned(reader, startOfst, endOfst, path, sb, varName)
	{
		let name;
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+".2")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "algorithmIdentifier";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name, false);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+".3")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.BIT_STRING)
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("signature = ");
				sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1)), ':', null));
				sb.push("\r\n");
			}
		}		
	}

	static isTBSCertificate(reader, startOfst, endOfst, path)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt < 6)
		{
			return false;
		}
		let i = 1;
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i)) == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			i++;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.INTEGER)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		return true;
	}

	static appendTBSCertificate(reader, startOfst, endOfst, path, sb, varName)
	{
		let name;
		let i = 1;
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("version = ");
				X509File.appendVersion(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1", sb);
				sb.push("\r\n");
				i++;
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.INTEGER)
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("serialNumber = ");
				sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen)), ':', null));
				sb.push("\r\n");
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "signature";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name, false);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "issuer";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendName(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "validity";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendValidity(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "subject";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendName(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "subjectPublicKeyInfo";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendSubjectPublicKeyInfo(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
				let pubKey = new X509PubKey(name, reader.getArrayBuffer(itemPDU.rawOfst, itemPDU.hdrLen + itemPDU.contLen));
				let key = pubKey.createKey();
				if (key)
				{
					sb.push(key.toString());
					sb.push("\r\n");
				}
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_3)
			{
				name = "extensions";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendCRLExtensions(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
			}
		}
	}

	static isCertificate(reader, startOfst, endOfst, path)
	{
		return X509File.isSigned(reader, startOfst, endOfst, path) && X509File.isTBSCertificate(reader, startOfst, endOfst, path+".1");
	}

	static appendCertificate(reader, startOfst, endOfst, path, sb, varName)
	{
		X509File.appendTBSCertificate(reader, startOfst, endOfst, path+".1", sb, varName);
		X509File.appendSigned(reader, startOfst, endOfst, path, sb, varName);
	}

	static isTBSCertList(reader, startOfst, endOfst, path)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		let i = 1;
		if (cnt < 4)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i)) == ASN1ItemType.INTEGER)
		{
			i++;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.UTCTIME)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i)) == ASN1ItemType.UTCTIME)
		{
			i++;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		let itemType = ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++));
		if (itemType != ASN1ItemType.CONTEXT_SPECIFIC_0 && itemType != ASN1ItemType.UNKNOWN)
		{
			return false;
		}
		return true;
	}

	static appendTBSCertList(reader, startOfst, endOfst, path, sb, varName)
	{
		let dt;
		let name;
		let i = 1;
		let itemPDU;
		let subitemPDU;
		let subsubitemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.INTEGER)
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("version = ");
				X509File.appendVersion(reader, startOfst, endOfst, path+"."+(i), sb);
				sb.push("\r\n");
				i++;
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "signature";
				if (varName)
				{
					name = varName+"."+name;
				}
				X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name, false);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				name = "issuer";
				if (varName)
				{
					name = varName+"."+name;
				}
				X509File.appendName(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.UTCTIME && (dt = ASN1Util.pduParseUTCTimeCont(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen)))
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("thisUpdate = ");
				sb.push(dt.toStringNoZone());
				sb.push("\r\n");
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i))) != null && itemPDU.itemType == ASN1ItemType.UTCTIME)
		{
			if (dt = ASN1Util.pduParseUTCTimeCont(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen))
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("nextUpdate = ");
				sb.push(dt.toStringNoZone());
				sb.push("\r\n");
			}
			i++;
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			let j = 0;
			while (true)
			{
				j++;
				if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, j.toString())) == null || subitemPDU.itemType != ASN1ItemType.SEQUENCE)
				{
					break;
				}
	
				if ((subsubitemPDU = ASN1Util.pduGetItem(reader, subitemPDU.rawOfst + subitemPDU.hdrLen, subitemPDU.rawOfst + subItemPDU.hdrLen + subitemPDU.contLen, "1")) != null && subsubitemPDU.itemType == ASN1ItemType.INTEGER)
				{
					if (varName)
					{
						sb.push(varName+".");
					}
					sb.push("revokedCertificates[");
					sb.push(j.toString());
					sb.push("].userCertificate = ");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(subsubitemPDU.rawOfst + subsubitemPDU.hdrLen, subsubitemPDU.contLen)), ':', null));
					sb.push("\r\n");
				}
				if ((subsubitemPDU = ASN1Util.pduGetItem(reader, subitemPDU.rawOfst + subitemPDU.hdrLen, subitemPDU.rawOfst + subItemPDU.hdrLen + subitemPDU.contLen, "2")) != null && subsubitemPDU.itemType == ASN1ItemType.UTCTIME && (dt = ASN1Util.pduParseUTCTimeCont(reader, subsubitemPDU.rawOfst + subsubitemPDU.hdrLen, subsubitemPDU.rawOfst + subsubitemPDU.hdrLen + subsubitemPDU.contLen)))
				{
					if (varName)
					{
						sb.push(varName+".");
					}
					sb.push("revokedCertificates[");
					sb.push(j);
					sb.push("].revocationDate = ");
					sb.push(dt.toStringNoZone());
					sb.push("\r\n");
				}
				if ((subsubitemPDU = ASN1Util.pduGetItem(reader, subitemPDU.rawOfst + subitemPDU.hdrLen, subitemPDU.rawOfst + subItemPDU.hdrLen + subitemPDU.contLen, "3")) != null && subsubitemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					name = "revokedCertificates[";
					if (varName)
					{
						name = varName+"."+name;
					}
					name = name + j + "].crlEntryExtensions";
					X509File.appendCRLExtensions(reader, subsubitemPDU.rawOfst, subsubitemPDU.rawOfst + subsubitemPDU.hdrLen + subsubitemPDU.contLen, sb, name);
				}
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				name = "crlExtensions";
				if (varName)
				{
					name = varName + "." + name;
				}
				X509File.appendCRLExtensions(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, name);
			}
		}
	}

	static isCertificateList(reader, startOfst, endOfst, path)
	{
		return X509File.isSigned(reader, startOfst, endOfst, path) && X509File.isTBSCertList(reader, startOfst, endOfst, path+".1");
	}

	static appendCertificateList(reader, startOfst, endOfst, path, sb, varName)
	{
		X509File.appendTBSCertList(reader, startOfst, endOfst, path+".1", sb, varName);
		X509File.appendSigned(reader, startOfst, endOfst, path, sb, varName);
	}

/*	static isPrivateKeyInfo(reader, startOfst, endOfst, path)
	{
		UOSInt cnt = ASN1Util.pduCountItem(pdu, pduEnd, path);
		if (cnt != 3 && cnt != 4)
		{
			return false;
		}
		Char sbuff[256];
		Char *sptr;
		sptr = Text.StrConcat(sbuff, path);
		Text.StrConcat(sptr, ".1");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.INTEGER)
		{
			return false;
		}
		Text.StrConcat(sptr, ".2");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		Text.StrConcat(sptr, ".3");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.OCTET_STRING)
		{
			return false;
		}
		if (cnt == 4)
		{
			Text.StrConcat(sptr, ".4");
			if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.SET)
			{
				return false;
			}
		}
		return true;
	}

	static appendPrivateKeyInfo(reader, startOfst, endOfst, path, sb)
	{
		Char sbuff[256];
		Char *sptr;
		const UInt8 *itemPDU;
		UOSInt len;
		Net.ASN1Util.ItemType itemType;
		KeyType keyType = KeyType.Unknown;
		sptr = Text.StrConcat(sbuff, path);
		Text.StrConcat(sptr, ".1");
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, sbuff, len, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.INTEGER)
			{
				sb.push("version = "));
				AppendVersion(pdu, pduEnd, sbuff, sb);
				sb.push("\r\n"));
			}
		}
		Text.StrConcat(sptr, ".2");
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, sbuff, len, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.SEQUENCE)
			{
				AppendAlgorithmIdentifier(itemPDU, itemPDU + len, sb, CSTR("privateKeyAlgorithm"), false, &keyType);
			}
		}
		Text.StrConcat(sptr, ".3");
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, sbuff, len, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.OCTET_STRING)
			{
				sb.push("privateKey = "));
				sb.push("\r\n"));
				if (keyType != KeyType.Unknown)
				{
					Crypto.Cert.X509Key privkey(CSTR("PrivKey"), Data.ByteArrayR(itemPDU, len), keyType);
					privkey.ToString(sb);
				}
			}
		}
	}

	static isCertificateRequestInfo(reader, startOfst, endOfst, path)
	{
		UOSInt cnt = ASN1Util.pduCountItem(pdu, pduEnd, path);
		if (cnt < 4)
		{
			return false;
		}
		Char sbuff[256];
		Char *sptr = Text.StrConcat(sbuff, path);
		*sptr++ = '.';
		UOSInt i = 1;
		Text.StrUOSInt(sptr, i++);
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.INTEGER)
		{
			return false;
		}
		Text.StrUOSInt(sptr, i++);
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		Text.StrUOSInt(sptr, i++);
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		Text.StrUOSInt(sptr, i++);
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return false;
		}
		return true;
	}

	static appendCertificateRequestInfo(reader, startOfst, endOfst, path, sb)
	{
		Char sbuff[256];
		Char *sptr = Text.StrConcat(sbuff, path);
		*sptr++ = '.';
		UOSInt i = 1;
		const UInt8 *itemPDU;
		UOSInt itemLen;
		const UInt8 *subitemPDU;
		UOSInt subitemLen;
		Net.ASN1Util.ItemType itemType;
		Text.StrUOSInt(sptr, i++);
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, sbuff, itemLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.INTEGER)
			{
				sb.push("serialNumber = "));
				AppendVersion(pdu, pduEnd, sbuff, sb);
				sb.push("\r\n"));
			}
		}
		Text.StrUOSInt(sptr, i++);
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, sbuff, itemLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.SEQUENCE)
			{
				AppendName(itemPDU, itemPDU + itemLen, sb, CSTR("subject"));
			}
		}
		Text.StrUOSInt(sptr, i++);
		UOSInt itemOfst;
		if ((itemPDU = Net.ASN1Util.PDUGetItemRAW(pdu, pduEnd, sbuff, itemLen, itemOfst)) != 0)
		{
			if (itemPDU[0] == ASN1ItemType.SEQUENCE)
			{
				AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, CSTR("subjectPublicKeyInfo"));
				Crypto.Cert.X509PubKey *pubKey;
				Crypto.Cert.X509Key *key;
				NEW_CLASS(pubKey, Crypto.Cert.X509PubKey(CSTR("PubKey"), Data.ByteArrayR(itemPDU, itemOfst + itemLen)));
				key = pubKey->CreateKey();
				if (key)
				{
					key->ToString(sb);
					sb->AppendLB(Text.LineBreakType.CRLF);
					DEL_CLASS(key);
				}
				DEL_CLASS(pubKey);
			}
		}
		Text.StrUOSInt(sptr, i++);
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, sbuff, itemLen, itemType)) != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			i = 1;
			Text.StrUOSInt(sbuff, i);
			while ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, sbuff, subitemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
			{
				const UInt8 *extOID;
				UOSInt extOIDLen;
				const UInt8 *ext;
				UOSInt extLen;
				if ((extOID = ASN1Util.pduGetItem(subitemPDU, subitemPDU + subitemLen, "1", extOIDLen, itemType)) != 0 && itemType == ASN1ItemType.OID &&
					(ext = ASN1Util.pduGetItem(subitemPDU, subitemPDU + subitemLen, "2", extLen, itemType)) != 0 && itemType == ASN1ItemType.SET)
				{
					if (Net.ASN1Util.OIDEqualsText(extOID, extOIDLen, UTF8STRC("1.2.840.113549.1.9.14")))
					{
						AppendCRLExtensions(ext, ext + extLen, sb, CSTR("extensionRequest"));
					}
					else if (Net.ASN1Util.OIDEqualsText(extOID, extOIDLen, UTF8STRC("1.3.6.1.4.1.311.13.2.3"))) //szOID_OS_VERSION
					{
						AppendMSOSVersion(ext, ext + extLen, sb, CSTR("osVersion"));
					}
					else if (Net.ASN1Util.OIDEqualsText(extOID, extOIDLen, UTF8STRC("1.3.6.1.4.1.311.21.20"))) //szOID_REQUEST_CLIENT_INFO
					{
						AppendMSRequestClientInfo(ext, ext + extLen, sb, CSTR("reqClientInfo"));
					}
					else if (Net.ASN1Util.OIDEqualsText(extOID, extOIDLen, UTF8STRC("1.3.6.1.4.1.311.13.2.2"))) //szOID_ENROLLMENT_CSP_PROVIDER
					{
						AppendMSEnrollmentCSPProvider(ext, ext + extLen, sb, CSTR("enrollCSPProv"));
					}
				}
				Text.StrUOSInt(sbuff, ++i);
			}
		}
	}

	static isCertificateRequest(reader, startOfst, endOfst, path)
	{
		Char sbuff[256];
		Text.StrConcat(Text.StrConcat(sbuff, path), ".1");
		return IsSigned(pdu, pduEnd, path) && IsCertificateRequestInfo(pdu, pduEnd, sbuff);
	}

	static appendCertificateRequest(reader, startOfst, endOfst, path, sb)
	{
		Char sbuff[256];
		Text.StrConcat(Text.StrConcat(sbuff, path), ".1");
		AppendCertificateRequestInfo(pdu, pduEnd, sbuff, sb);
		AppendSigned(pdu, pduEnd, path, sb, CSTR_NULL);
	}

	static isPublicKeyInfo(reader, startOfst, endOfst, path)
	{
		UOSInt cnt = ASN1Util.pduCountItem(pdu, pduEnd, path);
		if (cnt < 2)
		{
			return false;
		}
		Char sbuff[256];
		Char *sptr = Text.StrConcat(sbuff, path);
		Text.StrConcat(sptr, ".1");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		Text.StrConcat(sptr, ".2");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.BIT_STRING)
		{
			return false;
		}
		return true;
	}

	static appendPublicKeyInfo(reader, startOfst, endOfst, path, sb)
	{
		UOSInt itemOfst;
		UOSInt buffSize;
		const UInt8 *buff = Net.ASN1Util.PDUGetItemRAW(pdu, pduEnd, path, buffSize, itemOfst);
		if (buff[0] == ASN1ItemType.SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(buff + itemOfst, buff + itemOfst + buffSize, sb, CSTR("PubKey"));
			Crypto.Cert.X509PubKey *pubKey;
			Crypto.Cert.X509Key *key;
			NEW_CLASS(pubKey, Crypto.Cert.X509PubKey(CSTR("PubKey"), Data.ByteArrayR(buff, itemOfst + buffSize)));
			key = pubKey->CreateKey();
			if (key)
			{
				key->ToString(sb);
				sb->AppendLB(Text.LineBreakType.CRLF);
				DEL_CLASS(key);
			}
			DEL_CLASS(pubKey);
		}
	}

	static isContentInfo(reader, startOfst, endOfst, path)
	{
		if (ASN1Util.pduGetItemType(pdu, pduEnd, path) != ASN1ItemType.SEQUENCE)
			return false;
		UOSInt cnt = ASN1Util.pduCountItem(pdu, pduEnd, path);
		if (cnt != 2)
		{
			return false;
		}
		Char sbuff[256];
		Char *sptr = Text.StrConcat(sbuff, path);
		Text.StrConcat(sptr, ".1");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.OID)
		{
			return false;
		}
		Text.StrConcat(sptr, ".2");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return false;
		}
		return true;
	}

	static appendContentInfo(reader, startOfst, endOfst, path, sb, varName, dataType)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		UOSInt buffSize;
		Net.ASN1Util.ItemType itemType;
		const UInt8 *buff = ASN1Util.pduGetItem(pdu, pduEnd, path, buffSize, itemType);
		if (itemType == ASN1ItemType.SEQUENCE)
		{
			Net.ASN1Util.ItemType itemType1;
			UOSInt contentTypeLen;
			const UInt8 *contentType = ASN1Util.pduGetItem(buff, buff + buffSize, "1", contentTypeLen, itemType1);
			Net.ASN1Util.ItemType itemType2;
			UOSInt contentLen;
			const UInt8 *content = ASN1Util.pduGetItem(buff, buff + buffSize, "2", contentLen, itemType2);
	
			if (contentType)
			{
				if (varName.v)
				{
					sb.push(varName);
					sb->AppendUTF8Char('.');
				}
				sb.push("content-type = "));
				Net.ASN1Util.OIDToString(contentType, contentTypeLen, sb);
				const Net.ASN1OIDDB.OIDInfo *oid = Net.ASN1OIDDB.OIDGetEntry(contentType, contentTypeLen);
				if (oid)
				{
					sb.push(" ("));
					sb->AppendSlow((const UTF8Char*)oid->name);
					sb->AppendUTF8Char(')');
				}
				sb.push("\r\n"));
			}
			if (contentType && content)
			{
				if (Net.ASN1Util.OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.1"))) //data
				{
					UOSInt itemLen;
					const UInt8 *itemPDU = ASN1Util.pduGetItem(content, content + contentLen, "1", itemLen, itemType);
					if (itemPDU != 0 && itemType == ASN1ItemType.OCTET_STRING)
					{
						sptr = varName.ConcatTo(sbuff);
						sptr = Text.StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
						AppendData(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), dataType);
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.2"))) //signedData
				{
					sptr = varName.ConcatTo(sbuff);
					sptr = Text.StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
					AppendPKCS7SignedData(content, content + contentLen, sb, CSTRP(sbuff, sptr));
				}
				else if (Net.ASN1Util.OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.3"))) //envelopedData
				{
	
				}
				else if (Net.ASN1Util.OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.4"))) //signedAndEnvelopedData
				{
	
				}
				else if (Net.ASN1Util.OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.5"))) //digestedData
				{
	
				}
				else if (Net.ASN1Util.OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.6"))) //encryptedData
				{
					sptr = varName.ConcatTo(sbuff);
					sptr = Text.StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
					AppendEncryptedData(content, content + contentLen, sb, CSTRP(sbuff, sptr), dataType);
				}
			}
		}
	}

	static isPFX(reader, startOfst, endOfst, path)
	{
		if (ASN1Util.pduGetItemType(pdu, pduEnd, path) != ASN1ItemType.SEQUENCE)
			return false;
		UOSInt cnt = ASN1Util.pduCountItem(pdu, pduEnd, path);
		if (cnt != 2 && cnt != 3)
		{
			return false;
		}
		Char sbuff[256];
		Char *sptr = Text.StrConcat(sbuff, path);
		Text.StrConcat(sptr, ".1");
		if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.INTEGER)
		{
			return false;
		}
		Text.StrConcat(sptr, ".2");
		if (!IsContentInfo(pdu, pduEnd, sbuff))
		{
			return false;
		}
		if (cnt == 3)
		{
			Text.StrConcat(sptr, ".3");
			if (ASN1Util.pduGetItemType(pdu, pduEnd, sbuff) != ASN1ItemType.SEQUENCE)
			{
				return false;
			}
		}
		return true;		
	}

	static appendPFX(reader, startOfst, endOfst, path, sb, varName)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt buffSize;
		Net.ASN1Util.ItemType itemType;
		const UInt8 *buff = ASN1Util.pduGetItem(pdu, pduEnd, path, buffSize, itemType);
		if (itemType == ASN1ItemType.SEQUENCE)
		{
			UOSInt cnt = ASN1Util.pduCountItem(buff, buff + buffSize, 0);
	
			Net.ASN1Util.ItemType itemType;
			UOSInt versionLen;
			const UInt8 *version = ASN1Util.pduGetItem(buff, buff + buffSize, "1", versionLen, itemType);
			if (version && itemType == ASN1ItemType.INTEGER)
			{
				if (varName.v)
				{
					sb.push(varName);
					sb->AppendUTF8Char('.');
				}
				sb.push("version = "));
				Net.ASN1Util.IntegerToString(version, versionLen, sb);
				sb.push("\r\n"));
			}
			sptr = sbuff;
			if (varName.v)
			{
				sptr = varName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text.StrConcatC(sptr, UTF8STRC("authSafe"));
			AppendContentInfo(buff, buff + buffSize, "2", sb, CSTRP(sbuff, sptr), ContentDataType.AuthenticatedSafe);
			if (cnt == 3)
			{
				sptr = sbuff;
				if (varName.v)
				{
					sptr = varName.ConcatTo(sptr);
					*sptr++ = '.';
				}
				sptr = Text.StrConcatC(sptr, UTF8STRC("macData"));
				AppendMacData(buff, buff + buffSize, "3", sb, CSTRP(sbuff, sptr));
			}
		}
	}*/

	static appendVersion(reader, startOfst, endOfst, path, sb)
	{
		let itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path);
		if (itemPDU != null && itemPDU.itemType == ASN1ItemType.INTEGER)
		{
			if (itemPDU.contLen == 1)
			{
				switch (reader.readUInt8(itemPDU.rawOfst + itemPDU.hdrLen))
				{
				case 0:
					sb.push("v1");
					break;
				case 1:
					sb.push("v2");
					break;
				case 2:
					sb.push("v3");
					break;
				}
			}
		}
	}

	static appendAlgorithmIdentifier(reader, startOfst, endOfst, sb, varName, pubKey)
	{
		let itemPDU;
		let keyType = KeyType.Unknown;
		let innerKeyType = KeyType.Unknown;
		let algorithm = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		let parameters = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2");
		if (algorithm && algorithm.algorithmType == ASN1ItemType.OID)
		{
			sb.push(varName+".");
			sb.push("algorithm = ");
			sb.push(ASN1Util.oidToString(new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen))));
			keyType = keyTypeFromOID(new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen)), pubKey);
			let oid = null; //oiddb.oidGetEntry(new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen)));
			if (oid)
			{
				sb.push(" (" + oid.name + ")");
			}
			sb.push("\r\n");
		}
		if (parameters)
		{
			let oid = new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen));
			if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.5.13") && parameters.itemType == ASN1ItemType.SEQUENCE)
			{
				if ((itemPDU = ASN1Util.pduGetItem(reader, parameters.rawOfst + parameters.hdrLen, parameters.rawOfst + parameters.hdrLen + parameters.contLen, "1")) != null)
				{
					if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						innerKeyType = X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, varName + ".pbes2.kdf", true);
					}
				}
				if ((itemPDU = ASN1Util.pduGetItem(reader, parameters.rawOfst + parameters.hdrLen, parameters.rawOfst + parameters.hdrLen + parameters.contLen, "2")) != null)
				{
					if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						innerKeyType = X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, varName + ".pbes2.encryptScheme", true);
					}
				}
			}
			else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.5.12") && parameters.itemType == ASN1ItemType.SEQUENCE)
			{
				if ((itemPDU = ASN1Util.pduGetItem(reader, parameters.rawOfst + parameters.hdrLen, parameters.rawOfst + parameters.hdrLen + parameters.contLen, "1")) != null)
				{
					if (itemPDU.itemType == ASN1ItemType.OCTET_STRING)
					{
						sb.push(varName);
						sb.push(".pbkdf2.salt = ");
						sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen)), ' ', null));
						sb.push("\r\n");
					}
				}
				if ((itemPDU = ASN1Util.pduGetItem(reader, parameters.rawOfst + parameters.hdrLen, parameters.rawOfst + parameters.hdrLen + parameters.contLen, "2")) != null)
				{
					if (itemPDU.itemType == ASN1ItemType.INTEGER)
					{
						sb.push(varName);
						sb.push(".pbkdf2.iterationCount = ");
						sb.push(ASN1Util.integerToString(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen));
						sb.push("\r\n");
					}
				}
				if ((itemPDU = ASN1Util.pduGetItem(reader, parameters.rawOfst + parameters.hdrLen, parameters.rawOfst + parameters.hdrLen + parameters.contLen, "3")) != null)
				{
					if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						innerKeyType = X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, varName + ".pbkdf2.prf", true);
					}
				}
			}
			else if (ASN1Util.oidEqualsText(oid, "2.16.840.1.101.3.4.1.42") && parameters.itemType == ASN1ItemType.OCTET_STRING)
			{
				sb.push(varName);
				sb.push(".aes256-cbc.iv = ");
				sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(parameters.rawOfst + parameters.hdrLen, parameters.contLen)), ' ', null));
				sb.push("\r\n");
			}
			else if (ASN1Util.oidEqualsText(oid, "1.2.840.10045.2.1") && parameters.itemType == ASN1ItemType.OID)
			{
				sb.push(varName);
				sb.push(".ecPublicKey.parameters = ");
				sb.push(ASN1Util.oidToString(new Uint8Array(reader.getArrayBuffer(parameters.rawOfst + parameters.hdrLen, parameters.contLen))));
				let oidInfo = null; //oiddb.oidGetEntry(new Uint8Array(reader.getArrayBuffer(parameters.rawOfst + parameters.hdrLen, parameters.contLen)));
				if (oidInfo)
				{
					sb.push(" ("+oidInfo.name+")");
				}
				sb.push("\r\n");
			}
			else
			{
				sb.push(varName+".parameters = ");
				if (parameters.itemType == ASN1ItemType.NULL)
				{
					sb.push("NULL");
				}
				sb.push("\r\n");
			}
		}
		return keyType;
	}

	static appendValidity(reader, startOfst, endOfst, sb, varName)
	{
		let dt;
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if ((itemPDU.itemType == ASN1ItemType.UTCTIME || itemPDU.itemType == ASN1ItemType.GENERALIZEDTIME) && (dt = ASN1Util.pduParseUTCTimeCont(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen)))
			{
				sb.push(varName+".notBefore = ");
				sb.push(dt.toStringNoZone());
				sb.push("\r\n");
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null)
		{
			if ((itemPDU.itemType == ASN1ItemType.UTCTIME || itemPDU.itemType == ASN1ItemType.GENERALIZEDTIME) && (dt = ASN1Util.pduParseUTCTimeCont(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen)))
			{
				sb.push(varName+".notAfter = ");
				sb.push(dt.toStringNoZone());
				sb.push("\r\n");
			}
		}
	}

	static appendSubjectPublicKeyInfo(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let keyType = KeyType.Unknown;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				keyType = X509File.appendAlgorithmIdentifier(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, varName+".algorithm", true);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.BIT_STRING)
			{
				sb.push(varName);
				sb.push(".subjectPublicKey = ");
				sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1)), ':', null));
				sb.push("\r\n");
				if (keyType != KeyType.Unknown)
				{
					sptr = Text.StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".subjectPublicKey"));
					let pkey = new X509Key(varName+".subjectPublicKey", reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1), keyType);
					sb.push(pkey.toString());
					sb.push("\r\n");
				}
			}
		}
	}

	static appendName(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		let i = 0;
		while (i < cnt)
		{
			i++;
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, i.toString())) != null)
			{
				if (itemPDU.itemType == ASN1ItemType.SET)
				{
					X509File.appendRelativeDistinguishedName(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, varName);
				}
			}
		}
	}

	static appendRelativeDistinguishedName(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		let i = 0;
		while (i < cnt)
		{
			i++;
	
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, i.toString())) != null)
			{
				if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					X509File.appendAttributeTypeAndDistinguishedValue(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, sb, varName);
				}
			}
		}
	}

	static appendAttributeTypeAndDistinguishedValue(reader, startOfst, endOfst, sb, varName)
	{
		let typePDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		let valuePDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2");
		if (typePDU && valuePDU && typePDU.itemType == ASN1ItemType.OID)
		{
			sb.push(varName+".");
			let typeOID = reader.getArrayBuffer(typePDU.rawOfst + typePDU.hdrLen, typePDU.contLen);
			if (ASN1Util.oidEqualsText(typeOID, "2.5.4.3"))
			{
				sb.push("commonName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.4"))
			{
				sb.push("surname");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.6"))
			{
				sb.push("countryName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.7"))
			{
				sb.push("localityName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.8"))
			{
				sb.push("stateOrProvinceName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.9"))
			{
				sb.push("streetAddress");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.10"))
			{
				sb.push("organizationName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.11"))
			{
				sb.push("organizationalUnitName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.12"))
			{
				sb.push("title");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.42"))
			{
				sb.push("givenName");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "2.5.4.43"))
			{
				sb.push("initials");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "0.9.2342.19200300.100.1.25"))
			{
				sb.push("domainComponent");
			}
			else if (ASN1Util.oidEqualsText(typeOID, "1.2.840.113549.1.9.1"))
			{
				sb.push("emailAddress");
			}
			else
			{
				sb.push(ASN1Util.oidToString(typeOID));
			}
			sb.push(" = ");
			if (valuePDU.itemType == ASN1ItemType.BMPSTRING)
			{
				sb.push(reader.readUTF16(valuePDU.rawOfst + valuePDU.hdrLen, valuePDU.contLen >> 1, false));
			}
			else
			{
				sb.push(reader.readUTF8(valuePDU.rawOfst + valuePDU.hdrLen, valuePDU.contLen));
			}
			sb.push("\r\n");
		}
	}

	static appendCRLExtensions(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let subItemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE || itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				let i = 1;
				while ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, i.toString())) != null)
				{
					X509File.appendCRLExtension(reader, subItemPDU.rawOfst + subItemPDU.hdrLen, subItemPDU.rawOfst + subItemPDU.hdrLen + subItemPDU.contLen, sb, varName);
					i++;
				}
			}
		}
	}

/*	static appendCRLExtension(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		const UInt8 *extension = 0;
		UOSInt extensionLen = 0;
		const UInt8 *itemPDU;
		UOSInt itemLen;
		const UInt8 *subItemPDU;
		UOSInt subItemLen;
		Net.ASN1Util.ItemType itemType;
		if ((extension = ASN1Util.pduGetItem(pdu, pduEnd, "1", extensionLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.OID)
			{
				sb.push(varName);
				sb->AppendUTF8Char('.');
				sb.push("extensionType = "));
				Net.ASN1Util.OIDToString(extension, extensionLen, sb);
				const Net.ASN1OIDDB.OIDInfo *oid = Net.ASN1OIDDB.OIDGetEntry(extension, extensionLen);
				if (oid)
				{
					sb.push(" ("));
					sb->AppendSlow((const UTF8Char*)oid->name);
					sb->AppendUTF8Char(')');
				}
				sb.push("\r\n"));
			}
		}
		else
		{
			return;
		}
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "2", itemLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.BOOLEAN)
			{
				sb.push(varName);
				sb->AppendUTF8Char('.');
				sb.push("critical = "));
				Net.ASN1Util.BooleanToString(itemPDU, itemLen, sb);
				sb.push("\r\n"));
				if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "3", itemLen, itemType)) == 0)
				{
					return;
				}
			}
			if (itemType == ASN1ItemType.OCTET_STRING)
			{
				if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("1.3.6.1.5.5.7.1.1"))) //id-pe-authorityInfoAccess
				{
					if ((itemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
					{
						UOSInt i = 1;
						Text.StrUOSInt(sbuff, i);
						while ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
						{
							const UInt8 *descPDU;
							UOSInt descLen;
							if ((descPDU = ASN1Util.pduGetItem(subItemPDU, subItemPDU + subItemLen, "1", descLen, itemType)) != 0 && itemType == ASN1ItemType.OID)
							{
								sb.push(varName);
								sb.push(".authorityInfoAccess["));
								sb->AppendUOSInt(i);
								sb.push("].accessMethod = "));
								Net.ASN1Util.OIDToString(descPDU, descLen, sb);
								sb.push(" ("));
								Net.ASN1OIDDB.OIDToNameString(descPDU, descLen, sb);
								sb.push(")\r\n"));
							}
							sptr = varName.ConcatTo(sbuff);
							sptr = Text.StrConcatC(sptr, UTF8STRC(".authorityInfoAccess["));
							sptr = Text.StrUOSInt(sptr, i);
							sptr = Text.StrConcatC(sptr, UTF8STRC("].accessLocation"));
							AppendGeneralName(subItemPDU, subItemPDU + subItemLen, "2", sb, CSTRP(sbuff, sptr));
							
							Text.StrUOSInt(sbuff, ++i);
						}
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.14"))) //id-ce-subjectKeyIdentifier
				{
					sb.push(varName);
					sb->AppendUTF8Char('.');
					sb.push("subjectKeyId = "));
					if (itemLen == 22 && itemPDU[1] == 20)
					{
						sb->AppendHexBuff(itemPDU + 2, itemLen - 2, ':', Text.LineBreakType.None);
					}
					else
					{
						sb->AppendHexBuff(itemPDU, itemLen, ':', Text.LineBreakType.None);
					}
					sb.push("\r\n"));
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.15"))) //id-ce-keyUsage
				{
					if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.BIT_STRING)
					{
						sb.push(varName);
						sb->AppendUTF8Char('.');
						sb.push("keyUsage ="));
						if (subItemLen >= 2)
						{
							if (subItemPDU[1] & 0x80) sb.push(" digitalSignature"));
							if (subItemPDU[1] & 0x40) sb.push(" nonRepudiation"));
							if (subItemPDU[1] & 0x20) sb.push(" keyEncipherment"));
							if (subItemPDU[1] & 0x10) sb.push(" dataEncipherment"));
							if (subItemPDU[1] & 0x8) sb.push(" keyAgreement"));
							if (subItemPDU[1] & 0x4) sb.push(" keyCertSign"));
							if (subItemPDU[1] & 0x2) sb.push(" cRLSign"));
							if (subItemPDU[1] & 0x1) sb.push(" encipherOnly"));
						}
						if (subItemLen >= 3)
						{
							if (subItemPDU[2] & 0x80) sb.push(" decipherOnly"));
						}
						sb.push("\r\n"));
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.17"))) //id-ce-subjectAltName
				{
					sptr = varName.ConcatTo(sbuff);
					sptr = Text.StrConcatC(sptr, UTF8STRC(".subjectAltName"));
					AppendGeneralNames(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.19"))) //id-ce-basicConstraints
				{
					if ((itemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
					{
						if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.BOOLEAN)
						{
							sb.push(varName);
							sb->AppendUTF8Char('.');
							sb.push("basicConstraints.cA = "));
							Net.ASN1Util.BooleanToString(subItemPDU, subItemLen, sb);
							sb.push("\r\n"));
						}
						if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.INTEGER)
						{
							sb.push(varName);
							sb->AppendUTF8Char('.');
							sb.push("basicConstraints.pathLenConstraint = "));
							Net.ASN1Util.IntegerToString(subItemPDU, subItemLen, sb);
							sb.push("\r\n"));
						}
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.20"))) //id-ce-cRLNumber
				{
					if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.INTEGER)
					{
						sb.push(varName);
						sb->AppendUTF8Char('.');
						sb.push("cRLNumber = "));
						Net.ASN1Util.IntegerToString(subItemPDU, subItemLen, sb);
						sb.push("\r\n"));
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.21"))) //id-ce-cRLReasons
				{
					if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.ENUMERATED && subItemLen == 1)
					{
						sb.push(varName);
						sb->AppendUTF8Char('.');
						sb.push("cRLReasons = "));
						switch (subItemPDU[0])
						{
						case 0:
							sb.push("unspecified"));
							break;
						case 1:
							sb.push("keyCompromise"));
							break;
						case 2:
							sb.push("cACompromise"));
							break;
						case 3:
							sb.push("affiliationChanged"));
							break;
						case 4:
							sb.push("superseded"));
							break;
						case 5:
							sb.push("cessationOfOperation"));
							break;
						case 6:
							sb.push("certificateHold"));
							break;
						case 8:
							sb.push("removeFromCRL"));
							break;
						case 9:
							sb.push("privilegeWithdrawn"));
							break;
						case 10:
							sb.push("aACompromise"));
							break;
						default:
							sb.push("unknown"));
							break;
						}
						sb.push("\r\n"));
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.31"))) //id-ce-cRLDistributionPoints
				{
					if ((itemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
					{
						UOSInt i = 1;
						Text.StrUOSInt(sbuff, i);
						while (AppendDistributionPoint(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
						{
							Text.StrUOSInt(sbuff, ++i);
						}
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.32"))) //id-ce-certificatePolicies
				{
					if ((itemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
					{
						UOSInt i = 1;
						Text.StrUOSInt(sbuff, i);
						while (AppendPolicyInformation(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
						{
							Text.StrUOSInt(sbuff, ++i);
						}
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.35"))) //id-ce-authorityKeyIdentifier
				{
					if ((itemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
					{
						UOSInt i = 1;
						Text.StrUOSInt(sbuff, i);
						while ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType)) != 0)
						{
							if (itemType == 0x80)
							{
								sb.push(varName);
								sb->AppendUTF8Char('.');
								sb.push("authorityKey.keyId = "));
								sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text.LineBreakType.None);
								sb.push("\r\n"));
							}
							else if (itemType == 0x81 || itemType == 0xa1)
							{
								sptr = varName.ConcatTo(sbuff);
								sptr = Text.StrConcatC(sptr, UTF8STRC(".authorityKey.authorityCertIssuer"));
								AppendGeneralName(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTRP(sbuff, sptr));
							}
							else if (itemType == 0x82)
							{
								sb.push(varName);
								sb->AppendUTF8Char('.');
								sb.push("authorityKey.authorityCertSerialNumber = "));
								sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text.LineBreakType.None);
								sb.push("\r\n"));
							}
							Text.StrUOSInt(sbuff, ++i);
						}
					}
				}
				else if (Net.ASN1Util.OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.37"))) //id-ce-extKeyUsage
				{
					if ((itemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
					{
						UOSInt i = 1;
						const UInt8 *subItemPDU;
						UOSInt subItemLen;
						Text.StrUOSInt(sbuff, i);
	
						sb.push(varName);
						sb->AppendUTF8Char('.');
						sb.push("extKeyUsage ="));
	
						while ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType)) != 0)
						{
							if (itemType == ASN1ItemType.OID)
							{
								if (Net.ASN1Util.OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.1")))
								{
									sb.push(" serverAuth"));
								}
								else if (Net.ASN1Util.OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.2")))
								{
									sb.push(" clientAuth"));
								}
								else if (Net.ASN1Util.OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.3")))
								{
									sb.push(" codeSigning"));
								}
								else if (Net.ASN1Util.OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.4")))
								{
									sb.push(" emailProtection"));
								}
								else if (Net.ASN1Util.OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.8")))
								{
									sb.push(" timeStamping"));
								}
								else if (Net.ASN1Util.OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.9")))
								{
									sb.push(" OCSPSigning"));
								}
							}
							Text.StrUOSInt(sbuff, ++i);
						}
						sb.push("\r\n"));
					}
				}
			}
		}
	}

	static appendMSOSVersion(reader, startOfst, endOfst, sb, varName)
	{
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType);
		if (itemType == Net.ASN1Util.ItemType.IT_IA5STRING)
		{
			sb.push(varName);
			sb.push(".version = "));
			sb->AppendC(itemPDU, itemLen);
			sb.push("\r\n"));
		}
	}

	static appendMSRequestClientInfo(reader, startOfst, endOfst, sb, varName)
	{
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_SEQUENCE)
		{
			UOSInt subitemLen;
			const UInt8 *subitemPDU;
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subitemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_INTEGER)
			{
				sb.push(varName);
				sb.push(".unknown = "));
				Net.ASN1Util.IntegerToString(subitemPDU, subitemLen, sb);
				sb.push("\r\n"));
			}
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subitemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_UTF8STRING)
			{
				sb.push(varName);
				sb.push(".machine = "));
				sb->AppendC(subitemPDU, subitemLen);
				sb.push("\r\n"));
			}
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "3", subitemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_UTF8STRING)
			{
				sb.push(varName);
				sb.push(".user = "));
				sb->AppendC(subitemPDU, subitemLen);
				sb.push("\r\n"));
			}
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "4", subitemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_UTF8STRING)
			{
				sb.push(varName);
				sb.push(".software = "));
				sb->AppendC(subitemPDU, subitemLen);
				sb.push("\r\n"));
			}
		}
	}

	static appendMSEnrollmentCSPProvider(reader, startOfst, endOfst, sb, varName)
	{
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_SEQUENCE)
		{
			UOSInt subitemLen;
			const UInt8 *subitemPDU;
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subitemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_INTEGER)
			{
				sb.push(varName);
				sb.push(".unknown = "));
				Net.ASN1Util.IntegerToString(subitemPDU, subitemLen, sb);
				sb.push("\r\n"));
			}
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subitemLen, itemType)) != 0 && itemType == Net.ASN1Util.ItemType.IT_BMPSTRING)
			{
				sb.push(varName);
				sb.push(".provider = "));
				sb->AppendUTF16BE(subitemPDU, subitemLen >> 1);
				sb.push("\r\n"));
			}
		}
	}

	static appendGeneralNames(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[11];
		const UInt8 *itemPDU;
		UOSInt itemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.SEQUENCE)
			{
				UOSInt i = 1;
				Text.StrUOSInt(sbuff, i);
				while (AppendGeneralName(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
				{
					Text.StrUOSInt(sbuff, ++i);
				}
			}
		}
	}

	static appendGeneralName(reader, startOfst, endOfst, path, sb, varName)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		Net.ASN1Util.ItemType itemType;
		const UInt8 *subItemPDU;
		UOSInt subItemLen;
		if ((subItemPDU = ASN1Util.pduGetItem(pdu, pduEnd, path, subItemLen, itemType)) != 0)
		{
			switch (0x8F & (UOSInt)itemType)
			{
			case 0x80:
				sb.push(varName);
				sb.push(".otherName = "));
				sb->AppendC(subItemPDU, subItemLen);
				sb.push("\r\n"));
				return true;
			case 0x81:
				sb.push(varName);
				sb.push(".rfc822Name = "));
				sb->AppendC(subItemPDU, subItemLen);
				sb.push("\r\n"));
				return true;
			case 0x82:
				sb.push(varName);
				sb.push(".dNSName = "));
				sb->AppendC(subItemPDU, subItemLen);
				sb.push("\r\n"));
				return true;
			case 0x83:
				sb.push(varName);
				sb.push(".x400Address = "));
				sb->AppendC(subItemPDU, subItemLen);
				sb.push("\r\n"));
				return true;
			case 0x84:
				if ((subItemPDU = ASN1Util.pduGetItem(subItemPDU, subItemPDU + subItemLen, path, subItemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
				{
					sptr = varName.ConcatTo(sbuff);
					sptr = Text.StrConcatC(sptr, UTF8STRC(".directoryName"));
					AppendName(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
				}
				return true;
			case 0x85:
				sb.push(varName);
				sb.push(".ediPartyName = "));
				sb->AppendC(subItemPDU, subItemLen);
				sb.push("\r\n"));
				return true;
			case 0x86:
				sb.push(varName);
				sb.push(".uniformResourceIdentifier = "));
				sb->AppendC(subItemPDU, subItemLen);
				sb.push("\r\n"));
				return true;
			case 0x87:
				sb.push(varName);
				sb.push(".iPAddress = "));
				if (subItemLen == 4)
				{
					sptr = Net.SocketUtil.GetIPv4Name(sbuff, ReadNUInt32(subItemPDU));
					sb->AppendP(sbuff, sptr);
				}
				else if (subItemLen == 16)
				{
					Net.SocketUtil.AddressInfo addr;
					Net.SocketUtil.SetAddrInfoV6(addr, subItemPDU, 0);
					sptr = Net.SocketUtil.GetAddrName(sbuff, addr);
					sb->AppendP(sbuff, sptr);
				}
				sb.push("\r\n"));
				return true;
			case 0x88:
				sb.push(varName);
				sb.push(".registeredID = "));
				Net.ASN1Util.OIDToString(subItemPDU, subItemLen, sb);
				{
					const Net.ASN1OIDDB.OIDInfo *ent = Net.ASN1OIDDB.OIDGetEntry(subItemPDU, subItemLen);
					if (ent)
					{
						sb.push(" ("));
						sb->AppendSlow((const UTF8Char*)ent->name);
						sb->AppendUTF8Char(')');
					}
				}
				sb.push("\r\n"));
				return true;
			}
		}
		return false;
	}

	static appendDistributionPoint(reader, startOfst, endOfst, path, sb, varName)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		const UInt8 *itemPDU;
		UOSInt itemLen;
		const UInt8 *subItemPDU;
		UOSInt subItemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, path, itemLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.SEQUENCE)
			{
				UOSInt i = 1;
				Text.StrUOSInt(sbuff, i);
				while ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType)) != 0)
				{
					switch ((UOSInt)itemType)
					{
					case ASN1ItemType.CONTEXT_SPECIFIC_0:
						sptr = varName.ConcatTo(sbuff);
						*sptr++ = '.';
						sptr = Text.StrConcatC(sptr, UTF8STRC("distributionPoint"));
						AppendDistributionPointName(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
						break;
					case ASN1ItemType.CONTEXT_SPECIFIC_1:
						if ((subItemPDU = ASN1Util.pduGetItem(subItemPDU, subItemPDU + subItemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.BIT_STRING)
						{
							sb.push(varName);
							sb.push(".reasons ="));
							if (subItemLen >= 2)
							{
								if (subItemPDU[1] & 0x80) sb.push("unused"));
								if (subItemPDU[1] & 0x40) sb.push("keyCompromise"));
								if (subItemPDU[1] & 0x20) sb.push("cACompromise"));
								if (subItemPDU[1] & 0x10) sb.push("affiliationChanged"));
								if (subItemPDU[1] & 0x8) sb.push("superseded"));
								if (subItemPDU[1] & 0x4) sb.push("cessationOfOperation"));
								if (subItemPDU[1] & 0x2) sb.push("certificateHold"));
								if (subItemPDU[1] & 0x1) sb.push("privilegeWithdrawn"));
							}
							if (subItemLen >= 3)
							{
								if (subItemPDU[2] & 0x80) sb.push("aACompromise"));
							}
							sb.push("\r\n"));
						}
						break;
					case ASN1ItemType.CONTEXT_SPECIFIC_2:
						sptr = varName.ConcatTo(sbuff);
						*sptr++ = '.';
						sptr = Text.StrConcatC(sptr, UTF8STRC("cRLIssuer"));
						AppendGeneralNames(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
						break;
					}
					Text.StrUOSInt(sbuff, ++i);
				}
				return true;
			}
		}
		return false;
	}

	static appendIntegerppendDistributionPointName(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		UOSInt i;
		Char pathBuff[16];
		const UInt8 *itemPDU;
		UOSInt itemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0)
		{
			if (itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = Text.StrConcatC(sptr, UTF8STRC("fullName"));
				i = 0;
				Text.StrUOSInt(pathBuff, ++i);
				while (AppendGeneralName(itemPDU, itemPDU + itemLen, pathBuff, sb, CSTRP(sbuff, sptr)))
				{
					Text.StrUOSInt(pathBuff, ++i);
				}
			}
			else if (itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = Text.StrConcatC(sptr, UTF8STRC("nameRelativeToCRLIssuer"));
				AppendRelativeDistinguishedName(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
			}
		}
	}

	static appendPolicyInformation(reader, startOfst, endOfst, path, sb, varName)
	{
		UTF8Char sbuff[64];
		const UInt8 *itemPDU;
		UOSInt itemLen;
		const UInt8 *subItemPDU;
		UOSInt subItemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, path, itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType);
			if (subItemPDU != 0 && itemType == ASN1ItemType.OID)
			{
				sb.push(varName);
				sb->AppendUTF8Char('.');
				sb.push("policyIdentifier = "));
				Net.ASN1Util.OIDToString(subItemPDU, subItemLen, sb);
				sb.push(" ("));
				Net.ASN1OIDDB.OIDToNameString(subItemPDU, subItemLen, sb);
				sb->AppendUTF8Char(')');
				sb.push("\r\n"));
			}
			subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType);
			if (subItemPDU != 0 && itemType == ASN1ItemType.SEQUENCE)
			{
				const UInt8 *policyQualifierInfoPDU;
				UOSInt policyQualifierInfoLen;
				UOSInt i = 0;
				Text.StrUOSInt(sbuff, ++i);
				while ((policyQualifierInfoPDU = ASN1Util.pduGetItem(subItemPDU, subItemPDU + subItemLen, (const Char*)sbuff, policyQualifierInfoLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
				{
					if ((itemPDU = ASN1Util.pduGetItem(policyQualifierInfoPDU, policyQualifierInfoPDU + policyQualifierInfoLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.OID)
					{
						sb.push(varName);
						sb->AppendUTF8Char('.');
						sb.push("policyQualifiers["));
						sb->AppendUOSInt(i);
						sb.push("].policyQualifierId = "));
						Net.ASN1Util.OIDToString(itemPDU, itemLen, sb);
						sb.push(" ("));
						Net.ASN1OIDDB.OIDToNameString(itemPDU, itemLen, sb);
						sb->AppendUTF8Char(')');
						sb.push("\r\n"));
					}
					if ((itemPDU = ASN1Util.pduGetItem(policyQualifierInfoPDU, policyQualifierInfoPDU + policyQualifierInfoLen, "2", itemLen, itemType)) != 0)
					{
						if (itemType == ASN1ItemType.IA5STRING)
						{
							sb.push(varName);
							sb->AppendUTF8Char('.');
							sb.push("policyQualifiers["));
							sb->AppendUOSInt(i);
							sb.push("].qualifier = "));
							sb->AppendC(itemPDU, itemLen);
							sb.push("\r\n"));
						}
						else if (itemType == ASN1ItemType.SEQUENCE)
						{
							/////////////////////////////////// UserNotice
						}
					}
					Text.StrUOSInt(sbuff, ++i);
				}
			}
			return true;
		}
		return false;
	}

	static appendPKCS7SignedData(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[16];
		Net.ASN1Util.ItemType itemType;
		UOSInt itemOfst;
		UOSInt itemLen;
		const UInt8 *itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType);
		if (itemPDU != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			UOSInt i;
			UOSInt subItemLen;
			const UInt8 *subItemPDU;
			if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.INTEGER)
			{
				sb.push("signedData.version = "));
				Net.ASN1Util.IntegerToString(subItemPDU, subItemLen, sb);
				sb.push("\r\n"));
			}
			if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.SET)
			{
				AppendPKCS7DigestAlgorithmIdentifiers(subItemPDU, subItemPDU + subItemLen, sb, CSTR("signedData.digestAlgorithms"));
			}
			if ((subItemPDU = Net.ASN1Util.PDUGetItemRAW(itemPDU, itemPDU + itemLen, "3", subItemLen, itemOfst)) != 0 && subItemPDU[0] == ASN1ItemType.SEQUENCE)
			{
				AppendContentInfo(subItemPDU, subItemPDU + itemOfst + subItemLen, "1", sb, CSTR("signedData.contentInfo"), ContentDataType.Unknown);
			}
			i = 4;
			subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "4", subItemLen, itemType);
			if (subItemPDU != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.certificates"));
				Text.StrUOSInt(sbuff, ++i);
				subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType);
			}
			if (subItemPDU != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				//AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.crls"));
				Text.StrUOSInt(sbuff, ++i);
				subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType);
			}
			if (subItemPDU != 0 && itemType == ASN1ItemType.SET)
			{
				AppendPKCS7SignerInfos(subItemPDU, subItemPDU + subItemLen, sb, CSTR("signedData.signerInfos"));
			}
		}
	}

	static appendPKCS7DigestAlgorithmIdentifiers(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[16];
		UOSInt i;
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		i = 0;
		while (true)
		{
			Text.StrUOSInt(sbuff, ++i);
			if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, (const Char*)sbuff, itemLen, itemType)) == 0)
			{
				return;
			}
			if (itemType == ASN1ItemType.SEQUENCE)
			{
				AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, varName, false, 0);
			}
		}
	}

	static appendPKCS7SignerInfos(reader, startOfst, endOfst, sb, varName)
	{
		Char cbuff[32];
		UOSInt i;
		UOSInt itemOfst;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		i = 0;
		while (true)
		{
			Text.StrUOSInt(cbuff, ++i);
			itemPDU = Net.ASN1Util.PDUGetItemRAW(pdu, pduEnd, cbuff, itemLen, itemOfst);
			if (itemPDU == 0)
			{
				break;
			}
			if (itemPDU[0] == ASN1ItemType.SEQUENCE)
			{
				AppendPKCS7SignerInfo(itemPDU, itemPDU + itemOfst + itemLen, sb, varName);
			}
		}
	}

	static appendPKCS7SignerInfo(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		Char cbuff[32];
		UOSInt i;
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType);
		if (itemPDU == 0 || itemType != ASN1ItemType.SEQUENCE)
		{
			return;
		}
		UOSInt subItemLen;
		const UInt8 *subItemPDU;
		if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.INTEGER)
		{
			sb.push(varName);
			sb->AppendUTF8Char('.');
			sb.push("version = "));
			Net.ASN1Util.IntegerToString(subItemPDU, subItemLen, sb);
			sb.push("\r\n"));
		}
		if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text.StrConcatC(sptr, UTF8STRC("issuerAndSerialNumber"));
			AppendIssuerAndSerialNumber(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
		}
		if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "3", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text.StrConcatC(sptr, UTF8STRC("digestAlgorithm"));
			AppendAlgorithmIdentifier(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr), false, 0);
		}
		i = 4;
		if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "4", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text.StrConcatC(sptr, UTF8STRC("authenticatedAttributes"));
			AppendPKCS7Attributes(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
			Text.StrUOSInt(cbuff, ++i);
			subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, cbuff, subItemLen, itemType);
		}
		if (subItemPDU != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text.StrConcatC(sptr, UTF8STRC("digestEncryptionAlgorithm"));
			AppendAlgorithmIdentifier(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr), false, 0);
		}
		Text.StrUOSInt(cbuff, ++i);
		if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, cbuff, subItemLen, itemType)) != 0 && itemType == ASN1ItemType.OCTET_STRING)
		{
			sb.push(varName);
			sb->AppendUTF8Char('.');
			sb.push("encryptedDigest = "));
			sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text.LineBreakType.None);
			sb.push("\r\n"));
		}
		Text.StrUOSInt(cbuff, ++i);
		if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, cbuff, subItemLen, itemType)) != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text.StrConcatC(sptr, UTF8STRC("unauthenticatedAttributes"));
			AppendPKCS7Attributes(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
		}
	}

	static appendIssuerAndSerialNumber(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text.StrConcatC(sptr, UTF8STRC("issuer"));
			AppendName(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
		}
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "2", itemLen, itemType)) != 0 && itemType == ASN1ItemType.INTEGER)
		{
			sb.push(varName);
			sb->AppendUTF8Char('.');
			sb.push("serialNumber = "));
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text.LineBreakType.None);
			sb.push("\r\n"));
		}
	}

	static appendPKCS7Attributes(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		Char cbuff[16];
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		UOSInt oidLen;
		const UInt8 *oidPDU;
		Net.ASN1Util.ItemType oidType;
		UOSInt valueLen;
		const UInt8 *valuePDU;
		UOSInt i = 0;
		while (true)
		{
			Text.StrUOSInt(cbuff, ++i);
			if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, cbuff, itemLen, itemType)) == 0)
			{
				return;
			}
			if (itemType == ASN1ItemType.SEQUENCE)
			{
				oidPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, oidType);
				valuePDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", valueLen, itemType);
				if (oidPDU != 0 && oidType == ASN1ItemType.OID)
				{
					sb.push(varName);
					sb->AppendUTF8Char('.');
					sb.push("attributeType = "));
					Net.ASN1Util.OIDToString(oidPDU, oidLen, sb);
					const Net.ASN1OIDDB.OIDInfo *oid = Net.ASN1OIDDB.OIDGetEntry(oidPDU, oidLen);
					if (oid)
					{
						sb.push(" ("));
						sb->AppendSlow((const UTF8Char*)oid->name);
						sb->AppendUTF8Char(')');
					}
					sb.push("\r\n"));
				}
				if (valuePDU && itemType == ASN1ItemType.SET)
				{
					if (Net.ASN1Util.OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.3"))) //contentType
					{
						if ((itemPDU = ASN1Util.pduGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.OID)
						{
							sb.push(varName);
							sb->AppendUTF8Char('.');
							sb.push("contentType = "));
							Net.ASN1Util.OIDToString(itemPDU, itemLen, sb);
							const Net.ASN1OIDDB.OIDInfo *oid = Net.ASN1OIDDB.OIDGetEntry(itemPDU, itemLen);
							if (oid)
							{
								sb.push(" ("));
								sb->AppendSlow((const UTF8Char*)oid->name);
								sb->AppendUTF8Char(')');
							}
							sb.push("\r\n"));
						}
					}
					else if (Net.ASN1Util.OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.4"))) //messageDigest
					{
						if ((itemPDU = ASN1Util.pduGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.OCTET_STRING)
						{
							sb.push(varName);
							sb->AppendUTF8Char('.');
							sb.push("messageDigest = "));
							sb->AppendHexBuff(itemPDU, itemLen, ':', Text.LineBreakType.None);
							sb.push("\r\n"));
						}
					}
					else if (Net.ASN1Util.OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.5"))) //signing-time
					{
						if ((itemPDU = ASN1Util.pduGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.UTCTIME)
						{
							sb.push(varName);
							sb->AppendUTF8Char('.');
							sb.push("signing-time = "));
							Net.ASN1Util.UTCTimeToString(itemPDU, itemLen, sb);
							sb.push("\r\n"));
						}
					}
					else if (Net.ASN1Util.OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.15"))) //smimeCapabilities
					{
						/////////////////////////////////////
					}
					else if (Net.ASN1Util.OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.16.2.11"))) //id-aa-encrypKeyPref
					{
						if ((itemPDU = ASN1Util.pduGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
						{
							sptr = varName.ConcatTo(sbuff);
							*sptr++ = '.';
							sptr = Text.StrConcatC(sptr, UTF8STRC("encrypKeyPref"));
							AppendIssuerAndSerialNumber(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
						}
					}
					else if (Net.ASN1Util.OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.3.6.1.4.1.311.16.4"))) //outlookExpress
					{
						if ((itemPDU = ASN1Util.pduGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
						{
							sptr = varName.ConcatTo(sbuff);
							*sptr++ = '.';
							sptr = Text.StrConcatC(sptr, UTF8STRC("outlookExpress"));
							AppendIssuerAndSerialNumber(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
						}
					}
					
				}
			}
		}
	}

	static appendMacData(reader, startOfst, endOfst, path, sb, varName)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		const UInt8 *itemPDU;
		UOSInt itemLen;
		const UInt8 *subItemPDU;
		UOSInt subItemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, path, itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
			{
				sptr = varName.ConcatTo(sbuff);
				sptr = Text.StrConcatC(sptr, UTF8STRC(".mac"));
				AppendDigestInfo(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
			}
			if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.OCTET_STRING)
			{
				sb.push(varName);
				sb->AppendUTF8Char('.');
				sb.push("macSalt = "));
				sb->AppendHexBuff(subItemPDU, subItemLen, ' ', Text.LineBreakType.None);
				sb.push("\r\n"));
			}
			if ((subItemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "3", subItemLen, itemType)) != 0 && itemType == ASN1ItemType.INTEGER)
			{
				sb.push(varName);
				sb->AppendUTF8Char('.');
				sb.push("iterations = "));
				Net.ASN1Util.IntegerToString(subItemPDU, subItemLen, sb);
				sb.push("\r\n"));
			}
			return true;
		}
		return false;
	}

	static appendDigestInfo(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		const UInt8 *itemPDU;
		UOSInt itemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			sptr = Text.StrConcatC(sptr, UTF8STRC(".digestAlgorithm"));
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), false, 0);
		}
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "2", itemLen, itemType)) != 0 && itemType == ASN1ItemType.OCTET_STRING)
		{
			sb.push(varName);
			sb.push(".digest = "));
			sb->AppendHexBuff(itemPDU, itemLen, ' ', Text.LineBreakType.None);
			sb.push("\r\n"));
		}
	}

	static appendData(reader, startOfst, endOfst, sb, varName, dataType)
	{
		switch (dataType)
		{
		case ContentDataType.AuthenticatedSafe:
			AppendAuthenticatedSafe(pdu, pduEnd, sb, varName);
			break;
		case ContentDataType.Unknown:
		default:
			break;
		}
	}

	static appendEncryptedData(reader, startOfst, endOfst, sb, varName, dataType)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		const UInt8 *itemPDU;
		UOSInt itemLen;
		const UInt8 *subitemPDU;
		UOSInt subitemLen;
		Net.ASN1Util.ItemType itemType;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			if (varName.v)
			{
				sb.push(varName);
				sb->AppendUTF8Char('.');
			}
			sb.push("version = "));
			AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
			sb.push("\r\n"));
	
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "2", subitemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
			{
				sptr = sbuff;
				if (varName.v)
				{
					sptr = varName.ConcatTo(sptr);
					*sptr++ = '.';
				}
				sptr = Text.StrConcatC(sptr, UTF8STRC("encryptedContentInfo"));
				AppendEncryptedContentInfo(subitemPDU, subitemPDU + subitemLen, sb, CSTRP(sbuff, sptr), dataType);
			}
			if ((subitemPDU = ASN1Util.pduGetItem(itemPDU, itemPDU + itemLen, "3", subitemLen, itemType)) != 0 && itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				sptr = sbuff;
				if (varName.v)
				{
					sptr = varName.ConcatTo(sptr);
					*sptr++ = '.';
				}
				sptr = Text.StrConcatC(sptr, UTF8STRC("unprotectedAttributes"));
				AppendPKCS7Attributes(subitemPDU, subitemPDU + subitemLen, sb, CSTRP(sbuff, sptr));
			}
		}
	}

	static appendAuthenticatedSafe(reader, startOfst, endOfst, sb, varName)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		Char cbuff[16];
		const UInt8 *itemPDU;
		UOSInt itemLen;
		Net.ASN1Util.ItemType itemType;
		UOSInt i;
		UOSInt j;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			i = 0;
			j = ASN1Util.pduCountItem(itemPDU, itemPDU + itemLen, 0);
			while (i < j)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '[';
				sptr = Text.StrUOSInt(sptr, i);
				*sptr++ = ']';
				*sptr = 0;
				Text.StrUOSInt(cbuff, ++i);
				AppendContentInfo(itemPDU, itemPDU + itemLen, cbuff, sb, CSTRP(sbuff, sptr), ContentDataType.Unknown);
			}
		}
	}

	static appendEncryptedContentInfo(reader, startOfst, endOfst, sb, varName, dataType)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		Net.ASN1Util.ItemType itemType;
		UOSInt itemLen;
		const UInt8 *itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "1", itemLen, itemType)) != 0 && itemType == ASN1ItemType.OID)
		{
			sb.push(varName);
			sb.push(".contentType = "));
			Net.ASN1Util.OIDToString(itemPDU, itemLen, sb);
			const Net.ASN1OIDDB.OIDInfo *oid = Net.ASN1OIDDB.OIDGetEntry(itemPDU, itemLen);
			if (oid)
			{
				sb.push(" ("));
				sb->AppendSlow((const UTF8Char*)oid->name);
				sb->AppendUTF8Char(')');
			}
			sb.push("\r\n"));
		}
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "2", itemLen, itemType)) != 0 && itemType == ASN1ItemType.SEQUENCE)
		{
			sptr = sbuff;
			if (varName.v)
			{
				sptr = varName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text.StrConcatC(sptr, UTF8STRC("contentEncryptionAlgorithm"));
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), false, 0);
		}
		if ((itemPDU = ASN1Util.pduGetItem(pdu, pduEnd, "3", itemLen, itemType)) != 0 && (itemType & 0x8F) == 0x80)
		{
			sb.push(varName);
			sb.push(".encryptedContent = "));
			sb->AppendHexBuff(itemPDU, itemLen, ' ', Text.LineBreakType.None);
			sb.push("\r\n"));
		}
	}*/

	static nameGetByOID(reader, startOfst, endOfst, oidText)
	{
		if (!(reader instanceof data.ByteReader))
			return null;
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
					if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID && ASN1Util.oidEqualsText(new Uint8Array(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen)), oidText))
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

	static namesGet(reader, startOfst, endOfst)
	{
		let names = {};
		if (!(reader instanceof data.ByteReader))
			return names;
		let itemPDU;
		let oidPDU;
		let strPDU;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		let i = 0;
		while (i < cnt)
		{
			i++;
	
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, i.toString()+".1")) != null)
			{
				if (itemType == ASN1ItemType.SEQUENCE)
				{
					oidPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
					if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID)
					{
						strPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "2");
						if (strPDU)
						{
							let oid = new Uint8Array(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen));
							if (ASN1Util.oidEqualsText(oid, "2.5.4.6"))
							{
								names.countryName = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.4.8"))
							{
								names.stateOrProvinceName = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.4.7"))
							{
								names.localityName = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.4.10"))
							{
								names.organizationName = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.4.11"))
							{
								names.organizationUnitName = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.4.3"))
							{
								names.commonName = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
							else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.1"))
							{
								names.emailAddress = reader.readUTF8(strPDU.rawOfst + strPDU.hdrLen, strPDU.contLen);
							}
						}
					}
				}
			}
		}
		return names;
	}

	static extensionsGet(reader, startOfst, endOfst)
	{
		let ext = {};
		if (!(reader instanceof data.ByteReader))
			return ext;
		let itemPDU;
		let oidPDU;
		let strPDU;
		let subItemPDU;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		let i = 0;
		while (i < cnt)
		{
			i++;
	
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, i.toString())) != null)
			{
				if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					oidPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
					if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID)
					{
						strPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "2");
						if (strPDU && strPDU.itemType == ASN1ItemType.BOOLEAN)
						{
							strPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "3");
						}
						if (strPDU && strPDU.itemType == ASN1ItemType.OCTET_STRING)
						{
							let oid = new Uint8Array(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen));
							if (ASN1Util.oidEqualsText(oid, "2.5.29.17")) //id-ce-subjectAltName
							{
								ext.subjectAltName = [];
								let j = 0;
								let k = ASN1Util.pduCountItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1");
								while (j < k)
								{
									j++;
									subItemPDU = ASN1Util.pduGetItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1."+j);
									if (subItemPDU)
									{
										if (subItemPDU.itemType == 0x87)
										{
											ext.subjectAltName.push(net.getIPv4Name(reader.readInt32(subItemPDU.rawOfst + subItemPDU.hdrLen, false)));
										}
										else
										{
											ext.subjectAltName.push(reader.readUTF8(subItemPDU.rawOfst + subItemPDU.hdrLen, subItemPDU.contLen));
										}
									}
								}
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.29.14")) //id-ce-subjectKeyIdentifier
							{
								subItemPDU = ASN1Util.pduGetItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1");
								if (subItemPDU && subItemPDU.contLen == 20)
								{
									ext.subjKeyId = reader.getArrayBuffer(subItemPDU.rawOfst + subItemPDU.hdrLen, subItemPDU.contLen);
								}
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.29.35")) //id-ce-authorityKeyIdentifier
							{
								subItemPDU = ASN1Util.pduGetItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1.1");
								if (subItemPDU && subItemPDU.contLen == 20)
								{
									ext.authKeyId = reader.getArrayBuffer(subItemPDU.rawOfst + subItemPDU.hdrLen, subItemPDU.contLen);
								}
							}
							else if (ASN1Util.oidEqualsText(oid, "2.5.29.15")) //id-ce-keyUsage
							{
								subItemPDU = ASN1Util.pduGetItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1");
								if (subItemPDU && subItemPDU.itemType == ASN1ItemType.BIT_STRING && subItemPDU.contLen >= 2)
								{
									let v = reader.readUInt8(subItemPDU.rawOfst + subItemPDU.hdrLen + 1);
									ext.caCert = (v & 6) != 0;
									ext.digitalSign = (v & 0x80) != 0;
								}
							}
						}
					}
				}
			}
		}
		return ext;
	}

	static extensionsGetCRLDistributionPoints(reader, startOfst, endOfst)
	{
		let ret = [];
		let itemPDU;
		let oidPDU;
		let strPDU;
		let subItemPDU;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		let i = 0;
		while (i < cnt)
		{
			i++;
	
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, i.toString())) != null)
			{
				if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					oidPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
					if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID)
					{
						strPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "2");
						if (strPDU && strPDU.itemType == ASN1ItemType.BOOLEAN)
						{
							strPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "3");
						}
						if (strPDU && strPDU.itemType == ASN1ItemType.OCTET_STRING)
						{
							if (ASN1Util.oidEqualsText(new Uint8Array(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen)), "2.5.29.31")) //id-ce-cRLDistributionPoints
							{
								let j = 0;
								let k = ASN1Util.pduCountItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1");
								while (j < k)
								{
									j++;
									subItemPDU = ASN1Util.pduGetItem(reader, strPDU.rawOfst + strPDU.hdrLen, strPDU.rawOfst + strPDU.hdrLen + strPDU.contLen, "1."+j);
									if (subItemPDU && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
									{
										X509File.distributionPointAdd(reader, subItemPDU.rawOfst + subItemPDU.hdrLen, subItemPDU.rawOfst + subItemPDU.hdrLen + subItemPDU.contLen, ret);
									}
								}
							}
						}
					}
				}
			}
		}
		return ret;
	}

	static distributionPointAdd(reader, startOfst, endOfst, distPoints)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst, itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1")) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst, itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1")) != null && itemPDU.itemType == ASN1ItemType.CHOICE_6)
				{
					distPoints.push(reader.readUTF8(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen));
					return true;
				}
			}
		}
		return false;
	}

	static publicKeyGetNew(reader, startOfst, endOfst)
	{
		let oidPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1.1");
		let bstrPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2");
		if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID && bstrPDU != null && bstrPDU.itemType == ASN1ItemType.BIT_STRING)
		{
			let keyType = X509File.keyTypeFromOID(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen), true);
			if (keyType == KeyType.ECPublic)
			{
				let paramPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1.2");
				if (paramPDU != null && paramPDU.itemType == ASN1ItemType.OID)
				{
					return X509Key.fromECPublicKey(reader.getArrayBuffer(bstrPDU.rawOfst + bstrPDU.hdrLen + 1, bstrPDU.contLen - 1), reader.getArrayBuffer(paramPDU.rawOfst + paramPDU.hdrLen, paramPDU.contLen));
				}
			}
			else if (keyType != KeyType.Unknown)
			{
				return new X509Key("public.key", reader.getArrayBuffer(bstrPDU.rawOfst + bstrPDU.hdrLen + 1, bstrPDU.contLen - 1), keyType);
			}
		}
		return null;
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

	static keyTypeFromOID(oid, pubKey)
	{
		let arr = new Uint8Array(oid);
		if (ASN1Util.oidEqualsText(arr, "1.2.840.113549.1.1.1"))
		{
			if (pubKey)
			{
				return KeyType.RSAPublic;
			}
			else
			{
				return KeyType.RSA;
			}
		}
		else if (ASN1Util.oidEqualsText(arr, "1.2.840.10045.2.1"))
		{
			return KeyType.ECPublic;
		}
		return KeyType.Unknown;
	}

	static algorithmIdentifierGet(reader, startOfst, endOfst)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, null);
		if (cnt != 2 && cnt != 1)
		{
			return AlgType.Unknown;
		}
		let itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		if (itemPDU == null || itemPDU.itemType != ASN1ItemType.OID)
		{
			return AlgType.Unknown;
		}
		let oid = new Uint8Array(reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen));
		if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.2")) //md2WithRSAEncryption
		{
			return AlgType.MD2WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.4")) //md5WithRSAEncryption
		{
			return AlgType.MD5WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.5")) //sha1WithRSAEncryption
		{
			return AlgType.SHA1WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.11")) //sha256WithRSAEncryption
		{
			return AlgType.SHA256WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.12")) //sha384WithRSAEncryption
		{
			return AlgType.SHA384WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.13")) //sha512WithRSAEncryption
		{
			return AlgType.SHA512WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.1.14")) //sha224WithRSAEncryption
		{
			return AlgType.SHA224WithRSAEncryption;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.10045.4.3.2")) //ecdsa-with-SHA256
		{
			return AlgType.ECDSAWithSHA256;
		}
		else if (ASN1Util.oidEqualsText(oid, "1.2.840.10045.4.3.3")) //ecdsa-with-SHA384
		{
			return AlgType.ECDSAWithSHA384;
		}
		else
		{
			return AlgType.Unknown;
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
			}*/
		let issuerCN = this.getIssuerCN();
		if (issuerCN == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let dt;
		let currTime = new Date().getTime();
		if ((dt = this.getNotBefore()) == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		if (dt.toEpochMS() > currTime)
		{
			return CertValidStatus.Expired;
		}
		if ((dt = this.getNotAfter()) == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		if (dt.toEpochMS() < currTime)
		{
			return CertValidStatus.Expired;
		}
		let signedInfo;
		if ((signedInfo = this.getSignedInfo()) == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let hashType = algTypeGetHash(signedInfo.algType);
		if (hashType == hash.HashType.Unknown)
		{
			return CertValidStatus.UnsupportedAlgorithm;
		}

		let issuer;// = trustStore.getCertByCN(issuerCN);
		if (issuer == null)
		{
			if (!this.isSelfSigned())
			{
				return CertValidStatus.UnknownIssuer;
			}
			let key = this.getNewPublicKey();
			if (key == null)
			{
				return CertValidStatus.FileFormatInvalid;
			}
			let signValid = key.signatureVerify(hashType, signedInfo.payload, signedInfo.signature);
			if (signValid)
			{
				return CertValidStatus.SelfSigned;
			}
			else
			{
				return CertValidStatus.SignatureInvalid;
			}
		}

		let key = issuer.getNewPublicKey();
		if (key == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let signValid = key.signatureVerify(hashType, signedInfo.payload, signedInfo.signature);
		if (!signValid)
		{
			return CertValidStatus.SignatureInvalid;
		}

		let crlDistributionPoints = this.getCRLDistributionPoints();
		//////////////////////////
		// CRL
		return CertValidStatus.Valid;
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
		let sb = [];
		if (X509File.isCertificate(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendCertificate(this.reader, 0, this.reader.getLength(), "1", sb, null);
		}
		return sb.join();
	}

	createNames()
	{
		return new ASN1Names().setCertificate();
	}

	getIssuerNames()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return null;
		}
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4");
			if (pdu)
			{
				return X509File.namesGet(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
			if (pdu)
			{
				return X509File.namesGet(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		return null;
	}

	getSubjNames()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return null;
		}
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.6");
			if (pdu)
			{
				return X509File.namesGet(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.5");
			if (pdu)
			{
				return X509File.namesGet(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		return null;
	}

	getExtensions()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return null;
		}
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.8.1");
			if (pdu)
			{
				return X509File.extensionsGet(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.7.1");
			if (pdu)
			{
				return X509File.extensionsGet(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		return null;
	}

	getNewPublicKey()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return null;
		}
		if (pdu.itemType == ASN1Util.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.7");
			if (pdu)
			{
				return X509File.publicKeyGetNew(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.6");
			if (pdu)
			{
				return X509File.publicKeyGetNew(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		return null;
	}

	getKeyId()
	{
		let key = this.getNewPublicKey();
		if (key == null)
		{
			return null;
		}
		return key.getKeyId();
	}

	getNotBefore()
	{
		let tmpBuff;
		if (ASN1Util.pduGetItemType(this.reader, 0, this.reader.getLength(), "1.1.1") == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.5.1");
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4.1");
		}
		if (tmpBuff == null)
			return null;
		if (tmpBuff.itemType == ASN1ItemType.UTCTIME || tmpBuff.itemType == ASN1ItemType.GENERALIZEDTIME)
		{
			return ASN1Util.pduParseUTCTimeCont(this.reader, tmpBuff.rawOfst + tmpBuff.hdrLen, tmpBuff.rawOfst + tmpBuff.hdrLen + tmpBuff.contLen);
		}
		return null;
	}

	getNotAfter()
	{
		let tmpBuff;
		if (ASN1Util.pduGetItemType(this.reader, 0, this.reader.getLength(), "1.1.1") == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.5.2");
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4.2");
		}
		if (tmpBuff == null)
			return null;
		if (tmpBuff.itemType == ASN1ItemType.UTCTIME || tmpBuff.itemType == ASN1ItemType.GENERALIZEDTIME)
		{
			return ASN1Util.pduParseUTCTimeCont(this.reader, tmpBuff.rawOfst + tmpBuff.hdrLen, tmpBuff.rawOfst + tmpBuff.hdrLen + tmpBuff.contLen);
		}
		return null;
	}

	domainValid(domain)
	{
		let valid = false;
		let subjNames;
		if (subjNames = this.getSubjNames())
		{
			if (subjNames.commonName)
			{
				if (subjNames.commonName.startsWith("*."))
				{
					valid = (domain.toUpperCase() == subjNames.commonName.substring(2).toUpperCase()) || domain.toUpperCase().endsWith(subjNames.commonName.substring(1).toUpperCase());
				}
				else
				{
					valid = domain.toUpperCase() == subjNames.commonName;
				}
			}
			if (valid)
			{
				return true;
			}
		}
	
		let exts;
		let s;
		if (exts = this.getExtensions())
		{
			if (exts.subjectAltName)
			{
				let i = 0;
				let j = exts.subjectAltName.length;
				while (i < j)
				{
					s = exts.subjectAltName[i];
					if (s.startsWith("*."))
					{
						valid = (domain.toUpperCase() == s.substring(2).toUpperCase()) || domain.toUpperCase().endsWith(s.substring(1).toUpperCase());
					}
					else
					{
						valid = domain.toUpperCase() == s.toUpperCase();
					}
					if (valid)
						break;
				}
			}
			if (valid)
			{
				return true;
			}
		}
		return false;
	}

	isSelfSigned()
	{
		let subjNames;
		let issueNames;
		if ((issueNames = this.getIssuerNames()) && (subjNames = this.getSubjNames()))
		{
			return issueNames.commonName == subjNames.commonName;
		}
		return false;
	}

	getCRLDistributionPoints()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return [];
		}
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.8.1");
			if (pdu)
			{
				return X509File.extensionsGetCRLDistributionPoints(reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.7.1");
			if (pdu)
			{
				return X509File.extensionsGetCRLDistributionPoints(reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		return [];
	}

	getIssuerNamesSeq()
	{
		let pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (pdu == null)
		{
			return null;
		}
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4");
			if (pdu && pdu.itemType == ASN1ItemType.SEQUENCE)
			{
				return this.reader.getArrayBuffer(pdu.rawOfst + pdu.hdrLen, pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
			if (pdu && pdu.itemType == ASN1ItemType.SEQUENCE)
			{
				return this.reader.getArrayBuffer(pdu.rawOfst + pdu.hdrLen, pdu.contLen);
			}
		}
		return null;
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
		return null;
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

	static fromECPublicKey(buff, paramOID)
	{
		let pdu = new ASN1PDUBuilder();
		pdu.beginSequence();
		pdu.beginSequence();
		pdu.appendOIDString("1.2.840.10045.2.1");
		pdu.appendOID(paramOID);
		pdu.endLevel();
		pdu.appendBitString(0, buff);
		pdu.endLevel();
		return new X509Key("ECPublic.key", pdu.getArrayBuffer(), KeyType.ECPublic);
	}
}

export class X509PubKey extends X509File
{
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getFileType()
	{
		return X509FileType.PublicKey;
	}

	toShortName()
	{
		let oidPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		if (oidPDU == null || oidPDU.itemType != ASN1ItemType.OID)
		{
			return null;
		}
		let keyType = X509File.keyTypeFromOID(reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen), true);
		let keyPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
		if (keyPDU && keyPDU.itemType == ASN1ItemType.OCTET_STRING)
		{
			let sb = [];
			sb.push(keyTypeGetName(keyType)+" ");
			sb.push(keyGetLeng(reader, keyPDU.rawOfst + keyPDU.hdrLen, keyPDU.rawOfst + keyPDU.hdrLen + keyPDU.contLen, keyType));
			sb.push(" bits");
			return sb.join("");
		}
		return null;
	}

	isValid()
	{
		return CertValidStatus.SignatureInvalid;
	}

	clone()
	{
		return new X509PubKey(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		let sb = [];
		if (X509File.isPublicKeyInfo(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendPublicKeyInfo(this.reader, 0, this.reader.getLength(), "1", sb);
		}
		return sb.join("");
	}

	createNames()
	{
		return null;
	}
	
	createKey()
	{
		let keyTypeOID = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1");
		let keyData = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
		if (keyTypeOID != null && keyData != null)
		{
			if (this.reader.readUInt8(keyData.rawOfst + keyData.hdrLen) == 0)
			{
				keyData.hdrLen++;
				keyData.contLen--;
			}
			return new X509Key(this.sourceName, this.reader.getArrayBuffer(keyData.rawOfst + keyData.hdrLen, keyData.contLen), X509File.keyTypeFromOID(this.reader.getArrayBuffer(keyTypeOID.rawOfst + keyTypeOID.hdrLen, keyTypeOID.contLen), true));
		}
		return null;
	}

	static createFromKeyBuff(keyType, buff, sourceName)
	{
		let keyPDU = new ASN1PDUBuilder();
		keyPDU.beginSequence();
		keyPDU.beginSequence();
		let oidStr = keyTypeGetOID(keyType);
		keyPDU.appendOIDString(oidStr);
		keyPDU.appendNull();
		keyPDU.endLevel();
		if (keyType == KeyType.RSAPublic)
		{
			keyPDU.appendBitString(0, buff);
		}
		else
		{
			keyPDU.appendBitString(0, buff);
		}
		keyPDU.endLevel();
		return new X509PubKey(sourceName, keyPDU.getArrayBuffer());
	}

	static createFromKey(key)
	{
		return createFromKeyBuff(key.getKeyType(), key.getASN1Buff().getArrayBuffer(), key.sourceName);
	}
}

export function algTypeGetHash(algType)
{
	switch (algType)
	{
	case AlgType.SHA1WithRSAEncryption:
		return hash.HashType.SHA1;
	case AlgType.SHA256WithRSAEncryption:
		return hash.HashType.SHA256;
	case AlgType.SHA512WithRSAEncryption:
		return hash.HashType.SHA512;
	case AlgType.SHA384WithRSAEncryption:
		return hash.HashType.SHA384;
	case AlgType.SHA224WithRSAEncryption:
		return hash.HashType.SHA224;
	case AlgType.MD2WithRSAEncryption:
		return hash.HashType.Unknown;
	case AlgType.MD5WithRSAEncryption:
		return hash.HashType.MD5;
	case AlgType.ECDSAWithSHA256:
		return hash.HashType.SHA256;
	case AlgType.ECDSAWithSHA384:
		return hash.HashType.SHA384;
	case AlgType.Unknown:
	default:
		return hash.HashType.Unknown;
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