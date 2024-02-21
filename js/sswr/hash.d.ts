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