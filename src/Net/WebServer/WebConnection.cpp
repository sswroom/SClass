#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/WebConnection.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextEnc/FormEncoding.h"
#include "Text/TextEnc/URIEncoding.h"

#define IP_HEADER_SIZE 20
#define TCP_HEADER_SIZE 20
#define WRITE_BUFFER_SIZE ((1500 - IP_HEADER_SIZE - TCP_HEADER_SIZE) * 4)

Net::WebServer::WebConnection::WebConnection(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Net::TCPClient *cli, WebListener *svr, IWebHandler *hdlr, Bool allowProxy, Bool allowKA) : Net::WebServer::IWebResponse(UTF8STRC("WebConnection"))
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->cli = cli;
	this->svr = svr;
	this->cstm = 0;
	this->hdlr = hdlr;
	this->currReq = 0;
	this->dataBuff = MemAlloc(UInt8, 4096);
	this->dataBuffSize = 4096;
	this->buffSize = 0;
	this->respLeng = 0;
	this->allowProxy = allowProxy;
	this->allowKA = allowKA;
	this->proxyMode = false;
	this->proxyCli = 0;
	this->logger = 0;
	this->loggerObj = 0;
	this->sseHdlr = 0;
	this->sseHdlrObj = 0;
	NEW_CLASS(this->respHeaders, Text::StringBuilderUTF8());
}

Net::WebServer::WebConnection::~WebConnection()
{
	if (this->sseHdlr)
	{
		this->sseHdlr(this, this->sseHdlrObj);
		this->sseHdlr = 0;
	}
	if (this->proxyMode)
	{
		this->proxyCli->Close();
		while (this->proxyMode)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->cstm);
	DEL_CLASS(this->cli);
	MemFree(this->dataBuff);
	SDEL_CLASS(this->currReq);
	DEL_CLASS(this->respHeaders);
}

void Net::WebServer::WebConnection::ReceivedData(const UInt8 *buff, UOSInt size)
{
	Text::PString sarr[4];
	UOSInt i;
	UOSInt j;
	UOSInt lineStart;
	UOSInt strIndex;
	if (this->proxyMode)
	{
		if (this->proxyCli->Write(buff, size) != size)
		{
			this->proxyCli->Close();
			this->cli->Close();
		}
		return;
	}
	else
	{
		if (this->dataBuffSize < this->buffSize + size)
		{
			while (this->dataBuffSize < this->buffSize + size)
			{
				this->dataBuffSize = this->dataBuffSize << 1;
			}
			UInt8 *newBuff = MemAlloc(UInt8, this->dataBuffSize);
			MemCopyNO(newBuff, this->dataBuff, this->buffSize);
			MemFree(this->dataBuff);
			this->dataBuff = newBuff;
		}
		MemCopyNO(&this->dataBuff[this->buffSize], buff, size);
		this->buffSize += size;

		j = this->buffSize - 1;
		i = 0;
		lineStart = 0;

		if (this->currReq && this->currReq->DataStarted())
		{
			i += this->currReq->DataPut(buff, size);
			if (!this->currReq->DataFull())
			{
				this->buffSize = 0;
				return;
			}

			this->ProcessResponse();
		}

		while (i < j)
		{
			if (this->dataBuff[i] == 13 && this->dataBuff[i + 1] == 10)
			{
				this->dataBuff[i] = 0;
				if (lineStart == i)
				{
					if (this->currReq)
					{
						if (this->currReq->HasData())
						{
							this->currReq->DataStart();
							i += this->currReq->DataPut(&this->dataBuff[i + 2], j - i - 1);
							if (!this->currReq->DataFull())
							{
								break;
							}
						}

						this->ProcessResponse();
					}
				}
				else if (this->currReq == 0)
				{
					if (Text::StrSplitP(sarr, 4, (UTF8Char*)&this->dataBuff[lineStart], i - lineStart, ' ') == 3)
					{
						if (Text::StrEqualsC(sarr[2].v, sarr[2].len, UTF8STRC("RTSP/1.0")))
						{
							Net::WebServer::IWebRequest::RequestProtocol reqProto = Net::WebServer::IWebRequest::RequestProtocol::RTSP1_0;
							Bool secureConn = false;
							this->respHeaders->ClearStr();
							this->respHeaderSent = false;
							this->respTranEnc = 0;
							Net::SocketUtil::AddressInfo cliAddr;
							this->cli->GetRemoteAddr(&cliAddr);
							UInt16 cliPort = this->cli->GetRemotePort();
							UInt16 svrPort = this->cli->GetLocalPort();

							if (this->currReq)
							{
								DEL_CLASS(this->currReq);
								this->currReq = 0;
							}

							if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("DESCRIBE")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_DESCRIBE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("ANNOUNCE")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_ANNOUNCE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("GET_PARAMETER")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_GET_PARAMETER, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("OPTIONS")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_OPTIONS, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("PAUSE")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_PAUSE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("PLAY")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_PLAY, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("RECORD")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_RECORD, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("REDIRECT")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_REDIRECT, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("SETUP")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_SETUP, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("SET_PARAMETER")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_SET_PARAMETER, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("TEARDOWN")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::RTSP_TEARDOWN, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else
							{
								this->respHeaderSent = false;
								this->respTranEnc = 0;
								this->respStatus = Net::WebStatus::SC_METHOD_NOT_ALLOWED;
								this->respDataEnd = false;
								this->respHeaders->ClearStr();

								this->Close();

								this->cli->Close();
							}
						}
						else
						{
							Net::WebServer::IWebRequest::RequestProtocol reqProto;
							Bool secureConn = this->cli->IsSSL();
							if (Text::StrEqualsC(sarr[2].v, sarr[2].len, UTF8STRC("HTTP/2")))
							{
								reqProto = Net::WebServer::IWebRequest::RequestProtocol::HTTP2_0;
							}
							else if (Text::StrEqualsC(sarr[2].v, sarr[2].len, UTF8STRC("HTTP/1.1")))
							{
								reqProto = Net::WebServer::IWebRequest::RequestProtocol::HTTP1_1;
							}
							else if (Text::StrEqualsC(sarr[2].v, sarr[2].len, UTF8STRC("HTTP/1.0")))
							{
								reqProto = Net::WebServer::IWebRequest::RequestProtocol::HTTP1_0;
							}
							else
							{
								this->respHeaderSent = false;
								this->respTranEnc = 0;
								this->respStatus = Net::WebStatus::SC_VERSION_NOT_SUPPORTED;
								this->respDataEnd = false;
								this->respHeaders->ClearStr();

								this->Close();

								this->cli->Close();
								return;
							}
							this->respHeaders->ClearStr();
							this->respHeaderSent = false;
							this->respTranEnc = 0;
							Net::SocketUtil::AddressInfo cliAddr;
							this->cli->GetRemoteAddr(&cliAddr);
							UInt16 cliPort = this->cli->GetRemotePort();
							UInt16 svrPort = this->cli->GetLocalPort();

							if (this->currReq)
							{
								DEL_CLASS(this->currReq);
								this->currReq = 0;
							}

							if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("GET")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("POST")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::HTTP_POST, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("PUT")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::HTTP_PUT, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("PATCH")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::HTTP_PATCH, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("DELETE")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::HTTP_DELETE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEqualsC(sarr[0].v, sarr[0].len, UTF8STRC("CONNECT")))
							{
								NEW_CLASS(this->currReq, WebRequest(sarr[1].v, sarr[1].len, Net::WebServer::IWebRequest::RequestMethod::HTTP_CONNECT, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else
							{
								this->respHeaderSent = false;
								this->respStatus = Net::WebStatus::SC_METHOD_NOT_ALLOWED;
								this->respDataEnd = false;
								this->respHeaders->ClearStr();

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
						this->respHeaders->ClearStr();

						this->Close();

						this->cli->Close();
					}
				}
				else
				{
					strIndex = Text::StrIndexOf(&this->dataBuff[lineStart], ':');
					if (strIndex != INVALID_INDEX)
					{
						UOSInt nameLen = strIndex;
						this->dataBuff[lineStart + strIndex] = 0;
						strIndex++;
						while (this->dataBuff[lineStart + strIndex] == ' ')
						{
							strIndex++;
						}
						this->dataBuff[i] = 0;
						if (this->currReq)
						{
							this->currReq->AddHeaderC(&this->dataBuff[lineStart], nameLen, &this->dataBuff[lineStart + (UOSInt)strIndex], i - lineStart - strIndex);
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
				MemCopyO(this->dataBuff, &this->dataBuff[lineStart], this->buffSize - lineStart);
				this->buffSize -= lineStart;
			}
		}
	}
}

void Net::WebServer::WebConnection::ProxyData(const UInt8 *buff, UOSInt size)
{
	if (this->cstm)
	{
		this->cstm->Write(buff, size);
	}
	else
	{
		this->cli->Write(buff, size);
	}
	if (this->logger)
	{
		this->logger(this->loggerObj, size);
	}
}

void Net::WebServer::WebConnection::EndProxyConn()
{
	if (this->proxyCli)
	{
		this->proxyMode = false;
		DEL_CLASS(this->proxyCli);
		this->proxyCli = 0;
		this->svr->LogMessageC(this->currReq, UTF8STRC("End Proxy Conn"));
	}
	else
	{
		this->proxyMode = false;
	}
}

void Net::WebServer::WebConnection::ProxyShutdown()
{
	if (this->proxyCli)
	{
		this->proxyCli->ShutdownSend();
	}
}

void Net::WebServer::WebConnection::ProcessTimeout()
{
	UTF8Char sbuff[32];
	Text::StringBuilderUTF8 sb;
	this->cli->GetRemoteName(sbuff);
	sb.Append(sbuff);
	sb.AppendC(UTF8STRC(" "));
	if (this->currReq)
	{
		Text::String *uri = this->currReq->GetRequestURI();
		if (uri)
		{
			sb.Append(uri);
			sb.AppendChar(' ', 1);
		}
	}
	sb.AppendC(UTF8STRC("Process Timeout"));
	this->svr->LogMessageC(0, sb.ToString(), sb.GetLength());
}

Text::String *Net::WebServer::WebConnection::GetRequestURL()
{
	if (this->currReq)
	{
		return this->currReq->GetRequestURI();
	}
	return 0;
}

void Net::WebServer::WebConnection::SendHeaders(Net::WebServer::IWebRequest::RequestProtocol protocol)
{
	UInt8 *buff;
	UTF8Char *sptr;
	buff = MemAlloc(UInt8, 256 + (this->respHeaders->GetLength() * 3));
	sptr = (UTF8Char*)buff;
	if (protocol == Net::WebServer::IWebRequest::RequestProtocol::HTTP1_0)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("HTTP/1.0 "));
	}
	else if (protocol == Net::WebServer::IWebRequest::RequestProtocol::RTSP1_0)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("RTSP/1.0 "));
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("HTTP/1.1 "));
	}
	sptr = Text::StrInt32(sptr, this->respStatus);
	*sptr++ = ' ';
	sptr = Text::StrConcat((UTF8Char*)sptr, Net::WebStatus::GetCodeName(this->respStatus));
	sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));

	sptr = Text::StrConcatC(sptr, this->respHeaders->ToString(), this->respHeaders->GetLength());
	sptr = Text::StrConcatC(sptr, UTF8STRC("\r\n"));

	if (this->cstm)
	{
		this->cstm->Write(buff, (UOSInt)(sptr - (UTF8Char*)buff));
	}
	else
	{
		this->cli->Write(buff, (UOSInt)(sptr - (UTF8Char*)buff));
	}
	if (this->logger)
	{
		this->logger(this->loggerObj, (UOSInt)(sptr - (UTF8Char*)buff));
	}
	MemFree(buff);
	this->respHeaderSent = true;
}

void Net::WebServer::WebConnection::ProcessResponse()
{
	this->respHeaderSent = false;
	this->respLeng = 0;
	this->respStatus = Net::WebStatus::SC_OK;
	this->respDataEnd = false;
	this->respHeaders->ClearStr();

	Text::String *reqURI = this->currReq->GetRequestURI();
	Net::WebServer::IWebRequest::RequestMethod reqMeth = this->currReq->GetReqMethod();
	if (reqMeth == Net::WebServer::IWebRequest::RequestMethod::HTTP_CONNECT && this->allowProxy)
	{
		Net::TCPClient *proxyCli;
		UTF8Char sbuff[512];
		UOSInt i;
		reqURI->ConcatTo(sbuff);
		i = Text::StrIndexOf(sbuff, ':');
		if (i == INVALID_INDEX || i == 0)
		{
			this->respStatus = Net::WebStatus::SC_BAD_REQUEST;
			this->AddDefHeaders(this->currReq);
			if (!this->respHeaderSent)
			{
				this->SendHeaders(this->currReq->GetProtocol());
			}
			this->svr->LogAccess(this->currReq, this, 0);
			return;
		}
	
		sbuff[i] = 0;
		NEW_CLASS(proxyCli, Net::TCPClient(this->sockf, sbuff, (UInt16)Text::StrToInt32(&sbuff[i + 1])));
		if (proxyCli->IsConnectError())
		{
			DEL_CLASS(proxyCli);
			this->respStatus = Net::WebStatus::SC_NOT_FOUND;
			this->AddDefHeaders(this->currReq);
			if (!this->respHeaderSent)
			{
				this->SendHeaders(this->currReq->GetProtocol());
			}
			this->svr->LogAccess(this->currReq, this, 0);
			return;
		}
		
		this->proxyCli = proxyCli;
		this->AddDefHeaders(this->currReq);
		if (!this->respHeaderSent)
		{
			this->SendHeaders(this->currReq->GetProtocol());
		}
		this->svr->LogAccess(this->currReq, this, 0);
		this->proxyMode = true;
		this->svr->AddProxyConn(this, this->proxyCli);
	}
	else if ((reqMeth == Net::WebServer::IWebRequest::RequestMethod::HTTP_GET || reqMeth == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST) && reqURI->StartsWith(UTF8STRC("http://")))
	{
		Manage::HiResClock clk;
		Double t;
		Net::HTTPClient *httpCli;
		Text::StringBuilderUTF8 sb;

		clk.Start();
		if (this->allowProxy)
		{
			httpCli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, 0, 0, true, reqURI->StartsWith(UTF8STRC("https://")));
			httpCli->SetTimeout(5000);
			if (reqMeth == Net::WebServer::IWebRequest::RequestMethod::HTTP_GET)
			{
				httpCli->Connect(reqURI->v, reqURI->leng, "GET", 0, 0, false);
			}
			else
			{
				httpCli->Connect(reqURI->v, reqURI->leng, "POST", 0, 0, false);
			}

			if (httpCli->IsError())
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Conn Err: "));
				sb.Append(reqURI);
				this->svr->LogMessageC(this->currReq, sb.ToString(), sb.GetLength());

				this->respStatus = Net::WebStatus::SC_NOT_FOUND;
				this->AddDefHeaders(this->currReq);
				if (!this->respHeaderSent)
				{
					this->SendHeaders(this->currReq->GetProtocol());
				}
			}
			else
			{
				UInt8 buff[2048];
				UTF8Char *sbuffHdr = MemAlloc(UTF8Char, 65536);
				UOSInt i;
				UOSInt j;
				UOSInt k;
				Text::String *s;
				Bool lengFound = false;
				
				i = 0;
				j = this->currReq->GetHeaderCnt();
				while (i < j)
				{
					s = this->currReq->GetHeaderName(i);
					if (s->EqualsICase((const UTF8Char*)"Host"))
					{
					}
					else if (s->EqualsICase((const UTF8Char*)"Proxy-Connection"))
					{
					}
					else if (s->EqualsICase((const UTF8Char*)"Accept-Encoding"))
					{
					}
					else
					{
						Text::String *s2 = this->currReq->GetHeaderValue(i);
						httpCli->AddHeaderC(s->v, s->leng, s2->v, s2->leng);
					}
					i++;
				}
				if (reqMeth == Net::WebServer::IWebRequest::RequestMethod::HTTP_POST)
				{
					const UInt8 *reqBuff;
					UOSInt reqSize;
					reqBuff = this->currReq->GetReqData(&reqSize);
					if (reqBuff && reqSize > 0)
					{
						httpCli->Write(reqBuff, reqSize);
					}
				}

				this->SetStatusCode(httpCli->GetRespStatus());
				i = 0;
				j = httpCli->GetRespHeaderCnt();
				while (i < j)
				{
					UTF8Char *hdrPtr = httpCli->GetRespHeader(i, sbuffHdr);
					k = Text::StrIndexOf(sbuffHdr, (const UTF8Char*)": ");
					if (k != INVALID_INDEX)
					{
						sbuffHdr[k] = 0;
						if (Text::StrEqualsICase(sbuffHdr, (const UTF8Char*)"Content-Length"))
						{
							lengFound = true;
						}
						if (Text::StrEqualsICase(sbuffHdr, (const UTF8Char*)"Server"))
						{
						}
						else if (Text::StrEqualsICase(sbuffHdr, (const UTF8Char*)"Connection"))
						{
						}
						else if (Text::StrEqualsICase(sbuffHdr, (const UTF8Char*)"Transfer-Encoding"))
						{
						}
						else
						{
							this->AddHeaderC(sbuffHdr, k, &sbuffHdr[k + 2], (UOSInt)(hdrPtr - &sbuffHdr[k + 2]));
						}
					}
					else
					{
					}
					i++;
				}
				this->AddHeaderS(UTF8STRC("Server"), this->svr->GetServerName());
				this->AddHeaderC(UTF8STRC("Proxy-Connection"), UTF8STRC("closed"));

				if (lengFound)
				{
					while (true)
					{
						k = httpCli->Read(buff, 2048);
						if (k <= 0)
							break;
						if (this->Write(buff, k) == 0)
							break;
					}
				}
				else
				{
					while (true)
					{
						k = httpCli->Read(buff, 2048);
						if (k <= 0)
							break;
						if (this->Write(buff, k) == 0)
						{
							break;
						}
					}
	/*				IO::MemoryStream mstm(L"Net.WebConnection.mstm");
					UInt8 *memBuff;
					OSInt memSize;
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
					this->SendHeaders(this->currReq->GetProtocol());
				}
				MemFree(sbuffHdr);
			}
			DEL_CLASS(httpCli);
			this->cli->ShutdownSend();
		}
		else
		{
			if (!this->respHeaderSent)
			{
				this->SendHeaders(this->currReq->GetProtocol());
			}
		}

		t = clk.GetTimeDiff();
		this->svr->LogAccess(this->currReq, this, t);

		DEL_CLASS(this->currReq);
		this->currReq = 0;
	}
	else
	{
		Manage::HiResClock clk;
		Double t;
		clk.Start();

		this->hdlr->WebRequest(this->currReq, this);
		if (!this->respHeaderSent)
		{
			this->SendHeaders(this->currReq->GetProtocol());
		}
		if (!this->respDataEnd && this->respTranEnc == 1)
		{
			if (this->cstm)
			{
				this->respLeng += this->cstm->Write((const UInt8*)"0\r\n\r\n", 5);
			}
			else
			{
				this->respLeng += this->cli->Write((const UInt8*)"0\r\n\r\n", 5);
			}
			this->respDataEnd = true;
		}
		t = clk.GetTimeDiff();
		this->svr->LogAccess(this->currReq, this, t);
		SDEL_CLASS(this->cstm);
		if (this->sseHdlr == 0)
		{
			Text::String *connHdr = this->currReq->GetSHeader(UTF8STRC("Connection"));
			if (this->allowKA && connHdr && connHdr->Equals(UTF8STRC("keep-alive")))
			{
			}
			else
			{
				this->cli->ShutdownSend();
			}
			DEL_CLASS(this->currReq);
			this->currReq = 0;
		}
	}
}

void Net::WebServer::WebConnection::EnableWriteBuffer()
{
	if (this->cstm == 0)
	{
		NEW_CLASS(this->cstm, IO::BufferedOutputStream(this->cli, WRITE_BUFFER_SIZE));
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

Bool Net::WebServer::WebConnection::AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	if (this->respHeaderSent)
		return false;
	this->respHeaders->AppendC(name, nameLen);
	this->respHeaders->AppendC(UTF8STRC(": "));
	this->respHeaders->AppendC(value, valueLen);
	this->respHeaders->AppendC(UTF8STRC("\r\n"));

	if (Text::StrEqualsICase(name, (const UTF8Char*)"Transfer-Encoding") && Text::StrEqualsC(value, valueLen, UTF8STRC("chunked")))
	{
		this->respTranEnc = 1;
		this->cli->SetNoDelay(false);
	}

	return true;
}

Bool Net::WebServer::WebConnection::AddDefHeaders(Net::WebServer::IWebRequest *req)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	AddTimeHeader(UTF8STRC("Date"), &dt);
	AddHeaderS(UTF8STRC("Server"), this->svr->GetServerName());
	Text::String *connHdr = req->GetSHeader(UTF8STRC("Connection"));
	if (this->allowKA && connHdr && connHdr->Equals(UTF8STRC("keep-alive")))
	{
		AddHeaderC(UTF8STRC("Connection"), UTF8STRC("keep-alive"));
		AddHeaderC(UTF8STRC("Keep-Alive"), UTF8STRC("timeout=10, max=1000"));
	}
	else
	{
		AddHeaderC(UTF8STRC("Connection"), UTF8STRC("close"));
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
		if (this->cstm)
		{
			this->respLeng += this->cstm->Write((const UInt8*)"0\r\n\r\n", 5);
			this->cstm->Flush();
		}
		else
		{
			this->respLeng += this->cli->Write((const UInt8*)"0\r\n\r\n", 5);
		}
		this->respDataEnd = true;
	}
	this->cli->ShutdownSend();
}

Bool Net::WebServer::WebConnection::ResponseSSE(Int32 timeoutMS, SSEDisconnectHandler hdlr, void *userObj)
{
	if (this->sseHdlr)
	{
		return false;
	}
	this->cli->SetTimeout(timeoutMS);
	this->sseHdlrObj = userObj;
	this->sseHdlr = hdlr;
	this->AddContentType(UTF8STRC("text/event-stream"));
	if (!this->respHeaderSent)
	{
		this->SendHeaders(this->currReq->GetProtocol());
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
		sb.Append(eventName);
		sb.AppendLB(Text::LineBreakType::LF);
	}
	if (data)
	{
		sb.AppendC(UTF8STRC("data:"));
		sb.Append(data);
		sb.AppendLB(Text::LineBreakType::LF);
	}
	sb.AppendLB(Text::LineBreakType::LF);
	return this->cli->Write(sb.ToString(), sb.GetLength()) == sb.GetLength();
}

const UTF8Char *Net::WebServer::WebConnection::GetRespHeaders()
{
	return this->respHeaders->ToString();
}

UOSInt Net::WebServer::WebConnection::Read(UInt8 *buff, UOSInt size)
{
	return 0;
}

UOSInt Net::WebServer::WebConnection::Write(const UInt8 *buff, UOSInt size)
{
	if (!this->respHeaderSent)
	{
		if (this->currReq)
		{
			SendHeaders(this->currReq->GetProtocol());
		}
		else
		{
			SendHeaders(Net::WebServer::IWebRequest::RequestProtocol::HTTP1_1);
		}
	}
	if (this->respDataEnd)
		return 0;
	if (this->logger)
	{
		this->logger(this->loggerObj, size);
	}
	this->svr->ExtendTimeout(cli);
	if (this->respTranEnc == 1)
	{
		UOSInt retSize = 0;
		UOSInt ohSize;
		UOSInt writeSize;
		UInt8 sbuff[2048 + 7];
		UTF8Char *sptr;
		while (size > 0)
		{
			writeSize = size;
			if (writeSize > 2048)
			{
				writeSize = 2048;		
			}
			sptr = Text::StrConcatC(Text::StrHexVal32V(sbuff, (UInt32)size), UTF8STRC("\r\n"));
			ohSize = (UOSInt)(sptr - sbuff) + 2;
			MemCopyNO(sptr, buff, writeSize);
			buff += writeSize;
			sptr += writeSize;
			size -= writeSize;
			sptr[0] = 13;
			sptr[1] = 10;
			sptr += 2;
			if (this->cstm)
			{
				writeSize = this->cstm->Write(sbuff, (UOSInt)(sptr - sbuff));
			}
			else
			{
				writeSize = this->cli->Write(sbuff, (UOSInt)(sptr - sbuff));
			}
			this->respLeng += (UOSInt)(sptr - sbuff);
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
		this->respLeng += size;
		if (this->cstm)
		{
			return this->cstm->Write(buff, size);
		}
		else
		{
			return this->cli->Write(buff, size);
		}
	}
}

Int32 Net::WebServer::WebConnection::Flush()
{
	return 0;
}

void Net::WebServer::WebConnection::Close()
{
	if (!this->respHeaderSent)
	{
		if (this->currReq)
		{
			SendHeaders(this->currReq->GetProtocol());
		}
		else
		{
			SendHeaders(Net::WebServer::IWebRequest::RequestProtocol::HTTP1_1);
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

void Net::WebServer::WebConnection::SetSendLogger(SendLogger logger, void *userObj)
{
	this->logger = logger;
	this->loggerObj = userObj;
}
