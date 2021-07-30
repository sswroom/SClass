#include "Stdafx.h"
#include "Net/WinSSLClient.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define SECURITY_WIN32
#include <sspi.h>

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

Net::WinSSLClient::WinSSLClient(Net::SocketFactory *sockf, UInt32 *s, void *hCred, const UTF8Char *hostName, Bool skipCertCheck) : TCPClient(sockf, s)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->hCred = (CredHandle*)hCred;
	this->clsData->step = 0;
	this->clsData->recvBuff = 0;
	this->clsData->decBuff = 0;

	const WChar *wptr = Text::StrToWCharNew(hostName);
	UInt32 retFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;
	TimeStamp ts;
	SecBuffer outputBuff[3];
	SecBuffer_Set(&outputBuff[0], SECBUFFER_EMPTY, 0, 0);
	SecBufferDesc inputDesc;
	SecBufferDesc outputDesc;
	SecBufferDesc_Set(&outputDesc, outputBuff, 1);

	if (skipCertCheck)
	{
		retFlags |= ISC_REQ_MANUAL_CRED_VALIDATION;
	}

	SECURITY_STATUS status;
	status = InitializeSecurityContext(
		this->clsData->hCred,
		0,
		(WChar*)wptr,
		(unsigned long)retFlags,
		0,
		0,
		0,
		0,
		&this->clsData->ctxt,
		&outputDesc,
		(unsigned long*)&retFlags,
		&ts
		);
	if (status != SEC_I_CONTINUE_NEEDED)
	{
		Text::StrDelNew(wptr);
		return;
	}
	Net::SocketFactory::ErrorType et;
	if (this->sockf->SendData(s, (UInt8*)outputBuff[0].pvBuffer, outputBuff[0].cbBuffer, &et) != outputBuff[0].cbBuffer)
	{
		FreeContextBuffer(outputBuff[0].pvBuffer);
		Text::StrDelNew(wptr);
		return;
	}
	FreeContextBuffer(outputBuff[0].pvBuffer);

	this->sockf->SetRecvTimeout(s, 3000);
	SecBuffer inputBuff[2];
	UInt8 recvBuff[2048];
	UOSInt recvOfst = 0;
	UOSInt recvSize;
	UOSInt i;
	while (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_INCOMPLETE_MESSAGE)
	{
		if (recvOfst == 0 || status == SEC_E_INCOMPLETE_MESSAGE)
		{
			recvSize = this->sockf->ReceiveData(s, &recvBuff[recvOfst], 2048 - recvOfst, &et);
			if (recvSize <= 0)
			{
				Text::StrDelNew(wptr);
				return;
			}
			recvOfst += recvSize;
		}

		SecBuffer_Set(&inputBuff[0], SECBUFFER_TOKEN, recvBuff, (UInt32)recvOfst);
		SecBuffer_Set(&inputBuff[1], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&inputDesc, inputBuff, 2);

		SecBuffer_Set(&outputBuff[0], SECBUFFER_TOKEN, 0, 0);
#if defined(SECBUFFER_ALERT)
		SecBuffer_Set(&outputBuff[1], SECBUFFER_ALERT, 0, 0);
#else
		SecBuffer_Set(&outputBuff[1], SECBUFFER_EMPTY, 0, 0);
#endif
		SecBuffer_Set(&outputBuff[2], SECBUFFER_EMPTY, 0, 0);
		SecBufferDesc_Set(&outputDesc, outputBuff, 3);

		status = InitializeSecurityContext(
			this->clsData->hCred,
			&this->clsData->ctxt,
			(WChar*)wptr,
			retFlags,
			0,
			0,
			&inputDesc,
			0,
			0,
			&outputDesc,
			(unsigned long*)&retFlags,
			&ts);

		if (status == SEC_E_INCOMPLETE_MESSAGE)
		{

		}
		else if (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_OK)
		{
			Bool succ = true;
			i = 0;
			while (i < 3)
			{
				if (outputBuff[i].BufferType == SECBUFFER_TOKEN && outputBuff[i].cbBuffer > 0)
				{
					if (this->sockf->SendData(s, (const UInt8*)outputBuff[i].pvBuffer, outputBuff[i].cbBuffer, &et) != outputBuff[i].cbBuffer)
					{
						succ = false;
					}
				}

				if (outputBuff[i].pvBuffer)
				{
					FreeContextBuffer(outputBuff[i].pvBuffer);
				}
				i++;
			}
			if (!succ)
			{
				Text::StrDelNew(wptr);
				return;
			}
			if (inputBuff[1].BufferType == SECBUFFER_EXTRA)
			{
				MemCopyNO(recvBuff, inputBuff[1].pvBuffer, inputBuff[1].cbBuffer);
				recvOfst = inputBuff[1].cbBuffer;
			}
			else
			{
				recvOfst = 0;
			}
		}
		else
		{
			if (status == SEC_I_INCOMPLETE_CREDENTIALS)
			{

			}
			Text::StrDelNew(wptr);
			return;
		}
	}

	Text::StrDelNew(wptr);
	this->clsData->step = 1;
	MemClear(&this->clsData->stmSizes, sizeof(this->clsData->stmSizes));
	status = QueryContextAttributes(&this->clsData->ctxt, SECPKG_ATTR_STREAM_SIZES, &this->clsData->stmSizes);
	this->clsData->recvBuffSize = this->clsData->stmSizes.cbHeader + this->clsData->stmSizes.cbMaximumMessage + this->clsData->stmSizes.cbTrailer;
	this->clsData->recvBuff = MemAlloc(UInt8, this->clsData->recvBuffSize);
	this->clsData->recvOfst = 0;
	this->clsData->decBuff = MemAlloc(UInt8, this->clsData->stmSizes.cbMaximumMessage);
	this->clsData->decSize = 0;
}

Net::WinSSLClient::~WinSSLClient()
{
	//SSL_free(this->clsData->ssl);
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
			return size;
		}
		MemCopyNO(buff, this->clsData->decBuff, this->clsData->decSize);
		ret = this->clsData->decSize;
		buff += ret;
		size -= ret;
		this->clsData->decSize = 0;
	}
	if (s && (this->flags & 6) == 0)
	{
		Net::SocketFactory::ErrorType et;
		SecBufferDesc buffDesc;
		SecBuffer buffs[4];
		SECURITY_STATUS status = SEC_E_INCOMPLETE_MESSAGE;
		while (status == SEC_E_INCOMPLETE_MESSAGE)
		{
			UOSInt recvSize = this->sockf->ReceiveData(this->s, &this->clsData->recvBuff[this->clsData->recvOfst], this->clsData->recvBuffSize - this->clsData->recvOfst, &et);
			if (recvSize <= 0)
			{
				this->flags |= 2;
				return ret;
			}
			this->clsData->recvOfst += recvSize;
			SecBuffer_Set(&buffs[0], SECBUFFER_DATA, this->clsData->recvBuff, (UInt32)this->clsData->recvBuffSize);
			SecBuffer_Set(&buffs[1], SECBUFFER_EMPTY, 0, 0);
			SecBuffer_Set(&buffs[2], SECBUFFER_EMPTY, 0, 0);
			SecBuffer_Set(&buffs[3], SECBUFFER_EMPTY, 0, 0);
			SecBufferDesc_Set(&buffDesc, buffs, 4);
			status = DecryptMessage(&this->clsData->ctxt, &buffDesc, 0, 0);
		}

		if (status != SEC_E_OK && status != SEC_I_RENEGOTIATE)
		{
			this->flags |= 2;
			this->sockf->DestroySocket(this->s);
			return ret;
		}

		UOSInt i = 0;
		while (i < 4)
		{
			if (buffs[i].BufferType == SECBUFFER_DATA)
			{
				if (buffs[i].cbBuffer <= size)
				{
					MemCopyNO(buff, buffs[i].pvBuffer, buffs[i].cbBuffer);
					size -= buffs[i].cbBuffer;
					buff += buffs[i].cbBuffer;
					ret += buffs[i].cbBuffer;
				}
				else
				{
					if (size > 0)
					{
						MemCopyNO(buff, buffs[i].pvBuffer, size);
						ret += size;
						MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], size + (UInt8*)buffs[i].pvBuffer, buffs[i].cbBuffer - size);
						this->clsData->decSize += buffs[i].cbBuffer - size;
						size = 0;
					}
					else
					{
						MemCopyNO(&this->clsData->decBuff[this->clsData->decSize], buffs[i].pvBuffer, buffs[i].cbBuffer);
						this->clsData->decSize += buffs[i].cbBuffer;
					}
				}
			}
			i++;
		}
		return ret;
	}
	else
	{
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
		UOSInt writeSize = 0;
		while (size > this->clsData->stmSizes.cbMaximumMessage)
		{
			writeSize += Write(buff, this->clsData->stmSizes.cbMaximumMessage);
			buff += this->clsData->stmSizes.cbMaximumMessage;
			size -= this->clsData->stmSizes.cbMaximumMessage;
		}
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
			this->flags |= 1;
			return 0;
		}
		Net::SocketFactory::ErrorType et;
		UOSInt ret = this->sockf->SendData(this->s, encBuff, outputBuff[0].cbBuffer + outputBuff[1].cbBuffer + outputBuff[2].cbBuffer, &et);// SSL_write(this->clsData->ssl, buff, (int)size);
		if (ret > outputBuff[0].cbBuffer + outputBuff[2].cbBuffer)
		{
			ret -= outputBuff[0].cbBuffer + outputBuff[2].cbBuffer;
			this->currCnt += ret;
			return ret;
		}
		this->flags |= 1;
		return 0;
	}
	else
	{
		return 0;
	}
}

void *Net::WinSSLClient::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt ret = this->Read(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::WinSSLClient::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	*incomplete = false;
	return (UOSInt)reqData;
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
