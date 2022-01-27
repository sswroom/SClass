#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Net/SSLEngine.h"
#include "Parser/FileParser/X509Parser.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::SSLEngine::ServerThread(void *userObj)
{
	ThreadState *state = (ThreadState*)userObj;
	state->status = ThreadStatus::NewClient;
	while (!state->me->threadToStop)
	{
		if (state->s)
		{
			state->status = ThreadStatus::Processing;
			Socket *s = state->s;
			state->s = 0;

			Net::TCPClient *cli = state->me->CreateServerConn(s);
			if (cli)
			{
				state->clientReady(cli, state->clientReadyObj);
			}
			else
			{
			}
			state->status = ThreadStatus::Running;
		}
		else if (state->status == ThreadStatus::NewClient)
		{
			state->status = ThreadStatus::Running;
		}
		state->evt->Wait(10000);		
	}
	state->status = ThreadStatus::NotRunning;
	return 0;
}

Net::SSLEngine::SSLEngine(Net::SocketFactory *sockf)
{
	this->sockf = sockf;
	this->maxThreadCnt = 10;
	this->currThreadCnt = 0;
	NEW_CLASS(this->threadMut, Sync::Mutex());
	this->threadMut->SetDebName((const UTF8Char*)"SSLEngine");
	this->threadSt = MemAlloc(ThreadState, this->maxThreadCnt);
	this->threadToStop = false;
	UOSInt i = this->maxThreadCnt;
	while (i-- > 0)
	{
		this->threadSt[i].status = ThreadStatus::NotRunning;
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
			if (this->threadSt[i].status != ThreadStatus::NotRunning)
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
	Crypto::Cert::X509File *certASN1 = 0;
	Crypto::Cert::X509File *keyASN1 = 0;
	if (certFile)
	{
		if ((certASN1 = (Crypto::Cert::X509File*)parser.ParseFilePath(certFile)) == 0)
		{
			return false;
		}
		if (certASN1->GetFileType() != Crypto::Cert::X509File::FileType::Cert)
		{
			DEL_CLASS(certASN1);
			return false;
		}
	}
	if (keyFile)
	{
		if ((keyASN1 = (Crypto::Cert::X509File*)parser.ParseFilePath(keyFile)) == 0)
		{
			SDEL_CLASS(certASN1);
			return false;
		}
	}
	Crypto::Cert::X509Cert *issuerCert = Crypto::Cert::CertUtil::FindIssuer((Crypto::Cert::X509Cert*)certASN1);
	Bool ret = this->SetServerCertsASN1((Crypto::Cert::X509Cert*)certASN1, keyASN1, issuerCert);
	SDEL_CLASS(issuerCert);
	SDEL_CLASS(certASN1);
	SDEL_CLASS(keyASN1);
	return ret;
}

void Net::SSLEngine::ServerInit(Socket *s, ClientReadyHandler readyHdlr, void *userObj)
{
	UOSInt i = 0;
	UOSInt j = INVALID_INDEX;
	Bool found = false;
	Sync::MutexUsage mutUsage(this->threadMut);
	while (!found)
	{
		i = 0;
		while (i < this->currThreadCnt)
		{
			if (this->threadSt[i].status == ThreadStatus::Running)
			{
				j = i;
			}
			else if (this->threadSt[i].s == s)
			{
				found = true;
				break;
			}
			i++;
		}
		if (found)
		{
			break;
		}
		if (j != INVALID_INDEX)
		{
			i = j;
			found = true;
			this->threadSt[i].clientReady = readyHdlr;
			this->threadSt[i].clientReadyObj = userObj;
			this->threadSt[i].s = s;
			this->threadSt[i].evt->Set();
			this->threadSt[i].status = ThreadStatus::NewClient;
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
			this->threadSt[i].status = ThreadStatus::Starting;
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

Text::CString Net::SSLEngine::ErrorTypeGetName(ErrorType err)
{
	switch (err)
	{
	case ErrorType::None:
		return CSTR("No Error");
	case ErrorType::HostnameNotResolved:
		return CSTR("Hostname cannot not resolve");
	case ErrorType::OutOfMemory:
		return CSTR("Out of memory");
	case ErrorType::CannotConnect:
		return CSTR("Cannot connect to destination");
	case ErrorType::InitSession:
		return CSTR("Failed in initializing session");
	case ErrorType::CertNotFound:
		return CSTR("Server Certification not found");
	case ErrorType::InvalidName:
		return CSTR("Invalid cert name");
	case ErrorType::SelfSign:
		return CSTR("Self Signed Certification");
	case ErrorType::InvalidPeriod:
		return CSTR("Valid period out of range");
	default:
		return CSTR("Unknown");
	}
}
