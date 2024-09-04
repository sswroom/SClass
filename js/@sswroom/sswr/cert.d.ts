import {ASN1Names} from "./certutil";
import * as data from "./data";
import * as hash from "./hash";

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
}

export enum ContentDataType
{
	Unknown,
	AuthenticatedSafe
}

declare class SignedInfo
{
	signature: ArrayBuffer;
	payload: ArrayBuffer;
	algType: AlgType;
}

declare class CertNames
{
	countryName: string;
	stateOrProvinceName: string;
	localityName: string;
	organizationName: string;
	organizationUnitName: string;
	commonName: string;
	emailAddress: string;
}

declare class CertExtensions
{
	subjectAltName?: string[];
	issuerAltName?: string[];
	subjKeyId?: ArrayBuffer;
	authKeyId?: ArrayBuffer;
	digitalSign: boolean;
	caCert: boolean;
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
	abstract isValid(): Promise<CertValidStatus>;

	toShortString(): string;
	isSignatureKey(key: X509Key): boolean;
	getSignedInfo(): SignedInfo | null;

	static isSigned(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // AuthenticationFramework
	static appendSigned(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): void; // AuthenticationFramework
	static isTBSCertificate(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // AuthenticationFramework
	static appendTBSCertificate(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): void; // AuthenticationFramework
	static isCertificate(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // AuthenticationFramework
	static appendCertificate(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): void; // AuthenticationFramework
	static isTBSCertList(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // RFC3280
	static appendTBSCertList(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): void; // RFC3280
	static isCertificateList(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // RFC3280
	static appendCertificateList(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string): void; // RFC3280
	static isPrivateKeyInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // PKCS-8
	static appendPrivateKeyInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[]): void; // PKCS-8
	static isCertificateRequestInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // PKCS-10
	static appendCertificateRequestInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[]): void; // PKCS-10
	static isCertificateRequest(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // PKCS-10
	static appendCertificateRequest(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[]): void; // PKCS-8
	static isPublicKeyInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // AuthenticationFramework
	static appendPublicKeyInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[]): void; // AuthenticationFramework
	static isContentInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // RFC2315 / PKCS7
	static appendContentInfo(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null, dataType: ContentDataType): void; // RFC2315 / PKCS7
	static isPFX(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null): boolean; // PKCS12
	static appendPFX(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): void; // PKCS12

	static appendVersion(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[]): void; // AuthenticationFramework
	static appendAlgorithmIdentifier(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null, pubKey: boolean): KeyType; // PKCS-5
	static appendValidity(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // AuthenticationFramework
	static appendSubjectPublicKeyInfo(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // AuthenticationFramework
	static appendName(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // InformationFramework
	static appendRelativeDistinguishedName(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // InformationFramework
	static appendAttributeTypeAndDistinguishedValue(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // InformationFramework
	static appendCRLExtensions(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendCRLExtension(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendMSOSVersion(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendMSRequestClientInfo(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendMSEnrollmentCSPProvider(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendGeneralNames(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendGeneralName(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): boolean;
	static appendDistributionPoint(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): boolean;
	static appendDistributionPointName(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void;
	static appendPolicyInformation(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): boolean;
	static appendPKCS7SignedData(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315
	static appendPKCS7DigestAlgorithmIdentifiers(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315
	static appendPKCS7SignerInfos(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315
	static appendPKCS7SignerInfo(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315
	static appendIssuerAndSerialNumber(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315
	static appendPKCS7Attributes(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315
	static appendMacData(reader: data.ByteReader, startOfst: number, endOfst: number, path: string | null, sb: string[], varName: string | null): boolean; // PKCS12
	static appendDigestInfo(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; // RFC2315 / PKCS7
	static appendData(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null, dataType: ContentDataType): void;
	static appendEncryptedData(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null, dataType: ContentDataType): void;
	static appendAuthenticatedSafe(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null): void; //PKCS12
	static appendEncryptedContentInfo(reader: data.ByteReader, startOfst: number, endOfst: number, sb: string[], varName: string | null, dataType: ContentDataType): void;

	static nameGetByOID(reader: data.ByteReader, startOfst: number, endOfst: number, oidText: string): string | null;
	static nameGetCN(reader: data.ByteReader, startOfst: number, endOfst: number): string | null;
	static namesGet(reader: data.ByteReader, startOfst: number, endOfst: number): CertNames;
	static extensionsGet(reader: data.ByteReader, startOfst: number, endOfst: number): CertExtensions;
	static extensionsGetCRLDistributionPoints(reader: data.ByteReader, startOfst: number, endOfst: number): string[];
	static distributionPointAdd(reader: data.ByteReader, startOfst: number, endOfst: number, distPoints: string[]): boolean;
	static publicKeyGetNew(reader: data.ByteReader, startOfst: number, endOfst: number): X509Key | null;

	static keyGetLeng(reader: data.ByteReader, startOfst: number, endOfst: number, keyType: KeyType): number;
	static keyTypeFromOID(oid: ArrayBuffer, pubKey: boolean): KeyType;
	static algorithmIdentifierGet(reader: data.ByteReader, startOfst: number, endOfst: number): AlgType;
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
	isValid(): Promise<CertValidStatus>;

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

export class X509CertReq extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getFileType(): X509FileType;
	toShortName(): string;
	isValid(): Promise<CertValidStatus>;
	
	clone(): X509CertReq;
	toString(): string;
	createNames(): ASN1Names;

	getNames(): CertNames;
	getExtensions(): CertExtensions;
	getNewPublicKey(): X509Key;
	getKeyId(): ArrayBuffer | null; //20 bytes
}

export class X509Key extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer, keyType: KeyType);
	getFileType(): X509FileType;
	toShortName(): string;
	isValid(): Promise<CertValidStatus>;
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
	getRSAPrime1(): ArrayBuffer | null;
	getRSAPrime2(): ArrayBuffer | null;
	getRSAExponent1(): ArrayBuffer | null;
	getRSAExponent2(): ArrayBuffer | null;
	getRSACoefficient(): ArrayBuffer | null;

	getECPrivate(): ArrayBuffer | null;
	getECPublic(): ArrayBuffer | null;
	getECName(): ECName;
	signatureVerify(hashType: hash.HashType, payload: ArrayBuffer, signature: ArrayBuffer): Promise<boolean>;
	static fromECPublicKey(buff: ArrayBuffer, paramOID: ArrayBuffer): X509Key;
}

export class X509PrivKey extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getFileType(): X509FileType;
	toShortName(): string;
	isValid(): Promise<CertValidStatus>;

	clone(): X509PrivKey;
	toString(): string;
	createNames(): ASN1Names;
	
	getKeyType(): KeyType;
	createKey(): X509Key;

	static createFromKeyBuff(keyType: KeyType, buff: ArrayBuffer, sourceName: string): X509PrivKey;
	static createFromKey(key: X509Key): X509PrivKey;
}

export class X509PubKey extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getFileType(): X509FileType;
	toShortName(): string;
	isValid(): Promise<CertValidStatus>;
	clone(): X509PubKey;
	toString(): string;
	createNames(): ASN1Names;
	
	createKey(): X509Key;

	static createFromKeyBuff(keyType: KeyType, buff: ArrayBuffer, sourceName: string): X509PubKey;
	static createFromKey(key: X509Key): X509PubKey;
}

export class X509PKCS7 extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getFileType(): X509FileType;
	toShortName(): string;
	
	getCertCount(): number;
	getCertName(index: number): string;
	getNewCert(index: number): X509Cert;
	isValid(): Promise<CertValidStatus>;

	clone(): X509PKCS7;
	toString(): string;
	createNames(): ASN1Names;

	isSignData(): boolean;
	getDigestType(): hash.HashType;
	getMessageDigest(): ArrayBuffer;
	getEncryptedDigest(): ArrayBuffer;
}

export class X509PKCS12 extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getFileType(): X509FileType;
	toShortName(): string;
	getCertCount(): number;
	getCertName(index: number): string;
	getNewCert(index: number): X509Cert;
	isValid(): Promise<CertValidStatus>;

	clone(): X509PKCS12;
	toString(): string;
	createNames(): ASN1Names;
}

export class X509FileList extends X509File
{
	fileList: X509File[];

	constructor(sourceName: string, cert: X509Cert);

	getFileType(): X509FileType;
	toShortName(): string;

	getCertCount(): number;
	getCertName(index: number): string | null;
	getNewCert(index: number): X509Cert | null;
	isValid(): Promise<CertValidStatus>;

	clone(): X509FileList;
	createX509Cert(): X509Cert;
	toString(): string;
	createNames(): ASN1Names;

	addFile(file: X509File): void;
	getFileCount(): number;
	getFile(index: number): X509File;
	setDefaultSourceName(): void;
}

export class X509CRL extends X509File
{
	constructor(sourceName: string, buff: ArrayBuffer);

	getFileType(): X509FileType;
	toShortName(): string;
	isValid(): Promise<CertValidStatus>;
	
	clone(): X509CRL;
	toString(): string;
	createNames(): ASN1Names;

	hasVersion(): boolean;
	getIssuerCN(): string | null;
	getThisUpdate(): data.Timestamp | null;
	getNextUpdate(): data.Timestamp | null;
	isRevoked(cert: X509Cert): boolean;
}

export function algTypeGetHash(algType: AlgType): hash.HashType;
export function fileTypeGetName(fileType: X509FileType): string;
export function keyTypeGetName(keyType: KeyType): string;
export function keyTypeGetOID(keyType: KeyType): string;
export function ecNameGetName(ecName: ECName): string;
export function ecNameGetOID(ecName: ECName): string;
export function ecNameFromOID(buff: ArrayBuffer): ECName;
export function hashTypeFromOID(oid: Uint8Array): hash.HashType;
