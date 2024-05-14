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
#include "Text/TextBinEnc/URIEncoding.h"

Net::HTTPOSClient::HTTPOSClient(NN<Net::SocketFactory> sockf, Text::CString userAgent, Bool kaConn) : Net::HTTPClient(sockf, kaConn)
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

Bool Net::HTTPOSClient::IsError() const
{
	return true;
}

UOSInt Net::HTTPOSClient::Read(const Data::ByteArray &buff)
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

Bool Net::HTTPOSClient::Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders)
{
	timeDNS.Set(-1);
	timeConn.Set(-1);
	return false;
}

void Net::HTTPOSClient::AddHeaderC(Text::CStringNN name, Text::CString value)
{
}

void Net::HTTPOSClient::EndRequest(OptOut<Double> timeReq, OptOut<Double> timeResp)
{
	timeReq.Set(-1);
	timeResp.Set(-1);
	return;
}

void Net::HTTPOSClient::SetTimeout(Data::Duration ms)
{
}

Bool Net::HTTPOSClient::IsSecureConn() const
{
	return false;
}

Bool Net::HTTPOSClient::SetClientCert(NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key)
{
	return false;
}

Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> Net::HTTPOSClient::GetServerCerts()
{
	return 0;
}
