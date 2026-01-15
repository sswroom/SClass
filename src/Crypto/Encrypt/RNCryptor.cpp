#include "Stdafx.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/RNCryptor.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Crypto/Hash/SHA256.h"
#include "Data/ByteBuffer.h"
#include "Data/RandomBytesGenerator.h"

UIntOS Crypto::Encrypt::RNCryptor::RemovePadding(UnsafeArray<UInt8> buff, UIntOS buffSize)
{
	UInt8 paddingSize = buff[buffSize - 1];
	if (paddingSize > 0 && paddingSize <= 15)
	{
		UIntOS i = paddingSize;
		while (i-- > 0)
		{
			if (buff[buffSize - i - 1] != paddingSize)
				return buffSize;
		}
		return buffSize - paddingSize;
	}
	return buffSize;
}

Bool Crypto::Encrypt::RNCryptor::Decrypt(NN<IO::SeekableStream> srcStream, NN<IO::Stream> destStream, Text::CStringNN password)
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
		UIntOS readSize;
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
				hmac.Calc(srcBuff.Arr().Ptr(), readSize);
				aes.Decrypt(srcBuff.Arr().Ptr(), readSize, destBuff);
				if (destStream->Write(Data::ByteArrayR(destBuff, 1048576)) != 1048576)
				{
					succ = false;
					break;
				}
				sizeLeft -= 1048576;
			}
			if (succ && sizeLeft > 0)
			{
				readSize = srcStream->Read(srcBuff.WithSize((UIntOS)sizeLeft));
				if (readSize != sizeLeft)
				{
					succ = false;
				}
				else
				{
					hmac.Calc(srcBuff.Arr().Ptr(), readSize);
					aes.Decrypt(srcBuff.Arr().Ptr(), readSize, destBuff);
					readSize = RemovePadding(destBuff, readSize);
					if (destStream->Write(Data::ByteArrayR(destBuff, readSize)) != readSize)
					{
						succ = false;
					}
				}
			}
			MemFree(destBuff);
		}
		else
		{
			readSize = (UIntOS)sizeLeft;
			Data::ByteBuffer srcBuff(readSize);
			destBuff = MemAlloc(UInt8, readSize);
			readSize = srcStream->Read(srcBuff);
			if (readSize != sizeLeft)
			{
				succ = false;
			}
			else
			{
				hmac.Calc(srcBuff.Arr().Ptr(), readSize);
				aes.Decrypt(srcBuff.Arr().Ptr(), readSize, destBuff);
				readSize = RemovePadding(destBuff, readSize);
				if (destStream->Write(Data::ByteArrayR(destBuff, readSize)) != readSize)
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

Bool Crypto::Encrypt::RNCryptor::Encrypt(NN<IO::SeekableStream> srcStream, NN<IO::Stream> destStream, Text::CStringNN password)
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
	Data::ByteBuffer srcBuff((UIntOS)fileLength + 16);
	if (srcStream->Read(srcBuff.WithSize((UIntOS)fileLength)) != fileLength)
	{
		return false;
	}
	if (fileLength & 15)
	{
		paddingSize = (UInt8)(16 - (fileLength & 15));
		UIntOS i = 0;
		while (i < paddingSize)
		{
			srcBuff[(UIntOS)fileLength + i] = paddingSize;
			i++;
		}
		fileLength += (UIntOS)paddingSize;
	}
	UInt8 encKey[32];
	UInt8 hmacKey[32];
	UInt8 hmacCalc[32];
	UInt8 *destBuff = MemAlloc(UInt8, (UIntOS)fileLength);

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
	aes.Encrypt(srcBuff.Arr().Ptr(), (UIntOS)fileLength, destBuff);
	hmac.Calc(destBuff, (UIntOS)fileLength);
	hmac.GetValue(hmacCalc);
	Bool succ = true;
	if (destStream->Write(Data::ByteArrayR(header, 34)) != 34 ||
		destStream->Write(Data::ByteArrayR(destBuff, (UIntOS)fileLength)) != fileLength ||
		destStream->Write(Data::ByteArrayR(hmacCalc, 32)) != 32)
	{
		succ = false;
	}
	MemFree(destBuff);
	return succ;
}
