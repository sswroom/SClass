import * as data from "./data";

export enum ASN1Type
{
	X509
}

export enum X509FileType
{
	Cert,
	Key,
	CertRequest,
	PrivateKey,
	PublicKey,
	PKCS7,
	PKCS12,
	CRL,
	FileList
}

export enum KeyType
{
	Unknown,
	RSA,
	DSA,
	ECDSA,
	ED25519,
	RSAPublic,
	ECPublic
}

export enum CertValidStatus
{
	Valid,
	SelfSigned,
	SignatureInvalid,
	Revoked,
	FileFormatInvalid,
	UnknownIssuer,
	Expired,
	UnsupportedAlgorithm
}

export enum AlgType
{
	Unknown,
	MD2WithRSAEncryption,
	MD5WithRSAEncryption,
	SHA1WithRSAEncryption,
	SHA256WithRSAEncryption,
	SHA384WithRSAEncryption,
	SHA512WithRSAEncryption,
	SHA224WithRSAEncryption,
	ECDSAWithSHA256,
	ECDSAWithSHA384
}

export enum ECName
{
	Unknown,
	secp256r1,
	secp384r1,
	secp521r1
};

export enum ASN1ItemType
{
	UNKNOWN,
	BOOLEAN,
	INTEGER,
	BIT_STRING,
	OCTET_STRING,
	NULL,
	OID,
	ENUMERATED,
	UTF8STRING,
	NUMERICSTRING,
	PRINTABLESTRING,
	T61STRING,
	VIDEOTEXSTRING,
	IA5STRING,
	UTCTIME,
	GENERALIZEDTIME,
	UNIVERSALSTRING,
	BMPSTRING,
	SEQUENCE,
	SET,
	CHOICE_0,
	CHOICE_1,
	CHOICE_2,
	CHOICE_3,
	CHOICE_4,
	CHOICE_5,
	CHOICE_6,
	CHOICE_7,
	CHOICE_8,
	CONTEXT_SPECIFIC_0,
	CONTEXT_SPECIFIC_1,
	CONTEXT_SPECIFIC_2,
	CONTEXT_SPECIFIC_3,
	CONTEXT_SPECIFIC_4
}

export enum RuleCond
{
	Any,
	TypeIsItemType,
	TypeIsTime,
	TypeIsString,
	TypeIsOpt,
	RepeatIfTypeIs,
	LastOIDAndTypeIs,
	AllNotMatch
};

declare class PDULenInfo
{
	nextOfst: number;
	pduLen: number;
}

declare class PDUValueInfo<ValType>
{
	nextOfst: number;
	val: ValType;
}

declare class PDUInfo
{
	rawOfst: number;
	hdrLen: number;
	contLen: number;
	itemType: ASN1ItemType;
}

export class ASN1Util
{
	static pduParseLen(reader: data.ByteReader, ofst: number, endOfst: number): PDULenInfo | null;

//	static const UInt8 *PDUParseSeq(const UInt8 *pdu, const UInt8 *pduEnd, UInt8 *type, const UInt8 **seqEnd);
	static pduParseUInt32(reader: data.ByteReader, ofst: number, endOfst: number): PDUValueInfo<number>;
	static pduParseString(reader: data.ByteReader, ofst: number, endOfst: number): PDUValueInfo<string>;
	static pduParseChoice(reader: data.ByteReader, ofst: number, endOfst: number): PDUValueInfo<number>;

	static pduParseUTCTimeCont(reader: data.ByteReader, startOfst: number, endOfst: number): data.Timestamp;
	static pduToString(reader: data.ByteReader, startOfst: number, endOfst: number, outLines: string[], level: number, names?: ASN1Names): number;
	static pduDSizeEnd(reader: data.ByteReader, startOfst: number, endOfst: number): number | null;
	static pduGetItem(reader: data.ByteReader, startOfst: number, endOfst: number, path: string): PDUInfo;
	static pduCountItem(reader: data.ByteReader, startOfst: number, endOfst: number, path?: string): number;
	static pduIsValid(reader: data.ByteReader, startOfst: number, endOfst: number): boolean;
//	static void PDUAnalyse(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR buff, UOSInt pduOfst, UOSInt pduEndOfst, Net::ASN1Names *names);

	static oidCompare(oid1: Uint8Array | number[], oid2: Uint8Array | number[]): number;
//	static Bool OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len);
	static oidEqualsText(oidPDU: Uint8Array | number[], oidText: string): boolean;
//	static void OIDToString(const UInt8 *pdu, UOSInt pduSize, NotNullPtr<Text::StringBuilderUTF8> sb);
//	static UOSInt OIDCalcPDUSize(const UTF8Char *oidText, UOSInt oidTextLen);
	static oidText2PDU(oidText: string): number[];

//	static void OIDToCPPCode(const UInt8 *oid, UOSInt oidLen, const UTF8Char *objectName, UOSInt nameLen, NotNullPtr<Text::StringBuilderUTF8> sb);

	static booleanToString(reader: data.ByteReader, ofst: number, len: number): string;
	static integerToString(reader: data.ByteReader, ofst: number, len: number): string;
	static utcTimeToString(reader: data.ByteReader, ofst: number, len: number): string;
	static itemTypeGetName(itemType: ASN1ItemType): string;
	static str2Digit(reader: data.ByteReader, ofst: number): number;
}

declare class NameRule
{
	cond: RuleCond;
	itemType: ASN1ItemType;
	condParam: string | null;
	name: string;
	contentFunc?: (names: ASN1Names)=>void;
	enumVals?: string[];
}

declare class RuleContainer
{
	rules: NameRule[];
	parent: RuleContainer;
	
	constructor();
}

declare class SignedInfo
{
	signature: ArrayBuffer;
	payload: ArrayBuffer;
	algType: AlgType;
}

export class ASN1Names
{
	private addRule(rule: RuleCond): void;

	constructor();

	readBegin(): void;
	readName(itemType: ASN1ItemType, len: number, reader: data.ByteReader, ofst: number): string;
	readNameNoDef(itemType: ASN1ItemType, len: number, reader: data.ByteReader, ofst: number): string | null;
	readContainer(): void;
	readContainerEnd(): void;

	anyCond(): ASN1Names;
	typeIs(itemType: ASN1ItemType): ASN1Names;
	typeIsTime(): ASN1Names;
	typeIsString(): ASN1Names;
	typeIsOpt(index: number): ASN1Names;
	repeatIfTypeIs(itemType: ASN1ItemType): ASN1Names;
	lastOIDAndTypeIs(oidText: string, itemType: ASN1ItemType): ASN1Names;
	allNotMatch(): ASN1Names;

	container(name: string, contFunc: (name: ASN1Names)=>void): ASN1Names;
	nextValue(name: string): ASN1Names;
	enum(name: string, enums: string[]): ASN1Names;

	setCertificate(): ASN1Names; //PKIX1Explicit88
	setRSAPublicKey(): ASN1Names; //PKCS-1
	setRSAPrivateKey(): ASN1Names; //PKCS-1
	setPKCS7ContentInfo(): ASN1Names; //PKCS-7
	setCertificationRequest(): ASN1Names; //PKCS-10
	setPFX(): ASN1Names; //PKCS-12
}


class General
{
	static pbeParam(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("salt");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("iterations");
	}
	
	static extendedValidationCertificates(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.OCTET_STRING).nextValue("signedCertTimestamp");
	}
	
	static attributeOutlookExpress(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("issuerAndSerialNumber", PKCS7.issuerAndSerialNumberCont);
	}
}

class InformationFramework
{
	static attributeCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("attrId");
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.1", ASN1ItemType.SET).container("attrValues", AttributeEmailAddress);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.2", ASN1ItemType.SET).container("attrValues", AttributeUnstructuredName);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.3", ASN1ItemType.SET).container("attrValues", PKCS9.attributeContentType);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.4", ASN1ItemType.SET).container("attrValues", PKCS9.attributeMessageDigest);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.5", ASN1ItemType.SET).container("attrValues", PKCS9.attributeSigningTime);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.6", ASN1ItemType.SET).container("attrValues", AttributeCounterSignature);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.7", ASN1ItemType.SET).container("attrValues", AttributeChallengePassword);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.8", ASN1ItemType.SET).container("attrValues", AttributeUnstructuredAddress);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.9", ASN1ItemType.SET).container("attrValues", AttributeExtendedCertificateAttributes);
	
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.15", ASN1ItemType.SET).container("attrValues", PKCS9.attributeSMIMECapabilities);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.16.2.11", ASN1ItemType.SET).container("attrValues", RFC8551.smimeEncryptionKeyPreference);
	
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.20", ASN1ItemType.SET).container("attrValues", PKCS9.attributeFriendlyName);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.21", ASN1ItemType.SET).container("attrValues", PKCS9.attributeLocalKeyId);
		names.lastOIDAndTypeIs("1.3.6.1.4.1.311.16.4", ASN1ItemType.SET).container("attrValues", General.attributeOutlookExpress);
		
		names.typeIs(ASN1ItemType.SET).nextValue("attrValues");
	}
}

class PKCS1
{
	static rsaPublicKey(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("RSAPublicKey", PKCS1.rsaPublicKeyCont);
	}

	static rsaPublicKeyCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("modulus");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("publicExponent");
	}

	static rsaPrivateKey(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("RSAPrivateKey", PKCS1.rsaPrivateKeyCont);
	}

	static rsaPrivateKeyCont(names)
	{
		let version = ["two-prime", "multi"];
		names.typeIs(ASN1ItemType.INTEGER).enum("Version", version);
		names.typeIs(ASN1ItemType.INTEGER).nextValue("modulus");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("publicExponent");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("privateExponent");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("prime1");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("prime2");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("exponent1");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("exponent2");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("coefficient");
		names.typeIs(ASN1ItemType.SEQUENCE).container("otherPrimeInfos", PKCS1.otherPrimeInfos);
	}

	static otherPrimeInfos(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("prime");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("exponent");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("coefficient");
	}

	static addDigestInfo(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKCS1.digestInfoCont);
	}

	static digestInfoCont(names)
	{
		PKIX1Explicit88.addAlgorithmIdentifier(names, "digestAlgorithm");
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("digest");
	}

}

class PKCS7
{
	static addContentInfo(names: ASN1Names, name: string): void;
	static contentInfo(names: ASN1Names): void;
	static contentInfoCont(names: ASN1Names): void;
	static data(names: ASN1Names): void;
	static signedData(names: ASN1Names): void;
	static signedDataCont(names: ASN1Names): void;
	static digestAlgorithmIdentifiers(names: ASN1Names): void;
	static certificateSet(names: ASN1Names): void;
	static certificateRevocationLists(names: ASN1Names): void;
	static signerInfos(names: ASN1Names): void;
	static signerInfoCont(names: ASN1Names): void;
	static issuerAndSerialNumberCont(names: ASN1Names): void;
	static addDigestInfo(names: ASN1Names, name: string): void;
	static digestInfoCont(names: ASN1Names): void;
	static envelopedData(names: ASN1Names): void;
	static envelopedDataCont(names: ASN1Names): void;
	static originatorInfoCont(names: ASN1Names): void;
	static recipientInfos(names: ASN1Names): void;
	static keyTransportRecipientInfoCont(names: ASN1Names): void;
	static signedAndEnvelopedData(names: ASN1Names): void;
	static signedAndEnvelopedDataCont(names: ASN1Names): void;
	static digestedData(names: ASN1Names): void;
	static digestedDataCont(names: ASN1Names): void;
	static encryptedData(names: ASN1Names): void;
	static encryptedDataCont(names: ASN1Names): void;
	static encryptedContentInfoCont(names: ASN1Names): void;
	static authenticatedData(names: ASN1Names): void;
}

class PKCS8
{
	static privateKeyInfo(names: ASN1Names): void;
	static privateKeyInfoCont(names: ASN1Names): void;
	static encryptedPrivateKeyInfo(names: ASN1Names): void;
	static encryptedPrivateKeyInfoCont(names: ASN1Names): void;
}

class PKCS9
{
	static attributeContentType(names: ASN1Names): void;
	static attributeMessageDigest(names: ASN1Names): void;
	static attributeSigningTime(names: ASN1Names): void;
	static attributeFriendlyName(names: ASN1Names): void;
	static attributeSMIMECapabilities(names: ASN1Names): void;
	static attributeLocalKeyId(names: ASN1Names): void;
	static smimeCapabilitiesCont(names: ASN1Names): void;
	static smimeCapabilityCont(names: ASN1Names): void;
}

class PKCS10
{
	static addCertificationRequestInfo(names: ASN1Names, name: string): void;
	static certificationRequestInfoCont(names: ASN1Names): void;
	static attributesCont(names: ASN1Names): void;
	static certificationRequest(names: ASN1Names): void;
	static certificationRequestCont(names: ASN1Names): void;
}

class PKCS12
{
	static pfx(names: ASN1Names): void;
	static pfxCont(names: ASN1Names): void;
	static addMacData(names: ASN1Names, name: string): void;
	static macDataCont(names: ASN1Names): void;
	static authenticatedSafeContentInfoCont(names: ASN1Names): void;
	static authenticatedSafeData(names: ASN1Names): void;
	static authenticatedSafeEnvelopedData(names: ASN1Names): void;
	static authenticatedSafeEncryptedData(names: ASN1Names): void;
	static authenticatedSafe(names: ASN1Names): void;
	static authSafeContentInfo(names: ASN1Names): void;
	static authSafeContentInfoCont(names: ASN1Names): void;
	static safeContentsData(names: ASN1Names): void;
	static safeContents(names: ASN1Names): void;
	static safeContentsCont(names: ASN1Names): void;
	static safeBagCont(names: ASN1Names): void;
	static certBag(names: ASN1Names): void;
	static certBagCont(names: ASN1Names): void;
	static x509Certificate(names: ASN1Names): void;
	static sdsiCertificate(names: ASN1Names): void;
	static crlBag(names: ASN1Names): void;
	static crlBagCont(names: ASN1Names): void;
	static x509CRL(names: ASN1Names): void;
	static secretBag(names: ASN1Names): void;
	static secretBagCont(names: ASN1Names): void;
	static pkcs12Attributes(names: ASN1Names): void;
}

class PKIX1Explicit88
{
	static addAttributeTypeAndValue(names: ASN1Names, name: string): void;
	static attributeTypeAndValueCont(names: ASN1Names): void;
	static addName(names: ASN1Names, name: string): void;
	static name(names: ASN1Names): void;
	static rdnSequenceCont(names: ASN1Names): void;
	static relativeDistinguishedName(names: ASN1Names): void;
	static relativeDistinguishedNameCont(names: ASN1Names): void;
	static certificate(names: ASN1Names): void;
	static certificateCont(names: ASN1Names): void;
	static addTBSCertificate(names: ASN1Names, name: string): void;
	static tbsCertificateCont(names: ASN1Names): void;
	static version(names: ASN1Names): void;
	static addValidity(names: ASN1Names, name: string): void;
	static validityCont(names: ASN1Names): void;
	static addSubjectPublicKeyInfo(names: ASN1Names, name: string): void;
	static subjectPublicKeyInfoCont(names: ASN1Names): void;
	static addExtensions(names: ASN1Names, name: string): void;
	static extensions(names: ASN1Names): void;
	static extensionsCont(names: ASN1Names): void;
	static extensionCont(names: ASN1Names): void;
	static certificateList(names: ASN1Names): void;
	static certificateListCont(names: ASN1Names): void;
	static addTBSCertList(names: ASN1Names, name: string): void;
	static tbsCertListCont(names: ASN1Names): void;
	static revokedCertificates(names: ASN1Names): void;
	static revokedCertificateCont(names: ASN1Names): void;
	static addAlgorithmIdentifier(names: ASN1Names, name: string): void;
	static algorithmIdentifierCont(names: ASN1Names): void;
}

declare class PKIX1Implicit88
{
	static authorityKeyIdentifier(names: ASN1Names): void;
	static authorityKeyIdentifierCont(names: ASN1Names): void;
	static subjectKeyIdentifier(names: ASN1Names): void;
	static keyUsage(names: ASN1Names): void;
	static certificatePolicies(names: ASN1Names): void;
	static certificatePoliciesCont(names: ASN1Names): void;
	static policyInformationCont(names: ASN1Names): void;
	static policyQualifiers(names: ASN1Names): void;
	static policyQualifierInfoCont(names: ASN1Names): void;
	static generalNames(names: ASN1Names): void;
	static generalNameCont(names: ASN1Names): void;
	static basicConstraints(names: ASN1Names): void;
	static basicConstraintsCont(names: ASN1Names): void;
	static crlDistributionPoints(names: ASN1Names): void;
	static crlDistributionPointsCont(names: ASN1Names): void;
	static distributionPointCont(names: ASN1Names): void;
	static distributionPointName(names: ASN1Names): void;
	static reasonFlags(names: ASN1Names): void;
	static extKeyUsageSyntax(names: ASN1Names): void;
	static extKeyUsageSyntaxCont(names: ASN1Names): void;
}

declare class RFC2459
{
	static authorityInfoAccessSyntax(names: ASN1Names): void;
	static authorityInfoAccessSyntaxCont(names: ASN1Names): void;
	static accessDescriptionCont(names: ASN1Names): void;
}

declare class RFC8551
{
	static smimeEncryptionKeyPreference(names: ASN1Names): void;
}

export class ASN1PDUBuilder
{
	seqOffset: number[];
	buff: number[];

	constructor();
	
	beginOther(type: ASN1ItemType): void;
	beginSequence(): void;
	beginSet(): void;
	beginContentSpecific(n: number): void;
	endLevel(): void;
	endAll(): void;
	appendBool(v: boolean): void;
	appendInt32(v: number): void;
	appendBitString(bitLeft: number, buff: ArrayBuffer): void;
	appendOctetString(buff: ArrayBuffer | string): void;
	appendNull(): void;
	appendOID(buff: ArrayBuffer): void;
	appendOIDString(oidStr: string): void;
	appendChoice(v: number): void;
	appendPrintableString(s: string): void;
	appendUTF8String(s: string): void;
	appendIA5String(s: string): void;
	appendUTCTime(t: data.Timestamp): void;
	appendOther(type: ASN1ItemType, buff: ArrayBuffer): void;
	appendContentSpecific(n: number, buff: ArrayBuffer): void;
	appendSequence(buff: ArrayBuffer): void;
	appendInteger(buff: ArrayBuffer): void;

	getArrayBuffer(): ArrayBuffer;
	appendTypeLen(type: ASN1ItemType, len: number): void;
	appendArrayBuffer(buff: ArrayBuffer): void;
};

export abstract class ASN1Data extends data.ParsedObject
{
	reader: data.ByteReader;

	constructor(sourceName: string, objType: string, buff: ArrayBuffer);

	abstract getASN1Type() : ASN1Type;
	abstract clone(): ASN1Data;
	abstract toString(): string;
	abstract createNames(): ASN1Names;

	toASN1String(): string | null;
	getASN1Buff(): data.ByteReader;

	static appendInteger(arr: string[], reader: data.ByteReader, ofst: number, len: number): void;
}

export abstract class X509File extends ASN1Data
{
	constructor(sourceName: string, objType: string, buff: ArrayBuffer);

	getASN1Type(): ASN1Type;
	abstract getFileType(): X509FileType;
	abstract toShortName(): string;

	getCertCount(): number;
	getCertName(index: number): string;
	getNewCert(index: number): X509Cert | null;
	abstract isValid(): CertValidStatus;

	toShortString(): string;
	isSignatureKey(key: X509Key): boolean;
	getSignedInfo(): SignedInfo | null;

	static keyGetLeng(reader: data.ByteReader, startOfst: number, endOfst: number, keyType: KeyType): number;
}

export class X509Key extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer, keyType: KeyType);
	getFileType(): X509FileType;
	toShortName(): string;
	isValid(): CertValidStatus;
	clone(): X509Key;
	toString(): string;
	createNames(): ASN1Names;
	getKeyType(): KeyType;
	getKeySizeBits(): number;
	isPrivateKey() : boolean;

	createPublicKey(): X509Key | null;
	getKeyId(): ArrayBuffer | null;

	getRSAModulus(): ArrayBuffer | null;
	getRSAPublicExponent(): ArrayBuffer | null;
	getRSAPrivateExponent(): ArrayBuffer | null;
	getRSAPrime1(): ArrayByffer | null;
	getRSAPrime2(): ArrayBuffer | null;
	getRSAExponent1(): ArrayBuffer | null;
	getRSAExponent2(): ArrayBuffer | null;
	getRSACoefficient(): ArrayBuffer | null;

	getECPrivate(): ArrayBuffer | null;
	getECPublic(): ArrayBuffer | null;
	getECName(): ECName;
}

export function fileTypeGetName(fileType: X509FileType): string;
export function keyTypeGetName(keyType: KeyType): string;
export function ecNameGetName(ecName: ECName): string;
export function ecNameGetOID(ecName: ECName): string;
export function ecNameFromOID(buff: ArrayBuffer): ECName;
