#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPServerMonitor.h"
#include "Sync/Thread.h"

UInt32 __stdcall Net::HTTPServerMonitor::ThreadProc(void *userObj)
{
	Net::HTTPServerMonitor *me = (Net::HTTPServerMonitor*)userObj;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		me->currOnline = me->CheckOnline();
		me->threadEvt->Wait(30000);
	}
	me->threadRunning = false;
	return 0;
}

Bool Net::HTTPServerMonitor::CheckOnline()
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, this->url, "GET", false);
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	DEL_CLASS(cli);
	return status == Net::WebStatus::SC_OK;
}

Net::HTTPServerMonitor::HTTPServerMonitor(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *url)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->url = Text::StrCopyNew(url);
	this->currOnline = false;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Net.HTTPServerMonitor.threadEvt"));

	Sync::Thread::Create(ThreadProc, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
}

Net::HTTPServerMonitor::~HTTPServerMonitor()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
	DEL_CLASS(this->threadEvt);
	Text::StrDelNew(this->url);
}

Bool Net::HTTPServerMonitor::IsOnline()
{
	return this->currOnline;
}