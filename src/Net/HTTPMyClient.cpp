#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPMyClient.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/URIEncoding.h"

//#define LOGREPLY
//#define SHOWDEBUG
//#define DEBUGSPEED
#if defined(SHOWDEBUG) || defined(DEBUGSPEED)
#include <stdio.h>
#endif

#if defined(LOGREPLY)
#include "IO/FileStream.h"
#include "IO/Path.h"
struct Net::HTTPMyClient::ClassData
{
	IO::FileStream *fs;
};

#endif

#define BUFFSIZE 8192

Net::HTTPMyClient::HTTPMyClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn), reqMstm(1024)
{
	if (userAgent.v == 0)
	{
		userAgent = CSTR("sswr/1.0");
	}
#if defined(LOGREPLY)
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	this->clsData = MemAlloc(ClassData, 1);
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("HTTPClient_"));
	dt.SetCurrTimeUTC();
	sptr = Text::StrInt64(sptr, dt.ToTicks());
	sptr = Text::StrConcatC(sptr, UTF8STRC(".dat"));
	NEW_CLASS(this->clsData->fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
#endif
	this->ssl = ssl;
	this->cli = 0;
	this->cliHost = 0;
	this->writing = false;
	this->dataBuff = 0;
	this->buffSize = 0;
	this->buffOfst = 0;
	this->contEnc = 0;
	this->timeout = 120000;
	this->userAgent = Text::String::New(userAgent);
	this->dataBuff = MemAlloc(UInt8, BUFFSIZE);
}

Net::HTTPMyClient::~HTTPMyClient()
{
	if (this->cli)
	{
		this->sockf->SetLinger(this->cli->GetSocket(), 0);
		this->cli->ShutdownSend();
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	SDEL_STRING(this->cliHost);
	if (this->dataBuff)
	{
		MemFree(this->dataBuff);
		this->dataBuff = 0;
	}
	UOSInt i = this->reqHeaders.GetCount();
	while (i-- > 0)
	{
		this->reqHeaders.GetItem(i)->Release();
	}
	this->userAgent->Release();
#if defined(LOGREPLY)
	DEL_CLASS(this->clsData->fs);
	MemFree(this->clsData);
#endif
}

Bool Net::HTTPMyClient::IsError() const
{
	return this->cli == 0;
}

UOSInt Net::HTTPMyClient::ReadRAW(UInt8 *buff, UOSInt size)
{
	this->EndRequest(0, 0);
	if (this->respStatus == 0)
		return 0;

	if (size > BUFFSIZE)
	{
		size = BUFFSIZE;
	}

	if (size > (this->contLeng - this->contRead))
	{
		if ((size = (UOSInt)(this->contLeng - this->contRead)) <= 0)
		{
			return 0;
		}
	}
#ifdef SHOWDEBUG
	printf("Read size = %d\r\n", (Int32)size);
#endif

	if (this->contEnc == 1)
	{
		UOSInt i;
		UOSInt j;
		UOSInt sizeOut = 0;
#ifdef SHOWDEBUG
		printf("chunkSizeLeft = %d, buffSize = %d\r\n", (UInt32)chunkSizeLeft, (UInt32)buffSize);
#endif
		if (chunkSizeLeft > 0)
		{
			if (chunkSizeLeft > this->buffSize && BUFFSIZE - 1 - this->buffSize > 0)
			{
				if (this->cli == 0)
				{
					return 0;
				}
				i = cli->Read(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize);
				if (i == 0 && this->buffSize <= 0)
				{
#ifdef SHOWDEBUG
					printf("Return Read size(1) = %d\r\n", 0);
#endif
					if (this->cli->IsClosed())
					{
						DEL_CLASS(this->cli);
						this->cli = 0;
					}
					return 0;
				}
#ifdef SHOWDEBUG
				printf("Read from remote(1) = %d\r\n", (Int32)i);
#endif
#ifdef LOGREPLY
				if (i > 0)
				{
					this->clsData->fs->Write(&this->dataBuff[this->buffSize], i);
				}
#endif
				this->totalDownload += i;
				this->buffSize += i;
			}
			if (this->chunkSizeLeft <= 2)
			{
				while (this->chunkSizeLeft > this->buffSize)
				{
					i = cli->Read(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize);
					if (i == 0)
					{
#ifdef SHOWDEBUG
						printf("Return Read size(1.2) = %d\r\n", 0);
#endif
						if (this->cli->IsClosed())
						{
							DEL_CLASS(this->cli);
							this->cli = 0;
						}
						return 0;
					}
#ifdef SHOWDEBUG
					printf("Read from remote(1.2) = %d\r\n", (Int32)i);
#endif
#ifdef LOGREPLY
					if (i > 0)
					{
						this->clsData->fs->Write(&this->dataBuff[this->buffSize], i);
					}
#endif
					this->totalDownload += i;
					this->buffSize += i;
				}
				MemCopyO(this->dataBuff, &this->dataBuff[this->chunkSizeLeft], this->buffSize - this->chunkSizeLeft);
				this->buffSize -= this->chunkSizeLeft;
				this->chunkSizeLeft = 0;
			}
			else if (size >= this->buffSize)
			{
				sizeOut = this->buffSize;
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				MemCopyNO(buff, this->dataBuff, sizeOut);
				buff += sizeOut;
				size -= sizeOut;
				this->chunkSizeLeft -= sizeOut;
				this->buffSize -= sizeOut;
				if (this->buffSize > 0)
				{
					MemCopyO(this->dataBuff, &this->dataBuff[sizeOut], this->buffSize);
				}
				if (this->chunkSizeLeft < 2)
				{
					sizeOut -= (2 - this->chunkSizeLeft);
				}
#ifdef SHOWDEBUG
				printf("Return read size(2) = %d\r\n", (Int32)sizeOut);
#endif
				return sizeOut;
			}
			else
			{
				sizeOut = size;
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				MemCopyNO(buff, this->dataBuff, sizeOut);
				buff += sizeOut;
				size -= sizeOut;
				this->chunkSizeLeft -= sizeOut;
				this->buffSize -= sizeOut;
				if (this->buffSize > 0)
				{
					MemCopyO(this->dataBuff, &this->dataBuff[sizeOut], this->buffSize);
				}
				if (this->chunkSizeLeft < 2)
				{
					sizeOut -= (2 - this->chunkSizeLeft);
				}
#ifdef SHOWDEBUG
				printf("Return read size(3) = %d\r\n", (Int32)sizeOut);
#endif
				return sizeOut;
			}
		}

		this->dataBuff[this->buffSize] = 0;
		if (this->dataBuff[0] == '\r' && this->dataBuff[1] == '\n')
		{
			MemCopyO(this->dataBuff, &this->dataBuff[2], this->buffSize - 2);
			buffSize -= 2;
		}
		if (buffSize <= 0)
		{
			i = cli->Read(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize);
			if (i == 0 && this->buffSize <= 0)
			{
#ifdef SHOWDEBUG
				printf("Return read size(4) = %d\r\n", 0);
#endif
				return 0;
			}
#ifdef SHOWDEBUG
			printf("Read from remote(2) = %d\r\n", (Int32)i);
#endif
#ifdef LOGREPLY
			if (i > 0)
			{
				this->clsData->fs->Write(&this->dataBuff[this->buffSize], i);
			}
#endif
			this->totalDownload += i;
			this->buffSize += i;
		}
		while (INVALID_INDEX == (i = Text::StrIndexOfC(this->dataBuff, this->buffSize, UTF8STRC("\r\n"))))
		{
			if (this->cli == 0)
			{
				return 0;
			}
			if ((BUFFSIZE - 1 - this->buffSize) == 0)
			{
#ifdef SHOWDEBUG
				printf("Return read size(5) = %d\r\n", 0);
#endif
				return 0;
			}
			i = cli->Read(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize);
			if (i == 0)
			{
#ifdef SHOWDEBUG
				printf("Return read size(6) = %d\r\n", 0);
#endif
				if (this->cli->IsClosed())
				{
					DEL_CLASS(this->cli);
					this->cli = 0;
				}
				return 0;
			}
#ifdef SHOWDEBUG
			printf("Read from remote(3) = %d\r\n", (Int32)i);
#endif
#ifdef LOGREPLY
			this->clsData->fs->Write(&this->dataBuff[this->buffSize], i);
#endif
			this->totalDownload += i;
			this->buffSize += i;
			this->dataBuff[this->buffSize] = 0;
			if (this->dataBuff[0] == '\r' && this->dataBuff[1] == '\n')
			{
				MemCopyO(this->dataBuff, &this->dataBuff[2], this->buffSize - 2);
				buffSize -= 2;
			}
		}
		this->dataBuff[i] = 0;
#ifdef SHOWDEBUG
		printf("Chunk size %s\r\n", this->dataBuff);
#endif
		j = Text::StrHex2UInt32C((Char*)this->dataBuff);
		if (j == 0 && i == 1 && this->dataBuff[0] == '0')
		{
			i = 3;
			if (this->buffSize >= 5 && this->dataBuff[3] == 13 && this->dataBuff[4] == 10)
			{
				i = 5;
			}
			this->buffSize -= i;
			if (this->buffSize)
			{
				MemCopyO(this->dataBuff, &this->dataBuff[i], this->buffSize);
			}
			this->contLeng = 0;
#ifdef SHOWDEBUG
			printf("Return read size(7) = %d\r\n", 0);
#endif
			return 0;
		}
		if (j == 0)
		{
			this->dataBuff[i] = 13;
#ifdef SHOWDEBUG
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(this->dataBuff, i, ' ', Text::LineBreakType::None);
			printf("Return read size(8) = %d, i = %d, (%s)\r\n", 0, (UInt32)i, sb.ToString());
#endif
			return 0;
		}
		this->chunkSizeLeft = j + 2;
#ifdef SHOWDEBUG
		printf("set chunkSizeLeft = %d\r\n", (UInt32)this->chunkSizeLeft);
#endif
		i += 2;
		if (this->buffSize == i)
		{
			this->buffSize = 0;
			i = cli->Read(this->dataBuff, BUFFSIZE - 1);
			if (i == 0)
			{
#ifdef SHOWDEBUG
				printf("Return read size(4.2) = %d\r\n", 0);
#endif
				return 0;
			}
#ifdef SHOWDEBUG
			printf("Read from remote(2.2) = %d\r\n", (Int32)i);
#endif
#ifdef LOGREPLY
			if (i > 0)
			{
				this->clsData->fs->Write(&this->dataBuff[this->buffSize], i);
			}
#endif
			this->totalDownload += i;
			this->buffSize += i;
			i = 0;
		}
		if (this->buffSize > i)
		{
			if (size >= this->buffSize - i)
			{
				sizeOut = this->buffSize - i;
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				MemCopyNO(buff, &this->dataBuff[i], sizeOut);
				buff += sizeOut;
				size -= sizeOut;
				this->chunkSizeLeft -= sizeOut;
				this->buffSize -= sizeOut + i;
				if (this->buffSize > 0)
				{
					MemCopyO(this->dataBuff, &this->dataBuff[sizeOut + i], this->buffSize);
				}
				if (this->chunkSizeLeft < 2)
				{
					sizeOut -= (2 - this->chunkSizeLeft);
				}
#ifdef SHOWDEBUG
				printf("Return read size(9) = %d\r\n", (Int32)sizeOut);
#endif
				return sizeOut;
			}
			else
			{
				sizeOut = size;
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				MemCopyNO(buff, &this->dataBuff[i], sizeOut);
				buff += sizeOut;
				size -= sizeOut;
				this->chunkSizeLeft -= sizeOut;
				this->buffSize -= sizeOut + i;
				if (this->buffSize > 0)
				{
					MemCopyO(this->dataBuff, &this->dataBuff[sizeOut + i], this->buffSize);
				}
				if (this->chunkSizeLeft < 2)
				{
					sizeOut -= (2 - this->chunkSizeLeft);
				}
#ifdef SHOWDEBUG
				printf("Return read size(10) = %d\r\n", (Int32)sizeOut);
#endif
				return sizeOut;
			}
		}
		else
		{
			this->buffSize = 0;
#ifdef SHOWDEBUG
			printf("Return read size(11) = %d\r\n", 0);
#endif
			return 0;
		}
	}
	else
	{
		if (this->buffSize == 0)
		{
			if (this->cli == 0)
			{
				return 0;
			}
			this->buffSize = cli->Read(this->dataBuff, size);
			this->totalDownload += this->buffSize;
#ifdef SHOWDEBUG
			printf("Read from remote(4) = %d\r\n", (Int32)this->buffSize);
/*			if (this->buffSize == 0)
			{
				printf("WSA Error code=0x%X\r\n", WSAGetLastError());
			}*/
#endif
#ifdef LOGREPLY
			if (this->buffSize > 0)
			{
				this->clsData->fs->Write(this->dataBuff, this->buffSize);
			}
#endif
			if (this->cli->IsClosed())
			{
				DEL_CLASS(this->cli);
				this->cli = 0;
			}
		}
		if (this->buffSize >= size)
		{
			MemCopyNO(buff, this->dataBuff, size);
			if (this->buffSize > size)
			{
				MemCopyO(this->dataBuff, &this->dataBuff[size], this->buffSize - size);
			}
			this->buffSize -= size;
			this->contRead += size;
#ifdef SHOWDEBUG
			printf("Return read size(12) = %d\r\n", (Int32)size);
#endif
			return size;
		}
		else
		{
			MemCopyNO(buff, this->dataBuff, this->buffSize);
			size = this->buffSize;
			this->contRead += this->buffSize;
			this->buffSize = 0;
#ifdef SHOWDEBUG
			printf("Return read size(13) = %d\r\n", (Int32)size);
#endif
			return size;
		}
	}
}

UOSInt Net::HTTPMyClient::Read(UInt8 *buff, UOSInt size)
{
	this->EndRequest(0, 0);
	if (this->respStatus == 0)
		return 0;

	return this->ReadRAW(buff, size);
}

UOSInt Net::HTTPMyClient::Write(const UInt8 *buff, UOSInt size)
{
	if (this->canWrite && !this->hasForm)
	{
		if (!writing)
		{
			//cli->Write((UInt8*)"\r\n", 2);
			this->reqMstm.Write((UInt8*)"\r\n", 2);
		}
		writing = true;
		//return cli->Write(buff, size);
		return this->reqMstm.Write(buff, size);
	}
	return 0;
}

Int32 Net::HTTPMyClient::Flush()
{
	return 0;
}

void Net::HTTPMyClient::Close()
{
	if (this->cli)
	{
		this->cli->ShutdownSend();
		this->cli->Close();
	}
}

Bool Net::HTTPMyClient::Recover()
{
	//////////////////////////////////////////
	return false;
}

Bool Net::HTTPMyClient::Connect(Text::CString url, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UOSInt urltmpLen;
	UTF8Char svrname[256];
	UTF8Char *svrnameEnd;
	UTF8Char host[256];

	UOSInt i;
	UOSInt hostLen;
	Text::CString ptr1;
	Text::CString ptr2;
	Text::PString ptrs[2];
	UTF8Char *cptr;
	UInt16 port;
	Bool secure = false;
	this->hdrLen = 0;

	this->url->Release();
	this->url = Text::String::New(url);
	if (url.StartsWith(UTF8STRC("http://")))
	{
		ptr1 = url.Substring(7);
		secure = false;
	}
	else if (url.StartsWith(UTF8STRC("https://")))
	{
		ptr1 = url.Substring(8);
		secure = true;
	}
	else
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}

	if (secure && this->ssl == 0)
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}

	this->SetSourceName(url);
#ifdef SHOWDEBUG
	printf("Request URL: %s %s\r\n", Net::WebUtil::RequestMethodGetName(method).v, url.v);
#endif
	i = ptr1.IndexOf('/');
	if (i != INVALID_INDEX)
	{
		MemCopyNO(urltmp, ptr1.v, i * sizeof(UTF8Char));
		urltmp[i] = 0;
		ptr2 = ptr1.Substring(i);
	}
	else
	{
		ptr2 = CSTR_NULL;
		ptr1.ConcatTo(urltmp);
	}
	cptr = Text::TextBinEnc::URIEncoding::URIDecode(urltmp, urltmp);
	urltmpLen = (UOSInt)(cptr - urltmp);
	cptr = Text::StrConcatC(host, UTF8STRC("Host: "));
	cptr = Text::StrConcatC(cptr, urltmp, urltmpLen);
	cptr = Text::StrConcatC(cptr, UTF8STRC("\r\n"));
	hostLen = (UOSInt)(cptr - host);
	if (urltmp[0] == '[')
	{
		i = Text::StrIndexOfCharC(urltmp, urltmpLen, ']');
		if (i == INVALID_INDEX)
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		svrnameEnd = Text::StrConcatC(svrname, &urltmp[1], i - 1);
		if (urltmp[i + 1] == ':')
		{
			Text::StrToUInt16S(&urltmp[i + 2], &port, 0);
			urltmp[i + 1] = 0;
			urltmpLen = i + 1;
		}
		else
		{
			if (secure)
			{
				port = 443;
			}
			else
			{
				port = 80;
			}
		}
	}
	else
	{
		i = Text::StrSplitP(ptrs, 2, {urltmp, urltmpLen}, ':');
		if (i == 2)
		{
			Text::StrToUInt16S(ptrs[1].v, &port, 0);
			svrnameEnd = Text::StrConcatC(svrname, ptrs[0].v, ptrs[0].leng);
			urltmpLen = ptrs[0].leng;
		}
		else
		{
			if (secure)
			{
				port = 443;
			}
			else
			{
				port = 80;
			}
			svrnameEnd = Text::StrConcatC(svrname, ptrs[0].v, ptrs[0].leng);
		}
	}

	this->clk.Start();
	if (this->cliHost == 0)
	{
		this->cliHost = Text::String::New(urltmp, urltmpLen).Ptr();

		Double t1;
		Net::SocketUtil::AddressInfo addr;
		if (Text::StrEqualsICaseC(svrname, (UOSInt)(svrnameEnd - svrname), UTF8STRC("localhost")))
		{
			addr.addrType = Net::AddrType::IPv4;
			WriteNUInt32(addr.addr, Net::SocketUtil::GetIPAddr(CSTR("127.0.0.1")));
		}
		else if (!sockf->DNSResolveIP(CSTRP(svrname, svrnameEnd), &addr))
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		if (timeDNS)
		{
			*timeDNS = this->clk.GetTimeDiff();
		}
		this->svrAddr = addr;
		if (addr.addrType != Net::AddrType::Unknown)
		{
#if defined(SHOWDEBUG)
			Net::SocketUtil::GetAddrName(svrname, &this->svrAddr);
			printf("Server IP: %s:%d, t = %d\r\n", svrname, port, (Int32)this->svrAddr.addrType);
#endif
			if (secure)
			{
				Net::SSLEngine::ErrorType err;
				this->cli = this->ssl->ClientConnect(this->cliHost->ToCString(), port, &err, this->timeout);
#if defined(SHOWDEBUG)
				if (this->cli == 0)
				{
					printf("Connect error: %s\r\n", Net::SSLEngine::ErrorTypeGetName(err).v);
				}
#endif
			}
			else
			{
				NEW_CLASS(this->cli, Net::TCPClient(sockf, &this->svrAddr, port, this->timeout));
			}
		}
		else
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		t1 = this->clk.GetTimeDiff();
		if (timeConn)
		{
			*timeConn = t1;
		}
#ifdef DEBUGSPEED
		if (t1 > 0.01)
		{
			printf("Time in connect: %lf\n", t1);
		}
#endif

		if (this->cli == 0)
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		else if (this->cli->IsConnectError())
		{
#if defined(SHOWDEBUG)
			printf("Error in connect to server\r\n");
#endif
			DEL_CLASS(this->cli);
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		this->sockf->SetLinger(this->cli->GetSocket(), 0);
		this->sockf->SetNoDelay(this->cli->GetSocket(), true);
	}
	else if (Text::StrEqualsC(this->cliHost->v, this->cliHost->leng, urltmp, urltmpLen))
	{
		if (this->buffSize > 0)
		{
			this->contRead += this->buffSize;
			this->buffSize = 0;
		}
		while (this->contRead != this->contLeng)
		{
			UOSInt size = BUFFSIZE;
			if (size > (this->contLeng - this->contRead))
			{
				size = (UOSInt)(this->contLeng - this->contRead);
			}
			size = this->cli->Read(this->dataBuff, size);
			this->totalDownload += size;
#ifdef SHOWDEBUG
			printf("Read from remote(5), size = %d\r\n", (Int32)size);
#endif
			if (size == 0)
			{
				if (timeDNS)
				{
					*timeDNS = -1;
				}
				if (timeConn)
				{
					*timeConn = -1;
				}
				return false;
			}
#ifdef LOGREPLY
			this->clsData->fs->Write(this->dataBuff, size);
#endif
			this->contRead += size;
		}
		if (timeDNS)
		{
			*timeDNS = 0;
		}
		if (timeConn)
		{
			*timeConn = 0;
		}
		this->contRead = 0;
		i = this->headers.GetCount();
		while (i-- > 0)
		{
			this->headers.RemoveAt(i)->Release();
		}
		this->headers.Clear();
		LIST_FREE_STRING(&this->reqHeaders);
		this->reqHeaders.Clear();
	}
	else
	{
		if (timeDNS)
		{
			*timeDNS = -1;
		}
		if (timeConn)
		{
			*timeConn = -1;
		}
		return false;
	}

	if (ptr2.v == 0)
	{
		ptr2 = CSTR("/");
	}
	i = ptr2.leng;
	if ((i + 16) > BUFFSIZE)
	{
		MemFree(this->dataBuff);
		this->dataBuff = MemAlloc(UInt8, (i + 16));
	}
	switch (method)
	{
	case Net::WebUtil::RequestMethod::HTTP_POST:
		this->canWrite = true;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("POST "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
		break;
	case Net::WebUtil::RequestMethod::HTTP_PUT:
		this->canWrite = true;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("PUT "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
		break;
	case Net::WebUtil::RequestMethod::HTTP_PATCH:
		this->canWrite = true;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("PATCH "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
		break;
	case Net::WebUtil::RequestMethod::HTTP_DELETE:
		this->canWrite = true;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("DELETE "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
		break;
	case Net::WebUtil::RequestMethod::HTTP_CONNECT:
	case Net::WebUtil::RequestMethod::Unknown:
	case Net::WebUtil::RequestMethod::HTTP_GET:
		this->canWrite = false;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("GET "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
		break;

	case Net::WebUtil::RequestMethod::RTSP_DESCRIBE:
	case Net::WebUtil::RequestMethod::RTSP_ANNOUNCE:
	case Net::WebUtil::RequestMethod::RTSP_GET_PARAMETER:
	case Net::WebUtil::RequestMethod::RTSP_OPTIONS:
	case Net::WebUtil::RequestMethod::RTSP_PAUSE:
	case Net::WebUtil::RequestMethod::RTSP_PLAY:
	case Net::WebUtil::RequestMethod::RTSP_RECORD:
	case Net::WebUtil::RequestMethod::RTSP_REDIRECT:
	case Net::WebUtil::RequestMethod::RTSP_SETUP:
	case Net::WebUtil::RequestMethod::RTSP_SET_PARAMETER:
	case Net::WebUtil::RequestMethod::RTSP_TEARDOWN:
		this->canWrite = false;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("TEARDOWN "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" RTSP/1.0\r\n"));
		break;
	}
	this->reqMstm.Write(dataBuff, (UOSInt)(cptr - (UTF8Char*)dataBuff));
	this->reqMstm.Write((UInt8*)host, hostLen);

	if (defHeaders)
	{
		this->AddHeaderC(CSTR("User-Agent"), this->userAgent->ToCString());
		this->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
		this->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
		if (this->kaConn)
		{
			this->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
		}
		else
		{
			this->AddHeaderC(CSTR("Connection"), CSTR("close"));
		}
	}
	return true;
}

void Net::HTTPMyClient::AddHeaderC(Text::CString name, Text::CString value)
{
	UInt8 buff[512];
	UTF8Char *sptr;
	if (this->reqHeaders.SortedIndexOfPtr(name.v, name.leng) >= 0)
		return;

	if (this->cli && !this->writing)
	{
		if (name.leng + value.leng + 5 > 512)
		{
			this->reqMstm.Write(name.v, name.leng);
			this->reqMstm.Write(UTF8STRC(": "));
			this->reqMstm.Write(value.v, value.leng);
			this->reqMstm.Write(UTF8STRC("\r\n"));
#ifdef SHOWDEBUG
			printf("Add Header: %s: %s\r\n", name.v, value.v);
#endif
		}
		else
		{
			sptr = buff;
			MemCopyNO(sptr, name.v, name.leng);
			sptr += name.leng;
			WriteNUInt16(sptr, ReadNUInt16((const UInt8*)": "));
			sptr += 2;
			MemCopyNO(sptr, value.v, value.leng);
			sptr += value.leng;
			WriteNUInt16(sptr, ReadNUInt16((const UInt8*)"\r\n"));
			sptr += 2;
#ifdef SHOWDEBUG
			*sptr = 0;
			printf("Add Header: %s", buff);
#endif
			this->reqMstm.Write(buff, (UOSInt)(sptr - (UTF8Char*)buff));
		}
		this->reqHeaders.SortedInsert(Text::String::New(name).Ptr());
	}
}

void Net::HTTPMyClient::EndRequest(Double *timeReq, Double *timeResp)
{
	if ((this->writing && !this->canWrite) || this->cli == 0)
	{
		if (timeReq)
		{
			*timeReq = -1;
		}
		if (timeResp)
		{
			*timeResp = -1;
		}
		return;
	}
	else
	{
		Double t1;
#ifdef SHOWDEBUG
		printf("End Request begin\r\n");
#endif
		if (this->hasForm)
		{
			UOSInt len = this->formSb->GetLength();
			this->AddContentLength(len);
			this->hasForm = false;
			this->Write((UInt8*)this->formSb->ToString(), len);
			DEL_CLASS(this->formSb);
			this->formSb = 0;
		}
		this->canWrite = false;
		this->writing = true;

		this->reqMstm.Write((UInt8*)"\r\n", 2);
		UOSInt reqSize;
		UOSInt writeSize = 0;
		UOSInt currSize = 0;
		UInt8 *reqBuff = this->reqMstm.GetBuff(&reqSize);
		while (writeSize < reqSize)
		{
			currSize = this->cli->Write(&reqBuff[writeSize], reqSize - writeSize);
#ifdef SHOWDEBUG
			printf("Writing %d bytes, sent %d bytes\r\n", (UInt32)(reqSize - writeSize), (UInt32)currSize);
#endif
			if (currSize <= 0)
				break;
			this->totalUpload += currSize;
			writeSize += currSize;
		}
		this->reqMstm.Clear();

		this->sockf->SetLinger(cli->GetSocket(), 0);
		if (!this->kaConn && !this->cli->IsSSL())
			this->cli->ShutdownSend();
		this->cli->SetTimeout(this->timeout);
		t1 = this->clk.GetTimeDiff();
		if (timeReq)
		{
			*timeReq = t1;
		}

		this->buffSize = 0;
		while (this->buffSize < 32)
		{
			UOSInt recvSize = cli->Read(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize);
#ifdef SHOWDEBUG
			printf("Read from remote(6) = %d\r\n", (Int32)recvSize);
#endif
#ifdef LOGREPLY
			if (recvSize > 0)
			{
				this->clsData->fs->Write(&this->dataBuff[this->buffSize], recvSize);
			}
#endif
			this->totalDownload += recvSize;
			this->buffSize += recvSize;
			if (recvSize <= 0)
				break;
		}
		t1 = this->clk.GetTimeDiff();
		if (timeResp)
		{
			*timeResp = t1;
		}
#ifdef DEBUGSPEED
		if (t1 > 0.01)
		{
			printf("Request time = %lf\n", t1);
		}
#endif
#ifdef SHOWDEBUG
		printf("Read buffSize = %d\r\n", (Int32)this->buffSize);
#endif
		this->dataBuff[this->buffSize] = 0;
		if (Text::StrStartsWithC((UTF8Char*)this->dataBuff, this->buffSize, UTF8STRC("HTTP/")))
		{
			UTF8Char buff[256];
			UTF8Char *ptrs[3];
			UTF8Char *ptr;
			UTF8Char *ptrEnd;
			NotNullPtr<Text::String> s;
			UOSInt i;
			i = Text::StrIndexOfC(this->dataBuff, this->buffSize, UTF8STRC("\r\n"));
			MemCopyNO(buff, this->dataBuff, i);
			buff[i] = 0;
			Text::StrSplit(ptrs, 3, buff, ' ');
			this->respStatus = (Net::WebStatus::StatusCode)Text::StrToInt32(ptrs[1]);

			if (this->respStatus == Net::WebStatus::SC_UNKNOWN)
			{
#ifdef SHOWDEBUG
				printf("Unhandled HTTP response: %s\r\n", buff);
#endif
				this->respStatus = Net::WebStatus::SC_UNKNOWN;
			}

			ptr = (UTF8Char*)&this->dataBuff[i + 2];
			ptrEnd = &this->dataBuff[this->buffSize];
			this->contLeng = 0x7fffffff;
			this->contRead = 0;
			Bool header = true;
			Bool eventStream = false;
			UInt32 keepAliveTO = 0;
			while (header)
			{
				while ((i = Text::StrIndexOfCharC(ptr, (UOSInt)(ptrEnd - ptr), '\n')) != INVALID_INDEX && i > 0)
				{
					if (i == 1 && ptr[0] == '\r')
					{
						ptr++;
						i = 0;
						break;
					}
					if (ptr[i - 1] == '\r')
					{
						s = Text::String::New(ptr, i - 1);
					}
					else
					{
						s = Text::String::New(ptr, i);
					}
#ifdef SHOWDEBUG
					printf("Read Header: %s\r\n", s->v);
#endif
					this->headers.Add(s);

					if (s->StartsWithICase(UTF8STRC("Transfer-Encoding: ")))
					{
						if (Text::StrStartsWithC(&s->v[19], s->leng - 19, UTF8STRC("chunked")))
						{
							this->contEnc = 1;
							this->chunkSizeLeft = 0;
						}
					}
					else if (s->StartsWithICase(UTF8STRC("Content-")))
					{
						if (s->StartsWithICase(8, UTF8STRC("Length: ")))
						{
							s->leng = (UOSInt)(Text::StrTrimC(&s->v[16], s->leng - 16) - s->v);
							Text::StrToUInt64S(&s->v[16], &this->contLeng, 0);
						}
						else if (s->StartsWithICase(8, UTF8STRC("Type: text/event-stream")))
						{
							eventStream = true;
						}
						else if (s->StartsWithICase(8, UTF8STRC("Disposition: ")))
						{
							UOSInt i = s->IndexOf(UTF8STRC("filename="), 21);
							if (i >= 0)
							{
								if (s->v[i + 9] == '"')
								{
									UOSInt j = s->IndexOf('"', i + 10);
									if (j > 0)
									{
										NotNullPtr<Text::String> tmpS = Text::String::New(&s->v[i + 10], j - i - 10);
										this->SetSourceName(tmpS);
										tmpS->Release();
									}
								}
								else
								{
									this->SetSourceName(s->ToCString().Substring(i + 9));
								}
							}
						}
					}
					else if (s->StartsWithICase(UTF8STRC("Keep-Alive: timeout=")))
					{
						keepAliveTO = Text::StrToUInt32(&s->v[20]);
					}

					ptr = &ptr[i + 1];
				}
				if (i == 0)
				{
					ptr = &ptr[1];
					this->hdrLen += (UOSInt)(ptr - (UTF8Char*)this->dataBuff);
					this->buffSize -= (UOSInt)(ptr - (UTF8Char*)this->dataBuff);
					MemCopyO(this->dataBuff, ptr, this->buffSize);

					header = false;
					break;
				}
				this->hdrLen += (UOSInt)(ptr - (UTF8Char*)this->dataBuff);
				this->buffSize = (UOSInt)(ptrEnd - ptr);
				MemCopyO(this->dataBuff, ptr, this->buffSize);
				i = cli->Read(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize);
				if (i <= 0)
				{
					header = false;
					break;
				}
				else
				{
#ifdef SHOWDEBUG
					printf("Read from remote(7) = %d\r\n", (Int32)i);
#endif
#ifdef LOGREPLY
					this->clsData->fs->Write(&this->dataBuff[this->buffSize], i);
#endif
					this->totalDownload += i;
					this->buffSize += i;
					this->dataBuff[this->buffSize] = 0;
					ptr = (UTF8Char*)this->dataBuff;
					ptrEnd = &this->dataBuff[this->buffSize];
				}
			}
			if (eventStream && keepAliveTO != 0)
			{
				cli->SetTimeout((Int32)keepAliveTO * 1000);
			}
		}
		else
		{
#ifdef SHOWDEBUG
			printf("No reply HTTP header\r\n");
			printf("Reply: %s\r\n", (Char*)this->dataBuff);
#endif
			this->respStatus = Net::WebStatus::SC_UNKNOWN;
		}
#ifdef SHOWDEBUG
		printf("End Request end status = %d\r\n", this->respStatus);
#endif
	}
}

void Net::HTTPMyClient::SetTimeout(Data::Duration timeout)
{
	this->timeout = timeout;
	if (this->cli)
		this->cli->SetTimeout(timeout);
}

Bool Net::HTTPMyClient::IsSecureConn()
{
	if (this->cli == 0)
	{
		return false;
	}
	return this->cli->IsSSL();
}

Bool Net::HTTPMyClient::SetClientCert(Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key)
{
	if (this->ssl == 0)
		return false;
	return this->ssl->ClientSetCertASN1(cert, key);
}

const Data::ReadingList<Crypto::Cert::Certificate *> *Net::HTTPMyClient::GetServerCerts()
{
	if (this->cli && this->cli->IsSSL())
	{
		return ((Net::SSLClient*)this->cli)->GetRemoteCerts();
	}
	return 0;
}
