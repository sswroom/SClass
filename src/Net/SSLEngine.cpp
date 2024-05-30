#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/TrustStore.h"
#include "Crypto/Cert/X509FileList.h"
#include "Net/SSLEngine.h"
#include "Parser/FileParser/X509Parser.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Optional<Crypto::Cert::CertStore> Net::SSLEngine::trustStore = 0;
UInt32 Net::SSLEngine::trustStoreCnt = 0;

UInt32 __stdcall Net::SSLEngine::ServerThread(AnyType userObj)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<ThreadState> state = userObj.GetNN<ThreadState>();
	NN<Socket> s;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("SSLEngine")), state->index);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
	state->status = ThreadStatus::NewClient;
	while (!state->me->threadToStop)
	{
		if (state->s.SetTo(s))
		{
			state->status = ThreadStatus::Processing;
			state->s = 0;

			NN<Net::SSLClient> cli;
			if (state->me->CreateServerConn(s).SetTo(cli))
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

Net::SSLEngine::SSLEngine(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	this->maxThreadCnt = 10;
	this->currThreadCnt = 0;
	this->usedTrustStore = 0;
	this->threadMut.SetDebName((const UTF8Char*)"SSLEngine");
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
			Sync::SimpleThread::Sleep(1);
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
	if (this->usedTrustStore.NotNull())
	{
		if (Sync::Interlocked::DecrementU32(trustStoreCnt) == 0)
		{
			trustStore.Delete();
		}
	}
}

Bool Net::SSLEngine::ServerSetCerts(Text::CStringNN certFile, Text::CStringNN keyFile, Text::CString caFile)
{
	Parser::FileParser::X509Parser parser;
	NN<Crypto::Cert::X509File> certASN1;
	NN<Crypto::Cert::X509File> keyASN1;
	Data::ArrayListNN<Crypto::Cert::X509Cert> cacerts;
	UOSInt i;
	UOSInt j;
	if (certFile.leng > 0)
	{
		if (!Optional<Crypto::Cert::X509File>::ConvertFrom(parser.ParseFilePath(certFile)).SetTo(certASN1))
		{
			return false;
		}
		if (certASN1->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
		{
		}
		else if (certASN1->GetFileType() == Crypto::Cert::X509File::FileType::FileList)
		{
			Bool found = false;
			Crypto::Cert::X509FileList *fileList = (Crypto::Cert::X509FileList*)certASN1.Ptr();
			NN<Crypto::Cert::X509File> file;
			i = 0;
			j = fileList->GetFileCount();
			while (i < j)
			{
				if (fileList->GetFile(i).SetTo(file) && file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
				{
					if (!found)
					{
						found = true;
						certASN1 = NN<Crypto::Cert::X509File>::ConvertFrom(file->Clone());
					}
					else
					{
						cacerts.Add(NN<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone()));
					}
				}
				i++;
			}
			DEL_CLASS(fileList);
			if (!found)
				return false;
		}
		else
		{
			certASN1.Delete();
			return false;
		}
	}
	else
	{
		return false;
	}
	if (keyFile.leng > 0)
	{
		if (!Optional<Crypto::Cert::X509File>::ConvertFrom(parser.ParseFilePath(keyFile)).SetTo(keyASN1))
		{
			certASN1.Delete();
			cacerts.DeleteAll();
			return false;
		}
	}
	else
	{
		certASN1.Delete();
		cacerts.DeleteAll();
		return false;
	}
	if (caFile.leng > 0)
	{
		NN<Crypto::Cert::X509File> cacertASN1;
		if (!Optional<Crypto::Cert::X509File>::ConvertFrom(parser.ParseFilePath(caFile.OrEmpty())).SetTo(cacertASN1))
		{
			return false;
		}
		if (cacertASN1->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
		{
			cacerts.Add(NN<Crypto::Cert::X509Cert>::ConvertFrom(cacertASN1));
		}
		else if (cacertASN1->GetFileType() == Crypto::Cert::X509File::FileType::FileList)
		{
			Crypto::Cert::X509FileList *fileList = (Crypto::Cert::X509FileList*)cacertASN1.Ptr();
			NN<Crypto::Cert::X509File> file;
			i = 0;
			j = fileList->GetFileCount();
			while (i < j)
			{
				if (fileList->GetFile(i).SetTo(file) && file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
				{
					cacerts.Add(NN<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone()));
				}
				i++;
			}
			DEL_CLASS(fileList);
		}
		else
		{
			cacertASN1.Delete();
			certASN1.Delete();
			cacerts.DeleteAll();
			return false;
		}
	}
	if (cacerts.GetCount() == 0)
	{
		NN<Crypto::Cert::X509Cert> issuerCert;
		if (Crypto::Cert::CertUtil::FindIssuer(NN<Crypto::Cert::X509Cert>::ConvertFrom(certASN1)).SetTo(issuerCert))
		{
			cacerts.Add(issuerCert);
		}		
	}
	Bool ret = this->ServerSetCertsASN1(NN<Crypto::Cert::X509Cert>::ConvertFrom(certASN1), keyASN1, cacerts);
	certASN1.Delete();
	keyASN1.Delete();
	cacerts.DeleteAll();
	return ret;
}

Bool Net::SSLEngine::ServerSetCerts(Text::CStringNN certFile, Text::CStringNN keyFile)
{
	return ServerSetCerts(certFile, keyFile, CSTR_NULL);
}

void Net::SSLEngine::ServerInit(NN<Socket> s, ClientReadyHandler readyHdlr, AnyType userObj)
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
			NEW_CLASS(this->threadSt[i].evt, Sync::Event(true));
			this->threadSt[i].clientReady = readyHdlr;
			this->threadSt[i].clientReadyObj = userObj;
			this->threadSt[i].s = s;
			this->threadSt[i].status = ThreadStatus::Starting;
			this->threadSt[i].me = this;
			this->threadSt[i].index = i;
			Sync::ThreadUtil::Create(ServerThread, &this->threadSt[i]);
			break;
		}
		else
		{
			mutUsage.EndUse();
			Sync::SimpleThread::Sleep(10);
			mutUsage.BeginUse();
		}
	}
}

NN<Crypto::Cert::CertStore> Net::SSLEngine::GetTrustStore()
{
	NN<Crypto::Cert::CertStore> store;
	if (!this->usedTrustStore.SetTo(store))
	{
		Sync::Interlocked::IncrementU32(trustStoreCnt);
		if (!trustStore.SetTo(store))
		{
			store = Crypto::Cert::TrustStore::Load();
			trustStore = store;
		}
		this->usedTrustStore = store;
		return store;
	}
	else
	{
		return store;
	}
}

Text::CStringNN Net::SSLEngine::ErrorTypeGetName(ErrorType err)
{
	switch (err)
	{
	case ErrorType::None:
		return CSTR("No Error");
	case ErrorType::InitEnv:
		return CSTR("Failed in initializing environment");
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
