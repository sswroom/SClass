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
#include "Text/TextEnc/FormEncoding.h"
#include "Text/TextEnc/URIEncoding.h"

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

Net::HTTPMyClient::HTTPMyClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn)
{
	if (userAgent == 0)
	{
		userAgent = (const UTF8Char*)"sswr/1.0";
	}
#if defined(LOGREPLY)
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	this->clsData = MemAlloc(ClassData, 1);
	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"HTTPClient_");
	dt.SetCurrTimeUTC();
	sptr = Text::StrInt64(sptr, dt.ToTicks());
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".dat");
	NEW_CLASS(this->clsData->fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
#endif
	this->ssl = ssl;
	this->cli = 0;
	this->cliHost = 0;
	this->writing = false;
	this->dataBuff = 0;
	this->buffSize = 0;
	this->buffOfst = 0;
	this->contEnc = 0;
	this->timeOutMS = 5000;
	this->userAgent = Text::StrCopyNew(userAgent);
	this->dataBuff = MemAlloc(UInt8, BUFFSIZE);
	NEW_CLASS(this->reqHeaders, Data::ArrayListStrUTF8());
	NEW_CLASS(this->reqMstm, IO::MemoryStream(1024, (const UTF8Char*)"Net.HTTPMyClient.reqMstm"));
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
	SDEL_TEXT(this->cliHost);
	if (this->dataBuff)
	{
		MemFree(this->dataBuff);
		this->dataBuff = 0;
	}
	UOSInt i = this->reqHeaders->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->reqHeaders->GetItem(i));
	}
	DEL_CLASS(this->reqHeaders);
	DEL_CLASS(this->reqMstm);
	Text::StrDelNew(this->userAgent);
#if defined(LOGREPLY)
	DEL_CLASS(this->clsData->fs);
	MemFree(this->clsData);
#endif
}

Bool Net::HTTPMyClient::IsError()
{
	return this->cli == 0;
}

UOSInt Net::HTTPMyClient::Read(UInt8 *buff, UOSInt size)
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
		if (chunkSizeLeft > 0)
		{
			if (BUFFSIZE - 1 - this->buffSize > 0)
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
				this->buffSize += i;
			}
			if (size >= this->buffSize)
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
			this->buffSize += i;
		}
		while (INVALID_INDEX == (i = Text::StrIndexOf((Char*)this->dataBuff, "\r\n")))
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
			this->buffSize += i;
			this->dataBuff[this->buffSize] = 0;
			if (this->dataBuff[0] == '\r' && this->dataBuff[1] == '\n')
			{
				MemCopyO(this->dataBuff, &this->dataBuff[2], this->buffSize - 2);
				buffSize -= 2;
			}
		}
		this->dataBuff[i] = 0;
		j = Text::StrHex2UInt32C((Char*)this->dataBuff);
		if (j == 0 && i == 1 && this->dataBuff[0] == '0')
		{
			i = 3;
			this->buffSize -= 3;
			if (this->buffSize)
			{
				MemCopyO(this->dataBuff, &this->dataBuff[3], this->buffSize);
			}
#ifdef SHOWDEBUG
			printf("Return read size(7) = %d\r\n", 0);
#endif
			return 0;
		}
		if (j == 0)
		{
			this->dataBuff[i] = 13;
#ifdef SHOWDEBUG
			printf("Return read size(8) = %d\r\n", 0);
#endif
			return 0;
		}
		this->chunkSizeLeft = j;
		if (this->dataBuff[i + 2] == 13 && this->dataBuff[i + 3] == 10)
		{
			i += 4;
		}
		else
		{
			i += 2;
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

UOSInt Net::HTTPMyClient::Write(const UInt8 *buff, UOSInt size)
{
	if (this->canWrite && !this->hasForm)
	{
		if (!writing)
		{
			//cli->Write((UInt8*)"\r\n", 2);
			this->reqMstm->Write((UInt8*)"\r\n", 2);
		}
		writing = true;
		//return cli->Write(buff, size);
		return this->reqMstm->Write(buff, size);
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

Bool Net::HTTPMyClient::Connect(const UTF8Char *url, const Char *method, Double *timeDNS, Double *timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UTF8Char svrname[256];
	UTF8Char host[256];

	UOSInt i;
	UOSInt hostLen;
	const UTF8Char *ptr1;
	const UTF8Char *ptr2;
	UTF8Char *ptrs[2];
	UTF8Char *cptr;
	UInt16 port;
	Bool secure = false;

	SDEL_TEXT(this->url);
	this->url = Text::StrCopyNew(url);
	if (Text::StrStartsWith(url, (const UTF8Char*)"http://"))
	{
		ptr1 = &url[7];
		secure = false;
	}
	else if (Text::StrStartsWith(url, (const UTF8Char*)"https://"))
	{
		ptr1 = &url[8];
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

#ifdef SHOWDEBUG
	printf("Request URL: %s %s\r\n", method, url);
#endif
	i = Text::StrIndexOf(ptr1, '/');
	if (i != INVALID_INDEX)
	{
		MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
		urltmp[i] = 0;
		ptr2 = &ptr1[i];
	}
	else
	{
		i = Text::StrCharCnt(ptr1);
		ptr2 = 0;
		MemCopyNO(urltmp, ptr1, i * sizeof(UTF8Char));
		urltmp[i] = 0;
	}
	Text::TextEnc::URIEncoding::URIDecode(urltmp, urltmp);
	cptr = Text::StrConcat(host, (UTF8Char*)"Host: ");
	cptr = Text::StrConcat(cptr, urltmp);
	cptr = Text::StrConcat(cptr, (UTF8Char*)"\r\n");
	hostLen = (UOSInt)(cptr - host);
	if (urltmp[0] == '[')
	{
		i = Text::StrIndexOf(urltmp, ']');
		if (i == INVALID_INDEX)
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		Text::StrConcatC(svrname, &urltmp[1], i - 1);
		if (urltmp[i + 1] == ':')
		{
			Text::StrToUInt16S(&urltmp[i + 2], &port, 0);
			urltmp[i + 1] = 0;
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
		i = Text::StrSplit(ptrs, 2, urltmp, ':');
		if (i == 2)
		{
			Text::StrToUInt16S(ptrs[1], &port, 0);
			Text::StrConcat(svrname, ptrs[0]);
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
			Text::StrConcat(svrname, ptrs[0]);
		}
	}

	this->clk->Start();
	if (this->cliHost == 0)
	{
		this->cliHost = Text::StrCopyNew(urltmp);

		Double t1;
		Net::SocketUtil::AddressInfo addr;
		if (Text::StrEqualsICase(svrname, (const UTF8Char*)"localhost"))
		{
			addr.addrType = Net::AddrType::IPv4;
			WriteNUInt32(addr.addr, Net::SocketUtil::GetIPAddr((const UTF8Char*)"127.0.0.1"));
		}
		else if (!sockf->DNSResolveIP(svrname, &addr))
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		if (timeDNS)
		{
			*timeDNS = clk->GetTimeDiff();
		}
		this->svrAddr = addr;
		if (addr.addrType != Net::AddrType::Unknown)
		{
#ifdef SHOWDEBUG
			Net::SocketUtil::GetAddrName(svrname, &this->svrAddr);
			printf("Server IP: %s:%d, t = %d\r\n", svrname, port, (Int32)this->svrAddr.addrType);
#endif
			if (secure)
			{
				Net::SSLEngine::ErrorType err;
				this->cli = this->ssl->Connect(this->cliHost, port, &err);
#ifdef SHOWDEBUG				
				if (this->cli == 0)
				{
					printf("Connect error: %s\r\n", Net::SSLEngine::ErrorTypeGetName(err));
				}
#endif
			}
			else
			{
				NEW_CLASS(this->cli, Net::TCPClient(sockf, &this->svrAddr, port));
			}
		}
		else
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		t1 = clk->GetTimeDiff();
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
#ifdef SHOWDEBUG
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
	else if (Text::StrEquals(this->cliHost, urltmp))
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
		i = this->headers->GetCount();
		while (i-- > 0)
		{
			MemFree(this->headers->RemoveAt(i));
		}
		this->headers->Clear();
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

	if (ptr2 == 0)
	{
		ptr2 = (const UTF8Char*)"/";
	}
	i = Text::StrCharCnt(ptr2);
	if ((i + 16) > BUFFSIZE)
	{
		MemFree(this->dataBuff);
		this->dataBuff = MemAlloc(UInt8, (i + 16));
	}
	if (method)
	{
		if (Text::StrEquals(method, "POST"))
		{
			this->canWrite = true;
			this->writing = false;
			cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"POST ");
		}
		else if (Text::StrEquals(method, "PUT"))
		{
			this->canWrite = true;
			this->writing = false;
			cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"PUT ");
		}
		else if (Text::StrEquals(method, "PATCH"))
		{
			this->canWrite = true;
			this->writing = false;
			cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"PATCH ");
		}
		else if (Text::StrEquals(method, "DELETE"))
		{
			this->canWrite = false;
			this->writing = false;
			cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"DELETE ");
		}
		else
		{
			this->canWrite = false;
			this->writing = false;
			cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"GET ");
		}
	}
	else
	{
		this->canWrite = false;
		this->writing = false;
		cptr = Text::StrConcat(dataBuff, (const UTF8Char*)"GET ");
	}
	cptr = Text::StrConcat(cptr, ptr2);
	cptr = Text::StrConcat(cptr, (const UTF8Char*)" HTTP/1.1\r\n");
	this->reqMstm->Write(dataBuff, (UOSInt)(cptr - (UTF8Char*)dataBuff));
	this->reqMstm->Write((UInt8*)host, hostLen);

	if (defHeaders)
	{
		this->AddHeader((const UTF8Char*)"User-Agent", this->userAgent);
		this->AddHeader((const UTF8Char*)"Accept", (const UTF8Char*)"*/*");
		this->AddHeader((const UTF8Char*)"Accept-Charset", (const UTF8Char*)"*");
		if (this->kaConn)
		{
			this->AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"keep-alive");
		}
		else
		{
			this->AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"close");
		}
	}
	return true;
}

void Net::HTTPMyClient::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	UInt8 buff[512];
	Char *sptr;
	if (this->reqHeaders->SortedIndexOf(name) >= 0)
		return;

	if (this->cli && !this->writing)
	{
		UOSInt i;
		UOSInt j;
		i = Text::StrCharCnt(name);
		j = Text::StrCharCnt(value);
		if (i + j + 5 > 512)
		{
			UInt8 *cbuff;
			cbuff = MemAlloc(UInt8, i + j + 5);
			sptr = Text::StrConcatC((Char*)cbuff, (const Char*)name, i);
			sptr = Text::StrConcatC(sptr, ": ", 2);
			sptr = Text::StrConcatC(sptr, (const Char*)value, j);
			sptr = Text::StrConcatC(sptr, "\r\n", 2);
#ifdef SHOWDEBUG
			printf("Add Header: %s", cbuff);
#endif
			this->reqMstm->Write(cbuff, (UOSInt)(sptr - (Char*)cbuff));
			MemFree(cbuff);
		}
		else
		{
			sptr = Text::StrConcatC((Char*)buff, (const Char*)name, i);
			sptr = Text::StrConcatC(sptr, ": ", 2);
			sptr = Text::StrConcatC(sptr, (const Char*)value, j);
			sptr = Text::StrConcatC(sptr, "\r\n", 2);
#ifdef SHOWDEBUG
			printf("Add Header: %s", buff);
#endif
			this->reqMstm->Write(buff, (UOSInt)(sptr - (Char*)buff));
		}
		this->reqHeaders->SortedInsert(Text::StrCopyNew(name));
	}
}

void Net::HTTPMyClient::EndRequest(Double *timeReq, Double *timeResp)
{
	if (this->cli == 0 || (this->writing && !this->canWrite))
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

		this->reqMstm->Write((UInt8*)"\r\n", 2);
		UOSInt reqSize;
		UOSInt writeSize = 0;
		UOSInt currSize = 0;
		UInt8 *reqBuff = this->reqMstm->GetBuff(&reqSize);
		while (writeSize < reqSize)
		{
			currSize = this->cli->Write(&reqBuff[writeSize], reqSize - writeSize);
#ifdef SHOWDEBUG
			printf("Writing %d bytes, sent %d bytes\r\n", (UInt32)(reqSize - writeSize), (UInt32)currSize);
#endif
			if (currSize <= 0)
				break;
			writeSize += currSize;
		}
		this->reqMstm->Clear();

		this->sockf->SetLinger(cli->GetSocket(), 0);
		if (!this->kaConn)
			this->cli->ShutdownSend();
		this->cli->SetTimeout(this->timeOutMS);
		t1 = this->clk->GetTimeDiff();
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
			this->buffSize += recvSize;
			if (recvSize <= 0)
				break;
		}
		t1 = this->clk->GetTimeDiff();
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
		if (Text::StrStartsWith((Char*)this->dataBuff, "HTTP/"))
		{
			Char buff[256];
			Char *ptrs[3];
			Char *ptr;
			UTF8Char *sptr;
			UOSInt i;
			i = Text::StrIndexOf((Char*)this->dataBuff, "\r\n");
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

			ptr = (Char*)&this->dataBuff[i + 2];
			this->contLeng = 0x7fffffff;
			this->contRead = 0;
			Bool header = true;
			Bool eventStream = false;
			UInt32 keepAliveTO = 0;
			while (header)
			{
				while ((i = Text::StrIndexOf(ptr, "\n")) != INVALID_INDEX && i > 0)
				{
					if (i == 1 && ptr[0] == '\r')
					{
						ptr++;
						i = 0;
						break;
					}
					if (ptr[i - 1] == '\r')
					{
						MemCopyNO(sptr = MemAlloc(UTF8Char, i), ptr, i - 1);
						sptr[i - 1] = 0;
					}
					else
					{
						MemCopyNO(sptr = MemAlloc(UTF8Char, i + 1), ptr, i);
						sptr[i] = 0;
					}
#ifdef SHOWDEBUG
					printf("Read Header: %s\r\n", sptr);
#endif
					this->headers->Add(sptr);

					if (Text::StrStartsWithICase(sptr, (const UTF8Char*)"Content-Length: "))
					{
						Text::StrTrim(&sptr[16]);
						this->contLeng = Text::StrToUInt64(&sptr[16]);
					}
					else if (Text::StrStartsWithICase(sptr, (const UTF8Char*)"Transfer-Encoding: "))
					{
						if (Text::StrStartsWith(&sptr[19], (const UTF8Char*)"chunked"))
						{
							this->contEnc = 1;
							this->chunkSizeLeft = 0;
						}
					}
					else if (Text::StrStartsWithICase(sptr, (const UTF8Char*)"Content-Type: text/event-stream"))
					{
						eventStream = true;
					}
					else if (Text::StrStartsWithICase(sptr, (const UTF8Char*)"Keep-Alive: timeout="))
					{
						keepAliveTO = Text::StrToUInt32(&sptr[20]);
					}

					ptr = &ptr[i + 1];
				}
				if (i == 0)
				{
					ptr = &ptr[1];
					this->buffSize -= (UOSInt)(ptr - (Char*)this->dataBuff);
					MemCopyO(this->dataBuff, ptr, this->buffSize);

					header = false;
					break;
				}
				this->buffSize = Text::StrCharCnt(ptr);
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
					this->buffSize += i;
					this->dataBuff[this->buffSize] = 0;
					ptr = (Char*)this->dataBuff;
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

void Net::HTTPMyClient::SetTimeout(Int32 ms)
{
	this->timeOutMS = ms;
	if (this->cli)
		this->cli->SetTimeout(ms);
}

Bool Net::HTTPMyClient::IsSecureConn()
{
	if (this->cli == 0)
	{
		return false;
	}
	return this->cli->IsSSL();
}

Crypto::Cert::Certificate *Net::HTTPMyClient::GetServerCert()
{
	if (this->cli && this->cli->IsSSL())
	{
		return ((Net::SSLClient*)this->cli)->GetRemoteCert();
	}
	return 0;
}
