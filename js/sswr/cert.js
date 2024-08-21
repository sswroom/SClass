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
	/**
	 * @param {string} sourceName
	 * @param {string} objType
	 * @param {ArrayBuffer} buff
	 */
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

	/**
	 * @returns {ASN1Names|null}
	 */
	createNames()
	{
		throw new Error("createNames must be inherit");
	}

	getASN1Buff()
	{
		return this.reader;
	}

	/**
	 * @param {string[]} arr
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} len
	 */
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
	/**
	 * @param {string} sourceName
	 * @param {string} objType
	 * @param {ArrayBuffer} buff
	 */
	constructor(sourceName, objType, buff)
	{
		super(sourceName, objType, buff);
	}

	/**
	 * @returns {number}
	 */
	getFileType()
	{
		throw new Error("getFileType must be inherited");
	}

	/**
	 * @returns {string|null}
	 */
	toShortName()
	{
		throw new Error("toShortName must be inherited");
	}

	/**
	 * @returns {Promise<string>}
	 */
	async isValid()
	{
		return CertValidStatus.FileFormatInvalid;
	}

	getASN1Type()
	{
		return ASN1Type.X509;
	}

	getCertCount()
	{
		return 0;
	}

	/**
	 * @param {number} index
	 * @returns {string|null}
	 */
	getCertName(index)
	{
		return null;
	}

	/**
	 * @param {number} index
	 * @returns {X509Cert|null}
	 */
	getNewCert(index)
	{
		return null;
	}

	toShortString()
	{
		return fileTypeGetName(this.getFileType())+": "+this.toShortName();
	}

	/**
	 * @param {X509Key} key
	 */
	isSignatureKey(key)
	{
		let data = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1");
		let signature = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
		if (data == null || signature == null || signature.itemType != ASN1ItemType.BIT_STRING)
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 * @param {string | null} varName
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 * @param {string|null} varName
	 */
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
				sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(itemPDU.dataOfst, itemPDU.contLen)), ':', null));
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
				X509File.appendAlgorithmIdentifier(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, name, false);
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
				X509File.appendName(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, name);
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 */
	static isCertificate(reader, startOfst, endOfst, path)
	{
		return X509File.isSigned(reader, startOfst, endOfst, path) && X509File.isTBSCertificate(reader, startOfst, endOfst, path+".1");
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 * @param {string | null} varName
	 */
	static appendCertificate(reader, startOfst, endOfst, path, sb, varName)
	{
		X509File.appendTBSCertificate(reader, startOfst, endOfst, path+".1", sb, varName);
		X509File.appendSigned(reader, startOfst, endOfst, path, sb, varName);
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {(string | number)[]} sb
	 * @param {string|null} varName
	 */
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
	
				if ((subsubitemPDU = ASN1Util.pduGetItem(reader, subitemPDU.dataOfst, subitemPDU.endOfst, "1")) != null && subsubitemPDU.itemType == ASN1ItemType.INTEGER)
				{
					if (varName)
					{
						sb.push(varName+".");
					}
					sb.push("revokedCertificates[");
					sb.push(j.toString());
					sb.push("].userCertificate = ");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(subsubitemPDU.dataOfst, subsubitemPDU.contLen)), ':', null));
					sb.push("\r\n");
				}
				if ((subsubitemPDU = ASN1Util.pduGetItem(reader, subitemPDU.dataOfst, subitemPDU.endOfst, "2")) != null && subsubitemPDU.itemType == ASN1ItemType.UTCTIME && (dt = ASN1Util.pduParseUTCTimeCont(reader, subsubitemPDU.rawOfst + subsubitemPDU.hdrLen, subsubitemPDU.rawOfst + subsubitemPDU.hdrLen + subsubitemPDU.contLen)))
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
				if ((subsubitemPDU = ASN1Util.pduGetItem(reader, subitemPDU.dataOfst, subitemPDU.endOfst, "3")) != null && subsubitemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					name = "revokedCertificates[";
					if (varName)
					{
						name = varName+"."+name;
					}
					name = name + j + "].crlEntryExtensions";
					X509File.appendCRLExtensions(reader, subsubitemPDU.rawOfst, subsubitemPDU.endOfst, sb, name);
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
				X509File.appendCRLExtensions(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, name);
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 */
	static isCertificateList(reader, startOfst, endOfst, path)
	{
		return X509File.isSigned(reader, startOfst, endOfst, path) && X509File.isTBSCertList(reader, startOfst, endOfst, path+".1");
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 * @param {string|null} varName
	 */
	static appendCertificateList(reader, startOfst, endOfst, path, sb, varName)
	{
		X509File.appendTBSCertList(reader, startOfst, endOfst, path+".1", sb, varName);
		X509File.appendSigned(reader, startOfst, endOfst, path, sb, varName);
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
	static isPrivateKeyInfo(reader, startOfst, endOfst, path)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt != 3 && cnt != 4)
		{
			return false;
		}
		let sbuff;
		sbuff = path + ".1";
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, sbuff) != ASN1ItemType.INTEGER)
		{
			return false;
		}
		sbuff = path + ".2";
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, sbuff) != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		sbuff = path + ".3";
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, sbuff) != ASN1ItemType.OCTET_STRING)
		{
			return false;
		}
		if (cnt == 4)
		{
			sbuff = path + ".4";
			if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, sbuff) != ASN1ItemType.SET)
			{
				return false;
			}
		}
		return true;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 */
	static appendPrivateKeyInfo(reader, startOfst, endOfst, path, sb)
	{
		let sbuff;
		let itemPDU;
		let keyType = KeyType.Unknown;
		sbuff = path + ".1";
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, sbuff)) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.INTEGER)
			{
				sb.push("version = ");
				X509File.appendVersion(reader, startOfst, endOfst, sbuff, sb);
				sb.push("\r\n");
			}
		}
		sbuff = path + ".2";
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, sbuff)) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				keyType = X509File.appendAlgorithmIdentifier(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, "privateKeyAlgorithm", false);
			}
		}
		sbuff = path + ".3";
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, sbuff)) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.OCTET_STRING)
			{
				sb.push("privateKey = ");
				sb.push("\r\n");
				if (keyType != KeyType.Unknown)
				{
					let privkey = new X509PrivKey("PrivKey", reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen));
					sb.push(privkey.toString());
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
	static isCertificateRequestInfo(reader, startOfst, endOfst, path)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt < 4)
		{
			return false;
		}
		let i = 1;
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
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+"."+(i++)) != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return false;
		}
		return true;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 */
	static appendCertificateRequestInfo(reader, startOfst, endOfst, path, sb)
	{
		let i = 1;
		let itemPDU;
		let subitemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.INTEGER)
			{
				sb.push("serialNumber = ");
				X509File.appendVersion(reader, startOfst, endOfst, path+"."+(i), sb);
				sb.push("\r\n");
			}
		}
		i++;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				X509File.appendName(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, "subject");
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				X509File.appendSubjectPublicKeyInfo(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, "subjectPublicKeyInfo");
				let pubKey = new X509PubKey("PubKey", reader.getArrayBuffer(itemPDU.rawOfst, itemPDU.hdrLen + itemPDU.contLen));
				let key = pubKey.createKey();
				if (key)
				{
					sb.push(key.toString() + "\r\n");
				}
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path+"."+(i++))) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			i = 1;
			while ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString())) != null && subitemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let extOID;
				let ext;
				if ((extOID = ASN1Util.pduGetItem(reader, subitemPDU.dataOfst, subitemPDU.endOfst, "1")) != null && extOID.itemType == ASN1ItemType.OID &&
					(ext = ASN1Util.pduGetItem(reader, subitemPDU.dataOfst, subitemPDU.endOfst, "2")) != null && ext.itemType == ASN1ItemType.SET)
				{
					let oid = reader.getU8Arr(extOID.dataOfst, extOID.contLen);
					if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.14"))
					{
						X509File.appendCRLExtensions(reader, ext.dataOfst, ext.endOfst, sb, "extensionRequest");
					}
					else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.4.1.311.13.2.3")) //szOID_OS_VERSION
					{
						X509File.appendMSOSVersion(reader, ext.dataOfst, ext.endOfst, sb, "osVersion");
					}
					else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.4.1.311.21.20")) //szOID_REQUEST_CLIENT_INFO
					{
						X509File.appendMSRequestClientInfo(reader, ext.dataOfst, ext.endOfst, sb, "reqClientInfo");
					}
					else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.4.1.311.13.2.2")) //szOID_ENROLLMENT_CSP_PROVIDER
					{
						X509File.appendMSEnrollmentCSPProvider(reader, ext.dataOfst, ext.endOfst, sb, "enrollCSPProv");
					}
				}
				i++;
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 */
	static isCertificateRequest(reader, startOfst, endOfst, path)
	{
		return X509File.isSigned(reader, startOfst, endOfst, path) && X509File.isCertificateRequestInfo(reader, startOfst, endOfst, path+".1");
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 */
	static appendCertificateRequest(reader, startOfst, endOfst, path, sb)
	{
		X509File.appendCertificateRequestInfo(reader, startOfst, endOfst, path+".1", sb);
		X509File.appendSigned(reader, startOfst, endOfst, path, sb, null);
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
	static isPublicKeyInfo(reader, startOfst, endOfst, path)
	{
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt < 2)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".1") != ASN1ItemType.SEQUENCE)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".2") != ASN1ItemType.BIT_STRING)
		{
			return false;
		}
		return true;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 */
	static appendPublicKeyInfo(reader, startOfst, endOfst, path, sb)
	{
		let buff = ASN1Util.pduGetItem(reader, startOfst, endOfst, path);
		if (buff.itemType == ASN1ItemType.SEQUENCE)
		{
			X509File.appendSubjectPublicKeyInfo(reader, buff.dataOfst, buff.endOfst, sb, "PubKey");
			let pubKey = new X509PubKey("PubKey", reader.getArrayBuffer(buff.rawOfst, buff.hdrLen + buff.contLen));
			let key = pubKey.createKey();
			if (key)
			{
				sb.push(key.toString()+"\r\n");
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
	static isContentInfo(reader, startOfst, endOfst, path)
	{
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path) != ASN1ItemType.SEQUENCE)
			return false;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt != 2)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".1") != ASN1ItemType.OID)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".2") != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return false;
		}
		return true;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 * @param {string | null} varName
	 * @param {number} dataType
	 */
	static appendContentInfo(reader, startOfst, endOfst, path, sb, varName, dataType)
	{
		let buff = ASN1Util.pduGetItem(reader, startOfst, endOfst, path);
		if (buff.itemType == ASN1ItemType.SEQUENCE)
		{
			let contentType = ASN1Util.pduGetItem(reader, buff.dataOfst, buff.endOfst, "1");
			let content = ASN1Util.pduGetItem(reader, buff.dataOfst, buff.endOfst, "2");
	
			if (contentType)
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("content-type = ");
				sb.push(ASN1Util.oidToString(reader.getU8Arr(contentType.dataOfst, contentType.contLen)));
				let oid = null; //oiddb.oidGetEntry(reader.getU8Arr(contentType.dataOfst, contentType.contLen));
				/*if (oid)
				{
					sb.push(" ("+oid.name+')');
				}*/
				sb.push("\r\n");
			}
			if (contentType && content)
			{
				let oid = reader.getU8Arr(contentType.dataOfst, contentType.contLen);
				if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.7.1")) //data
				{
					let itemPDU = ASN1Util.pduGetItem(reader, content.dataOfst, content.endOfst, "1");
					if (itemPDU != null && itemPDU.itemType == ASN1ItemType.OCTET_STRING)
					{
						X509File.appendData(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".pkcs7-content", dataType);
					}
				}
				else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.7.2")) //signedData
				{
					X509File.appendPKCS7SignedData(reader, content.dataOfst, content.endOfst, sb, varName+".pkcs7-content");
				}
				else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.7.3")) //envelopedData
				{
	
				}
				else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.7.4")) //signedAndEnvelopedData
				{
	
				}
				else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.7.5")) //digestedData
				{
	
				}
				else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.7.6")) //encryptedData
				{
					X509File.appendEncryptedData(reader, content.dataOfst, content.endOfst, sb, varName + ".pkcs7-content", dataType);
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null | undefined} path
	 */
	static isPFX(reader, startOfst, endOfst, path)
	{
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path) != ASN1ItemType.SEQUENCE)
			return false;
		let cnt = ASN1Util.pduCountItem(reader, startOfst, endOfst, path);
		if (cnt != 2 && cnt != 3)
		{
			return false;
		}
		if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".1") != ASN1ItemType.INTEGER)
		{
			return false;
		}
		if (!X509File.isContentInfo(reader, startOfst, endOfst, path+".2"))
		{
			return false;
		}
		if (cnt == 3)
		{
			if (ASN1Util.pduGetItemType(reader, startOfst, endOfst, path+".3") != ASN1ItemType.SEQUENCE)
			{
				return false;
			}
		}
		return true;		
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {any[]} sb
	 * @param {string | null} varName
	 */
	static appendPFX(reader, startOfst, endOfst, path, sb, varName)
	{
		let buff = ASN1Util.pduGetItem(reader, startOfst, endOfst, path);
		if (buff.itemType == ASN1ItemType.SEQUENCE)
		{
			let cnt = ASN1Util.pduCountItem(reader, buff.dataOfst, buff.endOfst, null);
			let version = ASN1Util.pduGetItem(reader, buff.dataOfst, buff.endOfst, "1");
			if (version && version.itemType == ASN1ItemType.INTEGER)
			{
				if (varName)
				{
					sb.push(varName+".");
				}
				sb.push("version = ");
				sb.push(ASN1Util.integerToString(reader, version.dataOfst, version.contLen));
				sb.push("\r\n");
			}
			var name = "authSafe";
			if (varName)
			{
				name = varName + "."+name;
			}
			X509File.appendContentInfo(reader, buff.dataOfst, buff.endOfst, "2", sb, name, ContentDataType.AuthenticatedSafe);
			if (cnt == 3)
			{
				name = "macData";
				if (varName)
				{
					name = varName + "."+name;
				}
				X509File.appendMacData(reader, buff.dataOfst, buff.endOfst, "3", sb, name);
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {(string|number)[]} sb
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {(string|number)[]} sb
	 * @param {string} varName
	 * @param {boolean} pubKey
	 */
	static appendAlgorithmIdentifier(reader, startOfst, endOfst, sb, varName, pubKey)
	{
		let itemPDU;
		let keyType = KeyType.Unknown;
		let innerKeyType = KeyType.Unknown;
		let algorithm = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		let parameters = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2");
		if (algorithm && algorithm.itemType == ASN1ItemType.OID)
		{
			sb.push(varName+".");
			sb.push("algorithm = ");
			sb.push(ASN1Util.oidToString(new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen))));
			keyType = X509File.keyTypeFromOID(new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen)), pubKey);
			let oid = null; //oiddb.oidGetEntry(new Uint8Array(reader.getArrayBuffer(algorithm.rawOfst + algorithm.hdrLen, algorithm.contLen)));
			/*if (oid)
			{
				sb.push(" (" + oid.name + ")");
			}*/
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
				/*if (oidInfo)
				{
					sb.push(" ("+oidInfo.name+")");
				}*/
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendSubjectPublicKeyInfo(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let keyType = KeyType.Unknown;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				keyType = X509File.appendAlgorithmIdentifier(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".algorithm", true);
			}
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.BIT_STRING)
			{
				sb.push(varName);
				sb.push(".subjectPublicKey = ");
				sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(itemPDU.dataOfst + 1, itemPDU.contLen - 1)), ':', null));
				sb.push("\r\n");
				if (keyType != KeyType.Unknown)
				{
					let pkey = new X509Key(varName+".subjectPublicKey", reader.getArrayBuffer(itemPDU.dataOfst + 1, itemPDU.contLen - 1), keyType);
					sb.push(pkey.toString());
					sb.push("\r\n");
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
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
					X509File.appendRelativeDistinguishedName(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName);
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
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
					X509File.appendAttributeTypeAndDistinguishedValue(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName);
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendAttributeTypeAndDistinguishedValue(reader, startOfst, endOfst, sb, varName)
	{
		let typePDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		let valuePDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2");
		if (typePDU && valuePDU && typePDU.itemType == ASN1ItemType.OID)
		{
			sb.push(varName+".");
			let typeOID = reader.getU8Arr(typePDU.dataOfst, typePDU.contLen);
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendCRLExtension(reader, startOfst, endOfst, sb, varName)
	{
		let extension;
		let itemPDU;
		let subItemPDU;
		if ((extension = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if (extension.itemType == ASN1ItemType.OID)
			{
				sb.push(varName+".");
				sb.push("extensionType = ");
				sb.push(ASN1Util.oidToString(reader.getU8Arr(extension.dataOfst, extension.contLen)));
				let oid = null; //oiddb.oidGetEntry(reader.getU8Arr(extension.dataOfst, extension.contLen));
				/*if (oid)
				{
					sb.push(" ("+oid.name+')');
				}*/
				sb.push("\r\n");
			}
		}
		else
		{
			return;
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.BOOLEAN)
			{
				sb.push(varName+".");
				sb.push("critical = ");
				sb.push(ASN1Util.booleanToString(reader, itemPDU.dataOfst, itemPDU.contLen));
				sb.push("\r\n");
				if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "3")) == null)
				{
					return;
				}
			}
			if (itemPDU.itemType == ASN1ItemType.OCTET_STRING)
			{
				let extOID = reader.getU8Arr(extension.dataOfst, extension.contLen);
				if (ASN1Util.oidEqualsText(extOID, "1.3.6.1.5.5.7.1.1")) //id-pe-authorityInfoAccess
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						let i = 1;
						while ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString())) != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
						{
							let descPDU;
							if ((descPDU = ASN1Util.pduGetItem(reader, subItemPDU.dataOfst, subItemPDU.endOfst, "1")) != null && descPDU.itemType == ASN1ItemType.OID)
							{
								sb.push(varName);
								sb.push(".authorityInfoAccess[");
								sb.push(i.toString());
								sb.push("].accessMethod = ");
								sb.push(ASN1Util.oidToString(reader.getU8Arr(descPDU.dataOfst, descPDU.contLen)));
								sb.push(" (");
								//sb.push(oiddb.oidToNameString(reader.getU8Arr(descPDU.dataOfst, descPDU.contLen));
								sb.push(")\r\n");
							}
							let name = varName + ".authorityInfoAccess[" + i + "].accessLocation";
							X509File.appendGeneralName(reader, subItemPDU.dataOfst, subItemPDU.endOfst, "2", sb, name);
							
							i++;
						}
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.14")) //id-ce-subjectKeyIdentifier
				{
					sb.push(varName);
					sb.push(".subjectKeyId = ");
					if (itemPDU.contLen == 22 && reader.readUInt8(itemPDU.dataOfst + 1) == 20)
					{
						sb.push(text.u8Arr2Hex(reader.getU8Arr(itemPDU.dataOfst + 2, itemPDU.contLen - 2), ':', null));
					}
					else
					{
						sb.push(text.u8Arr2Hex(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen), ':', null));
					}
					sb.push("\r\n");
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.15")) //id-ce-keyUsage
				{
					if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.BIT_STRING)
					{
						sb.push(varName);
						sb.push(".keyUsage =");
						if (subItemPDU.contLen >= 2)
						{
							let v = reader.readUInt8(subItemPDU.dataOfst + 1);
							if (v & 0x80) sb.push(" digitalSignature");
							if (v & 0x40) sb.push(" nonRepudiation");
							if (v & 0x20) sb.push(" keyEncipherment");
							if (v & 0x10) sb.push(" dataEncipherment");
							if (v & 0x8) sb.push(" keyAgreement");
							if (v & 0x4) sb.push(" keyCertSign");
							if (v & 0x2) sb.push(" cRLSign");
							if (v & 0x1) sb.push(" encipherOnly");
						}
						if (subItemPDU.contLen >= 3)
						{
							let v = reader.readUInt8(subItemPDU.dataOfst + 2);
							if (v & 0x80) sb.push(" decipherOnly");
						}
						sb.push("\r\n");
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.17")) //id-ce-subjectAltName
				{
					let name = varName + ".subjectAltName";
					X509File.appendGeneralNames(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, name);
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.19")) //id-ce-basicConstraints
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.BOOLEAN)
						{
							sb.push(varName);
							sb.push(".basicConstraints.cA = ");
							sb.push(ASN1Util.booleanToString(reader, subItemPDU.dataOfst, subItemPDU.contLen));
							sb.push("\r\n");
						}
						if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subItemPDU.itemType == ASN1ItemType.INTEGER)
						{
							sb.push(varName);
							sb.push(".basicConstraints.pathLenConstraint = ");
							sb.push(ASN1Util.integerToString(reader, subItemPDU.dataOfst, subItemPDU.contLen));
							sb.push("\r\n");
						}
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.20")) //id-ce-cRLNumber
				{
					if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.INTEGER)
					{
						sb.push(varName);
						sb.push(".cRLNumber = ");
						sb.push(ASN1Util.integerToString(reader, subItemPDU.dataOfst, subItemPDU.contLen));
						sb.push("\r\n");
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.21")) //id-ce-cRLReasons
				{
					if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.ENUMERATED && subItemPDU.contLen == 1)
					{
						sb.push(varName);
						sb.push(".cRLReasons = ");
						switch (reader.readUInt8(subItemPDU.dataOfst))
						{
						case 0:
							sb.push("unspecified");
							break;
						case 1:
							sb.push("keyCompromise");
							break;
						case 2:
							sb.push("cACompromise");
							break;
						case 3:
							sb.push("affiliationChanged");
							break;
						case 4:
							sb.push("superseded");
							break;
						case 5:
							sb.push("cessationOfOperation");
							break;
						case 6:
							sb.push("certificateHold");
							break;
						case 8:
							sb.push("removeFromCRL");
							break;
						case 9:
							sb.push("privilegeWithdrawn");
							break;
						case 10:
							sb.push("aACompromise");
							break;
						default:
							sb.push("unknown");
							break;
						}
						sb.push("\r\n");
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.31")) //id-ce-cRLDistributionPoints
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						let i = 1;
						while (X509File.appendDistributionPoint(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString(), sb, varName))
						{
							i++;
						}
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.32")) //id-ce-certificatePolicies
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						let i = 1;
						while (X509File.appendPolicyInformation(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString(), sb, varName))
						{
							i++;
						}
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.35")) //id-ce-authorityKeyIdentifier
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						let i = 1;
						while ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString())) != null)
						{
							if (subItemPDU.itemType == 0x80)
							{
								sb.push(varName);
								sb.push(".authorityKey.keyId = ");
								sb.push(text.u8Arr2Hex(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen), ':', null));
								sb.push("\r\n");
							}
							else if (subItemPDU.itemType == 0x81 || subItemPDU.itemType == 0xa1)
							{
								let name = varName + ".authorityKey.authorityCertIssuer";
								X509File.appendGeneralName(reader, subItemPDU.dataOfst, subItemPDU.endOfst, "1", sb, name);
							}
							else if (subItemPDU.itemType == 0x82)
							{
								sb.push(varName);
								sb.push(".authorityKey.authorityCertSerialNumber = ");
								sb.push(text.u8Arr2Hex(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen), ':', null));
								sb.push("\r\n");
							}
							i++;
						}
					}
				}
				else if (ASN1Util.oidEqualsText(extOID, "2.5.29.37")) //id-ce-extKeyUsage
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
					{
						let i = 1;
						sb.push(varName);
						sb.push(".extKeyUsage =");
	
						while ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString())) != null)
						{
							if (subItemPDU.itemType == ASN1ItemType.OID)
							{
								let oid = reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen);
								if (ASN1Util.oidEqualsText(oid, "1.3.6.1.5.5.7.3.1"))
								{
									sb.push(" serverAuth");
								}
								else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.5.5.7.3.2"))
								{
									sb.push(" clientAuth");
								}
								else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.5.5.7.3.3"))
								{
									sb.push(" codeSigning");
								}
								else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.5.5.7.3.4"))
								{
									sb.push(" emailProtection");
								}
								else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.5.5.7.3.8"))
								{
									sb.push(" timeStamping");
								}
								else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.5.5.7.3.9"))
								{
									sb.push(" OCSPSigning");
								}
							}
							i++;
						}
						sb.push("\r\n");
					}
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendMSOSVersion(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		if (itemPDU && itemPDU.itemType == ASN1ItemType.IA5STRING)
		{
			sb.push(varName);
			sb.push(".version = ");
			sb.push(new TextDecoder().decode(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen)));
			sb.push("\r\n");
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendMSRequestClientInfo(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			let subitemPDU;
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subitemPDU.itemType == ASN1ItemType.INTEGER)
			{
				sb.push(varName);
				sb.push(".unknown = ");
				sb.push(ASN1Util.integerToString(reader, subitemPDU.dataOfst, subitemPDU.contLen));
				sb.push("\r\n");
			}
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subitemPDU.itemType == ASN1ItemType.UTF8STRING)
			{
				sb.push(varName);
				sb.push(".machine = ");
				sb.push(new TextDecoder().decode(reader.getU8Arr(subitemPDU.dataOfst, subitemPDU.contLen)));
				sb.push("\r\n");
			}
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "3")) != null && subitemPDU.itemType == ASN1ItemType.UTF8STRING)
			{
				sb.push(varName);
				sb.push(".user = ");
				sb.push(new TextDecoder().decode(reader.getU8Arr(subitemPDU.dataOfst, subitemPDU.contLen)));
				sb.push("\r\n");
			}
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "4")) != null && subitemPDU.itemType == ASN1ItemType.UTF8STRING)
			{
				sb.push(varName);
				sb.push(".software = ");
				sb.push(new TextDecoder().decode(reader.getU8Arr(subitemPDU.dataOfst, subitemPDU.contLen)));
				sb.push("\r\n");
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendMSEnrollmentCSPProvider(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			let subitemPDU;
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subitemPDU.itemType == ASN1ItemType.INTEGER)
			{
				sb.push(varName);
				sb.push(".unknown = ");
				sb.push(ASN1Util.integerToString(reader, subitemPDU.dataOfst, subitemPDU.contLen));
				sb.push("\r\n");
			}
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subitemPDU.itemType == ASN1ItemType.BMPSTRING)
			{
				let enc = new text.UTF16BETextBinEnc();
				sb.push(varName);
				sb.push(".provider = ");
				sb.push(enc.encodeBin(reader.getU8Arr(subitemPDU.dataOfst, subitemPDU.contLen)));
				sb.push("\r\n");
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
	static appendGeneralNames(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let i = 1;
				while (X509File.appendGeneralName(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString(), sb, varName))
				{
					i++;
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendGeneralName(reader, startOfst, endOfst, path, sb, varName)
	{
		let subItemPDU;
		if ((subItemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path)) != null)
		{
			switch (0x8F & subItemPDU.itemType)
			{
			case 0x80:
				sb.push(varName);
				sb.push(".otherName = ");
				sb.push(reader.readUTF8(subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
				return true;
			case 0x81:
				sb.push(varName);
				sb.push(".rfc822Name = ");
				sb.push(reader.readUTF8(subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
				return true;
			case 0x82:
				sb.push(varName);
				sb.push(".dNSName = ");
				sb.push(reader.readUTF8(subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
				return true;
			case 0x83:
				sb.push(varName);
				sb.push(".x400Address = ");
				sb.push(reader.readUTF8(subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
				return true;
			case 0x84:
				if ((subItemPDU = ASN1Util.pduGetItem(reader, subItemPDU.dataOfst, subItemPDU.endOfst, path)) != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					let name = varName + ".directoryName";
					X509File.appendName(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, name);
				}
				return true;
			case 0x85:
				sb.push(varName);
				sb.push(".ediPartyName = ");
				sb.push(reader.readUTF8(subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
				return true;
			case 0x86:
				sb.push(varName);
				sb.push(".uniformResourceIdentifier = ");
				sb.push(reader.readUTF8(subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
				return true;
			case 0x87:
				sb.push(varName);
				sb.push(".iPAddress = ");
				if (subItemPDU.contLen == 4)
				{
					sb.push(net.getIPv4Name(reader.readInt32(subItemPDU.dataOfst, false)));
				}
				else if (subItemPDU.contLen == 16)
				{
/*					Net.SocketUtil.AddressInfo addr;
					Net.SocketUtil.SetAddrInfoV6(addr, subItemPDU, 0);
					sptr = Net.SocketUtil.GetAddrName(sbuff, addr);
					sb->AppendP(sbuff, sptr);*/
				}
				sb.push("\r\n");
				return true;
			case 0x88:
				sb.push(varName);
				sb.push(".registeredID = ");
				sb.push(ASN1Util.oidToString(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen)));
				{
					let ent = null; //oiddb.oidGetEntry(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen));
					/*if (ent)
					{
						sb.push(" ("+ent.name+')');
					}*/
				}
				sb.push("\r\n");
				return true;
			}
		}
		return false;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendDistributionPoint(reader, startOfst, endOfst, path, sb, varName)
	{
		let itemPDU;
		let subItemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path)) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let i = 1;
				while ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, i.toString())) != null)
				{
					switch (subItemPDU.itemType)
					{
					case ASN1ItemType.CONTEXT_SPECIFIC_0:
						X509File.appendDistributionPointName(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".distributionPoint");
						break;
					case ASN1ItemType.CONTEXT_SPECIFIC_1:
						if ((subItemPDU = ASN1Util.pduGetItem(reader, subItemPDU.dataOfst, subItemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.BIT_STRING)
						{
							sb.push(varName);
							sb.push(".reasons =");
							if (subItemPDU.contLen >= 2)
							{
								let v = reader.readUInt8(subItemPDU.dataOfst + 1);
								if (v & 0x80) sb.push("unused");
								if (v & 0x40) sb.push("keyCompromise");
								if (v & 0x20) sb.push("cACompromise");
								if (v & 0x10) sb.push("affiliationChanged");
								if (v & 0x8) sb.push("superseded");
								if (v & 0x4) sb.push("cessationOfOperation");
								if (v & 0x2) sb.push("certificateHold");
								if (v & 0x1) sb.push("privilegeWithdrawn");
							}
							if (subItemPDU.contLen >= 3)
							{
								let v = reader.readUInt8(subItemPDU.dataOfst + 2);
								if (v & 0x80) sb.push("aACompromise");
							}
							sb.push("\r\n");
						}
						break;
					case ASN1ItemType.CONTEXT_SPECIFIC_2:
						X509File.appendGeneralNames(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".cRLIssuer");
						break;
					}
					i++;
				}
				return true;
			}
		}
		return false;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
	static appendDistributionPointName(reader, startOfst, endOfst, sb, varName)
	{
		let i;
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null)
		{
			if (itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				i = 0;
				while (X509File.appendGeneralName(reader, itemPDU.dataOfst, itemPDU.endOfst, (++i).toString(), sb, varName+".fullName"))
				{
					i++;
				}
			}
			else if (itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				X509File.appendRelativeDistinguishedName(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".nameRelativeToCRLIssuer");
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 * @param {any} varName
	 */
	static appendPolicyInformation(reader, startOfst, endOfst, path, sb, varName)
	{
		let itemPDU;
		let subItemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path)) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1");
			if (subItemPDU != null && subItemPDU.itemType == ASN1ItemType.OID)
			{
				sb.push(varName);
				sb.push(".policyIdentifier = ");
				sb.push(ASN1Util.oidToString(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen)));
				sb.push(" (");
				//sb.push(oiddb.oidToNameString(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen)));
				sb.push(')');
				sb.push("\r\n");
			}
			subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2");
			if (subItemPDU != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let policyQualifierInfoPDU;
				let i = 0;
				while ((policyQualifierInfoPDU = ASN1Util.pduGetItem(reader, subItemPDU.dataOfst, subItemPDU.endOfst, (++i).toString())) != null && policyQualifierInfoPDU.itemType == ASN1ItemType.SEQUENCE)
				{
					if ((itemPDU = ASN1Util.pduGetItem(reader, policyQualifierInfoPDU.dataOfst, policyQualifierInfoPDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.OID)
					{
						sb.push(varName);
						sb.push(",policyQualifiers["+i+"].policyQualifierId = ");
						sb.push(ASN1Util.oidToString(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen)));
						sb.push(" (");
						//sb.push(oiddb.oidToNameString(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen)));
						sb.push(')');
						sb.push("\r\n");
					}
					if ((itemPDU = ASN1Util.pduGetItem(reader, policyQualifierInfoPDU.dataOfst, policyQualifierInfoPDU.endOfst, "2")) != null)
					{
						if (itemPDU.itemType == ASN1ItemType.IA5STRING)
						{
							sb.push(varName);
							sb.push(".policyQualifiers["+i+"].qualifier = ");
							sb.push(reader.readUTF8(itemPDU.dataOfst, itemPDU.contLen));
							sb.push("\r\n");
						}
						else if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
						{
							/////////////////////////////////// UserNotice
						}
					}
				}
			}
			return true;
		}
		return false;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendPKCS7SignedData(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		if (itemPDU != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			let subItemPDU;
			if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.INTEGER)
			{
				sb.push("signedData.version = ");
				sb.push(ASN1Util.integerToString(reader, subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
			}
			if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subItemPDU.itemType == ASN1ItemType.SET)
			{
				X509File.appendPKCS7DigestAlgorithmIdentifiers(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, "signedData.digestAlgorithms");
			}
			if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "3")) != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				X509File.appendContentInfo(reader, subItemPDU.rawOfst, subItemPDU.endOfst, "1", sb, "signedData.contentInfo", ContentDataType.Unknown);
			}
			let i = 4;
			subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "4");
			if (subItemPDU != null && subItemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				X509File.appendCertificate(reader, subItemPDU.dataOfst, subItemPDU.endOfst, "1", sb, "signedData.certificates");
				subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, (++i).toString());
			}
			if (subItemPDU != null && subItemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				//AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.crls"));
				subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, (++i).toString());
			}
			if (subItemPDU != null && subItemPDU.itemType == ASN1ItemType.SET)
			{
				X509File.appendPKCS7SignerInfos(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, "signedData.signerInfos");
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
	static appendPKCS7DigestAlgorithmIdentifiers(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let i = 0;
		while (true)
		{
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, (++i).toString())) == null)
			{
				return;
			}
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				X509File.appendAlgorithmIdentifier(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName, false);
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
	static appendPKCS7SignerInfos(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let i = 0;
		while (true)
		{
			itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, (++i).toString());
			if (itemPDU == null)
			{
				break;
			}
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				X509File.appendPKCS7SignerInfo(reader, itemPDU.rawOfst, itemPDU.endOfst, sb, varName);
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendPKCS7SignerInfo(reader, startOfst, endOfst, sb, varName)
	{
		let i;
		let itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1");
		if (itemPDU == null || itemPDU.itemType != ASN1ItemType.SEQUENCE)
		{
			return;
		}
		let subItemPDU;
		if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.INTEGER)
		{
			sb.push(varName);
			sb.push(".version = ");
			sb.push(ASN1Util.integerToString(reader, subItemPDU.dataOfst, subItemPDU.contLen));
			sb.push("\r\n");
		}
		if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			X509File.appendIssuerAndSerialNumber(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".issuerAndSerialNumber");
		}
		if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "3")) != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			X509File.appendAlgorithmIdentifier(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".digestAlgorithm", false);
		}
		i = 4;
		if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "4")) != null && subItemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			X509File.appendPKCS7Attributes(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".authenticatedAttributes");
			subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, (++i).toString());
		}
		if (subItemPDU != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			X509File.appendAlgorithmIdentifier(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".digestEncryptionAlgorithm", false);
		}
		if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, (++i).toString())) != null && subItemPDU.itemType == ASN1ItemType.OCTET_STRING)
		{
			sb.push(varName);
			sb.push(".encryptedDigest = ");
			sb.push(text.u8Arr2Hex(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen), ':', null));
			sb.push("\r\n");
		}
		if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, (++i).toString())) != null && subItemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
		{
			X509File.appendPKCS7Attributes(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".unauthenticatedAttributes");
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendIssuerAndSerialNumber(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			X509File.appendName(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".issuer");
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null && itemPDU.itemType == ASN1ItemType.INTEGER)
		{
			sb.push(varName);
			sb.push(".serialNumber = ");
			sb.push(text.u8Arr2Hex(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen), ':', null));
			sb.push("\r\n");
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendPKCS7Attributes(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let oidPDU;
		let valuePDU;
		let i = 0;
		while (true)
		{
			if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, (++i).toString())) == null)
			{
				return;
			}
			if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				oidPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1");
				valuePDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2");
				if (oidPDU != null && oidPDU.itemType == ASN1ItemType.OID)
				{
					sb.push(varName);
					sb.push(".attributeType = ");
					sb.push(ASN1Util.oidToString(reader.getU8Arr(oidPDU.dataOfst, oidPDU.contLen)));
					let oid = null; //oiddb.oidGetEntry(reader.getU8Arr(oidPDU.dataOfst, oidPDU.contLen));
					/*if (oid)
					{
						sb.push(" ("+oid.name+")");
					}*/
					sb.push("\r\n");
				}
				if (valuePDU && valuePDU.itemType == ASN1ItemType.SET)
				{
					let oid = reader.getU8Arr(oidPDU.dataOfst, oidPDU.contLen);
					if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.3")) //contentType
					{
						if ((itemPDU = ASN1Util.pduGetItem(reader, valuePDU.dataOfst, valuePDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.OID)
						{
							sb.push(varName);
							sb.push(".contentType = ");
							sb.push(ASN1Util.oidToString(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen)));
							let oid = null; //oiddb.oidGetEntry(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen));
							/*if (oid)
							{
								sb.push(" ("+oid.name+")");
							}*/
							sb.push("\r\n");
						}
					}
					else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.4")) //messageDigest
					{
						if ((itemPDU = ASN1Util.pduGetItem(reader, valuePDU.dataOfst, valuePDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.OCTET_STRING)
						{
							sb.push(varName);
							sb.push(".messageDigest = ");
							sb.push(text.u8Arr2Hex(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen), ':', null));
							sb.push("\r\n");
						}
					}
					else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.5")) //signing-time
					{
						if ((itemPDU = ASN1Util.pduGetItem(reader, valuePDU.dataOfst, valuePDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.UTCTIME)
						{
							sb.push(varName);
							sb.push(".signing-time = ");
							sb.push(ASN1Util.utcTimeToString(reader, itemPDU.dataOfst, itemPDU.contLen));
							sb.push("\r\n");
						}
					}
					else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.15")) //smimeCapabilities
					{
						/////////////////////////////////////
					}
					else if (ASN1Util.oidEqualsText(oid, "1.2.840.113549.1.9.16.2.11")) //id-aa-encrypKeyPref
					{
						if ((itemPDU = ASN1Util.pduGetItem(reader, valuePDU.dataOfst, valuePDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
						{
							X509File.appendIssuerAndSerialNumber(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".encrypKeyPref");
						}
					}
					else if (ASN1Util.oidEqualsText(oid, "1.3.6.1.4.1.311.16.4")) //outlookExpress
					{
						if ((itemPDU = ASN1Util.pduGetItem(reader, valuePDU.dataOfst, valuePDU.endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
						{
							X509File.appendIssuerAndSerialNumber(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".outlookExpress");
						}
					}
					
				}
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} path
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendMacData(reader, startOfst, endOfst, path, sb, varName)
	{
		let itemPDU;
		let subItemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, path)) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "1")) != null && subItemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				X509File.appendDigestInfo(reader, subItemPDU.dataOfst, subItemPDU.endOfst, sb, varName+".mac");
			}
			if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subItemPDU.itemType == ASN1ItemType.OCTET_STRING)
			{
				sb.push(varName);
				sb.push(".macSalt = ");
				sb.push(text.u8Arr2Hex(reader.getU8Arr(subItemPDU.dataOfst, subItemPDU.contLen), ' ', null));
				sb.push("\r\n");
			}
			if ((subItemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "3")) != null && subItemPDU.itemType == ASN1ItemType.INTEGER)
			{
				sb.push(varName);
				sb.push(".iterations = ");
				sb.push(ASN1Util.integerToString(reader, subItemPDU.dataOfst, subItemPDU.contLen));
				sb.push("\r\n");
			}
			return true;
		}
		return false;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 */
	static appendDigestInfo(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			X509File.appendAlgorithmIdentifier(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, varName+".digestAlgorithm", false);
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null && itemPDU.itemType == ASN1ItemType.OCTET_STRING)
		{
			sb.push(varName);
			sb.push(".digest = ");
			sb.push(text.u8Arr2Hex(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen), ' ', null));
			sb.push("\r\n");
		}
	}

	/**
	 * @param {any} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 * @param {any} dataType
	 */
	static appendData(reader, startOfst, endOfst, sb, varName, dataType)
	{
		switch (dataType)
		{
		case ContentDataType.AuthenticatedSafe:
			X509File.appendAuthenticatedSafe(reader, startOfst, endOfst, sb, varName);
			break;
		case ContentDataType.Unknown:
		default:
			break;
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 * @param {any} dataType
	 */
	static appendEncryptedData(reader, startOfst, endOfst, sb, varName, dataType)
	{
		let itemPDU;
		let subitemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			if (varName)
			{
				sb.push(varName+".");
			}
			sb.push("version = ");
			X509File.appendVersion(reader, itemPDU.dataOfst, itemPDU.endOfst, "1", sb);
			sb.push("\r\n");
	
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "2")) != null && subitemPDU.itemType == ASN1ItemType.SEQUENCE)
			{
				let name = "encryptedContentInfo";
				if (varName)
				{
					name = varName+"."+name;
				}
				X509File.appendEncryptedContentInfo(reader, subitemPDU.dataOfst, subitemPDU.endOfst, sb, name, dataType);
			}
			if ((subitemPDU = ASN1Util.pduGetItem(reader, itemPDU.dataOfst, itemPDU.endOfst, "3")) != null && subitemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				let name = "unprotectedAttributes";
				if (varName)
				{
					name = varName+"."+name;
				}
				X509File.appendPKCS7Attributes(reader, subitemPDU.dataOfst, subitemPDU.endOfst, sb, name);
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any} sb
	 * @param {string} varName
	 */
	static appendAuthenticatedSafe(reader, startOfst, endOfst, sb, varName)
	{
		let itemPDU;
		let i;
		let j;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			i = 0;
			j = ASN1Util.pduCountItem(reader, itemPDU.dataOfst, itemPDU.endOfst, null);
			while (i < j)
			{
				X509File.appendContentInfo(reader, itemPDU.dataOfst, itemPDU.endOfst, (i + 1).toString(), sb, varName+"["+i+"]", ContentDataType.Unknown);
				i++;
			}
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} sb
	 * @param {string} varName
	 * @param {any} dataType
	 */
	static appendEncryptedContentInfo(reader, startOfst, endOfst, sb, varName, dataType)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.OID)
		{
			sb.push(varName);
			sb.push(".contentType = ");
			sb.push(ASN1Util.oidToString(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen)));
			let oid = null; //oiddb.oidGetEntry(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen));
			/*if (oid)
			{
				sb.push(" ("+oid.name+")");
			}*/
			sb.push("\r\n");
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "2")) != null && itemPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			let name = "contentEncryptionAlgorithm";
			if (varName)
			{
				name = varName+"."+name;
			}
			X509File.appendAlgorithmIdentifier(reader, itemPDU.dataOfst, itemPDU.endOfst, sb, name, false);
		}
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "3")) != null && (itemPDU.itemType & 0x8F) == 0x80)
		{
			sb.push(varName);
			sb.push(".encryptedContent = ");
			sb.push(text.u8Arr2Hex(reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen), ' ', null));
			sb.push("\r\n");
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string} oidText
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
	static nameGetCN(reader, startOfst, endOfst)
	{
		return X509File.nameGetByOID(reader, startOfst, endOfst, "2.5.4.3");
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
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
				if (itemPDU.itemType == ASN1ItemType.SEQUENCE)
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
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
								/**
								 * @type {any[]}
								 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {any[]} distPoints
	 */
	static distributionPointAdd(reader, startOfst, endOfst, distPoints)
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(reader, startOfst, endOfst, "1")) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1")) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
			{
				if ((itemPDU = ASN1Util.pduGetItem(reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1")) != null && itemPDU.itemType == ASN1ItemType.CHOICE_6)
				{
					distPoints.push(reader.readUTF8(itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.contLen));
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {number} keyType
	 */
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

	/**
	 * @param {ArrayBuffer | Iterable<number>} oid
	 * @param {boolean} pubKey
	 */
	static keyTypeFromOID(oid, pubKey)
	{
		let arr;
		if (oid instanceof ArrayBuffer)
			arr = new Uint8Array(oid);
		else
			arr = new Uint8Array(oid);
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

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
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
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
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

	/**
	 * @param {number} index
	 */
	getCertName(index)
	{
		if (index != 0)
			return null;
		return this.getSubjectCN();
	}

	/**
	 * @param {number} index
	 */
	getNewCert(index)
	{
		if (index != 0)
			return null;
		return this.clone();
	}

	async isValid()
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
			let signValid = await key.signatureVerify(hashType, signedInfo.payload, signedInfo.signature);
			if (signValid)
			{
				return CertValidStatus.SelfSigned;
			}
			else
			{
				return CertValidStatus.SignatureInvalid;
			}
		}

/*		let key = issuer.getNewPublicKey();
		if (key == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let signValid = key.signatureVerify(hashType, signedInfo.payload, signedInfo.signature);
		if (!signValid)
		{
			return CertValidStatus.SignatureInvalid;
		}

		let crlDistributionPoints = this.getCRLDistributionPoints();*/
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
		return sb.join("");
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
		if (pdu.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
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

	/**
	 * @param {string} domain
	 */
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
				return X509File.extensionsGetCRLDistributionPoints(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
			}
		}
		else
		{
			pdu = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.7.1");
			if (pdu)
			{
				return X509File.extensionsGetCRLDistributionPoints(this.reader, pdu.rawOfst + pdu.hdrLen, pdu.rawOfst + pdu.hdrLen + pdu.contLen);
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

export class X509CertReq extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getFileType()
	{
		return X509FileType.CertRequest;
	}

	toShortName()
	{
		let tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.2");
		if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
		{
			return X509File.nameGetCN(this.reader, tmpBuff.dataOfst, tmpBuff.endOfst);
		}
		return "";
	}

	async isValid()
	{
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
		let key = this.getNewPublicKey();
		if (key == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let valid = await key.signatureVerify(hashType, signedInfo.payload, signedInfo.signature);
		if (valid)
		{
			return CertValidStatus.Valid;
		}
		else
		{
			return CertValidStatus.SignatureInvalid;
		}
	}
	
	clone()
	{
		return new X509CertReq(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		let sb = [];
		if (X509File.isCertificateRequest(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendCertificateRequest(this.reader, 0, this.reader.getLength(), "1", sb);
		}
		return sb.join("");
	}

	createNames()
	{
		return new ASN1Names().setCertificationRequest();
	}

	getNames()
	{
		let namesPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.2");
		if (namesPDU)
		{
			return X509File.namesGet(this.reader, namesPDU.dataOfst, namesPDU.endOfst);
		}
		return null;
	}

	getExtensions()
	{
		let extPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4.1");
		if (extPDU && extPDU.itemType == ASN1ItemType.SEQUENCE)
		{
			let oid = ASN1Util.pduGetItem(this.reader, extPDU.dataOfst, extPDU.endOfst, "1");
			if (oid && ASN1Util.oidEqualsText(this.reader.getU8Arr(oid.dataOfst, oid.contLen), "1.2.840.113549.1.9.14")) //extensionRequest
			{
				let extSeq = ASN1Util.pduGetItem(this.reader, extPDU.dataOfst, extPDU.endOfst, "2.1");
				if (extSeq && extSeq.itemType == ASN1ItemType.SEQUENCE)
				{
					return X509File.extensionsGet(this.reader, extSeq.dataOfst, extSeq.endOfst);
				}
			}
		}
		return null;		
	}

	getNewPublicKey()
	{
		let keyPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
		if (keyPDU)
		{
			return X509File.publicKeyGetNew(this.reader, keyPDU.dataOfst, keyPDU.endOfst);
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
}

export class X509Key extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 * @param {number} keyType
	 */
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

	async isValid()
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
		if (this.keyType != KeyType.RSA) return null;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.4");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAPrime1()
	{
		if (this.keyType != KeyType.RSA) return null;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.5");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAPrime2()
	{
		if (this.keyType != KeyType.RSA) return null;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.6");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAExponent1()
	{
		if (this.keyType != KeyType.RSA) return null;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.7");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSAExponent2()
	{
		if (this.keyType != KeyType.RSA) return null;
		let len = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.8");
		if (len)
		{
			return this.reader.getArrayBuffer(len.rawOfst + len.hdrLen, len.contLen);
		}
		return null;
	}

	getRSACoefficient()
	{
		if (this.keyType != KeyType.RSA) return null;
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
			return null;
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
				return null;
			}
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.4");
			if (itemPDU != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_1)
			{
				itemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.rawOfst + itemPDU.hdrLen, itemPDU.rawOfst + itemPDU.hdrLen + itemPDU.contLen, "1");
				if (itemPDU != null && itemPDU.itemType == ASN1ItemType.BIT_STRING)
				{
					return this.reader.getArrayBuffer(itemPDU.rawOfst + itemPDU.hdrLen + 1, itemPDU.contLen - 1);
				}
				return null;
			}
			return null;
		}
		else
		{
			return null;
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

	/**
	 * @param {hash.HashType} hashType
	 * @param {ArrayBuffer} payload
	 * @param {ArrayBuffer} signature
	 */
	async signatureVerify(hashType, payload, signature)
	{
		let privKey = X509PrivKey.createFromKey(this);
		if (privKey == null)
		{
			return false;
		}
		let alg;
		if (this.keyType == KeyType.RSA)
		{
			alg = {name: "RSASSA-PKCS1-v1_5", hash: "SHA-256"};
			switch (hashType)
			{
			case hash.HashType.SHA1:
				alg.hash = "SHA-1";
				break;
			case hash.HashType.SHA224:
				alg.hash = "SHA-224";
				break;
			case hash.HashType.SHA256:
				alg.hash = "SHA-256";
				break;
			case hash.HashType.SHA384:
				alg.hash = "SHA-384";
				break;
			case hash.HashType.SHA512:
				alg.hash = "SHA-512";
				break;
			}
		}
		else if (this.keyType == KeyType.ECDSA)
		{
			return false;
		}
		else
		{
			return false;
		}
		let key = await window.crypto.subtle.importKey("pkcs8", privKey.getASN1Buff().getArrayBuffer(), alg, false, ["verify"]);
		let result = await window.crypto.subtle.verify(
			"RSASSA-PKCS1-v1_5",
			key,
			signature,
			payload
		  );
		
		return result;
	}

	/**
	 * @param {ArrayBuffer} buff
	 * @param {ArrayBuffer} paramOID
	 */
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

export class X509PrivKey extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getFileType()
	{
		return X509FileType.PrivateKey;	
	}

	toShortName()
	{
		let oidPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1");
		if (oidPDU == null || oidPDU.itemType != ASN1ItemType.OID)
		{
			return "";
		}
		let keyType = X509File.keyTypeFromOID(this.reader.getArrayBuffer(oidPDU.dataOfst, oidPDU.contLen), false);
		let keyPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
		if (keyPDU && keyPDU.itemType == ASN1ItemType.OCTET_STRING)
		{
			let sb = [];
			sb.push(keyTypeGetName(keyType));
			sb.push(' ');
			sb.push(X509File.keyGetLeng(this.reader, keyPDU.dataOfst, keyPDU.endOfst, keyType));
			sb.push(" bits");
			return sb.join("");
		}
		else
		{
			return "";
		}
	}

	async isValid()
	{
		return CertValidStatus.SignatureInvalid;
	}

	clone()
	{
		return new X509PrivKey(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		let sb = [];
		if (X509File.isPrivateKeyInfo(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendPrivateKeyInfo(this.reader, 0, this.reader.getLength(), "1", sb);
		}
		return sb.join("");
	}

	createNames()
	{
		return null;
	}
	
	getKeyType()
	{
		let keyTypeOID = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1");
		if (keyTypeOID != null)
		{
			return X509File.keyTypeFromOID(this.reader.getArrayBuffer(keyTypeOID.dataOfst, keyTypeOID.contLen), false);
		}
		return KeyType.Unknown;
	}

	createKey()
	{
		let keyType = this.getKeyType();
		if (keyType == KeyType.Unknown)
		{
			return null;
		}
		let keyData = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.3");
		if (keyData != null)
		{
			return new X509Key(this.sourceName, this.reader.getArrayBuffer(keyData.dataOfst, keyData.contLen), keyType);
		}
		return null;
	}

	/**
	 * @param {any} keyType
	 * @param {string | ArrayBuffer} buff
	 * @param {string | null} sourceName
	 */
	static createFromKeyBuff(keyType, buff, sourceName)
	{
		if (sourceName == null)
		{
			sourceName = "";
		}
		let keyPDU = new ASN1PDUBuilder();
		keyPDU.beginSequence();
		keyPDU.appendInt32(0);
		keyPDU.beginSequence();
		let oidStr = keyTypeGetOID(keyType);
		keyPDU.appendOIDString(oidStr);
		keyPDU.appendNull();
		keyPDU.endLevel();
		keyPDU.appendOctetString(buff);
		keyPDU.endLevel();
		return new X509PrivKey(sourceName, keyPDU.getArrayBuffer());
	}

	/**
	 * @param {X509Key} key
	 */
	static createFromKey(key)
	{
		if (!(key instanceof X509Key))
			return null;
		let keyType = key.getKeyType();
		if (keyType == KeyType.ECDSA)
		{
			let ecName = key.getECName();
			let keyBuff;
			let keyPDU = new ASN1PDUBuilder();
			keyPDU.beginSequence();
			keyPDU.appendInt32(0);
			keyPDU.beginSequence();
			let oidStr = keyTypeGetOID(keyType);
			keyPDU.appendOIDString(oidStr);
			oidStr = ecNameGetOID(ecName);
			keyPDU.appendOIDString(oidStr);
			keyPDU.endLevel();
			keyPDU.beginOther(4);
			keyPDU.beginSequence();
			keyPDU.appendInt32(1);
			keyBuff = key.getECPrivate();
			if (keyBuff)
			{
				keyPDU.appendOctetString(keyBuff);
			}
			else
			{
				keyPDU.appendOctetString(new Uint8Array().buffer);
			}
			keyBuff = key.getECPublic();
			if (keyBuff)
			{
				keyPDU.beginContentSpecific(1);
				keyPDU.appendBitString(0, keyBuff);
				keyPDU.endLevel();
			}
			keyPDU.endLevel();
			keyPDU.endLevel();
			keyPDU.endLevel();
			return new X509PrivKey(key.sourceName, keyPDU.getArrayBuffer());
		}
		else if (keyType == KeyType.RSA)
		{
			return X509PrivKey.createFromKeyBuff(keyType, key.getASN1Buff().getArrayBuffer(), key.sourceName);
		}
		else
		{
			return null;
		}	
	}
}

export class X509PubKey extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
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
		let keyType = X509File.keyTypeFromOID(this.reader.getArrayBuffer(oidPDU.rawOfst + oidPDU.hdrLen, oidPDU.contLen), true);
		let keyPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2");
		if (keyPDU && keyPDU.itemType == ASN1ItemType.OCTET_STRING)
		{
			let sb = [];
			sb.push(keyTypeGetName(keyType)+" ");
			sb.push(X509File.keyGetLeng(this.reader, keyPDU.rawOfst + keyPDU.hdrLen, keyPDU.rawOfst + keyPDU.hdrLen + keyPDU.contLen, keyType));
			sb.push(" bits");
			return sb.join("");
		}
		return null;
	}

	async isValid()
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

	/**
	 * @param {number} keyType
	 * @param {ArrayBuffer} buff
	 * @param {any} sourceName
	 */
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

	/**
	 * @param {{ getKeyType: () => any; getASN1Buff: () => { (): any; new (): any; getArrayBuffer: { (): any; new (): any; }; }; sourceName: any; }} key
	 */
	static createFromKey(key)
	{
		return X509PubKey.createFromKeyBuff(key.getKeyType(), key.getASN1Buff().getArrayBuffer(), key.sourceName);
	}
}

export class X509PKCS7 extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getFileType()
	{
		return X509FileType.PKCS7;
	}

	toShortName()
	{
		return null;
	}
	
	getCertCount()
	{
		let certListPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1.4");
		if (certListPDU == null || certListPDU.itemType != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return 0;
		}
		return ASN1Util.pduCountItem(this.reader, certListPDU.dataOfst, certListPDU.endOfst, null);
	}

	/**
	 * @param {number} index
	 */
	getCertName(index)
	{
		let certListPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1.4");
		if (certListPDU == null || certListPDU.itemType != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return null;
		}
	
		let tmpBuff;
		if (ASN1Util.pduGetItemType(this.reader, certListPDU.dataOfst, certListPDU.endOfst, (index + 1)+".1.1") == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, certListPDU.dataOfst, certListPDU.endOfst, (index + 1)+".1.6");
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, certListPDU.dataOfst, certListPDU.endOfst, (index + 1)+".1.5");
		}
		if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
		{
			return X509File.nameGetCN(this.reader, tmpBuff.dataOfst, tmpBuff.endOfst);
		}
		return null;
	}

	/**
	 * @param {number} index
	 */
	getNewCert(index)
	{
		let certListPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1.4");
		if (certListPDU == null || certListPDU.itemType != ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			return null;
		}
		let certPDU = ASN1Util.pduGetItem(this.reader, certListPDU.dataOfst, certListPDU.endOfst, (index + 1).toString());
		if (certPDU)
		{
			return new X509Cert(this.sourceName, this.reader.getArrayBuffer(certPDU.rawOfst, certPDU.hdrLen + certPDU.contLen));
		}
		return null;
	}

	async isValid()
	{
		return CertValidStatus.SignatureInvalid;
	}

	clone()
	{
		return new X509PKCS7(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		let sb = [];
		if (X509File.isContentInfo(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendContentInfo(this.reader, 0, this.reader.getLength(), "1", sb, null, ContentDataType.Unknown);
		}
		return sb.join("");
	}

	createNames()
	{
		return new ASN1Names().setPKCS7ContentInfo();
	}

	isSignData()
	{
		let itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1");
		if (itemPDU == null || itemPDU.itemType != ASN1ItemType.OID)
		{
			return false;
		}
		return ASN1Util.oidEqualsText(this.reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen), "1.2.840.113549.1.7.2");	
	}

	getDigestType()
	{
		if (!this.isSignData())
		{
			return hash.HashType.Unknown;
		}
		let itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1.5.1.3.1");
		if (itemPDU && itemPDU.itemType == ASN1ItemType.OID)
		{
			return hashTypeFromOID(this.reader.getU8Arr(itemPDU.dataOfst, itemPDU.contLen));
		}
		return hash.HashType.Unknown;
	}

	getMessageDigest()
	{
		let itemPDU;
		let subitemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1.5.1.4")) != null && itemPDU.itemType == ASN1ItemType.CONTEXT_SPECIFIC_0)
		{
			let i = 0;
			while (true)
			{
				if ((subitemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.dataOfst, itemPDU.endOfst, (++i)+".1")) == null)
				{
					break;
				}
				else if (subitemPDU.itemType == ASN1ItemType.OID && ASN1Util.oidEqualsText(this.reader.getU8Arr(subitemPDU.dataOfst, subitemPDU.contLen), "1.2.840.113549.1.9.4"))
				{
					if ((subitemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.dataOfst, itemPDU.endOfst, (i)+".2.1")) != null &&
						subitemPDU.itemType == ASN1ItemType.OCTET_STRING)
					{
						return this.reader.getArrayBuffer(subitemPDU.dataOfst, subitemPDU.contLen);
					}
				}
			}
		}
		return null;
	}

	getEncryptedDigest()
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.2.1.5.1.6")) != null && itemPDU.itemType == ASN1ItemType.OCTET_STRING)
		{
			return this.reader.getArrayBuffer(itemPDU.dataOfst, itemPDU.contLen);
		}
		return null;
	}
}

export class X509PKCS12 extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getFileType()
	{
		return X509FileType.PKCS12;
	}

	toShortName()
	{
		return null;
	}

	getCertCount()
	{
		return 0;
	}

	/**
	 * @param {number} index
	 */
	getCertName(index)
	{
		return null;
	}

	/**
	 * @param {number} index
	 */
	getNewCert(index)
	{
		return null;
	}

	async isValid()
	{
		return CertValidStatus.SignatureInvalid
	}

	clone()
	{
		return new X509PKCS12(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		let sb = [];
		if (X509File.isPFX(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendPFX(this.reader, 0, this.reader.getLength(), "1", sb, null);
		}
		return sb.join("");
	}

	createNames()
	{
		return new ASN1Names().setPFX();
	}
}

export class X509FileList extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {X509Cert} cert
	 */
	constructor(sourceName, cert)
	{
		super(sourceName, "application/x-pem-file", cert.reader.getArrayBuffer());
		this.fileList = [cert];
	}

	getFileType()
	{
		return X509FileType.FileList;
	}

	toShortName()
	{
		return this.fileList[0].toShortName();
	}

	getCertCount()
	{
		let ret = 0;
		let i;
		for (i in this.fileList)
		{
			ret += this.fileList[i].getCertCount();
		}
		return ret;
	}

	/**
	 * @param {number} index
	 */
	getCertName(index)
	{
		let i;
		let cnt;
		let file;
		for (i in this.fileList)
		{
			file = this.fileList[i];
			cnt = file.getCertCount();
			if (index < cnt)
			{
				return file.getCertName(index);
			}
			index -= cnt;
		}
		return null;
	}

	/**
	 * @param {number} index
	 */
	getNewCert(index)
	{
		let i;
		let cnt;
		let file;
		for (i in this.fileList)
		{
			file = this.fileList[i];
			cnt = file.getCertCount();
			if (index < cnt)
			{
				return file.getNewCert(index);
			}
			index -= cnt;
		}
		return null;
	}

	async isValid()
	{
		let status;
		let i;
		if (this.fileList.length > 1)
		{
			let file;
			for (i in this.fileList)
			{
				file = this.fileList[i];
				status = await file.isValid();
				if (status != CertValidStatus.Valid)
				{
					return status;
				}
			}
			return CertValidStatus.Valid;
		}
		else
		{
			return await this.fileList[0].isValid();
		}		
	}

	clone()
	{
		let fileList = new X509FileList(this.sourceName, this.fileList[0].clone());
		let i = 1;
		let j = this.fileList.length;
		while (i < j)
		{
			fileList.addFile(this.fileList[i].clone());
			i++;
		}
		return fileList;
	}

	createX509Cert()
	{
		return this.fileList[0].clone();
	}

	toString()
	{
		let sb = [];
		let i;
		for (i in this.fileList)
		{
			sb.push(this.fileList[i].toString());
		}
		return sb.join("\r\n");
	}

	createNames()
	{
		return this.fileList[0].createNames();
	}

	/**
	 * @param {any} file
	 */
	addFile(file)
	{
		this.fileList.push(file);
	}

	getFileCount()
	{
		return this.fileList.length;
	}

	/**
	 * @param {string | number} index
	 */
	getFile(index)
	{
		return this.fileList[index];
	}

	setDefaultSourceName()
	{
		let file;
		let j = -1;
		let i = this.fileList.length;
		while (i-- > 0)
		{
			if (this.fileList[i] && this.fileList[i].getFileType() == X509FileType.Cert)
			{
				this.fileList[i].setDefaultSourceName();
				j = i;
			}
		}
		if (j != -1)
		{
			this.setSourceName(this.fileList[j].sourceName);
		}
	}
}

export class X509CRL extends X509File
{
	/**
	 * @param {string} sourceName
	 * @param {ArrayBuffer} buff
	 */
	constructor(sourceName, buff)
	{
		super(sourceName, "application/x-pem-file", buff);
	}

	getFileType()
	{
		return X509FileType.CRL;
	}

	toShortName()
	{
		let tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
		if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
		{
			return X509File.nameGetCN(this.reader, tmpBuff.dataOfst, tmpBuff.endOfst);
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.2");
			if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
			{
				return X509File.nameGetCN(this.reader, tmpBuff.dataOfst, tmpBuff.endOfst);
			}
		}
		return null;
	}

	async isValid()
	{
		let issuer = this.getIssuerCN();
		if (issuer == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let dt;
		let currTime = data.Timestamp.now();
		if ((dt = this.getThisUpdate()) == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		if (dt.toTicks() > currTime.toTicks())
		{
			return CertValidStatus.Expired;
		}
		if (dt = this.getNextUpdate())
		{
			if (dt.toTicks() < currTime.toTicks())
			{
				return CertValidStatus.Expired;
			}
		}
		let signedInfo = this.getSignedInfo();
		if (signedInfo == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let hashType = algTypeGetHash(signedInfo.algType);
		if (hashType == hash.HashType.Unknown)
		{
			return CertValidStatus.UnsupportedAlgorithm;
		}
	
		let issuerCert = null; //trustStore.getCertByCN(issuer);
		if (issuerCert == null)
		{
			return CertValidStatus.UnknownIssuer;
		}
/*		let key = issuerCert.getNewPublicKey();
		if (key == null)
		{
			return CertValidStatus.FileFormatInvalid;
		}
		let signValid = key.signatureVerify(hashType, signedInfo.payload, signedInfo.signature);
		if (!signValid)
		{
			return CertValidStatus.SignatureInvalid;
		}*/
		return CertValidStatus.Valid;
	}
	
	clone()
	{
		return new X509CRL(this.sourceName, this.reader.getArrayBuffer());
	}

	toString()
	{
		let sb = [];
		if (X509File.isCertificateList(this.reader, 0, this.reader.getLength(), "1"))
		{
			X509File.appendCertificateList(this.reader, 0, this.reader.getLength(), "1", sb, null);
		}
		return sb.join("");
	}

	createNames()
	{
		return new ASN1Names().setCertificateList();
	}

	hasVersion()
	{
		let itemPDU;
		if ((itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.1")) != null && itemPDU.itemType == ASN1ItemType.INTEGER)
		{
			return true;
		}
		return false;
	}

	getIssuerCN()
	{
		let tmpBuff;
		if (this.hasVersion())
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
		}
		else
		{
			tmpBuff = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.2");
		}
		if (tmpBuff != null && tmpBuff.itemType == ASN1ItemType.SEQUENCE)
		{
			return X509File.nameGetCN(this.reader, tmpBuff.dataOfst, tmpBuff.endOfst);
		}
		else
		{
			return null;
		}
	}

	getThisUpdate()
	{
		let itemPDU;
		if (this.hasVersion())
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4");
		}
		else
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.3");
		}
		if (itemPDU != null && (itemPDU.itemType == ASN1ItemType.UTCTIME || itemPDU.itemType == ASN1ItemType.GENERALIZEDTIME))
		{
			return ASN1Util.pduParseUTCTimeCont(this.reader, itemPDU.dataOfst, itemPDU.endOfst);
		}
		else
		{
			return null;
		}
	}

	getNextUpdate()
	{
		let itemPDU;
		if (this.hasVersion())
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.5");
		}
		else
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4");
		}
		if (itemPDU != null && (itemPDU.itemType == ASN1ItemType.UTCTIME || itemPDU.itemType == ASN1ItemType.GENERALIZEDTIME))
		{
			return ASN1Util.pduParseUTCTimeCont(this.reader, itemPDU.dataOfst, itemPDU.endOfst);
		}
		else
		{
			return null;
		}
	}

	/**
	 * @param {{ getSerialNumber: () => any; }} cert
	 */
	isRevoked(cert)
	{
		let sn = cert.getSerialNumber();
		if (sn == null)
			return true;
		let itemPDU;
		let i;
		if (this.hasVersion())
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.5");
			i = 5;
		}
		else
		{
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1.4");
			i = 4;
		}
		if (itemPDU == null)
		{
			return false;
		}
		if (itemPDU.itemType != ASN1ItemType.SEQUENCE)
		{
			i++;
			itemPDU = ASN1Util.pduGetItem(this.reader, 0, this.reader.getLength(), "1.1."+i);
			if (itemPDU == null || itemPDU.itemType != ASN1ItemType.SEQUENCE)
			{
				return false;
			}
		}
		i = 0;
		while (true)
		{
			let subitemPDU;
			subitemPDU = ASN1Util.pduGetItem(this.reader, itemPDU.dataOfst, itemPDU.endOfst, (++i)+".1");
			if (subitemPDU == null)
				break;
			if (subitemPDU.itemType == ASN1ItemType.INTEGER && data.arrayBufferEquals(this.reader.getArrayBuffer(subitemPDU.dataOfst, subitemPDU.contLen), sn))
			{
				return true;
			}
		}
		return false;
	}
}

/**
 * @param {string} algType
 */
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

/**
 * @param {any} fileType
 */
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

/**
 * @param {number} keyType
 */
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

/**
 * @param {any} keyType
 */
export function keyTypeGetOID(keyType)
{
	switch (keyType)
	{
	case KeyType.RSA:
		return "1.2.840.113549.1.1.1";
	case KeyType.DSA:
		return "1.2.840.10040.4.1";
	case KeyType.ECDSA:
		return "1.2.840.10045.2.1";
	case KeyType.ED25519:
		return "1.3.101.112";
	case KeyType.ECPublic:
		return "1.2.840.10045.2.1";
	case KeyType.RSAPublic:
	case KeyType.Unknown:
	default:
		return "1.2.840.113549.1.1.1";
	}
}

/**
 * @param {number} ecName
 */
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

/**
 * @param {number} ecName
 */
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

/**
 * @param {ArrayBuffer | Iterable<number>} buff
 */
export function ecNameFromOID(buff)
{
	let arr;
	if (buff instanceof ArrayBuffer)
		arr = new Uint8Array(buff);
	else
		arr = new Uint8Array(buff);
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

/**
 * @param {Uint8Array | number[]} oid
 */
export function hashTypeFromOID(oid)
{
	if (ASN1Util.oidEqualsText(oid, "2.16.840.1.101.3.4.2.1"))
	{
		return hash.HashType.SHA256;
	}
	else if (ASN1Util.oidEqualsText(oid, "2.16.840.1.101.3.4.2.2"))
	{
		return hash.HashType.SHA384;
	}
	else if (ASN1Util.oidEqualsText(oid, "2.16.840.1.101.3.4.2.3"))
	{
		return hash.HashType.SHA512;
	}
	else if (ASN1Util.oidEqualsText(oid, "2.16.840.1.101.3.4.2.4"))
	{
		return hash.HashType.SHA224;
	}
	else if (ASN1Util.oidEqualsText(oid, "1.3.14.3.2.26"))
	{
		return hash.HashType.SHA1;
	}
	return hash.HashType.Unknown;
}
