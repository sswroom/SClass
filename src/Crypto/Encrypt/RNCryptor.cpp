#include "Stdafx.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/RNCryptor.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Crypto/Hash/SHA256.h"
#include "Data/ByteBuffer.h"
#include "Data/RandomBytesGenerator.h"

UOSInt Crypto::Encrypt::RNCryptor::RemovePadding(UInt8 *buff, UOSInt buffSize)
{
	UInt8 paddingSize = buff[buffSize - 1];
	if (paddingSize > 0 && paddingSize <= 15)
	{
		UOSInt i = paddingSize;
		while (i-- > 0)
		{
			if (buff[buffSize - i - 1] != paddingSize)
				return buffSize;
		}
		return buffSize - paddingSize;
	}
	return buffSize;
}

Bool Crypto::Encrypt::RNCryptor::Decrypt(IO::SeekableStream *srcStream, IO::Stream *destStream, Text::CString password)
{
	UInt8 header[34];
	UInt64 fileLength = srcStream->GetLength();
	if (srcStream->Read(BYTEARR(header)) != 34)
		return false;
	if (header[1] != 1)
		return false;
	if (header[0] == 2 || header[0] == 3)
	{
		UInt8 encKey[32];
		UInt8 hmacKey[32];
		UInt8 hmacSrc[32];
		UInt8 hmacCalc[32];
		Crypto::Hash::SHA1 sha1;
		Crypto::Hash::HMAC hmacSHA1(sha1, password.v, password.leng);
		Crypto::PBKDF2::Calc(&header[2], 8, 10000, 32, hmacSHA1, encKey);
		Crypto::PBKDF2::Calc(&header[10], 8, 10000, 32, hmacSHA1, hmacKey);
		Crypto::Encrypt::AES256 aes(encKey);
		aes.SetChainMode(Crypto::Encrypt::ChainMode::CBC);
		aes.SetIV(&header[18]);
		UInt64 sizeLeft = fileLength - 66;
		UOSInt readSize;
		UInt8 *destBuff;
		Bool succ = true;
		Crypto::Hash::SHA256 sha256;
		Crypto::Hash::HMAC hmac(sha256, hmacKey, 32);
		hmac.Calc(header, 34);
		if (sizeLeft > 1048576)
		{
			Data::ByteBuffer srcBuff(1048576);
			destBuff = MemAlloc(UInt8, 1048576);
			while (sizeLeft > 1048576)
			{
				readSize = srcStream->Read(srcBuff);
				if (readSize != 1048576)
				{
					succ = false;
					break;
				}
				hmac.Calc(srcBuff.Ptr(), readSize);
				aes.Decrypt(srcBuff.Ptr(), readSize, destBuff, 0);
				if (destStream->Write(destBuff, 1048576) != 1048576)
				{
					succ = false;
					break;
				}
				sizeLeft -= 1048576;
			}
			if (succ && sizeLeft > 0)
			{
				readSize = srcStream->Read(srcBuff.WithSize((UOSInt)sizeLeft));
				if (readSize != sizeLeft)
				{
					succ = false;
				}
				else
				{
					hmac.Calc(srcBuff.Ptr(), readSize);
					aes.Decrypt(srcBuff.Ptr(), readSize, destBuff, 0);
					readSize = RemovePadding(destBuff, readSize);
					if (destStream->Write(destBuff, readSize) != readSize)
					{
						succ = false;
					}
				}
			}
			MemFree(destBuff);
		}
		else
		{
			readSize = (UOSInt)sizeLeft;
			Data::ByteBuffer srcBuff(readSize);
			destBuff = MemAlloc(UInt8, readSize);
			readSize = srcStream->Read(srcBuff);
			if (readSize != sizeLeft)
			{
				succ = false;
			}
			else
			{
				hmac.Calc(srcBuff.Ptr(), readSize);
				aes.Decrypt(srcBuff.Ptr(), readSize, destBuff, 0);
				readSize = RemovePadding(destBuff, readSize);
				if (destStream->Write(destBuff, readSize) != readSize)
				{
					succ = false;
				}
			}
			MemFree(destBuff);
		}
		if (succ)
		{
			hmac.GetValue(hmacCalc);
			if (srcStream->Read(BYTEARR(hmacSrc)) != 32)
			{
				succ = false;
			}
			else if (!Text::StrEqualsC(hmacSrc, 32, hmacCalc, 32))
			{
				succ = false;
			}
		}
		return succ;
	}
	return false;
}

Bool Crypto::Encrypt::RNCryptor::Encrypt(IO::SeekableStream *srcStream, IO::Stream *destStream, Text::CString password)
{
	UInt8 header[34];
	UInt64 fileLength = srcStream->GetLength();
	if (fileLength > 1048576)
	{
		return false;
	}
	header[0] = 3;
	header[1] = 1;
	Data::RandomBytesGenerator rand;
	rand.NextBytes(&header[2], 32);
	UInt8 paddingSize = 0;
	Data::ByteBuffer srcBuff((UOSInt)fileLength + 16);
	if (srcStream->Read(srcBuff.WithSize((UOSInt)fileLength)) != fileLength)
	{
		return false;
	}
	if (fileLength & 15)
	{
		paddingSize = (UInt8)(16 - (fileLength & 15));
		UOSInt i = 0;
		while (i < paddingSize)
		{
			srcBuff[(UOSInt)fileLength + i] = paddingSize;
			i++;
		}
		fileLength += (UOSInt)paddingSize;
	}
	UInt8 encKey[32];
	UInt8 hmacKey[32];
	UInt8 hmacCalc[32];
	UInt8 *destBuff = MemAlloc(UInt8, (UOSInt)fileLength);

	Crypto::Hash::SHA1 sha1;
	Crypto::Hash::HMAC hmacSHA1(sha1, password.v, password.leng);
	Crypto::PBKDF2::Calc(&header[2], 8, 10000, 32, hmacSHA1, encKey);
	Crypto::PBKDF2::Calc(&header[10], 8, 10000, 32, hmacSHA1, hmacKey);
	Crypto::Encrypt::AES256 aes(encKey);
	aes.SetChainMode(Crypto::Encrypt::ChainMode::CBC);
	aes.SetIV(&header[18]);
	Crypto::Hash::SHA256 sha256;
	Crypto::Hash::HMAC hmac(sha256, hmacKey, 32);
	hmac.Calc(header, 34);
	aes.Encrypt(srcBuff.Ptr(), (UOSInt)fileLength, destBuff, 0);
	hmac.Calc(destBuff, (UOSInt)fileLength);
	hmac.GetValue(hmacCalc);
	Bool succ = true;
	if (destStream->Write(header, 34) != 34 ||
		destStream->Write(destBuff, (UOSInt)fileLength) != fileLength ||
		destStream->Write(hmacCalc, 32) != 32)
	{
		succ = false;
	}
	MemFree(destBuff);
	return succ;
}
