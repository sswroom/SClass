#include "Stdafx.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/SSLEngine.h"
#include "Parser/FileParser/X509Parser.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::SSLEngine::ServerThread(void *userObj)
{
	ThreadState *state = (ThreadState*)userObj;
	state->status = TS_RUNNING;
	while (!state->me->threadToStop)
	{
		if (state->s)
		{
			state->status = TS_PROCESSING;
			UInt32 *s = state->s;
			state->s = 0;

			Net::TCPClient *cli = state->me->CreateServerConn(s);
			if (cli)
			{
				state->clientReady(cli, state->clientReadyObj);
			}
			state->status = TS_RUNNING;
		}
		state->evt->Wait(10000);		
	}
	state->status = TS_NOT_RUNNING;
	return 0;
}

Net::SSLEngine::SSLEngine(Net::SocketFactory *sockf)
{
	this->sockf = sockf;
	this->maxThreadCnt = 10;
	this->currThreadCnt = 0;
	NEW_CLASS(this->threadMut, Sync::Mutex());
	this->threadSt = MemAlloc(ThreadState, this->maxThreadCnt);
	this->threadToStop = false;
	UOSInt i = this->maxThreadCnt;
	while (i-- > 0)
	{
		this->threadSt[i].status = TS_NOT_RUNNING;
		this->threadSt[i].s = 0;
	}
}

Net::SSLEngine::~SSLEngine()
{
	this->threadToStop = true;
	UOSInt i = this->currThreadCnt;
	while (i-- > 0)
	{
		this->threadSt[i].evt->Set();
	}
	Bool running = true;
	while (true)
	{
		running = false;
		i = this->currThreadCnt;
		while (i-- > 0)
		{
			if (this->threadSt[i].status != TS_NOT_RUNNING)
			{
				running = true;
				break;
			}
		}
		if (running)
		{
			Sync::Thread::Sleep(1);
		}
		else
		{
			break;
		}
	}
	i = this->currThreadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threadSt[i].evt);
	}
	MemFree(this->threadSt);
	DEL_CLASS(this->threadMut);
}

Bool Net::SSLEngine::SetServerCerts(const UTF8Char *certFile, const UTF8Char *keyFile)
{
	Parser::FileParser::X509Parser parser;
	Crypto::X509File *certASN1 = 0;
	Crypto::X509File *keyASN1 = 0;
	if (certFile)
	{
		if ((certASN1 = (Crypto::X509File*)parser.ParseFilePath(certFile)) == 0)
		{
			return false;
		}
	}
	if (keyFile)
	{
		if ((keyASN1 = (Crypto::X509File*)parser.ParseFilePath(keyFile)) == 0)
		{
			SDEL_CLASS(certASN1);
			return false;
		}
	}
	Bool ret = this->SetServerCertsASN1(certASN1, keyASN1);
	SDEL_CLASS(certASN1);
	SDEL_CLASS(keyASN1);
	return ret;
}

void Net::SSLEngine::ServerInit(UInt32 *s, ClientReadyHandler readyHdlr, void *userObj)
{
	UOSInt i = 0;
	Bool found = false;
	Sync::MutexUsage mutUsage(this->threadMut);
	while (!found)
	{
		while (i < this->currThreadCnt)
		{
			if (this->threadSt[i].status == TS_RUNNING)
			{
				found = true;
				this->threadSt[i].clientReady = readyHdlr;
				this->threadSt[i].clientReadyObj = userObj;
				this->threadSt[i].s = s;
				this->threadSt[i].evt->Set();
				break;
			}
			i++;
		}
		if (found)
		{
			break;
		}
		if (this->currThreadCnt < this->maxThreadCnt)
		{
			i = this->currThreadCnt;
			this->currThreadCnt++;
			NEW_CLASS(this->threadSt[i].evt, Sync::Event(true, (const UTF8Char*)"Net.SSLEngine.threadSt.evt"));
			this->threadSt[i].clientReady = readyHdlr;
			this->threadSt[i].clientReadyObj = userObj;
			this->threadSt[i].s = s;
			this->threadSt[i].status = TS_STARTING;
			this->threadSt[i].me = this;
			Sync::Thread::Create(ServerThread, &this->threadSt[i]);
			break;
		}
		else
		{
			mutUsage.EndUse();
			Sync::Thread::Sleep(10);
			mutUsage.BeginUse();
		}
	}
}

const UTF8Char *Net::SSLEngine::ErrorTypeGetName(ErrorType err)
{
	switch (err)
	{
	case ET_NONE:
		return (const UTF8Char*)"No Error";
	case ET_HOSTNAME_NOT_RESOLVED:
		return (const UTF8Char*)"Hostname cannot not resolve";
	case ET_OUT_OF_MEMORY:
		return (const UTF8Char*)"Out of memory";
	case ET_CANNOT_CONNECT:
		return (const UTF8Char*)"Cannot connect to destination";
	case ET_INIT_SESSION:
		return (const UTF8Char*)"Failed in initializing session";
	case ET_CERT_NOT_FOUND:
		return (const UTF8Char*)"Server Certification not found";
	case ET_INVALID_NAME:
		return (const UTF8Char*)"Invalid cert name";
	case ET_SELF_SIGN:
		return (const UTF8Char*)"Self Signed Certification";
	case ET_INVALID_PERIOD:
		return (const UTF8Char*)"Valid period out of range";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
