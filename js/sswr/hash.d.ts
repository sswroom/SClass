export enum HashType
{
	Unknown,
	// Primary Algorithm
	Adler32,
	CRC16,
	CRC16R,
	CRC32,
	CRC32R_IEEE,
	CRC32C,
	DJB2,
	DJB2a,
	FNV1,
	FNV1a,
	MD5,
	RIPEMD128,
	RIPEMD160,
	SDBM,
	SHA1,
	Excel,
	SHA224,
	SHA256,
	SHA384,
	SHA512,
	MD4,

	// Compound Algorithm
	SHA1_SHA1
}

export abstract class Hash
{
	abstract getName(): string;
	abstract clone(): Hash;
	abstract clear(): void;
	abstract calc(buff: ArrayBuffer): void;
	abstract getValue(): ArrayBuffer;
	abstract getBlockSize(): number;
}

export class SHA1 extends Hash
{
	intermediateHash: number[];
	messageLength: number;
	messageBlockIndex: number;
	messageBlock: number[];

	constructor();
	getName(): string;
	clone(): Hash;
	clear(): void;
	calc(buff: ArrayBuffer): void;
	getValue(): ArrayBuffer;
	getBlockSize(): number;

	static calcBlock(intermediateHash: number[], messageBlock: ArrayBuffer): void;
}

export class MD5 extends Hash
{
	messageLength: number;
	h: number[];
	buff: number[];
	buffSize: number;

	constructor();
	getName(): string;
	clone(): Hash;
	clear(): void;
	calc(buff: ArrayBuffer): void;
	getValue(): ArrayBuffer;
	getBlockSize(): number;

	static calcBlock(hVals: number[], block: DataView): void;
}