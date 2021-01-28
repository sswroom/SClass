#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Crypto/Cert/CertRequest.h"
#include "Crypto/Encrypt/Base64.h"

Crypto::Cert::CertRequest::CertRequest(const WChar *fileName)
{
	WChar sbuff[2048];
	UInt8 buff[2048];
	OSInt buffSize;
	UInt8 obuff[2048];
	OSInt obuffSize;
	WChar *sptr;
	WChar *sptr2;
	IO::FileStream *fs;
	IO::StreamReader *reader;
	this->reqBuff = 0;
	this->reqBuffSize = 0;

	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(reader, IO::StreamReader(fs, 65001));
	if (reader->ReadLine(sbuff, 2048))
	{
		if (Text::StrCompare(sbuff, L"-----BEGIN NEW CERTIFICATE REQUEST-----") == 0)
		{
			sptr = sbuff;
			while (true)
			{
				sptr2 = reader->ReadLine(sptr, &sbuff[2048] - sptr);
				if (sptr2 == 0)
					break;
				if (Text::StrCompare(sptr, L"-----END NEW CERTIFICATE REQUEST-----") == 0)
				{
					*sptr = 0;
					Text::Encoding enc(65001);
					buffSize = enc.ToBytes(buff, sbuff, sptr - sbuff);
					Crypto::Encrypt::Base64 b64;
					obuffSize = b64.Decrypt(buff, (Int32)buffSize, obuff, 0);
					this->reqBuff = MemAlloc(UInt8, obuffSize);
					this->reqBuffSize = obuffSize;
					MemCopy(this->reqBuff, obuff, obuffSize);
					break;
				}
				else
				{
					sptr = sptr2;
				}
			}
		}
	}

	DEL_CLASS(reader);
	DEL_CLASS(fs);
}

Crypto::Cert::CertRequest::~CertRequest()
{
	if (this->reqBuff)
	{
		MemFree(this->reqBuff);
	}
}
