#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Net/WinSSLClient.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define SECURITY_WIN32
#include <sspi.h>
#include <schannel.h>

//#define DEBUG_PRINT
#if defined(DEBUG_PRINT)
#ifdef _MSC_VER
#include <windows.h>
#include <stdio.h>
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); OutputDebugStringA(sbuff);}
#else
#include <stdio.h>
#endif
#endif

extern void SecBuffer_Set(SecBuffer *buff, UInt32 type, UInt8 *inpBuff, UInt32 leng);
extern void SecBufferDesc_Set(SecBufferDesc *desc, SecBuffer *buffs, UInt32 nBuffs);

struct Net::WinSSLClient::ClassData
{
	CredHandle *hCred;
	CtxtHandle ctxt;
	UOSInt step;
	SecPkgContext_StreamSizes stmSizes;
	UInt8 *recvBuff;
	UOSInt recvBuffSize;
	UOSInt recvOfst;
	UInt8 *decBuff;
	UOSInt decSize;

	UInt8 *readBuff;
	UOSInt readSize;
	void *readData;
	Sync::Event *readEvt;
	Data::ArrayListNN<Crypto::Cert::Certificate> *remoteCerts;
};

Net::WinSSLClient::WinSSLClient(NN<Net::SocketFactory> sockf, Socket *s, void *ctxt) : SSLClient(sockf, s)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ctxt = *(CredHandle*)ctxt;
	this->clsData->hCred = 0;
	this->clsData->step = 0;
	this->clsData->recvBuff = 0;
	this->clsData->decBuff = 0;
	this->clsData->step = 1;
	MemClear(&this->clsData->stmSizes, sizeof(this->clsData->stmSizes));
	
	QueryContextAttributes(&this->clsData->ctxt, SECPKG_ATTR_STREAM_SIZES, &this->clsData->stmSizes);
	this->clsData->recvBuffSize = this->clsData->stmSizes.cbHeader + (UOSInt)this->clsData->stmSizes.cbMaximumMessage + this->clsData->stmSizes.cbTrailer;
	this->clsData->recvBuff = MemAlloc(UInt8, this->clsData->recvBuffSize);
	this->clsData->recvOfst = 0;
	this->clsData->decBuff = MemAlloc(UInt8, this->clsData->stmSizes.cbMaximumMessage);
	this->clsData->decSize = 0;
	this->clsData->readBuff = 0;
	this->clsData->readSize = 0;
	this->clsData->remoteCerts = 0;
	this->clsData->readData = 0;
	this->clsData->readEvt = 0;

	PCCERT_CONTEXT serverCert = 0;
	PCCERT_CONTEXT thisCert = 0;
	PCCERT_CONTEXT lastCert = 0;
	QueryContextAttributes(&this->clsData->ctxt, SECPKG_ATTR_REMOTE_CERT_CONTEXT, &serverCert);
	if (serverCert)
	{
		DWORD dwVerificationFlags = 0;
		NN<Crypto::Cert::X509Cert> cert;
		NEW_CLASS(this->clsData->remoteCerts, Data::ArrayListNN<Crypto::Cert::Certificate>());
		NEW_CLASSNN(cert, Crypto::Cert::X509Cert(CSTR("RemoteCert"), Data::ByteArrayR(serverCert->pbCertEncoded, serverCert->cbCertEncoded)));
		this->clsData->remoteCerts->Add(cert);
		thisCert = CertGetIssuerCertificateFromStore(serverCert->hCertStore, serverCert, NULL, &dwVerificationFlags);
		while (thisCert)
		{
			NEW_CLASSNN(cert, Crypto::Cert::X509Cert(CSTR("RemoteCert"), Data::ByteArrayR(thisCert->pbCertEncoded, thisCert->cbCertEncoded)));
			this->clsData->remoteCerts->Add(cert);
			lastCert = thisCert;
			thisCert = CertGetIssuerCertificateFromStore(serverCert->hCertStore, lastCert, NULL, &dwVerificationFlags);
			CertFreeCertificateContext(lastCert);
		}
		CertFreeCertificateContext(serverCert);
	}
}

Net::WinSSLClient::~WinSSLClient()
{
	//SSL_free(this->clsData->ssl);
	if (this->clsData->step != 0)
	{
		DeleteSecurityContext(&this->clsData->ctxt);
		this->clsData->step = 0;
	}
	if (this->clsData->recvBuff)
	{
		MemFree(this->clsData->recvBuff);
	}
	if (this->clsData->decBuff)
	{
		MemFree(this->clsData->decBuff);
	}
	if (this->clsData->remoteCerts)
	{
		UOSInt i = this->clsData->remoteCerts->GetCount();
		while (i-- > 0)
		{
			NN<Crypto::Cert::Certificate> cert = this->clsData->remoteCerts->GetItemNoCheck(i);
			cert.Delete();
		}
		DEL_CLASS(this->clsData->remoteCerts);
	}
	MemFree(this->clsData);
}

UOSInt Net::WinSSLClient::Read(const Data::ByteArray &buff)
{
	if (this->clsData->step == 0)
	{
		return 0;
	}
#if defined(DEBUG_PRINT)
	UTF8Char debugBuff[64];
	Data::DateTime debugDt;
#endif
	Data::ByteArray myBuff = buff;
	UOSInt ret = 0;
	if (this->clsData->decSize > 0)
	{
		if (this->clsData->decSize >= myBuff.GetSize())
		{
			myBuff.CopyFrom(Data::ByteArrayR(this->clsData->decBuff, myBuff.GetSize()));
			if (this->clsData->decSize > myBuff.GetSize())
			{
				MemCopyO(this->clsData->decBuff, &this->clsData->decBuff[myBuff.GetSize()], this->clsData->decSize - myBuff.GetSize());
				this->clsData->decSize -= myBuff.GetSize();
			}
			else
			{
				this->clsData->decSize = 0;
			}
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Return size1 = %d\r\n", debugBuff, (UInt32)myBuff.GetSize());
#endif
			return myBuff.GetSize();
		}
		myBuff.CopyFrom(Data::ByteArray(this->clsData->decBuff, this->clsData->decSize));
		ret = this->clsData->decSize;
		this->clsData->decSize = 0;
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Return size0 = %d\r\n", debugBuff, (UInt32)ret);
#endif
		return ret;
	}
	if (s && (this->flags & 6) == 0)
	{
		Net::SocketFactory::ErrorType et;
		SecBufferDesc buffDesc;
		SecBuffer buffs[4];
		SECURITY_STATUS status = SEC_E_INCOMPLETE_MESSAGE;

		SecBuffer_Set(&buffs[0], SECBUFFER_DATA, this->clsData->recvBuff, (UInt32)this->clsData->recvOfst);
		SecBuffer_Set(&buffs[1], SECBUFFER_EMPTY, 0, 0);
		SecBuffer_Set(&buffs[2], SECBUFFER_EMPTY, 0, 0);
		SecBuffer_Set(&buffs[3], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&buffDesc, buffs, 4);
		status = DecryptMessage(&this->clsData->ctxt, &buffDesc, 0, 0);
		while (status == SEC_E_INCOMPLETE_MESSAGE)
		{
			UOSInt recvSize = this->sockf->ReceiveData(this->s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, et);
			if (recvSize <= 0)
			{
				this->flags |= 2;
				return ret;
			}
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Recv size = %d\r\n", debugBuff, (UInt32)recvSize);
#endif
			this->clsData->recvOfst += recvSize;
			SecBuffer_Set(&buffs[0], SECBUFFER_DATA, this->clsData->recvBuff, (UInt32)this->clsData->recvOfst);
			SecBuffer_Set(&buffs[1], SECBUFFER_EMPTY, 0, 0);
			SecBuffer_Set(&buffs[2], SECBUFFER_EMPTY, 0, 0);
			SecBuffer_Set(&buffs[3], SECBUFFER_EMPTY, 0, 0);
			SecBufferDesc_Set(&buffDesc, buffs, 4);
			status = DecryptMessage(&this->clsData->ctxt, &buffDesc, 0, 0);
		}

		if (status != SEC_E_OK && status != SEC_I_RENEGOTIATE)
		{
			this->flags |= 6;
			this->sockf->DestroySocket(this->s);
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Return size2 = %d\r\n", debugBuff, (UInt32)ret);
#endif
			return ret;
		}

		this->clsData->recvOfst = 0;
		UOSInt i = 0;
		while (i < 4)
		{
			if (buffs[i].BufferType == SECBUFFER_DATA)
			{
				if (buffs[i].cbBuffer <= myBuff.GetSize())
				{
#if defined(DEBUG_PRINT)
					debugDt.SetCurrTime();
					debugDt.ToString(debugBuff, "HH:mm:ss.fff");
					printf("%s Dec size1 = %d, size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer, (UInt32)myBuff.GetSize());
#endif
					myBuff.CopyFrom(Data::ByteArrayR((UInt8*)buffs[i].pvBuffer, buffs[i].cbBuffer));
					myBuff += (UInt32)buffs[i].cbBuffer;
					ret += buffs[i].cbBuffer;
				}
				else
				{
					if (myBuff.GetSize() > 0)
					{
#if defined(DEBUG_PRINT)
						debugDt.SetCurrTime();
						debugDt.ToString(debugBuff, "HH:mm:ss.fff");
						printf("%s Dec size2 = %d, size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer, (UInt32)myBuff.GetSize());
#endif
						myBuff.CopyFrom(Data::ByteArrayR((UInt8*)buffs[i].pvBuffer, myBuff.GetSize()));
						ret += myBuff.GetSize();
						MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], myBuff.GetSize() + (UInt8*)buffs[i].pvBuffer, buffs[i].cbBuffer - myBuff.GetSize());
						this->clsData->decSize += buffs[i].cbBuffer - myBuff.GetSize();
						myBuff += myBuff.GetSize();
					}
					else
					{
#if defined(DEBUG_PRINT)
						debugDt.SetCurrTime();
						debugDt.ToString(debugBuff, "HH:mm:ss.fff");
						printf("%s Dec size3 = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer);
#endif
						MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], buffs[i].pvBuffer, buffs[i].cbBuffer);
						this->clsData->decSize += buffs[i].cbBuffer;
					}
				}
			}
			else if (buffs[i].BufferType == SECBUFFER_EXTRA)
			{
#if defined(DEBUG_PRINT)
				debugDt.SetCurrTime();
				debugDt.ToString(debugBuff, "HH:mm:ss.fff");
				printf("%s Ext size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer);
#endif
				MemCopyO(&this->clsData->recvBuff[this->clsData->recvOfst], buffs[i].pvBuffer, buffs[i].cbBuffer);
				this->clsData->recvOfst += buffs[i].cbBuffer;
			}
			i++;
		}
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Return size3 = %d\r\n", debugBuff, (UInt32)ret);
#endif
		return ret;
	}
	else
	{
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Return size4 = %d\r\n", debugBuff, (UInt32)ret);
#endif
		return ret;
	}
}

UOSInt Net::WinSSLClient::Write(const UInt8 *buff, UOSInt size)
{
	if (this->clsData->step == 0)
	{
		return 0;
	}
	if (s && (this->flags & 5) == 0)
	{
#if defined(DEBUG_PRINT)
		UTF8Char debugBuff[64];
		Data::DateTime debugDt;
#endif
		UOSInt writeSize = 0;
		while (size > this->clsData->stmSizes.cbMaximumMessage)
		{
			writeSize += Write(buff, this->clsData->stmSizes.cbMaximumMessage);
			buff += this->clsData->stmSizes.cbMaximumMessage;
			size -= this->clsData->stmSizes.cbMaximumMessage;
		}
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Writing %d bytes, enc size = %d\r\n", debugBuff, (UInt32)size, (UInt32)(this->clsData->stmSizes.cbHeader + size + this->clsData->stmSizes.cbTrailer));
#endif
		UInt8 *encBuff = MemAlloc(UInt8, this->clsData->stmSizes.cbHeader + size + this->clsData->stmSizes.cbTrailer);
		MemCopyNO(&encBuff[this->clsData->stmSizes.cbHeader], buff, size);
		SecBuffer outputBuff[4];
		SecBufferDesc outputDesc;
		SecBuffer_Set(&outputBuff[0], SECBUFFER_STREAM_HEADER, &encBuff[0], this->clsData->stmSizes.cbHeader);
		SecBuffer_Set(&outputBuff[1], SECBUFFER_DATA, &encBuff[this->clsData->stmSizes.cbHeader], (UInt32)size);
		SecBuffer_Set(&outputBuff[2], SECBUFFER_STREAM_TRAILER, &encBuff[this->clsData->stmSizes.cbHeader + size], this->clsData->stmSizes.cbTrailer);
		SecBuffer_Set(&outputBuff[3], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&outputDesc, outputBuff, 4);

		if (FAILED(EncryptMessage(&this->clsData->ctxt, 0, &outputDesc, 0)))
		{
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Encrypt Failed\r\n", debugBuff);
#endif
			this->flags |= 1;
			return 0;
		}
		Net::SocketFactory::ErrorType et;
		UOSInt ret = this->sockf->SendData(this->s, encBuff, (UOSInt)outputBuff[0].cbBuffer + (UOSInt)outputBuff[1].cbBuffer + outputBuff[2].cbBuffer, et);// SSL_write(this->clsData->ssl, buff, (int)size);
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Sent %d bytes\r\n", debugBuff, (UInt32)ret);
#endif
		MemFree(encBuff);
		if (ret > (UOSInt)outputBuff[0].cbBuffer + outputBuff[2].cbBuffer)
		{
			ret -= (UOSInt)outputBuff[0].cbBuffer + outputBuff[2].cbBuffer;
			this->currCnt += ret;
			return ret + writeSize;
		}
		this->flags |= 1;
		return writeSize;
	}
	else
	{
		return 0;
	}
}

void *Net::WinSSLClient::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	if (this->clsData->step == 0)
	{
		return 0;
	}
	this->clsData->readBuff = buff.Ptr().Ptr();
	this->clsData->readSize = buff.GetSize();
	if (this->clsData->decSize > 0)
	{
		evt->Set();
		return (void*)-1;
	}
	if (this->clsData->recvOfst > 0)
	{
		SecBufferDesc buffDesc;
		SecBuffer buffs[4];
		SECURITY_STATUS status = SEC_E_INCOMPLETE_MESSAGE;
		SecBuffer_Set(&buffs[0], SECBUFFER_DATA, this->clsData->recvBuff, (UInt32)this->clsData->recvOfst);
		SecBuffer_Set(&buffs[1], SECBUFFER_EMPTY, 0, 0);
		SecBuffer_Set(&buffs[2], SECBUFFER_EMPTY, 0, 0);
		SecBuffer_Set(&buffs[3], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&buffDesc, buffs, 4);
		status = DecryptMessage(&this->clsData->ctxt, &buffDesc, 0, 0);
		if (status == SEC_E_OK)
		{
#if defined(DEBUG_PRINT)
			UTF8Char debugBuff[64];
			Data::DateTime debugDt;
#endif
			this->clsData->recvOfst = 0;
			UOSInt i = 0;
			while (i < 4)
			{
				if (buffs[i].BufferType == SECBUFFER_DATA)
				{
#if defined(DEBUG_PRINT)
					debugDt.SetCurrTime();
					debugDt.ToString(debugBuff, "HH:mm:ss.fff");
					printf("%s BeginRead Dec size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer);
#endif
					MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], buffs[i].pvBuffer, buffs[i].cbBuffer);
					this->clsData->decSize += buffs[i].cbBuffer;
				}
				else if (buffs[i].BufferType == SECBUFFER_EXTRA)
				{
#if defined(DEBUG_PRINT)
					debugDt.SetCurrTime();
					debugDt.ToString(debugBuff, "HH:mm:ss.fff");
					printf("%s BeginRead Ext size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer);
#endif
					MemCopyO(&this->clsData->recvBuff[this->clsData->recvOfst], buffs[i].pvBuffer, buffs[i].cbBuffer);
					this->clsData->recvOfst += buffs[i].cbBuffer;
				}
				i++;
			}

			evt->Set();
			return (void*)-1;
		}
	}

	if (s == 0 || (this->flags & 6) != 0)
		return 0;
	Net::SocketFactory::ErrorType et;
	this->clsData->readEvt = evt;
	void *data = sockf->BeginReceiveData(s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, evt, et);
	if (data == 0)
	{
		if (et == Net::SocketFactory::ET_SHUTDOWN)
		{
			this->flags |= 2;
		}
		else if (et == Net::SocketFactory::ET_DISCONNECT)
		{
			this->flags |= 2;
		}
		else
		{
			this->Close();
		}
	}
	else
	{
		this->clsData->readData = data;
	}
	return data;
}

UOSInt Net::WinSSLClient::EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	if (reqData == 0)
	{
		incomplete.Set(false);
		return 0;
	}
#if defined(DEBUG_PRINT)
	UTF8Char debugBuff[64];
	Data::DateTime debugDt;
#endif
	UOSInt ret = 0;
	if (this->clsData->decSize > 0)
	{
		if (this->clsData->decSize >= this->clsData->readSize)
		{
			MemCopyNO(this->clsData->readBuff, this->clsData->decBuff, this->clsData->readSize);
			if (this->clsData->decSize > this->clsData->readSize)
			{
				MemCopyO(this->clsData->decBuff, &this->clsData->decBuff[this->clsData->readSize], this->clsData->decSize - this->clsData->readSize);
				this->clsData->decSize -= this->clsData->readSize;
			}
			else
			{
				this->clsData->decSize = 0;
			}
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s EndRead Return size1 = %d, \r\n", debugBuff, (UInt32)this->clsData->readSize);
#endif
			return this->clsData->readSize;
		}
		MemCopyNO(this->clsData->readBuff, this->clsData->decBuff, this->clsData->decSize);
		ret = this->clsData->decSize;
		this->clsData->decSize = 0;
		return ret;
	}
	else if (this->clsData->readData == 0)
	{
		incomplete.Set(false);
		return 0;
	}
	Net::SocketFactory::ErrorType et;
	SecBufferDesc buffDesc;
	SecBuffer buffs[4];
	SECURITY_STATUS status = SEC_E_INCOMPLETE_MESSAGE;
	UOSInt recvSize;
	Bool incomp;

	status = SEC_E_INCOMPLETE_MESSAGE;
	recvSize = sockf->EndReceiveData(this->clsData->readData, toWait, incomp);
	if (recvSize <= 0)
	{
		incomplete.Set(incomp);
		if (incomp)
		{
			return 0;
		}
		this->clsData->readData = 0;
		this->flags |= 2;
		return ret;
	}
	this->clsData->readData = 0;

#if defined(DEBUG_PRINT)
	debugDt.SetCurrTime();
	debugDt.ToString(debugBuff, "HH:mm:ss.fff");
	printf("%s EndRead Recv size = %d\r\n", debugBuff, (UInt32)recvSize);
#endif
	this->clsData->recvOfst += recvSize;
	SecBuffer_Set(&buffs[0], SECBUFFER_DATA, this->clsData->recvBuff, (UInt32)this->clsData->recvOfst);
	SecBuffer_Set(&buffs[1], SECBUFFER_EMPTY, 0, 0);
	SecBuffer_Set(&buffs[2], SECBUFFER_EMPTY, 0, 0);
	SecBuffer_Set(&buffs[3], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc_Set(&buffDesc, buffs, 4);
	status = DecryptMessage(&this->clsData->ctxt, &buffDesc, 0, 0);
	if (status == SEC_E_INCOMPLETE_MESSAGE)
	{
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s EndRead Incomplete Message, ofst = %d, toWait = %d\r\n", debugBuff, (UInt32)this->clsData->recvOfst, toWait?1:0);
#endif
		if (!toWait)
		{
			Net::SocketFactory::ErrorType et;
			void *data = sockf->BeginReceiveData(s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, this->clsData->readEvt, et);
			if (data == 0)
			{
				if (et == Net::SocketFactory::ET_SHUTDOWN)
				{
					this->flags |= 2;
				}
				else if (et == Net::SocketFactory::ET_DISCONNECT)
				{
					this->flags |= 2;
				}
				else
				{
					this->Close();
				}
			}
			else
			{
				this->clsData->readData = data;
			}
			incomplete.Set(true);
			return 0;
		}
		while (status == SEC_E_INCOMPLETE_MESSAGE)
		{
			UOSInt recvSize = this->sockf->ReceiveData(this->s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, et);
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s EndRead Recv size2 = %d\r\n", debugBuff, (UInt32)recvSize);
#endif
			if (recvSize <= 0)
			{
				incomplete.Set(false);
				this->flags |= 2;
				return ret;
			}
			this->clsData->recvOfst += recvSize;
			SecBuffer_Set(&buffs[0], SECBUFFER_DATA, this->clsData->recvBuff, (UInt32)this->clsData->recvOfst);
			SecBuffer_Set(&buffs[1], SECBUFFER_EMPTY, 0, 0);
			SecBuffer_Set(&buffs[2], SECBUFFER_EMPTY, 0, 0);
			SecBuffer_Set(&buffs[3], SECBUFFER_EMPTY, 0, 0);
			SecBufferDesc_Set(&buffDesc, buffs, 4);
			status = DecryptMessage(&this->clsData->ctxt, &buffDesc, 0, 0);
		}
	}

	if (status != SEC_E_OK && status != SEC_I_RENEGOTIATE)
	{
		this->flags |= 6;
		this->sockf->DestroySocket(this->s);
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s EndRead Return size2 = %d\r\n", debugBuff, (UInt32)ret);
#endif
		incomplete.Set(false);
		return ret;
	}

#if defined(DEBUG_PRINT)
	debugDt.SetCurrTime();
	debugDt.ToString(debugBuff, "HH:mm:ss.fff");
	printf("%s EndRead Decrypted\r\n", debugBuff);
#endif

	this->clsData->recvOfst = 0;
	UOSInt i = 0;
	while (i < 4)
	{
		if (buffs[i].BufferType == SECBUFFER_DATA)
		{
			if (buffs[i].cbBuffer <= this->clsData->readSize)
			{
#if defined(DEBUG_PRINT)
				debugDt.SetCurrTime();
				debugDt.ToString(debugBuff, "HH:mm:ss.fff");
				printf("%s EndRead Dec size1 = %d, size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer, (UInt32)this->clsData->readSize);
#endif
				MemCopyNO(this->clsData->readBuff, buffs[i].pvBuffer, buffs[i].cbBuffer);
				this->clsData->readSize -= buffs[i].cbBuffer;
				this->clsData->readBuff += buffs[i].cbBuffer;
				ret += buffs[i].cbBuffer;
			}
			else
			{
				if (this->clsData->readSize > 0)
				{
#if defined(DEBUG_PRINT)
					debugDt.SetCurrTime();
					debugDt.ToString(debugBuff, "HH:mm:ss.fff");
					printf("%s EndRead Dec size2 = %d, size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer, (UInt32)this->clsData->readSize);
#endif
					MemCopyNO(this->clsData->readBuff, buffs[i].pvBuffer, this->clsData->readSize);
					ret += this->clsData->readSize;
					MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], this->clsData->readSize + (UInt8*)buffs[i].pvBuffer, buffs[i].cbBuffer - this->clsData->readSize);
					this->clsData->decSize += buffs[i].cbBuffer - this->clsData->readSize;
					this->clsData->readSize = 0;
				}
				else
				{
#if defined(DEBUG_PRINT)
					debugDt.SetCurrTime();
					debugDt.ToString(debugBuff, "HH:mm:ss.fff");
					printf("%s EndRead Dec size3 = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer);
#endif
					MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], buffs[i].pvBuffer, buffs[i].cbBuffer);
					this->clsData->decSize += buffs[i].cbBuffer;
				}
			}
		}
		else if (buffs[i].BufferType == SECBUFFER_EXTRA)
		{
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s EndRead Ext size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer);
#endif
			MemCopyO(&this->clsData->recvBuff[this->clsData->recvOfst], buffs[i].pvBuffer, buffs[i].cbBuffer);
			this->clsData->recvOfst += buffs[i].cbBuffer;
		}
		i++;
	}
#if defined(DEBUG_PRINT)
	debugDt.SetCurrTime();
	debugDt.ToString(debugBuff, "HH:mm:ss.fff");
	printf("%s EndRead Return size3 = %d\r\n", debugBuff, (UInt32)ret);
#endif
	incomplete.Set(false);
	return ret;
}

void Net::WinSSLClient::CancelRead(void *reqData)
{

}

void *Net::WinSSLClient::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt ret = this->Write(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::WinSSLClient::EndWrite(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
}

void Net::WinSSLClient::CancelWrite(void *reqData)
{

}

Int32 Net::WinSSLClient::Flush()
{
	return 0;
}

void Net::WinSSLClient::Close()
{
	if (this->s)
	{
		//SSL_shutdown(this->clsData->ssl);
	}
	this->Net::TCPClient::Close();
}

Bool Net::WinSSLClient::Recover()
{
	return false;
}

Optional<Crypto::Cert::Certificate> Net::WinSSLClient::GetRemoteCert()
{
	if (this->clsData->remoteCerts)
		return this->clsData->remoteCerts->GetItem(0);
	else
		return 0;
}

Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> Net::WinSSLClient::GetRemoteCerts()
{
	return this->clsData->remoteCerts;
}
