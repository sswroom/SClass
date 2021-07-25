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

Net::WebServer::WebConnection::WebConnection(Net::SocketFactory *sockf, Net::TCPClient *cli, WebListener *svr, IWebHandler *hdlr, Bool allowProxy, Bool allowKA) : Net::WebServer::IWebResponse((const UTF8Char*)"WebConnection")
{
	this->sockf = sockf;
	this->cli = cli;
	this->svr = svr;
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
	NEW_CLASS(this->respHeaders, Text::StringBuilderUTF8());
}

Net::WebServer::WebConnection::~WebConnection()
{
	if (this->proxyMode)
	{
		this->proxyCli->Close();
		while (this->proxyMode)
		{
			Sync::Thread::Sleep(10);
		}
	}
	DEL_CLASS(this->cli);
	MemFree(this->dataBuff);
	SDEL_CLASS(this->currReq);
	DEL_CLASS(this->respHeaders);
}

void Net::WebServer::WebConnection::ReceivedData(const UInt8 *buff, UOSInt size)
{
	UTF8Char sbuff[512];
	UTF8Char *sbuffTmp;
	UTF8Char *reqURL;
	Char *sarr[4];
	UOSInt i;
	UOSInt j;
	UOSInt lineStart;
	UOSInt strIndex;
	UOSInt strLen;
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
					if (Text::StrSplit(sarr, 4, (Char*)&this->dataBuff[lineStart], ' ') == 3)
					{
						if (Text::StrCompare(sarr[2], "RTSP/1.0") == 0)
						{
							Net::WebServer::IWebRequest::RequestProtocol reqProto = Net::WebServer::IWebRequest::REQPROTO_RTSP1_0;
							Bool secureConn = false;
							strLen = Text::StrCharCnt(sarr[1]);
							if (strLen > 511)
							{
								sbuffTmp = MemAlloc(UTF8Char, strLen + 1);
								Text::StrConcat(sbuffTmp, (UTF8Char*)sarr[1]);
								reqURL = sbuffTmp;
							}
							else
							{
								sbuffTmp = 0;
								Text::StrConcat(sbuff, (UTF8Char*)sarr[1]);
								reqURL = sbuff;
							}
							this->respHeaders->ClearStr();
							this->respHeaderSent = false;
							Net::SocketUtil::AddressInfo cliAddr;
							this->cli->GetRemoteAddr(&cliAddr);
							UInt16 cliPort = this->cli->GetRemotePort();
							UInt16 svrPort = this->cli->GetLocalPort();

							if (this->currReq)
							{
								DEL_CLASS(this->currReq);
								this->currReq = 0;
							}

							if (Text::StrCompare(sarr[0], "DESCRIBE") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_DESCRIBE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "ANNOUNCE") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_ANNOUNCE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "GET_PARAMETER") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_GET_PARAMETER, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "OPTIONS") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_OPTIONS, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "PAUSE") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_PAUSE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "PLAY") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_PLAY, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "RECORD") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_RECORD, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "REDIRECT") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_REDIRECT, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "SETUP") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_SETUP, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "SET_PARAMETER") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_SET_PARAMETER, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrCompare(sarr[0], "TEARDOWN") == 0)
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_RTSP_TEARDOWN, reqProto, secureConn, &cliAddr, cliPort, svrPort));
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
							if (sbuffTmp)
							{
								MemFree(sbuffTmp);
							}
						}
						else
						{
							Net::WebServer::IWebRequest::RequestProtocol reqProto;
							Bool secureConn = false;
							if (Text::StrEquals(sarr[2], "HTTP/1.1"))
							{
								reqProto = Net::WebServer::IWebRequest::REQPROTO_HTTP1_1;
							}
							else if (Text::StrEquals(sarr[2], "HTTP/1.0"))
							{
								reqProto = Net::WebServer::IWebRequest::REQPROTO_HTTP1_0;
							}
							else
							{
								this->respHeaderSent = false;
								this->respStatus = Net::WebStatus::SC_VERSION_NOT_SUPPORTED;
								this->respDataEnd = false;
								this->respHeaders->ClearStr();

								this->Close();

								this->cli->Close();
								return;
							}
							strLen = Text::StrCharCnt(sarr[1]);
							if (strLen > 511)
							{
								sbuffTmp = MemAlloc(UTF8Char, strLen + 1);
								Text::StrConcat(sbuffTmp, (UTF8Char*)sarr[1]);
								reqURL = sbuffTmp;
							}
							else
							{
								sbuffTmp = 0;
								Text::StrConcat(sbuff, (UTF8Char*)sarr[1]);
								reqURL = sbuff;
							}
							this->respHeaders->ClearStr();
							this->respHeaderSent = false;
							Net::SocketUtil::AddressInfo cliAddr;
							this->cli->GetRemoteAddr(&cliAddr);
							UInt16 cliPort = this->cli->GetRemotePort();
							UInt16 svrPort = this->cli->GetLocalPort();

							if (this->currReq)
							{
								DEL_CLASS(this->currReq);
								this->currReq = 0;
							}

							if (Text::StrEquals(sarr[0], "GET"))
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_HTTP_GET, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEquals(sarr[0], "POST"))
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_HTTP_POST, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEquals(sarr[0], "PUT"))
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_HTTP_PUT, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEquals(sarr[0], "PATCH"))
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_HTTP_PATCH, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEquals(sarr[0], "DELETE"))
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_HTTP_DELETE, reqProto, secureConn, &cliAddr, cliPort, svrPort));
							}
							else if (Text::StrEquals(sarr[0], "CONNECT"))
							{
								NEW_CLASS(this->currReq, WebRequest(reqURL, Net::WebServer::IWebRequest::REQMETH_HTTP_CONNECT, reqProto, secureConn, &cliAddr, cliPort, svrPort));
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
							if (sbuffTmp)
							{
								MemFree(sbuffTmp);
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
					strIndex = Text::StrIndexOf((Char*)&this->dataBuff[lineStart], ':');
					if (strIndex != INVALID_INDEX)
					{
						this->dataBuff[lineStart + (UOSInt)strIndex] = 0;
						strIndex++;
						while (this->dataBuff[lineStart + (UOSInt)strIndex] == ' ')
						{
							strIndex++;
						}
						this->dataBuff[i] = 0;
						if (this->currReq)
						{
							this->currReq->AddHeader(&this->dataBuff[lineStart], &this->dataBuff[lineStart + (UOSInt)strIndex]);
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
	this->cli->Write(buff, size);
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
		this->svr->LogMessage(this->currReq, (const UTF8Char*)"End Proxy Conn");
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
	sb.Append((const UTF8Char*)" ");
	if (this->currReq)
	{
		const UTF8Char *uri = this->currReq->GetRequestURI();
		if (uri)
		{
			sb.Append(uri);
			sb.AppendChar(' ', 1);
		}
	}
	sb.Append((const UTF8Char*)"Process Timeout");
	this->svr->LogMessage(0, sb.ToString());
}

const UTF8Char *Net::WebServer::WebConnection::GetRequestURL()
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
	Char *sptr;
	buff = MemAlloc(UInt8, 256 + (this->respHeaders->GetLength() * 3));
	sptr = (Char*)buff;
	if (protocol == Net::WebServer::IWebRequest::REQPROTO_HTTP1_0)
	{
		sptr = Text::StrConcat(sptr, "HTTP/1.0 ");
	}
	else if (protocol == Net::WebServer::IWebRequest::REQPROTO_RTSP1_0)
	{
		sptr = Text::StrConcat(sptr, "RTSP/1.0 ");
	}
	else
	{
		sptr = Text::StrConcat(sptr, "HTTP/1.1 ");
	}
	sptr = Text::StrInt32(sptr, this->respStatus);
	*sptr++ = ' ';
	sptr = (Char*)Text::StrConcat((UTF8Char*)sptr, Net::WebStatus::GetCodeName(this->respStatus));
	sptr = Text::StrConcat(sptr, "\r\n");

	sptr = Text::StrConcat(sptr, (Char*)this->respHeaders->ToString());
	sptr = Text::StrConcat(sptr, "\r\n");

	cli->Write(buff, (UOSInt)(sptr - (Char*)buff));
	if (this->logger)
	{
		this->logger(this->loggerObj, (UOSInt)(sptr - (Char*)buff));
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

	const UTF8Char *reqURI = this->currReq->GetRequestURI();
	Net::WebServer::IWebRequest::RequestMethod reqMeth = this->currReq->GetReqMethod();
	if (reqMeth == Net::WebServer::IWebRequest::REQMETH_HTTP_CONNECT && this->allowProxy)
	{
		Net::TCPClient *proxyCli;
		UTF8Char sbuff[512];
		UOSInt i;
		Text::StrConcat(sbuff, reqURI);
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
	else if ((reqMeth == Net::WebServer::IWebRequest::REQMETH_HTTP_GET || reqMeth == Net::WebServer::IWebRequest::REQMETH_HTTP_POST) && Text::StrStartsWith(reqURI, (const UTF8Char*)"http://"))
	{
		Manage::HiResClock clk;
		Double t;
		Net::HTTPClient *httpCli;
		Text::StringBuilderUTF8 sb;

		clk.Start();
		if (this->allowProxy)
		{
			httpCli = Net::HTTPClient::CreateClient(this->sockf, 0, true, Text::StrStartsWith(reqURI, (const UTF8Char*)"https://"));
			httpCli->SetTimeout(5000);
			if (reqMeth == Net::WebServer::IWebRequest::REQMETH_HTTP_GET)
			{
				httpCli->Connect(reqURI, "GET", 0, 0, false);
			}
			else
			{
				httpCli->Connect(reqURI, "POST", 0, 0, false);
			}

			if (httpCli->IsError())
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"Conn Err: ");
				sb.Append(reqURI);
				this->svr->LogMessage(this->currReq, sb.ToString());

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
				const UTF8Char *csptr;
				Bool lengFound = false;
				
				i = 0;
				j = this->currReq->GetHeaderCnt();
				while (i < j)
				{
					csptr = this->currReq->GetHeaderName(i);
					if (Text::StrEquals(csptr, (const UTF8Char*)"Host"))
					{
					}
					else if (Text::StrEquals(csptr, (const UTF8Char*)"Proxy-Connection"))
					{
					}
					else if (Text::StrEquals(csptr, (const UTF8Char*)"Accept-Encoding"))
					{
					}
					else
					{
						const UTF8Char *csptr2 = this->currReq->GetHeaderValue(i);
						httpCli->AddHeader(csptr, csptr2);
					}
					i++;
				}
				if (reqMeth == Net::WebServer::IWebRequest::REQMETH_HTTP_POST)
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
					httpCli->GetRespHeader(i, sbuffHdr);
					if (Text::StrStartsWith(sbuffHdr, (const UTF8Char*)"Content-Length: "))
					{
						lengFound = true;
					}
					k = Text::StrIndexOf(sbuffHdr, (const UTF8Char*)": ");
					if (k != INVALID_INDEX)
					{
						sbuffHdr[k] = 0;
						if (Text::StrEquals(sbuffHdr, (const UTF8Char*)"Server"))
						{
						}
						else if (Text::StrEquals(sbuffHdr, (const UTF8Char*)"Connection"))
						{
						}
						else if (Text::StrEquals(sbuffHdr, (const UTF8Char*)"Transfer-Encoding"))
						{
						}
						else
						{
							this->AddHeader(sbuffHdr, &sbuffHdr[k + 2]);
						}
					}
					else
					{
					}
					i++;
				}
				this->AddHeader((const UTF8Char*)"Server", this->svr->GetServerName());
				this->AddHeader((const UTF8Char*)"Proxy-Connection", (const UTF8Char*)"closed");

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

		t = clk.GetTimeDiff();
		this->svr->LogAccess(this->currReq, this, t);

		Text::StringBuilderUTF8 sb;
		this->currReq->GetHeader(&sb, (const UTF8Char*)"Connection");
		if (sb.Equals((const UTF8Char*)"keep-alive") && this->allowKA)
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

Bool Net::WebServer::WebConnection::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	if (this->respHeaderSent)
		return false;
	this->respHeaders->Append(name);
	this->respHeaders->Append((const UTF8Char*)": ");
	this->respHeaders->Append(value);
	this->respHeaders->Append((const UTF8Char*)"\r\n");
	return true;
}

Bool Net::WebServer::WebConnection::AddDefHeaders(Net::WebServer::IWebRequest *req)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Text::StringBuilderUTF8 sb;
	AddTimeHeader((const UTF8Char*)"Date", &dt);
	AddHeader((const UTF8Char*)"Server", this->svr->GetServerName());
	req->GetHeader(&sb, (const UTF8Char*)"Connection");
	if (this->allowKA && sb.Equals((const UTF8Char*)"keep-alive"))
	{
		AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"keep-alive");
		AddHeader((const UTF8Char*)"Keep-Alive", (const UTF8Char*)"timeout=10, max=1000");
	}
	else
	{
		AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"close");
	}
	return false;
}

UInt64 Net::WebServer::WebConnection::GetRespLength()
{
	return this->respLeng;
}

void Net::WebServer::WebConnection::ShutdownSend()
{
	this->cli->ShutdownSend();
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
			SendHeaders(Net::WebServer::IWebRequest::REQPROTO_HTTP1_1);
		}
	}
	if (this->respDataEnd)
		return 0;
	this->respLeng += size;
	if (this->logger)
	{
		this->logger(this->loggerObj, size);
	}
	this->svr->ExtendTimeout(cli);
	return cli->Write(buff, size);
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
			SendHeaders(Net::WebServer::IWebRequest::REQPROTO_HTTP1_1);
		}
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
