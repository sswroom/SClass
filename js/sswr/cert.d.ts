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

declare class SignedInfo
{
	signature: ArrayBuffer;
	payload: ArrayBuffer;
	algType: AlgType;
}

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
	getCertName(index: number): string | null;
	getNewCert(index: number): X509Cert | null;
	abstract isValid(): CertValidStatus;

	toShortString(): string;
	isSignatureKey(key: X509Key): boolean;
	getSignedInfo(): SignedInfo | null;

	static nameGetByOID(reader: data.ByteReader, startOfst: number, endOfst: number, oidText: string): string | null;
	static nameGetCN(reader: data.ByteReader, startOfst: number, endOfst: number): string | null;

	static keyGetLeng(reader: data.ByteReader, startOfst: number, endOfst: number, keyType: KeyType): number;
}

export class X509Cert extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getSubjectCN(): string | null;
	getIssuerCN(): string | null;
	setDefaultSourceName(): void;
	
	getFileType(): X509FileType;
	toShortName(): string;

	getCertCount(): number;
	getCertName(index: number): string | null;
	getNewCert(index: number): X509Cert | null;
	isValid(): CertValidStatus;

	clone(): ASN1Data;
	createX509Cert(): X509Cert;
	toString(): string;
	createNames(): ASN1Names;

	getIssuerNames(): CertNames | null;
	getSubjNames(): CertNames | null;
	getExtensions(): CertExtensions | null;
	getNewPublicKey(): X509Key | null;
	getKeyId(): ArrayBuffer | null;
	getNotBefore(): data.Timestamp | null;
	getNotAfter(): data.Timestamp | null;
	domainValid(domain: string): boolean;
	isSelfSigned(): boolean;
	getCRLDistributionPoints(): string[];

	getIssuerNamesSeq(): ArrayBuffer | null;
	getSerialNumber(): ArrayBuffer | null;
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
