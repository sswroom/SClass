#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/AESBase.h"
#include "Data/ByteTool.h"

Crypto::Encrypt::AES256::AES256(UnsafeArray<const UInt8> key) : Crypto::Encrypt::BlockCipher(16)
{
	this->SetKey(key);
}

Crypto::Encrypt::AES256::~AES256()
{

}

UOSInt Crypto::Encrypt::AES256::EncryptBlock(UnsafeArray<const UInt8> inBlock, UInt8 *outBlock) const
{
	UInt32 s0;
	UInt32 s1;
	UInt32 s2;
	UInt32 s3;
	UInt32 t0;
	UInt32 t1;
	UInt32 t2;
	UInt32 t3;
	s0 = ReadMUInt32(&inBlock[ 0]) ^ this->encRK[0];
	s1 = ReadMUInt32(&inBlock[ 4]) ^ this->encRK[1];
	s2 = ReadMUInt32(&inBlock[ 8]) ^ this->encRK[2];
	s3 = ReadMUInt32(&inBlock[12]) ^ this->encRK[3];
	/* round 1: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[ 4];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[ 5];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[ 6];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[ 7];
	/* round 2: */
	s0 = AESBase_Te0[t0 >> 24] ^ AESBase_Te1[(t1 >> 16) & 0xff] ^ AESBase_Te2[(t2 >>  8) & 0xff] ^ AESBase_Te3[t3 & 0xff] ^ this->encRK[ 8];
	s1 = AESBase_Te0[t1 >> 24] ^ AESBase_Te1[(t2 >> 16) & 0xff] ^ AESBase_Te2[(t3 >>  8) & 0xff] ^ AESBase_Te3[t0 & 0xff] ^ this->encRK[ 9];
	s2 = AESBase_Te0[t2 >> 24] ^ AESBase_Te1[(t3 >> 16) & 0xff] ^ AESBase_Te2[(t0 >>  8) & 0xff] ^ AESBase_Te3[t1 & 0xff] ^ this->encRK[10];
	s3 = AESBase_Te0[t3 >> 24] ^ AESBase_Te1[(t0 >> 16) & 0xff] ^ AESBase_Te2[(t1 >>  8) & 0xff] ^ AESBase_Te3[t2 & 0xff] ^ this->encRK[11];
	/* round 3: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[12];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[13];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[14];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[15];
	/* round 4: */
	s0 = AESBase_Te0[t0 >> 24] ^ AESBase_Te1[(t1 >> 16) & 0xff] ^ AESBase_Te2[(t2 >>  8) & 0xff] ^ AESBase_Te3[t3 & 0xff] ^ this->encRK[16];
	s1 = AESBase_Te0[t1 >> 24] ^ AESBase_Te1[(t2 >> 16) & 0xff] ^ AESBase_Te2[(t3 >>  8) & 0xff] ^ AESBase_Te3[t0 & 0xff] ^ this->encRK[17];
	s2 = AESBase_Te0[t2 >> 24] ^ AESBase_Te1[(t3 >> 16) & 0xff] ^ AESBase_Te2[(t0 >>  8) & 0xff] ^ AESBase_Te3[t1 & 0xff] ^ this->encRK[18];
	s3 = AESBase_Te0[t3 >> 24] ^ AESBase_Te1[(t0 >> 16) & 0xff] ^ AESBase_Te2[(t1 >>  8) & 0xff] ^ AESBase_Te3[t2 & 0xff] ^ this->encRK[19];
	/* round 5: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[20];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[21];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[22];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[23];
	/* round 6: */
	s0 = AESBase_Te0[t0 >> 24] ^ AESBase_Te1[(t1 >> 16) & 0xff] ^ AESBase_Te2[(t2 >>  8) & 0xff] ^ AESBase_Te3[t3 & 0xff] ^ this->encRK[24];
	s1 = AESBase_Te0[t1 >> 24] ^ AESBase_Te1[(t2 >> 16) & 0xff] ^ AESBase_Te2[(t3 >>  8) & 0xff] ^ AESBase_Te3[t0 & 0xff] ^ this->encRK[25];
	s2 = AESBase_Te0[t2 >> 24] ^ AESBase_Te1[(t3 >> 16) & 0xff] ^ AESBase_Te2[(t0 >>  8) & 0xff] ^ AESBase_Te3[t1 & 0xff] ^ this->encRK[26];
	s3 = AESBase_Te0[t3 >> 24] ^ AESBase_Te1[(t0 >> 16) & 0xff] ^ AESBase_Te2[(t1 >>  8) & 0xff] ^ AESBase_Te3[t2 & 0xff] ^ this->encRK[27];
	/* round 7: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[28];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[29];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[30];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[31];
	/* round 8: */
	s0 = AESBase_Te0[t0 >> 24] ^ AESBase_Te1[(t1 >> 16) & 0xff] ^ AESBase_Te2[(t2 >>  8) & 0xff] ^ AESBase_Te3[t3 & 0xff] ^ this->encRK[32];
	s1 = AESBase_Te0[t1 >> 24] ^ AESBase_Te1[(t2 >> 16) & 0xff] ^ AESBase_Te2[(t3 >>  8) & 0xff] ^ AESBase_Te3[t0 & 0xff] ^ this->encRK[33];
	s2 = AESBase_Te0[t2 >> 24] ^ AESBase_Te1[(t3 >> 16) & 0xff] ^ AESBase_Te2[(t0 >>  8) & 0xff] ^ AESBase_Te3[t1 & 0xff] ^ this->encRK[34];
	s3 = AESBase_Te0[t3 >> 24] ^ AESBase_Te1[(t0 >> 16) & 0xff] ^ AESBase_Te2[(t1 >>  8) & 0xff] ^ AESBase_Te3[t2 & 0xff] ^ this->encRK[35];
	/* round 9: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[36];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[37];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[38];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[39];
	/* round 10: */
	s0 = AESBase_Te0[t0 >> 24] ^ AESBase_Te1[(t1 >> 16) & 0xff] ^ AESBase_Te2[(t2 >>  8) & 0xff] ^ AESBase_Te3[t3 & 0xff] ^ this->encRK[40];
	s1 = AESBase_Te0[t1 >> 24] ^ AESBase_Te1[(t2 >> 16) & 0xff] ^ AESBase_Te2[(t3 >>  8) & 0xff] ^ AESBase_Te3[t0 & 0xff] ^ this->encRK[41];
	s2 = AESBase_Te0[t2 >> 24] ^ AESBase_Te1[(t3 >> 16) & 0xff] ^ AESBase_Te2[(t0 >>  8) & 0xff] ^ AESBase_Te3[t1 & 0xff] ^ this->encRK[42];
	s3 = AESBase_Te0[t3 >> 24] ^ AESBase_Te1[(t0 >> 16) & 0xff] ^ AESBase_Te2[(t1 >>  8) & 0xff] ^ AESBase_Te3[t2 & 0xff] ^ this->encRK[43];
	/* round 11: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[44];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[45];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[46];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[47];
	/* round 12: */
	s0 = AESBase_Te0[t0 >> 24] ^ AESBase_Te1[(t1 >> 16) & 0xff] ^ AESBase_Te2[(t2 >>  8) & 0xff] ^ AESBase_Te3[t3 & 0xff] ^ this->encRK[48];
	s1 = AESBase_Te0[t1 >> 24] ^ AESBase_Te1[(t2 >> 16) & 0xff] ^ AESBase_Te2[(t3 >>  8) & 0xff] ^ AESBase_Te3[t0 & 0xff] ^ this->encRK[49];
	s2 = AESBase_Te0[t2 >> 24] ^ AESBase_Te1[(t3 >> 16) & 0xff] ^ AESBase_Te2[(t0 >>  8) & 0xff] ^ AESBase_Te3[t1 & 0xff] ^ this->encRK[50];
	s3 = AESBase_Te0[t3 >> 24] ^ AESBase_Te1[(t0 >> 16) & 0xff] ^ AESBase_Te2[(t1 >>  8) & 0xff] ^ AESBase_Te3[t2 & 0xff] ^ this->encRK[51];
	/* round 13: */
	t0 = AESBase_Te0[s0 >> 24] ^ AESBase_Te1[(s1 >> 16) & 0xff] ^ AESBase_Te2[(s2 >>  8) & 0xff] ^ AESBase_Te3[s3 & 0xff] ^ this->encRK[52];
	t1 = AESBase_Te0[s1 >> 24] ^ AESBase_Te1[(s2 >> 16) & 0xff] ^ AESBase_Te2[(s3 >>  8) & 0xff] ^ AESBase_Te3[s0 & 0xff] ^ this->encRK[53];
	t2 = AESBase_Te0[s2 >> 24] ^ AESBase_Te1[(s3 >> 16) & 0xff] ^ AESBase_Te2[(s0 >>  8) & 0xff] ^ AESBase_Te3[s1 & 0xff] ^ this->encRK[54];
	t3 = AESBase_Te0[s3 >> 24] ^ AESBase_Te1[(s0 >> 16) & 0xff] ^ AESBase_Te2[(s1 >>  8) & 0xff] ^ AESBase_Te3[s2 & 0xff] ^ this->encRK[55];

	s0 =
		(AESBase_Te4[(t0 >> 24)       ] & 0xff000000) ^
		(AESBase_Te4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Te4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Te4[(t3      ) & 0xff] & 0x000000ff) ^
		this->encRK[56];
	WriteMUInt32(outBlock     , s0);
	s1 =
		(AESBase_Te4[(t1 >> 24)       ] & 0xff000000) ^
		(AESBase_Te4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Te4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Te4[(t0      ) & 0xff] & 0x000000ff) ^
		this->encRK[57];
	WriteMUInt32(outBlock +  4, s1);
	s2 =
		(AESBase_Te4[(t2 >> 24)       ] & 0xff000000) ^
		(AESBase_Te4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Te4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Te4[(t1      ) & 0xff] & 0x000000ff) ^
		this->encRK[58];
	WriteMUInt32(outBlock +  8, s2);
	s3 =
		(AESBase_Te4[(t3 >> 24)       ] & 0xff000000) ^
		(AESBase_Te4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Te4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Te4[(t2      ) & 0xff] & 0x000000ff) ^
		this->encRK[59];
	WriteMUInt32(outBlock + 12, s3);
	return 16;
}

UOSInt Crypto::Encrypt::AES256::DecryptBlock(UnsafeArray<const UInt8> inBlock, UInt8 *outBlock) const
{
	UInt32 s0;
	UInt32 s1;
	UInt32 s2;
	UInt32 s3;
	UInt32 t0;
	UInt32 t1;
	UInt32 t2;
	UInt32 t3;

	s0 = ReadMUInt32(&inBlock[ 0]) ^ this->decRK[0];
	s1 = ReadMUInt32(&inBlock[ 4]) ^ this->decRK[1];
	s2 = ReadMUInt32(&inBlock[ 8]) ^ this->decRK[2];
	s3 = ReadMUInt32(&inBlock[12]) ^ this->decRK[3];
	/* round 1: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[ 4];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[ 5];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[ 6];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[ 7];
	/* round 2: */
	s0 = AESBase_Td0[t0 >> 24] ^ AESBase_Td1[(t3 >> 16) & 0xff] ^ AESBase_Td2[(t2 >>  8) & 0xff] ^ AESBase_Td3[t1 & 0xff] ^ this->decRK[ 8];
	s1 = AESBase_Td0[t1 >> 24] ^ AESBase_Td1[(t0 >> 16) & 0xff] ^ AESBase_Td2[(t3 >>  8) & 0xff] ^ AESBase_Td3[t2 & 0xff] ^ this->decRK[ 9];
	s2 = AESBase_Td0[t2 >> 24] ^ AESBase_Td1[(t1 >> 16) & 0xff] ^ AESBase_Td2[(t0 >>  8) & 0xff] ^ AESBase_Td3[t3 & 0xff] ^ this->decRK[10];
	s3 = AESBase_Td0[t3 >> 24] ^ AESBase_Td1[(t2 >> 16) & 0xff] ^ AESBase_Td2[(t1 >>  8) & 0xff] ^ AESBase_Td3[t0 & 0xff] ^ this->decRK[11];
	/* round 3: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[12];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[13];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[14];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[15];
	/* round 4: */
	s0 = AESBase_Td0[t0 >> 24] ^ AESBase_Td1[(t3 >> 16) & 0xff] ^ AESBase_Td2[(t2 >>  8) & 0xff] ^ AESBase_Td3[t1 & 0xff] ^ this->decRK[16];
	s1 = AESBase_Td0[t1 >> 24] ^ AESBase_Td1[(t0 >> 16) & 0xff] ^ AESBase_Td2[(t3 >>  8) & 0xff] ^ AESBase_Td3[t2 & 0xff] ^ this->decRK[17];
	s2 = AESBase_Td0[t2 >> 24] ^ AESBase_Td1[(t1 >> 16) & 0xff] ^ AESBase_Td2[(t0 >>  8) & 0xff] ^ AESBase_Td3[t3 & 0xff] ^ this->decRK[18];
	s3 = AESBase_Td0[t3 >> 24] ^ AESBase_Td1[(t2 >> 16) & 0xff] ^ AESBase_Td2[(t1 >>  8) & 0xff] ^ AESBase_Td3[t0 & 0xff] ^ this->decRK[19];
	/* round 5: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[20];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[21];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[22];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[23];
	/* round 6: */
	s0 = AESBase_Td0[t0 >> 24] ^ AESBase_Td1[(t3 >> 16) & 0xff] ^ AESBase_Td2[(t2 >>  8) & 0xff] ^ AESBase_Td3[t1 & 0xff] ^ this->decRK[24];
	s1 = AESBase_Td0[t1 >> 24] ^ AESBase_Td1[(t0 >> 16) & 0xff] ^ AESBase_Td2[(t3 >>  8) & 0xff] ^ AESBase_Td3[t2 & 0xff] ^ this->decRK[25];
	s2 = AESBase_Td0[t2 >> 24] ^ AESBase_Td1[(t1 >> 16) & 0xff] ^ AESBase_Td2[(t0 >>  8) & 0xff] ^ AESBase_Td3[t3 & 0xff] ^ this->decRK[26];
	s3 = AESBase_Td0[t3 >> 24] ^ AESBase_Td1[(t2 >> 16) & 0xff] ^ AESBase_Td2[(t1 >>  8) & 0xff] ^ AESBase_Td3[t0 & 0xff] ^ this->decRK[27];
	/* round 7: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[28];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[29];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[30];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[31];
	/* round 8: */
	s0 = AESBase_Td0[t0 >> 24] ^ AESBase_Td1[(t3 >> 16) & 0xff] ^ AESBase_Td2[(t2 >>  8) & 0xff] ^ AESBase_Td3[t1 & 0xff] ^ this->decRK[32];
	s1 = AESBase_Td0[t1 >> 24] ^ AESBase_Td1[(t0 >> 16) & 0xff] ^ AESBase_Td2[(t3 >>  8) & 0xff] ^ AESBase_Td3[t2 & 0xff] ^ this->decRK[33];
	s2 = AESBase_Td0[t2 >> 24] ^ AESBase_Td1[(t1 >> 16) & 0xff] ^ AESBase_Td2[(t0 >>  8) & 0xff] ^ AESBase_Td3[t3 & 0xff] ^ this->decRK[34];
	s3 = AESBase_Td0[t3 >> 24] ^ AESBase_Td1[(t2 >> 16) & 0xff] ^ AESBase_Td2[(t1 >>  8) & 0xff] ^ AESBase_Td3[t0 & 0xff] ^ this->decRK[35];
	/* round 9: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[36];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[37];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[38];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[39];
	/* round 10: */
	s0 = AESBase_Td0[t0 >> 24] ^ AESBase_Td1[(t3 >> 16) & 0xff] ^ AESBase_Td2[(t2 >>  8) & 0xff] ^ AESBase_Td3[t1 & 0xff] ^ this->decRK[40];
	s1 = AESBase_Td0[t1 >> 24] ^ AESBase_Td1[(t0 >> 16) & 0xff] ^ AESBase_Td2[(t3 >>  8) & 0xff] ^ AESBase_Td3[t2 & 0xff] ^ this->decRK[41];
	s2 = AESBase_Td0[t2 >> 24] ^ AESBase_Td1[(t1 >> 16) & 0xff] ^ AESBase_Td2[(t0 >>  8) & 0xff] ^ AESBase_Td3[t3 & 0xff] ^ this->decRK[42];
	s3 = AESBase_Td0[t3 >> 24] ^ AESBase_Td1[(t2 >> 16) & 0xff] ^ AESBase_Td2[(t1 >>  8) & 0xff] ^ AESBase_Td3[t0 & 0xff] ^ this->decRK[43];
	/* round 11: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[44];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[45];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[46];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[47];
	/* round 12: */
	s0 = AESBase_Td0[t0 >> 24] ^ AESBase_Td1[(t3 >> 16) & 0xff] ^ AESBase_Td2[(t2 >>  8) & 0xff] ^ AESBase_Td3[t1 & 0xff] ^ this->decRK[48];
	s1 = AESBase_Td0[t1 >> 24] ^ AESBase_Td1[(t0 >> 16) & 0xff] ^ AESBase_Td2[(t3 >>  8) & 0xff] ^ AESBase_Td3[t2 & 0xff] ^ this->decRK[49];
	s2 = AESBase_Td0[t2 >> 24] ^ AESBase_Td1[(t1 >> 16) & 0xff] ^ AESBase_Td2[(t0 >>  8) & 0xff] ^ AESBase_Td3[t3 & 0xff] ^ this->decRK[50];
	s3 = AESBase_Td0[t3 >> 24] ^ AESBase_Td1[(t2 >> 16) & 0xff] ^ AESBase_Td2[(t1 >>  8) & 0xff] ^ AESBase_Td3[t0 & 0xff] ^ this->decRK[51];
	/* round 13: */
	t0 = AESBase_Td0[s0 >> 24] ^ AESBase_Td1[(s3 >> 16) & 0xff] ^ AESBase_Td2[(s2 >>  8) & 0xff] ^ AESBase_Td3[s1 & 0xff] ^ this->decRK[52];
	t1 = AESBase_Td0[s1 >> 24] ^ AESBase_Td1[(s0 >> 16) & 0xff] ^ AESBase_Td2[(s3 >>  8) & 0xff] ^ AESBase_Td3[s2 & 0xff] ^ this->decRK[53];
	t2 = AESBase_Td0[s2 >> 24] ^ AESBase_Td1[(s1 >> 16) & 0xff] ^ AESBase_Td2[(s0 >>  8) & 0xff] ^ AESBase_Td3[s3 & 0xff] ^ this->decRK[54];
	t3 = AESBase_Td0[s3 >> 24] ^ AESBase_Td1[(s2 >> 16) & 0xff] ^ AESBase_Td2[(s1 >>  8) & 0xff] ^ AESBase_Td3[s0 & 0xff] ^ this->decRK[55];

	s0 =
		(AESBase_Td4[(t0 >> 24)       ] & 0xff000000) ^
		(AESBase_Td4[(t3 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Td4[(t2 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Td4[(t1      ) & 0xff] & 0x000000ff) ^
		this->decRK[56];
	WriteMUInt32(outBlock     , s0);
	s1 =
		(AESBase_Td4[(t1 >> 24)       ] & 0xff000000) ^
		(AESBase_Td4[(t0 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Td4[(t3 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Td4[(t2      ) & 0xff] & 0x000000ff) ^
		this->decRK[57];
	WriteMUInt32(outBlock +  4, s1);
	s2 =
		(AESBase_Td4[(t2 >> 24)       ] & 0xff000000) ^
		(AESBase_Td4[(t1 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Td4[(t0 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Td4[(t3      ) & 0xff] & 0x000000ff) ^
		this->decRK[58];
	WriteMUInt32(outBlock +  8, s2);
	s3 =
		(AESBase_Td4[(t3 >> 24)       ] & 0xff000000) ^
		(AESBase_Td4[(t2 >> 16) & 0xff] & 0x00ff0000) ^
		(AESBase_Td4[(t1 >>  8) & 0xff] & 0x0000ff00) ^
		(AESBase_Td4[(t0      ) & 0xff] & 0x000000ff) ^
		this->decRK[59];
	WriteMUInt32(outBlock + 12, s3);
	return 16;
}

void Crypto::Encrypt::AES256::SetKey(UnsafeArray<const UInt8> key)
{
	UOSInt i;
	UOSInt j;
	UInt32 temp;

	this->encRK[0] = ReadMUInt32(&key[ 0]);
	this->encRK[1] = ReadMUInt32(&key[ 4]);
	this->encRK[2] = ReadMUInt32(&key[ 8]);
	this->encRK[3] = ReadMUInt32(&key[12]);
	this->encRK[4] = ReadMUInt32(&key[16]);
	this->encRK[5] = ReadMUInt32(&key[20]);
	this->encRK[6] = ReadMUInt32(&key[24]);
	this->encRK[7] = ReadMUInt32(&key[28]);
	i = 0;
	while (true)
	{
		temp  = this->encRK[i * 8 + 7];
		this->encRK[i * 8 +  8] = this->encRK[i * 8 + 0] ^
			(AESBase_Te4[(temp >> 16) & 0xff] & 0xff000000) ^
			(AESBase_Te4[(temp >>  8) & 0xff] & 0x00ff0000) ^
			(AESBase_Te4[(temp      ) & 0xff] & 0x0000ff00) ^
			(AESBase_Te4[(temp >> 24)       ] & 0x000000ff) ^
			AESBase_rcon[i];
		this->encRK[i * 8 +  9] = this->encRK[i * 8 + 1] ^ this->encRK[i * 8 +  8];
		this->encRK[i * 8 + 10] = this->encRK[i * 8 + 2] ^ this->encRK[i * 8 +  9];
		this->encRK[i * 8 + 11] = this->encRK[i * 8 + 3] ^ this->encRK[i * 8 + 10];
		if (i >= 6)
		{
			break;
		}
		temp  = this->encRK[i * 8 + 11];
		this->encRK[i * 8 + 12] = this->encRK[i * 8 + 4] ^
			(AESBase_Te4[(temp >> 24)       ] & 0xff000000) ^
			(AESBase_Te4[(temp >> 16) & 0xff] & 0x00ff0000) ^
			(AESBase_Te4[(temp >>  8) & 0xff] & 0x0000ff00) ^
			(AESBase_Te4[(temp      ) & 0xff] & 0x000000ff);
		this->encRK[i * 8 + 13] = this->encRK[i * 8 + 5] ^ this->encRK[i * 8 + 12];
		this->encRK[i * 8 + 14] = this->encRK[i * 8 + 6] ^ this->encRK[i * 8 + 13];
		this->encRK[i * 8 + 15] = this->encRK[i * 8 + 7] ^ this->encRK[i * 8 + 14];
		i++;
	}

	i = 0;
	j = 56;
	while (i < j)
	{
		this->decRK[i + 0] = this->encRK[j + 0];
		this->decRK[i + 1] = this->encRK[j + 1];
		this->decRK[i + 2] = this->encRK[j + 2];
		this->decRK[i + 3] = this->encRK[j + 3];
		this->decRK[j + 0] = this->encRK[i + 0];
		this->decRK[j + 1] = this->encRK[i + 1];
		this->decRK[j + 2] = this->encRK[i + 2];
		this->decRK[j + 3] = this->encRK[i + 3];
		i += 4;
		j -= 4;
	}
	this->decRK[i + 0] = this->encRK[i + 0];
	this->decRK[i + 1] = this->encRK[i + 1];
	this->decRK[i + 2] = this->encRK[i + 2];
	this->decRK[i + 3] = this->encRK[i + 3];

	i = 1;
	while (i < 14)
	{
		this->decRK[i * 4 + 0] =
			AESBase_Td0[AESBase_Te4[(this->decRK[i * 4 + 0] >> 24)       ] & 0xff] ^
			AESBase_Td1[AESBase_Te4[(this->decRK[i * 4 + 0] >> 16) & 0xff] & 0xff] ^
			AESBase_Td2[AESBase_Te4[(this->decRK[i * 4 + 0] >>  8) & 0xff] & 0xff] ^
			AESBase_Td3[AESBase_Te4[(this->decRK[i * 4 + 0]      ) & 0xff] & 0xff];
		this->decRK[i * 4 + 1] =
			AESBase_Td0[AESBase_Te4[(this->decRK[i * 4 + 1] >> 24)       ] & 0xff] ^
			AESBase_Td1[AESBase_Te4[(this->decRK[i * 4 + 1] >> 16) & 0xff] & 0xff] ^
			AESBase_Td2[AESBase_Te4[(this->decRK[i * 4 + 1] >>  8) & 0xff] & 0xff] ^
			AESBase_Td3[AESBase_Te4[(this->decRK[i * 4 + 1]      ) & 0xff] & 0xff];
		this->decRK[i * 4 + 2] =
			AESBase_Td0[AESBase_Te4[(this->decRK[i * 4 + 2] >> 24)       ] & 0xff] ^
			AESBase_Td1[AESBase_Te4[(this->decRK[i * 4 + 2] >> 16) & 0xff] & 0xff] ^
			AESBase_Td2[AESBase_Te4[(this->decRK[i * 4 + 2] >>  8) & 0xff] & 0xff] ^
			AESBase_Td3[AESBase_Te4[(this->decRK[i * 4 + 2]      ) & 0xff] & 0xff];
		this->decRK[i * 4 + 3] =
			AESBase_Td0[AESBase_Te4[(this->decRK[i * 4 + 3] >> 24)       ] & 0xff] ^
			AESBase_Td1[AESBase_Te4[(this->decRK[i * 4 + 3] >> 16) & 0xff] & 0xff] ^
			AESBase_Td2[AESBase_Te4[(this->decRK[i * 4 + 3] >>  8) & 0xff] & 0xff] ^
			AESBase_Td3[AESBase_Te4[(this->decRK[i * 4 + 3]      ) & 0xff] & 0xff];
		i++;
	}
}
