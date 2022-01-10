#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/SocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextEnc/URIEncoding.h"

Net::HTTPOSClient::HTTPOSClient(Net::SocketFactory *sockf, const UTF8Char *userAgent, UOSInt uaLen, Bool kaConn) : Net::HTTPClient(sockf, kaConn)
{
	this->clsData = 0;
	this->cliHost = 0;
	this->writing = false;
	this->dataBuff = 0;
	this->buffSize = 0;
//	this->timeOutMS = 5000;
	this->dataBuff = 0;
	this->reqMstm = 0;
}

Net::HTTPOSClient::~HTTPOSClient()
{
}

Bool Net::HTTPOSClient::IsError()
{
	return true;
}

UOSInt Net::HTTPOSClient::Read(UInt8 *buff, UOSInt size)
{
	return 0;
}

UOSInt Net::HTTPOSClient::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 Net::HTTPOSClient::Flush()
{
	return 0;
}

void Net::HTTPOSClient::Close()
{
}

Bool Net::HTTPOSClient::Recover()
{
	return false;
}

Bool Net::HTTPOSClient::Connect(const UTF8Char *url, UOSInt urlLen, const Char *method, Double *timeDNS, Double *timeConn, Bool defHeaders)
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

void Net::HTTPOSClient::AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
}

void Net::HTTPOSClient::EndRequest(Double *timeReq, Double *timeResp)
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

void Net::HTTPOSClient::SetTimeout(Int32 ms)
{
}
