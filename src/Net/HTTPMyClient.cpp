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

UOSInt Net::HTTPMyClient::ReadRAWInternal(Data::ByteArray buff)
{
	NN<Net::TCPClient> cli;
	if (buff.GetSize() > BUFFSIZE)
	{
		buff = buff.SubArray(0, BUFFSIZE);
	}

	if (buff.GetSize() > (this->contLeng - this->contRead))
	{
		if (this->contLeng <= this->contRead)
		{
			return 0;
		}
		buff = buff.SubArray(0, (UOSInt)(this->contLeng - this->contRead));
	}
#ifdef SHOWDEBUG
	printf("Read size = %d\r\n", (Int32)buff.GetSize());
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
				if (!this->cli.SetTo(cli))
				{
					return 0;
				}
				i = cli->Read(Data::ByteArray(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize));
				if (i == 0 && this->buffSize <= 0)
				{
#ifdef SHOWDEBUG
					printf("Return Read size(1) = %d\r\n", 0);
#endif
					if (cli->IsClosed())
					{
						this->cli.Delete();
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
			if (this->chunkSizeLeft <= 2 && this->cli.SetTo(cli))
			{
				while (this->chunkSizeLeft > this->buffSize)
				{
					i = cli->Read(Data::ByteArray(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize));
					if (i == 0)
					{
#ifdef SHOWDEBUG
						printf("Return Read size(1.2) = %d\r\n", 0);
#endif
						if (cli->IsClosed())
						{
							this->cli.Delete();
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
			else if (buff.GetSize() >= this->buffSize)
			{
				sizeOut = this->buffSize;
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				buff.CopyFrom(0, Data::ByteArray(this->dataBuff, sizeOut));
				buff = buff.SubArray(sizeOut);
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
				sizeOut = buff.GetSize();
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				buff.CopyFrom(0, Data::ByteArray(this->dataBuff, sizeOut));
				buff = buff.SubArray(sizeOut);
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
		if (buffSize <= 0 && this->cli.SetTo(cli))
		{
			i = cli->Read(Data::ByteArray(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize));
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
			if (!this->cli.SetTo(cli))
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
			i = cli->Read(Data::ByteArray(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize));
			if (i == 0)
			{
#ifdef SHOWDEBUG
				printf("Return read size(6) = %d\r\n", 0);
#endif
				if (cli->IsClosed())
				{
					this->cli.Delete();
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
		j = Text::StrHex2UInt32C(this->dataBuff);
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
		if (this->buffSize == i && this->cli.SetTo(cli))
		{
			this->buffSize = 0;
			i = cli->Read(Data::ByteArray(this->dataBuff, BUFFSIZE - 1));
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
			if (buff.GetSize() >= this->buffSize - i)
			{
				sizeOut = this->buffSize - i;
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				buff.CopyFrom(0, Data::ByteArray(&this->dataBuff[i], sizeOut));
				buff = buff.SubArray(sizeOut);
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
				sizeOut = buff.GetSize();
				if (sizeOut > this->chunkSizeLeft)
				{
					sizeOut = this->chunkSizeLeft;
				}
				buff.CopyFrom(0, Data::ByteArray(&this->dataBuff[i], sizeOut));
				buff = buff.SubArray(sizeOut);
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
			if (!this->cli.SetTo(cli))
			{
				return 0;
			}
			this->buffSize = cli->Read(Data::ByteArray(this->dataBuff, buff.GetSize()));
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
			if (cli->IsClosed())
			{
				this->cli.Delete();
			}
		}
		if (this->buffSize >= buff.GetSize())
		{
			buff.CopyFrom(0, Data::ByteArray(this->dataBuff, buff.GetSize()));
			if (this->buffSize > buff.GetSize())
			{
				MemCopyO(this->dataBuff, &this->dataBuff[buff.GetSize()], this->buffSize - buff.GetSize());
			}
			this->buffSize -= buff.GetSize();
			this->contRead += buff.GetSize();
#ifdef SHOWDEBUG
			printf("Return read size(12) = %d\r\n", (Int32)buff.GetSize());
#endif
			return buff.GetSize();
		}
		else
		{
			buff.CopyFrom(0, Data::ByteArray(this->dataBuff, this->buffSize));
			UOSInt size = this->buffSize;
			this->contRead += this->buffSize;
			this->buffSize = 0;
#ifdef SHOWDEBUG
			printf("Return read size(13) = %d\r\n", (Int32)size);
#endif
			return size;
		}
	}
}

Net::HTTPMyClient::HTTPMyClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn), reqMstm(1024)
{
	Text::CStringNN nnuserAgent;
	if (!userAgent.SetTo(nnuserAgent))
	{
		nnuserAgent = CSTR("sswr/1.0");
	}
#if defined(LOGREPLY)
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
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
	this->userAgent = Text::String::New(nnuserAgent);
	this->dataBuff = MemAlloc(UInt8, BUFFSIZE);
}

Net::HTTPMyClient::~HTTPMyClient()
{
	NN<Net::TCPClient> cli;
	if (this->cli.SetTo(cli))
	{
		NN<Socket> soc;
		if (cli->GetSocket().SetTo(soc))
		{
			this->sockf->SetLinger(soc, 0);
		}
		cli->ShutdownSend();
		this->cli.Delete();
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
	return this->cli.IsNull();
}

UOSInt Net::HTTPMyClient::ReadRAW(const Data::ByteArray &buff)
{
	this->EndRequest(0, 0);
	if (this->respStatus == 0)
		return 0;
	return this->ReadRAWInternal(buff);
}

UOSInt Net::HTTPMyClient::Read(const Data::ByteArray &buff)
{
	this->EndRequest(0, 0);
	if (this->respStatus == 0)
		return 0;

	return this->ReadRAWInternal(buff);
}

UOSInt Net::HTTPMyClient::Write(Data::ByteArrayR buff)
{
	if (this->canWrite && !this->hasForm)
	{
		if (!writing)
		{
			//cli->Write((UInt8*)"\r\n", 2);
			this->reqMstm.Write(Data::ByteArrayR(U8STR("\r\n"), 2));
		}
		writing = true;
		//return cli->Write(buff, size);
		return this->reqMstm.Write(buff);
	}
	return 0;
}

Int32 Net::HTTPMyClient::Flush()
{
	return 0;
}

void Net::HTTPMyClient::Close()
{
	NN<Net::TCPClient> cli;
	if (this->cli.SetTo(cli))
	{
		cli->ShutdownSend();
		cli->Close();
	}
}

Bool Net::HTTPMyClient::Recover()
{
	//////////////////////////////////////////
	return false;
}

Bool Net::HTTPMyClient::Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders)
{
	UTF8Char urltmp[256];
	UOSInt urltmpLen;
	UTF8Char svrname[256];
	UnsafeArray<UTF8Char> svrnameEnd;
	UTF8Char host[256];
	NN<Net::TCPClient> cli;

	UOSInt i;
	UOSInt hostLen;
	Text::CStringNN ptr1;
	Text::CString optptr2;
	Text::PString ptrs[2];
	UnsafeArray<UTF8Char> cptr;
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
		timeDNS.Set(-1);
		timeConn.Set(-1);
		return false;
	}

	if (secure && this->ssl.IsNull())
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
		return false;
	}

	this->SetSourceName(url);
#ifdef SHOWDEBUG
	printf("Request URL: %s %s\r\n", Net::WebUtil::RequestMethodGetName(method).v, url.v);
#endif
	i = ptr1.IndexOf('/');
	if (i != INVALID_INDEX)
	{
		MemCopyNO(urltmp, ptr1.v.Ptr(), i * sizeof(UTF8Char));
		urltmp[i] = 0;
		optptr2 = ptr1.Substring(i);
	}
	else
	{
		optptr2 = CSTR_NULL;
		ptr1.ConcatTo(urltmp);
	}
	NN<Text::String> hostName;
	if (this->forceHost.SetTo(hostName))
	{
		hostName->ConcatTo(urltmp);
		urltmpLen = hostName->leng;
	}
	else
	{
		cptr = Text::TextBinEnc::URIEncoding::URIDecode(urltmp, urltmp);
		urltmpLen = (UOSInt)(cptr - urltmp);
	}
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
			Text::StrToUInt16S(&urltmp[i + 2], port, 0);
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
			Text::StrToUInt16S(ptrs[1].v, port, 0);
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
		else if (!sockf->DNSResolveIP(CSTRP(svrname, svrnameEnd), addr))
		{
			this->cli = 0;

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		timeDNS.Set(this->clk.GetTimeDiff());
		this->svrAddr = addr;
		if (addr.addrType != Net::AddrType::Unknown)
		{
#if defined(SHOWDEBUG)
			Net::SocketUtil::GetAddrName(svrname, this->svrAddr);
			printf("Server IP: %s:%d, t = %d\r\n", svrname, port, (Int32)this->svrAddr.addrType);
#endif
			NN<Net::SSLEngine> ssl;
			if (secure && this->ssl.SetTo(ssl))
			{
				Net::SSLEngine::ErrorType err;
				this->cli = ssl->ClientConnect(this->cliHost->ToCString(), port, err, this->timeout);
#if defined(SHOWDEBUG)
				if (this->cli == 0)
				{
					printf("Connect error: %s\r\n", Net::SSLEngine::ErrorTypeGetName(err).v);
				}
#endif
			}
			else
			{
				NEW_CLASSNN(cli, Net::TCPClient(sockf, this->svrAddr, port, this->timeout));
				this->cli = cli;
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
		timeConn.Set(t1);
#ifdef DEBUGSPEED
		if (t1 > 0.01)
		{
			printf("Time in connect: %lf\n", t1);
		}
#endif

		NN<Socket> soc;
		if (!this->cli.SetTo(cli))
		{
			this->writing = true;
			this->canWrite = false;
			return false;
		}
		else if (cli->IsConnectError() || !cli->GetSocket().SetTo(soc))
		{
#if defined(SHOWDEBUG)
			printf("Error in connect to server\r\n");
#endif
			this->cli.Delete();

			this->writing = true;
			this->canWrite = false;
			return false;
		}
		this->sockf->SetLinger(soc, 0);
		this->sockf->SetNoDelay(soc, true);
	}
	else if (Text::StrEqualsC(this->cliHost->v, this->cliHost->leng, urltmp, urltmpLen) && this->cli.SetTo(cli))
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
			size = cli->Read(Data::ByteArray(this->dataBuff, size));
			this->totalDownload += size;
#ifdef SHOWDEBUG
			printf("Read from remote(5), size = %d\r\n", (Int32)size);
#endif
			if (size == 0)
			{
				timeDNS.Set(-1);
				timeConn.Set(-1);
				return false;
			}
#ifdef LOGREPLY
			this->clsData->fs->Write(this->dataBuff, size);
#endif
			this->contRead += size;
		}
		timeDNS.Set(0);
		timeConn.Set(0);
		this->contRead = 0;
		i = this->headers.GetCount();
		while (i-- > 0)
		{
			OPTSTR_DEL(this->headers.RemoveAt(i));
		}
		this->headers.Clear();
		LIST_FREE_STRING(&this->reqHeaders);
		this->reqHeaders.Clear();
	}
	else
	{
		timeDNS.Set(-1);
		timeConn.Set(-1);
		return false;
	}

	Text::CStringNN ptr2;
	if (!optptr2.SetTo(ptr2))
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
	case Net::WebUtil::RequestMethod::HTTP_HEAD:
		this->canWrite = true;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("HEAD "));
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
	case Net::WebUtil::RequestMethod::HTTP_TRACE:
		this->canWrite = false;
		this->writing = false;
		cptr = Text::StrConcatC(dataBuff, UTF8STRC("TRACE "));
		cptr = ptr2.ConcatTo(cptr);
		cptr = Text::StrConcatC(cptr, UTF8STRC(" HTTP/1.1\r\n"));
		break;

	case Net::WebUtil::RequestMethod::HTTP_OPTIONS:
	case Net::WebUtil::RequestMethod::RTSP_DESCRIBE:
	case Net::WebUtil::RequestMethod::RTSP_ANNOUNCE:
	case Net::WebUtil::RequestMethod::RTSP_GET_PARAMETER:
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
	this->reqMstm.Write(Data::ByteArrayR(dataBuff, (UOSInt)(cptr - (UTF8Char*)dataBuff)));
	this->reqMstm.Write(Data::ByteArrayR((UInt8*)host, hostLen));
#ifdef SHOWDEBUG
	printf("Resquest Data: %s", dataBuff);
	printf("Add Header: %s", host);
#endif


	if (defHeaders)
	{
		this->AddHeaderC(CSTR("User-Agent"), this->userAgent->ToCString());
		this->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
		this->AddHeaderC(CSTR("Accept-Language"), CSTR("*"));
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

void Net::HTTPMyClient::AddHeaderC(Text::CStringNN name, Text::CString value)
{
	UInt8 buff[512];
	NN<Net::TCPClient> cli;
	UnsafeArray<UTF8Char> sptr;
	if (this->reqHeaders.SortedIndexOfPtr(name.v, name.leng) >= 0)
	{
#ifdef SHOWDEBUG
		printf("Add Header Failed(duplicated): %s: %s\r\n", name.v, value.v);
#endif
		return;
	}

	if (this->cli.SetTo(cli) && !this->writing)
	{
		if (name.leng + value.leng + 5 > 512)
		{
			this->reqMstm.Write(name.ToByteArray());
			this->reqMstm.Write(CSTR(": ").ToByteArray());
			this->reqMstm.Write(value.OrEmpty().ToByteArray());
			this->reqMstm.Write(CSTR("\r\n").ToByteArray());
#ifdef SHOWDEBUG
			printf("Add Header: %s: %s\r\n", name.v, value.v);
#endif
		}
		else
		{
			sptr = buff;
			MemCopyNO(sptr.Ptr(), name.v.Ptr(), name.leng);
			sptr += name.leng;
			WriteNUInt16(&sptr[0], ReadNUInt16((const UInt8*)": "));
			sptr += 2;
			MemCopyNO(sptr.Ptr(), value.v.Ptr(), value.leng);
			sptr += value.leng;
			WriteNUInt16(&sptr[0], ReadNUInt16((const UInt8*)"\r\n"));
			sptr += 2;
#ifdef SHOWDEBUG
			*sptr = 0;
			printf("Add Header: %s", buff);
#endif
			this->reqMstm.Write(Data::ByteArrayR(buff, (UOSInt)(sptr - (UTF8Char*)buff)));
		}
		this->reqHeaders.SortedInsert(Text::String::New(name).Ptr());
	}
	else
	{
#ifdef SHOWDEBUG
		printf("Add Header Failed(Non-writing state): %s: %s\r\n", name.v, value.v);
#endif
	}
}

void Net::HTTPMyClient::EndRequest(OptOut<Double> timeReq, OptOut<Double> timeResp)
{
	NN<Net::TCPClient> cli;
	NN<Socket> soc;
	if ((this->writing && !this->canWrite) || !this->cli.SetTo(cli) || !cli->GetSocket().SetTo(soc))
	{
		timeReq.Set(-1);
		timeResp.Set(-1);
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
			this->Write(this->formSb->ToByteArray());
			DEL_CLASS(this->formSb);
			this->formSb = 0;
		}
		this->canWrite = false;
		this->writing = true;

		this->reqMstm.Write(Data::ByteArrayR((UInt8*)"\r\n", 2));
		UOSInt reqSize;
		UOSInt writeSize = 0;
		UOSInt currSize = 0;
		UnsafeArray<UInt8> reqBuff = this->reqMstm.GetBuff(reqSize);
		while (writeSize < reqSize)
		{
			currSize = cli->Write(Data::ByteArrayR(&reqBuff[writeSize], reqSize - writeSize));
#ifdef SHOWDEBUG
			printf("Writing %d bytes, sent %d bytes\r\n", (UInt32)(reqSize - writeSize), (UInt32)currSize);
#endif
			if (currSize <= 0)
				break;
			this->totalUpload += currSize;
			writeSize += currSize;
		}
		this->reqMstm.Clear();

		this->sockf->SetLinger(soc, 0);
		if (!this->kaConn && !cli->IsSSL())
			cli->ShutdownSend();
		cli->SetTimeout(this->timeout);
		t1 = this->clk.GetTimeDiff();
		timeReq.Set(t1);

		this->buffSize = 0;
		while (this->buffSize < 32)
		{
			UOSInt recvSize = cli->Read(Data::ByteArray(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize));
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
		timeResp.Set(t1);
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
			UnsafeArray<UTF8Char> ptrs[3];
			UnsafeArray<UTF8Char> ptr;
			UnsafeArray<UTF8Char> ptrEnd;
			NN<Text::String> s;
			UOSInt i;
			i = Text::StrIndexOfC(this->dataBuff, this->buffSize, UTF8STRC("\r\n"));
			MemCopyNO(buff, this->dataBuff, i);
			buff[i] = 0;
#ifdef SHOWDEBUG
			printf("Read HTTP response: %s\r\n", buff);
#endif
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
							Text::StrToUInt64S(&s->v[16], this->contLeng, 0);
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
										NN<Text::String> tmpS = Text::String::New(&s->v[i + 10], j - i - 10);
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
					MemCopyO(this->dataBuff, ptr.Ptr(), this->buffSize);

					header = false;
					break;
				}
				this->hdrLen += (UOSInt)(ptr - (UTF8Char*)this->dataBuff);
				this->buffSize = (UOSInt)(ptrEnd - ptr);
				MemCopyO(this->dataBuff, ptr.Ptr(), this->buffSize);
				i = cli->Read(Data::ByteArray(&this->dataBuff[this->buffSize], BUFFSIZE - 1 - this->buffSize));
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
	NN<Net::TCPClient> cli;
	this->timeout = timeout;
	if (this->cli.SetTo(cli))
		cli->SetTimeout(timeout);
}

Bool Net::HTTPMyClient::IsSecureConn() const
{
	NN<Net::TCPClient> cli;
	if (!this->cli.SetTo(cli))
	{
		return false;
	}
	return cli->IsSSL();
}

Bool Net::HTTPMyClient::SetClientCert(NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key)
{
	NN<Net::SSLEngine> ssl;
	if (!this->ssl.SetTo(ssl))
		return false;
	return ssl->ClientSetCertASN1(cert, key);
}

Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> Net::HTTPMyClient::GetServerCerts()
{
	NN<Net::TCPClient> cli;
	if (this->cli.SetTo(cli) && cli->IsSSL())
	{
		return NN<Net::SSLClient>::ConvertFrom(cli)->GetRemoteCerts();
	}
	return 0;
}
