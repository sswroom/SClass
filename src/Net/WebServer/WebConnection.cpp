#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/WebConnection.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

#define MAX_CHUNK_SIZE 8192
#define IP_HEADER_SIZE 20
#define TCP_HEADER_SIZE 20
#define WRITE_BUFFER_SIZE ((1500 - IP_HEADER_SIZE - TCP_HEADER_SIZE) * 4)
#if WRITE_BUFFER_SIZE < MAX_CHUNK_SIZE
#undef WRITE_BUFFER_SIZE
#define WRITE_BUFFER_SIZE MAX_CHUNK_SIZE
#endif

UIntOS Net::WebServer::WebConnection::SendData(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	NN<IO::SMTCWriter> logWriter;
	NN<IO::BufferedOutputStream> cstm;
	if (this->logWriter.SetTo(logWriter)) logWriter->TCPSend(this->cli, buff, buffSize);
	if (this->cstm.SetTo(cstm))
	{
		buffSize = cstm->Write(Data::ByteArrayR(buff, buffSize));
	}
	else
	{
		buffSize = this->cli->Write(Data::ByteArrayR(buff, buffSize));
	}
	if (this->logger)
	{
		this->logger(this->loggerObj, buffSize);
	}
	return buffSize;
}

Net::WebServer::WebConnection::WebConnection(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Net::TCPClient> cli, NN<WebListener> svr, NN<WebHandler> hdlr, Bool allowProxy, KeepAlive keepAlive) : Net::WebServer::WebResponse(CSTR("WebConnection"))
{
	this->clif = clif;
	this->ssl = ssl;
	this->cli = cli;
	this->svr = svr;
	this->cstm = nullptr;
	this->hdlr = hdlr;
	this->currReq = nullptr;
	this->dataBuff = MemAlloc(UInt8, 4096);
	this->dataBuffSize = 4096;
	this->buffSize = 0;
	this->respLeng = 0;
	this->allowProxy = allowProxy;
	this->keepAlive = keepAlive;
	this->proxyMode = false;
	this->proxyCli = nullptr;
	this->logger = 0;
	this->loggerObj = 0;
	this->logWriter = nullptr;
	this->sseHdlr = 0;
	this->sseHdlrObj = 0;
	this->protoHdlr = nullptr;
}

Net::WebServer::WebConnection::~WebConnection()
{
	NN<ProtocolHandler> protoHdlr;
	NN<Net::TCPClient> proxyCli;
	if (this->protoHdlr.SetTo(protoHdlr))
	{
		protoHdlr->ConnectionClosed();
		this->protoHdlr = nullptr;
	}
	if (this->sseHdlr)
	{
		this->sseHdlr(*this, this->sseHdlrObj);
		this->sseHdlr = 0;
	}
	if (this->proxyMode && this->proxyCli.SetTo(proxyCli))
	{
		proxyCli->Close();
		while (this->proxyMode)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	this->cstm.Delete();
	this->cli.Delete();
	MemFreeArr(this->dataBuff);
	this->currReq.Delete();
}

void Net::WebServer::WebConnection::ReceivedData(const Data::ByteArrayR &buff)
{
#if defined(VERBOSE)
	printf("WebConn: Received %d bytes\r\n", (UInt32)buff.GetSize());
#endif		
	Text::PString sarr[4];
	UIntOS i;
	UIntOS j;
	UIntOS lineStart;
	UIntOS strIndex;
	NN<ProtocolHandler> protoHdlr;
	NN<Net::TCPClient> proxyCli;
	if (this->protoHdlr.SetTo(protoHdlr))
	{
		protoHdlr->ProtocolData(buff.Arr().Ptr(), buff.GetSize());
		return;
	}
	else if (this->proxyMode && this->proxyCli.SetTo(proxyCli))
	{
		if (proxyCli->Write(buff) != buff.GetSize())
		{
			proxyCli->Close();
			this->cli->Close();
		}
		return;
	}
	else
	{
		if (this->dataBuffSize < this->buffSize + buff.GetSize())
		{
			while (this->dataBuffSize < this->buffSize + buff.GetSize())
			{
				this->dataBuffSize = this->dataBuffSize << 1;
			}
			UnsafeArray<UInt8> newBuff = MemAllocArr(UInt8, this->dataBuffSize);
			newBuff.CopyFromNO(this->dataBuff, this->buffSize);
			MemFreeArr(this->dataBuff);
			this->dataBuff = newBuff;
		}
		MemCopyNO(&this->dataBuff[this->buffSize], buff.Arr().Ptr(), buff.GetSize());
		this->buffSize += buff.GetSize();

		j = this->buffSize - 1;
		i = 0;
		lineStart = 0;

		NN<Net::WebServer::WebServerRequest> currReq;
		{
			Sync::MutexUsage mutUsage(this->procMut);
			if (this->currReq.SetTo(currReq) && currReq->DataStarted())
			{
				i += currReq->DataPut(buff.Arr().Ptr(), buff.GetSize());
				if (!currReq->DataFull())
				{
					this->buffSize = 0;
					return;
				}

				this->ProcessResponse();
			}
		}

		while (i < j)
		{
			if (this->dataBuff[i] == 13 && this->dataBuff[i + 1] == 10)
			{
				this->dataBuff[i] = 0;
#if defined(VERBOSE)
				printf("WebConn: %s\r\n", &this->dataBuff[lineStart]);
#endif		
				if (lineStart == i)
				{
					Sync::MutexUsage mutUsage(this->procMut);
					if (this->currReq.SetTo(currReq))
					{
						if (currReq->HasData())
						{
							currReq->DataStart();
#if defined(VERBOSE)
							printf("WebConn: Post data %d bytes\r\n", (UInt32)(j - i - 1));
#endif		
							i += currReq->DataPut(&this->dataBuff[i + 2], j - i - 1);
							if (!currReq->DataFull())
							{
#if defined(VERBOSE)
								printf("WebConn: Post not data full\r\n");
#endif		
								break;
							}
						}

						this->ProcessResponse();
					}
				}
				else if (this->currReq.IsNull())
				{
					if (Text::StrSplitP(sarr, 4, {(UTF8Char*)&this->dataBuff[lineStart], i - lineStart}, ' ') == 3)
					{
						if (Text::StrEqualsC(sarr[2].v, sarr[2].leng, UTF8STRC("RTSP/1.0")))
						{
							Net::WebServer::WebRequest::RequestProtocol reqProto = Net::WebServer::WebRequest::RequestProtocol::RTSP1_0;
							this->respHeaders.ClearStr();
							this->respHeaderSent = false;
							this->respTranEnc = 0;
							Net::SocketUtil::AddressInfo cliAddr;
							this->cli->GetRemoteAddr(cliAddr);
							UInt16 cliPort = this->cli->GetRemotePort();
							UInt16 svrPort = this->cli->GetLocalPort();

							{
								Sync::MutexUsage mutUsage(this->procMut);
								this->currReq.Delete();
							}

							Net::WebUtil::RequestMethod reqMeth = Net::WebUtil::Str2RequestMethod(sarr[0].ToCString());
							if (reqMeth != Net::WebUtil::RequestMethod::Unknown)
							{
								NEW_CLASSOPT(this->currReq, WebServerRequest(sarr[1].ToCString(), reqMeth, reqProto, this->cli, cliAddr, cliPort, svrPort));
							}
							else
							{
								this->respHeaderSent = false;
								this->respTranEnc = 0;
								this->respStatus = Net::WebStatus::SC_METHOD_NOT_ALLOWED;
								this->respDataEnd = false;
								this->respHeaders.ClearStr();

								this->Close();

								this->cli->Close();
							}
						}
						else
						{
							Net::WebServer::WebRequest::RequestProtocol reqProto;
							if (Text::StrEqualsC(sarr[2].v, sarr[2].leng, UTF8STRC("HTTP/1.1")))
							{
								reqProto = Net::WebServer::WebRequest::RequestProtocol::HTTP1_1;
							}
							else if (Text::StrEqualsC(sarr[2].v, sarr[2].leng, UTF8STRC("HTTP/1.0")))
							{
								reqProto = Net::WebServer::WebRequest::RequestProtocol::HTTP1_0;
							}
/*							else if (Text::StrEqualsC(sarr[2].v, sarr[2].leng, UTF8STRC("HTTP/2")))
							{
								reqProto = Net::WebServer::WebRequest::RequestProtocol::HTTP2_0;
							}*/
							else
							{
								this->respHeaderSent = false;
								this->respTranEnc = 0;
								this->respStatus = Net::WebStatus::SC_VERSION_NOT_SUPPORTED;
								this->respDataEnd = false;
								this->respHeaders.ClearStr();

								this->Close();

								this->cli->Close();
								return;
							}
							this->respHeaders.ClearStr();
							this->respHeaderSent = false;
							this->respTranEnc = 0;
							Net::SocketUtil::AddressInfo cliAddr;
							this->cli->GetRemoteAddr(cliAddr);
							UInt16 cliPort = this->cli->GetRemotePort();
							UInt16 svrPort = this->cli->GetLocalPort();

							{
								Sync::MutexUsage mutUsage(this->procMut);
								this->currReq.Delete();
							}

							Net::WebUtil::RequestMethod reqMeth = Net::WebUtil::Str2RequestMethod(sarr[0].ToCString());
							if (reqMeth != Net::WebUtil::RequestMethod::Unknown)
							{
								NEW_CLASSOPT(this->currReq, WebServerRequest(sarr[1].ToCString(), reqMeth, reqProto, this->cli, cliAddr, cliPort, svrPort));
							}
							else
							{
								this->respHeaderSent = false;
								this->respStatus = Net::WebStatus::SC_METHOD_NOT_ALLOWED;
								this->respDataEnd = false;
								this->respHeaders.ClearStr();

								this->Close();

								this->cli->Close();
							}
						}
					}
					else
					{
						this->respHeaderSent = false;
						this->respStatus = Net::WebStatus::SC_BAD_REQUEST;
						this->respDataEnd = false;
						this->respHeaders.ClearStr();

						this->Close();

						this->cli->Close();
					}
				}
				else
				{
					strIndex = Text::StrIndexOfCharC(&this->dataBuff[lineStart], i - lineStart, ':');
					if (strIndex != INVALID_INDEX)
					{
						UIntOS nameLen = strIndex;
						this->dataBuff[lineStart + strIndex] = 0;
						strIndex++;
						while (this->dataBuff[lineStart + strIndex] == ' ')
						{
							strIndex++;
						}
						this->dataBuff[i] = 0;
						if (this->currReq.SetTo(currReq))
						{
							currReq->AddHeader(Text::CStringNN(&this->dataBuff[lineStart], nameLen), Text::CStringNN(&this->dataBuff[lineStart + (UIntOS)strIndex], i - lineStart - strIndex));
						}
					}
				}
				i += 2;
				lineStart = i;
			}
			else
			{
				i++;
			}
		}
		if (lineStart)
		{
			if (lineStart >= this->buffSize)
			{
				this->buffSize = 0;
			}
			else
			{
				this->dataBuff.CopyFromO(&this->dataBuff[lineStart], this->buffSize - lineStart);
				this->buffSize -= lineStart;
			}
		}
	}
}

void Net::WebServer::WebConnection::ProxyData(const Data::ByteArrayR &buff)
{
	this->SendData(buff.Arr().Ptr(), buff.GetSize());
}

void Net::WebServer::WebConnection::EndProxyConn()
{
	NN<Net::TCPClient> proxyCli;
	if (this->proxyCli.SetTo(proxyCli))
	{
		this->proxyMode = false;
		proxyCli.Delete();
		this->proxyCli = nullptr;
		this->svr->LogMessageC(this->currReq, CSTR("End Proxy Conn"));
	}
	else
	{
		this->proxyMode = false;
	}
}

void Net::WebServer::WebConnection::ProxyShutdown()
{
	NN<Net::TCPClient> proxyCli;
	if (this->proxyCli.SetTo(proxyCli))
	{
		proxyCli->ShutdownSend();
	}
}

void Net::WebServer::WebConnection::ProcessTimeout()
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::WebServer::WebServerRequest> currReq;
	Text::StringBuilderUTF8 sb;
	sptr = this->cli->GetRemoteName(sbuff).Or(sbuff);
	sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" "));
	if (this->currReq.SetTo(currReq))
	{
		NN<Text::String> uri = currReq->GetRequestURI();
		sb.Append(uri);
		sb.AppendUTF8Char(' ');
	}
	sb.AppendC(UTF8STRC("Process Timeout"));
	this->svr->LogMessageC(nullptr, sb.ToCString());
}

Optional<Text::String> Net::WebServer::WebConnection::GetRequestURL()
{
	NN<Net::WebServer::WebServerRequest> currReq;
	if (this->currReq.SetTo(currReq))
	{
		return currReq->GetRequestURI();
	}
	return nullptr;
}

void Net::WebServer::WebConnection::SendHeaders(Net::WebServer::WebRequest::RequestProtocol protocol)
{
	UInt8 *buff;
	UnsafeArray<UTF8Char> sptr;
	buff = MemAlloc(UInt8, 256 + (this->respHeaders.GetLength() * 3));
	sptr = (UTF8Char*)buff;
	if (protocol == Net::WebServer::WebRequest::RequestProtocol::HTTP1_0)
	{
		sptr = Text::StrConcatNE(sptr, UTF8STRC("HTTP/1.0 "));
	}
	else if (protocol == Net::WebServer::WebRequest::RequestProtocol::RTSP1_0)
	{
		sptr = Text::StrConcatNE(sptr, UTF8STRC("RTSP/1.0 "));
	}
	else
	{
		sptr = Text::StrConcatNE(sptr, UTF8STRC("HTTP/1.1 "));
	}
	sptr = Text::StrInt32(sptr, this->respStatus);
	*sptr++ = ' ';
	Text::CStringNN codeName;
	if (Net::WebStatus::GetCodeName(this->respStatus).SetTo(codeName))
	{
		sptr = Text::StrConcatNE(sptr, codeName.v, codeName.leng);
	}
	sptr = Text::StrConcatNE(sptr, UTF8STRC("\r\n"));

	sptr = Text::StrConcatNE(sptr, this->respHeaders.ToString(), this->respHeaders.GetLength());
	sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));

#if defined(VERBOSE)
	printf("WebConn: Send headers\r\n%s", buff);
#endif
	this->SendData(buff, (UIntOS)(sptr - (UTF8Char*)buff));
	MemFree(buff);
	this->respHeaderSent = true;
}

void Net::WebServer::WebConnection::ProcessResponse()
{
	this->respHeaderSent = false;
	this->respLeng = 0;
	this->respStatus = Net::WebStatus::SC_OK;
	this->respDataEnd = false;
	this->respHeaders.ClearStr();
	NN<Net::WebServer::WebServerRequest> currReq;
	NN<IO::BufferedOutputStream> cstm;
	if (this->currReq.SetTo(currReq))
	{

		NN<Text::String> reqURI = currReq->GetRequestURI();
		Net::WebUtil::RequestMethod reqMeth = currReq->GetReqMethod();
		if (reqMeth == Net::WebUtil::RequestMethod::HTTP_CONNECT && this->allowProxy)
		{
			NN<Net::TCPClient> proxyCli;
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			UIntOS i;
			sptr = reqURI->ConcatTo(sbuff);
			i = Text::StrIndexOfCharC(sbuff, (UIntOS)(sptr - sbuff), ':');
			if (i == INVALID_INDEX || i == 0)
			{
				this->respStatus = Net::WebStatus::SC_BAD_REQUEST;
				this->AddDefHeaders(currReq);
				if (!this->respHeaderSent)
				{
					this->SendHeaders(currReq->GetProtocol());
				}
				this->svr->LogAccess(currReq, *this, 0);
				return;
			}
		
			sbuff[i] = 0;
			proxyCli = this->clif->Create(Text::CStringNN(sbuff, i), (UInt16)Text::StrToInt32(&sbuff[i + 1]), 30000);
			if (proxyCli->IsConnectError())
			{
				proxyCli.Delete();
				this->respStatus = Net::WebStatus::SC_NOT_FOUND;
				this->AddDefHeaders(currReq);
				if (!this->respHeaderSent)
				{
					this->SendHeaders(currReq->GetProtocol());
				}
				this->svr->LogAccess(currReq, *this, 0);
				return;
			}
			
			this->proxyCli = proxyCli.Ptr();
			this->AddDefHeaders(currReq);
			if (!this->respHeaderSent)
			{
				this->SendHeaders(currReq->GetProtocol());
			}
			this->svr->LogAccess(currReq, *this, 0);
			this->proxyMode = true;
			this->svr->AddProxyConn(*this, proxyCli);
		}
		else if ((reqMeth == Net::WebUtil::RequestMethod::HTTP_GET || reqMeth == Net::WebUtil::RequestMethod::HTTP_POST) && reqURI->StartsWith(UTF8STRC("http://")))
		{
			Manage::HiResClock clk;
			Double t;
			NN<Net::HTTPClient> httpCli;
			Text::StringBuilderUTF8 sb;

			clk.Start();
			if (this->allowProxy)
			{
				httpCli = Net::HTTPClient::CreateClient(this->clif, this->ssl, nullptr, true, reqURI->StartsWith(UTF8STRC("https://")));
				httpCli->SetTimeout(5000);
				httpCli->Connect(reqURI->ToCString(), reqMeth, 0, 0, false);

				if (httpCli->IsError())
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Conn Err: "));
					sb.Append(reqURI);
					this->svr->LogMessageC(this->currReq, sb.ToCString());

					this->respStatus = Net::WebStatus::SC_NOT_FOUND;
					this->AddDefHeaders(currReq);
					if (!this->respHeaderSent)
					{
						this->SendHeaders(currReq->GetProtocol());
					}
				}
				else
				{
					UInt8 buff[2048];
					UnsafeArray<UTF8Char> sbuffHdr = MemAllocArr(UTF8Char, 65536);
					UIntOS i;
					UIntOS j;
					UIntOS k;
					NN<Text::String> s;
					Bool lengFound = false;
					
					i = 0;
					j = currReq->GetHeaderCnt();
					while (i < j)
					{
						if (!currReq->GetHeaderName(i).SetTo(s))
						{
						}
						else if (s->EqualsICase(UTF8STRC("Host")))
						{
						}
						else if (s->EqualsICase(UTF8STRC("Proxy-Connection")))
						{
						}
						else if (s->EqualsICase(UTF8STRC("Accept-Encoding")))
						{
						}
						else
						{
							Optional<Text::String> s2 = currReq->GetHeaderValue(i);
							httpCli->AddHeaderC(s->ToCString(), OPTSTR_CSTR(s2));
						}
						i++;
					}
					if (reqMeth == Net::WebUtil::RequestMethod::HTTP_POST)
					{
						UnsafeArray<const UInt8> reqBuff;
						UIntOS reqSize;
						if (currReq->GetReqData(reqSize).SetTo(reqBuff) && reqSize > 0)
						{
							httpCli->Write(Data::ByteArrayR(reqBuff, reqSize));
						}
					}

					this->SetStatusCode(httpCli->GetRespStatus());
					i = 0;
					j = httpCli->GetRespHeaderCnt();
					while (i < j)
					{
						UnsafeArray<UTF8Char> hdrPtr;
						if (httpCli->GetRespHeader(i, sbuffHdr).SetTo(hdrPtr))
						{
							k = Text::StrIndexOfC(sbuffHdr, (UIntOS)(hdrPtr - sbuffHdr), UTF8STRC(": "));
							if (k != INVALID_INDEX)
							{
								sbuffHdr[k] = 0;
								if (Text::StrEqualsICaseC(sbuffHdr, (UIntOS)(hdrPtr - sbuffHdr), UTF8STRC("Content-Length")))
								{
									lengFound = true;
								}
								if (Text::StrEqualsICaseC(sbuffHdr, (UIntOS)(hdrPtr - sbuffHdr), UTF8STRC("Server")))
								{
								}
								else if (Text::StrEqualsICaseC(sbuffHdr, (UIntOS)(hdrPtr - sbuffHdr), UTF8STRC("Connection")))
								{
								}
								else if (Text::StrEqualsICaseC(sbuffHdr, (UIntOS)(hdrPtr - sbuffHdr), UTF8STRC("Transfer-Encoding")))
								{
								}
								else
								{
									this->AddHeader(Text::CStringNN(sbuffHdr, k), CSTRP(&sbuffHdr[k + 2], hdrPtr));
								}
							}
						}
						i++;
					}
					this->AddHeaderS(CSTR("Server"), this->svr->GetServerName());
					this->AddHeader(CSTR("Proxy-Connection"), CSTR("closed"));

					if (lengFound)
					{
						while (true)
						{
							k = httpCli->Read(BYTEARR(buff));
							if (k <= 0)
								break;
							if (this->Write(Data::ByteArray(buff, k)) == 0)
								break;
						}
					}
					else
					{
						while (true)
						{
							k = httpCli->Read(BYTEARR(buff));
							if (k <= 0)
								break;
							if (this->Write(Data::ByteArray(buff, k)) == 0)
							{
								break;
							}
						}
		/*				IO::MemoryStream mstm(L"Net.WebConnection.mstm");
						UInt8 *memBuff;
						IntOS memSize;
						while (true)
						{
							k = httpCli->Read(buff, 2048);
							if (k <= 0)
								break;
							mstm.Write(buff, k);
						}
						sb.ClearStr();
						memBuff = mstm.GetBuff(&memSize);
						sb.Append((Int32)memSize);
						this->AddHeader(L"Content-Length", sb.ToString());
						this->Write(memBuff, memSize);*/
					}

					if (!this->respHeaderSent)
					{
						this->SendHeaders(currReq->GetProtocol());
					}
					MemFreeArr(sbuffHdr);
				}
				httpCli.Delete();
				this->cli->ShutdownSend();
			}
			else
			{
				if (!this->respHeaderSent)
				{
					this->SendHeaders(currReq->GetProtocol());
				}
			}

			t = clk.GetTimeDiff();
			this->svr->LogAccess(currReq, *this, t);

			this->currReq.Delete();
		}
		else
		{
			Manage::HiResClock clk;
			Double t;
			clk.Start();

			this->hdlr->DoWebRequest(currReq, *this);
			if (!this->respHeaderSent)
			{
				this->SendHeaders(currReq->GetProtocol());
			}
			if (!this->respDataEnd && this->respTranEnc == 1)
			{
	#if defined(VERBOSE)
				printf("WebConn: chunked %d\r\n", 0);
	#endif
				if (this->cstm.SetTo(cstm))
				{
					this->respLeng += cstm->Write(CSTR("0\r\n\r\n").ToByteArray());
				}
				else
				{
					this->respLeng += this->cli->Write(CSTR("0\r\n\r\n").ToByteArray());
				}
				this->respDataEnd = true;
			}
			t = clk.GetTimeDiff();
			this->svr->LogAccess(currReq, *this, t);
			this->cstm.Delete();
			if (this->protoHdlr.NotNull())
			{
			}
			else if (this->sseHdlr == 0)
			{
				if (this->keepAlive == KeepAlive::No)
				{
					this->cli->ShutdownSend();
				}
				else if (currReq->GetProtocol() == Net::WebServer::WebRequest::RequestProtocol::HTTP1_0)
				{
					NN<Text::String> connHdr;
					if (currReq->GetSHeader(CSTR("Connection")).SetTo(connHdr) && connHdr->EqualsICase(UTF8STRC("keep-alive")))
					{
					}
					else
					{
						this->cli->ShutdownSend();
					}
				}
				else
				{
					NN<Text::String> connHdr;
					if (currReq->GetSHeader(CSTR("Connection")).SetTo(connHdr) && connHdr->EqualsICase(UTF8STRC("close")))
					{
						this->cli->ShutdownSend();
					}
				}
				this->currReq.Delete();
			}
		}
	}
}

void Net::WebServer::WebConnection::EnableWriteBuffer()
{
	if (this->cstm.IsNull())
	{
		NEW_CLASSOPT(this->cstm, IO::BufferedOutputStream(this->cli, WRITE_BUFFER_SIZE));
	}
}

Bool Net::WebServer::WebConnection::SetStatusCode(Net::WebStatus::StatusCode code)
{
	if (this->respHeaderSent)
	{
		return false;
	}
	if (Net::WebStatus::IsExist(code))
	{
		this->respStatus = code;
		return true;
	}
	else
	{
		return false;
	}
}

Int32 Net::WebServer::WebConnection::GetStatusCode()
{
	return this->respStatus;
}

Bool Net::WebServer::WebConnection::AddHeader(Text::CStringNN name, Text::CStringNN value)
{
	if (this->respHeaderSent)
		return false;
	this->respHeaders.AppendNE2(name.v, name.leng, UTF8STRC(": "));
	this->respHeaders.AppendC2(value.v, value.leng, UTF8STRC("\r\n"));

	if (value.Equals(UTF8STRC("chunked")) && name.EqualsICase(UTF8STRC("Transfer-Encoding")))
	{
		this->respTranEnc = 1;
		this->cli->SetNoDelay(false);
	}

	return true;
}

Bool Net::WebServer::WebConnection::AddDefHeaders(NN<Net::WebServer::WebRequest> req)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	AddTimeHeader(CSTR("Date"), dt);
	AddHeaderS(CSTR("Server"), this->svr->GetServerName());
	NN<Text::String> connHdr;
	if (req->GetSHeader(CSTR("Connection")).SetTo(connHdr) && connHdr->Equals(UTF8STRC("keep-alive")) && (this->keepAlive == KeepAlive::Always || (this->keepAlive == KeepAlive::Default && Net::WebServer::HTTPServerUtil::AllowKA(req->GetBrowser()))))
	{
		AddHeader(CSTR("Connection"), CSTR("keep-alive"));
		AddHeader(CSTR("Keep-Alive"), CSTR("timeout=10, max=1000"));
	}
	else
	{
		AddHeader(CSTR("Connection"), CSTR("close"));
	}
	return false;
}

UInt64 Net::WebServer::WebConnection::GetRespLength()
{
	return this->respLeng;
}

void Net::WebServer::WebConnection::ShutdownSend()
{
	if (!this->respDataEnd && this->respTranEnc == 1)
	{
#if defined(VERBOSE)
		printf("WebConn: chunked %d\r\n", 0);
#endif
		NN<IO::BufferedOutputStream> cstm;
		if (this->cstm.SetTo(cstm))
		{
			this->respLeng += cstm->Write(CSTR("0\r\n\r\n").ToByteArray());
			cstm->Flush();
		}
		else
		{
			this->respLeng += this->cli->Write(CSTR("0\r\n\r\n").ToByteArray());
		}
		this->respDataEnd = true;
	}
	this->cli->ShutdownSend();
}

Bool Net::WebServer::WebConnection::ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, AnyType userObj)
{
	NN<Net::WebServer::WebServerRequest> currReq;
	if (this->sseHdlr || !this->currReq.SetTo(currReq))
	{
		return false;
	}
	this->cli->SetTimeout(timeout);
	this->sseHdlrObj = userObj;
	this->sseHdlr = hdlr;
	this->AddContentType(CSTR("text/event-stream"));
	if (!this->respHeaderSent)
	{
		this->SendHeaders(currReq->GetProtocol());
	}
	return true;
}

Bool Net::WebServer::WebConnection::SSESend(const UTF8Char *eventName, const UTF8Char *data)
{
	if (this->sseHdlr == 0)
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	if (eventName)
	{
		sb.AppendC(UTF8STRC("event:"));
		sb.AppendSlow(eventName);
		sb.AppendLB(Text::LineBreakType::LF);
	}
	if (data)
	{
		sb.AppendC(UTF8STRC("data:"));
		sb.AppendSlow(data);
		sb.AppendLB(Text::LineBreakType::LF);
	}
	sb.AppendLB(Text::LineBreakType::LF);
	return this->cli->Write(sb.ToByteArray()) == sb.GetLength();
}

Bool Net::WebServer::WebConnection::SwitchProtocol(Optional<ProtocolHandler> protoHdlr)
{
	if (!this->respHeaderSent)
	{
		NN<Net::WebServer::WebServerRequest> currReq;
		if (this->currReq.SetTo(currReq))
		{
			SendHeaders(currReq->GetProtocol());
		}
		else
		{
			SendHeaders(Net::WebServer::WebRequest::RequestProtocol::HTTP1_1);
		}
	}
	if (this->respDataEnd)
		return false;
	this->protoHdlr = protoHdlr;
	return true;
}

Text::CStringNN Net::WebServer::WebConnection::GetRespHeaders()
{
	return this->respHeaders.ToCString();
}

Bool Net::WebServer::WebConnection::IsDown() const
{
	return this->respDataEnd;
}

UIntOS Net::WebServer::WebConnection::Read(const Data::ByteArray &buff)
{
	return 0;
}

UIntOS Net::WebServer::WebConnection::Write(Data::ByteArrayR buff)
{
	NN<Net::WebServer::WebServerRequest> currReq;
	if (this->protoHdlr.NotNull())
	{
		if (this->logger)
		{
			this->logger(this->loggerObj, buff.GetSize());
		}
		this->svr->ExtendTimeout(cli);
		return this->cli->Write(buff);
	}
	if (!this->respHeaderSent)
	{
		if (this->currReq.SetTo(currReq))
		{
			SendHeaders(currReq->GetProtocol());
		}
		else
		{
			SendHeaders(Net::WebServer::WebRequest::RequestProtocol::HTTP1_1);
		}
	}
	if (this->respDataEnd)
		return 0;
	this->svr->ExtendTimeout(cli);
	if (this->respTranEnc == 1)
	{
		UIntOS retSize = 0;
		UIntOS ohSize;
		UIntOS writeSize;
		UInt8 sbuff[MAX_CHUNK_SIZE + 10];
		UnsafeArray<UTF8Char> sptr;
		while (buff.GetSize() > 0)
		{
			writeSize = buff.GetSize();
			if (writeSize > MAX_CHUNK_SIZE)
			{
				writeSize = MAX_CHUNK_SIZE;		
			}
			sptr = Text::StrConcatC(Text::StrHexVal32V(sbuff, (UInt32)writeSize), UTF8STRC("\r\n"));
#if defined(VERBOSE)
			printf("WebConn: chunked %s", sbuff);
#endif
			ohSize = (UIntOS)(sptr - sbuff) + 2;
			MemCopyNO(sptr.Ptr(), buff.Ptr(), writeSize);
			buff += writeSize;
			sptr += writeSize;
			sptr[0] = 13;
			sptr[1] = 10;
			sptr += 2;
			writeSize = this->SendData(sbuff, (UIntOS)(sptr - sbuff));
			this->respLeng += (UIntOS)(sptr - sbuff);
			if (writeSize == 0)
			{
				break;
			}
			if (writeSize > ohSize)
			{
				retSize += writeSize - ohSize;
			}
		}
		return retSize;
	}
	else
	{
		this->respLeng += buff.GetSize();
		return this->SendData(buff.Arr(), buff.GetSize());
	}
}

Int32 Net::WebServer::WebConnection::Flush()
{
	return 0;
}

void Net::WebServer::WebConnection::Close()
{
	NN<Net::WebServer::WebServerRequest> currReq;
	if (!this->respHeaderSent)
	{
		if (this->currReq.SetTo(currReq))
		{
			SendHeaders(currReq->GetProtocol());
		}
		else
		{
			SendHeaders(Net::WebServer::WebRequest::RequestProtocol::HTTP1_1);
		}
	}
	if (!this->respDataEnd && this->respTranEnc == 1)
	{

	}
	this->respDataEnd = true;
}

Bool Net::WebServer::WebConnection::Recover()
{
	return false;
}

IO::StreamType Net::WebServer::WebConnection::GetStreamType() const
{
	return IO::StreamType::WebConnection;
}

void Net::WebServer::WebConnection::SetSendLogger(SendLogger logger, AnyType userObj)
{
	this->logger = logger;
	this->loggerObj = userObj;
}

void Net::WebServer::WebConnection::SetLogWriter(Optional<IO::SMTCWriter> logWriter)
{
	this->logWriter = logWriter;
}
