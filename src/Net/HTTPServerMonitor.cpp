#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPServerMonitor.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Net::HTTPServerMonitor::ThreadProc(AnyType userObj)
{
	NN<Net::HTTPServerMonitor> me = userObj.GetNN<Net::HTTPServerMonitor>();
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		me->currOnline = me->CheckOnline();
		me->threadEvt.Wait(30000);
	}
	me->threadRunning = false;
	return 0;
}

Bool Net::HTTPServerMonitor::CheckOnline()
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, this->url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	cli.Delete();
	return status == Net::WebStatus::SC_OK;
}

Net::HTTPServerMonitor::HTTPServerMonitor(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Text::String> url)
{
	this->clif = clif;
	this->ssl = ssl;
	this->url = url->Clone();
	this->currOnline = false;
	this->threadRunning = false;
	this->threadToStop = false;

	Sync::ThreadUtil::Create(ThreadProc, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

Net::HTTPServerMonitor::HTTPServerMonitor(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url)
{
	this->clif = clif;
	this->ssl = ssl;
	this->url = Text::String::New(url);
	this->currOnline = false;
	this->threadRunning = false;
	this->threadToStop = false;

	Sync::ThreadUtil::Create(ThreadProc, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

Net::HTTPServerMonitor::~HTTPServerMonitor()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->url->Release();
}

Bool Net::HTTPServerMonitor::IsOnline()
{
	return this->currOnline;
}