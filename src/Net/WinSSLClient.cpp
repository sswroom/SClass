#include "Stdafx.h"
#include "Net/WinSSLClient.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define SECURITY_WIN32
#include <sspi.h>

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
};

void SecBuffer_Set(SecBuffer *buff, UInt32 type, UInt8 *inpBuff, UInt32 leng)
{
	buff->BufferType = type;
	buff->pvBuffer = inpBuff;
	buff->cbBuffer = leng;
}

void SecBufferDesc_Set(SecBufferDesc *desc, SecBuffer *buffs, UInt32 nBuffs)
{
	desc->ulVersion = SECBUFFER_VERSION;
	desc->pBuffers = buffs;
	desc->cBuffers = nBuffs;
}

Net::WinSSLClient::WinSSLClient(Net::SocketFactory *sockf, Socket *s, void *ctxt) : SSLClient(sockf, s)
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
	this->clsData->recvBuffSize = this->clsData->stmSizes.cbHeader + this->clsData->stmSizes.cbMaximumMessage + this->clsData->stmSizes.cbTrailer;
	this->clsData->recvBuff = MemAlloc(UInt8, this->clsData->recvBuffSize);
	this->clsData->recvOfst = 0;
	this->clsData->decBuff = MemAlloc(UInt8, this->clsData->stmSizes.cbMaximumMessage);
	this->clsData->decSize = 0;
	this->clsData->readBuff = 0;
	this->clsData->readSize = 0;
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
	MemFree(this->clsData);
}

UOSInt Net::WinSSLClient::Read(UInt8 *buff, UOSInt size)
{
	if (this->clsData->step == 0)
	{
		return 0;
	}
#if defined(DEBUG_PRINT)
	UTF8Char debugBuff[64];
	Data::DateTime debugDt;
#endif
	UOSInt ret = 0;
	if (this->clsData->decSize > 0)
	{
		if (this->clsData->decSize >= size)
		{
			MemCopyNO(buff, this->clsData->decBuff, size);
			if (this->clsData->decSize > size)
			{
				MemCopyO(this->clsData->decBuff, &this->clsData->decBuff[size], this->clsData->decSize - size);
				this->clsData->decSize -= size;
			}
			else
			{
				this->clsData->decSize = 0;
			}
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s Return size1 = %d, \r\n", debugBuff, (UInt32)size);
#endif
			return size;
		}
		MemCopyNO(buff, this->clsData->decBuff, this->clsData->decSize);
		ret = this->clsData->decSize;
		this->clsData->decSize = 0;
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Return size0 = %d, \r\n", debugBuff, (UInt32)ret);
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
			UOSInt recvSize = this->sockf->ReceiveData(this->s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, &et);
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
				if (buffs[i].cbBuffer <= size)
				{
#if defined(DEBUG_PRINT)
					debugDt.SetCurrTime();
					debugDt.ToString(debugBuff, "HH:mm:ss.fff");
					printf("%s Dec size1 = %d, size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer, (UInt32)size);
#endif
					MemCopyNO(buff, buffs[i].pvBuffer, buffs[i].cbBuffer);
					size -= buffs[i].cbBuffer;
					buff += buffs[i].cbBuffer;
					ret += buffs[i].cbBuffer;
				}
				else
				{
					if (size > 0)
					{
#if defined(DEBUG_PRINT)
						debugDt.SetCurrTime();
						debugDt.ToString(debugBuff, "HH:mm:ss.fff");
						printf("%s Dec size2 = %d, size = %d\r\n", debugBuff, (UInt32)buffs[i].cbBuffer, (UInt32)size);
#endif
						MemCopyNO(buff, buffs[i].pvBuffer, size);
						ret += size;
						MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], size + (UInt8*)buffs[i].pvBuffer, buffs[i].cbBuffer - size);
						this->clsData->decSize += buffs[i].cbBuffer - size;
						size = 0;
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
		UOSInt ret = this->sockf->SendData(this->s, encBuff, outputBuff[0].cbBuffer + outputBuff[1].cbBuffer + outputBuff[2].cbBuffer, &et);// SSL_write(this->clsData->ssl, buff, (int)size);
#if defined(DEBUG_PRINT)
		debugDt.SetCurrTime();
		debugDt.ToString(debugBuff, "HH:mm:ss.fff");
		printf("%s Sent %d bytes\r\n", debugBuff, (UInt32)ret);
#endif
		MemFree(encBuff);
		if (ret > outputBuff[0].cbBuffer + outputBuff[2].cbBuffer)
		{
			ret -= outputBuff[0].cbBuffer + outputBuff[2].cbBuffer;
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

void *Net::WinSSLClient::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	if (this->clsData->step == 0)
	{
		return 0;
	}
	this->clsData->readBuff = buff;
	this->clsData->readSize = size;
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
	void *data = sockf->BeginReceiveData(s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, evt, &et);
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
	return data;
}

UOSInt Net::WinSSLClient::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	if (reqData == 0)
	{
		*incomplete = false;
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
	Net::SocketFactory::ErrorType et;
	SecBufferDesc buffDesc;
	SecBuffer buffs[4];
	SECURITY_STATUS status = SEC_E_INCOMPLETE_MESSAGE;
	UOSInt recvSize;
	Bool incomp;

	status = SEC_E_INCOMPLETE_MESSAGE;
	recvSize = sockf->EndReceiveData(reqData, toWait, &incomp);
	if (recvSize <= 0)
	{
		if (incomplete)
		{
			*incomplete = incomp;
		}
		if (incomp)
		{
			return 0;
		}
		this->flags |= 2;
		return ret;
	}

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
		if (!toWait)
		{
			if (incomplete)
			{
				*incomplete = true;
			}
			return 0;
		}
		while (status == SEC_E_INCOMPLETE_MESSAGE)
		{
			UOSInt recvSize = this->sockf->ReceiveData(this->s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, &et);
			if (recvSize <= 0)
			{
				if (incomplete)
				{
					*incomplete = false;
				}
				this->flags |= 2;
				return ret;
			}
#if defined(DEBUG_PRINT)
			debugDt.SetCurrTime();
			debugDt.ToString(debugBuff, "HH:mm:ss.fff");
			printf("%s EndRead Recv size2 = %d\r\n", debugBuff, (UInt32)recvSize);
#endif
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
		if (incomplete)
		{
			*incomplete = false;
		}
		return ret;
	}

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
	if (incomplete)
	{
		*incomplete = false;
	}
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

Crypto::Cert::Certificate *Net::WinSSLClient::GetRemoteCert()
{
	return 0;
}
