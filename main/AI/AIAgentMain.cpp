#include "Stdafx.h"
#include "AI/AIReadDBSkill.h"
#include "AI/SimpleAIAgent.h"
#include "Core/Core.h"
#include "DB/DBConfig.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/Java/JavaProperties.h"
#include "Net/OpenAIClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"

class AIAgentHandler : public Net::WebServer::WebServiceHandler
{
private:
	enum class RequestStatus
	{
		Pending,
		Processing,
		Completed,
		Failed
	};

	struct AIRequest
	{
		UInt64 requestId;
		NN<AI::AISession> session;
		NN<Text::String> question;
		Optional<Text::String> answer;
		RequestStatus status;
		Bool gotResult;
	};

	NN<AI::AIAgent> agent;
	NN<IO::LogTool> log;
	NN<Text::String> systemPrompt;
	Sync::Mutex reqMut;
	Data::UInt64FastMapNN<AIRequest> requests;
	Sync::Thread thread;

	static Text::CStringNN RequestStatusGetName(RequestStatus status)
	{
		switch (status)
		{
		case RequestStatus::Pending:
			return CSTR("Pending");
		case RequestStatus::Processing:
			return CSTR("Processing");
		case RequestStatus::Completed:
			return CSTR("Completed");
		case RequestStatus::Failed:
			return CSTR("Failed");
		default:
			return CSTR("Unknown");
		}
	}

	static void __stdcall ProcessThread(NN<Sync::Thread> t)
	{
		NN<AIAgentHandler> me = t->GetUserObj().GetNN<AIAgentHandler>();
		UIntOS i;
		UIntOS j;
		NN<AIRequest> aiReq;
		Text::StringBuilderUTF8 sb;
		while (!t->IsStopping())
		{
			Sync::MutexUsage mutUsage(me->reqMut);
			i = 0;
			j = me->requests.GetCount();
			while (i < j)
			{
				aiReq = me->requests.GetItemNoCheck(i);
				if (aiReq->status == RequestStatus::Pending)
				{
					aiReq->status = RequestStatus::Processing;
					mutUsage.EndUse();
					NN<Text::String> answer;
					if (!me->agent->Ask(aiReq->session, aiReq->question->ToCString()).SetTo(answer))
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Failed to get answer for question: "));
						sb.Append(aiReq->question);
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
						mutUsage.BeginUse();
						aiReq->status = RequestStatus::Failed;
					}
					else
					{
						mutUsage.BeginUse();
						aiReq->answer = answer;
						aiReq->status = RequestStatus::Completed;

						sb.ClearStr();
						sb.AppendC(UTF8STRC("Received answer, contains "));
						UIntOS messageCount = aiReq->session->GetMessageCount();
						sb.AppendUIntOS(messageCount);
						sb.AppendC(UTF8STRC(" messages."));
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
					}
				}
				if (aiReq->gotResult)
				{
					FreeAIRequest(aiReq);
					me->requests.RemoveAt(i);
					j--;
				}
				else
				{
					i++;
				}
			}
			mutUsage.EndUse();
			t->Wait(5000);
		}
	}

	static void __stdcall FreeAIRequest(NN<AIRequest> req)
	{
		req->question->Release();
		req->session.Delete();
		OPTSTR_DEL(req->answer);
		req.Delete();
	}

	static Bool __stdcall ReqFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
	{
		NN<AIAgentHandler> me = NN<AIAgentHandler>::ConvertFrom(svcHdlr);
		NN<Text::String> q;
		req->ParseHTTPForm();
		if (!req->GetHTTPFormStr(CSTR("q")).SetTo(q))
			return false;
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Received question: "));
		sb.Append(q);
		me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		NN<AIRequest> aiReq;
		NEW_CLASSNN(aiReq, AIRequest());
		aiReq->requestId = (UInt64)Data::DateTimeUtil::GetCurrTimeMillis();
		aiReq->session = me->agent->CreateSession(me->systemPrompt->ToCString());
		aiReq->question = q->Clone();
		aiReq->answer = nullptr;
		aiReq->status = RequestStatus::Pending;
		aiReq->gotResult = false;
		{
			Sync::MutexUsage mutUsage(me->reqMut);
			me->requests.Put(aiReq->requestId, aiReq);
		}
		me->thread.Notify();

		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddUInt64(CSTR("id"), aiReq->requestId);
		return me->ResponseJSONStr(req, resp, 0, json.Build());
	}

	static Bool __stdcall SessFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
	{
		NN<AIAgentHandler> me = NN<AIAgentHandler>::ConvertFrom(svcHdlr);
		req->ParseHTTPForm();
		NN<AIRequest> aiReq;
		UInt64 id;
		if (!req->GetHTTPFormUInt64(CSTR("id"), id))
		{
			return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		}
		Sync::MutexUsage mutUsage(me->reqMut);
		if (!me->requests.Get(id).SetTo(aiReq))
		{
			return resp->ResponseError(req, Net::WebStatus::SC_BAD_REQUEST);
		}

		NN<Text::String> answer;
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddStr(CSTR("question"), aiReq->question->ToCString());
		json.ObjectAddStr(CSTR("status"), RequestStatusGetName(aiReq->status));
		if (aiReq->answer.SetTo(answer))
			json.ObjectAddStr(CSTR("answer"), answer);
		if (aiReq->status == RequestStatus::Completed || aiReq->status == RequestStatus::Failed)
		{
			aiReq->gotResult = true;
		}
		json.ObjectBeginArray(CSTR("session"));
		UIntOS messageCount = aiReq->session->GetMessageCount();
		UIntOS i = 0;
		while (i < messageCount)
		{
			NN<Net::OpenAIMessage> message;
			if (aiReq->session->GetMessage(i).SetTo(message))
			{
				Text::CStringNN role;
				switch (message->GetRole())
				{
				case Net::OpenAIMessageRole::System:
					role = CSTR("system");
					break;
				case Net::OpenAIMessageRole::User:
					role = CSTR("user");
					break;
				case Net::OpenAIMessageRole::Assistant:
					role = CSTR("assistant");
					break;
				case Net::OpenAIMessageRole::Tool:
					role = CSTR("tool");
					break;
				default:
					role = CSTR("unknown");
					break;
				}
				json.ArrayBeginObject();
				json.ObjectAddStr(CSTR("role"), role);
				json.ObjectAddStr(CSTR("content"), message->GetContent());
				NN<Text::String> rawMessage;
				if (message->GetRawMessage().SetTo(rawMessage))
					json.ObjectAddStr(CSTR("rawMessage"), rawMessage);
				NN<Text::String> toolCallId;
				if (message->GetToolCallId().SetTo(toolCallId))
					json.ObjectAddStr(CSTR("toolCallId"), toolCallId);
				json.ObjectEnd();
			}
			i++;
		}
		json.ArrayEnd();
		return me->ResponseJSONStr(req, resp, 0, json.Build());
	}

public:
	AIAgentHandler(NN<AI::AIAgent> agent, NN<IO::LogTool> log, NN<Text::String> systemPrompt, Text::CStringNN webPath) : WebServiceHandler(webPath), thread(ProcessThread, this, CSTR("AIAgent"))
	{
		this->agent = agent;
		this->log = log;
		this->systemPrompt = systemPrompt->Clone();
		this->AddService(CSTR("/api/req"), Net::WebUtil::RequestMethod::HTTP_POST, ReqFunc);
		this->AddService(CSTR("/api/sess"), Net::WebUtil::RequestMethod::HTTP_POST, SessFunc);
		this->thread.Start();
	}
	
	virtual ~AIAgentHandler()
	{
		this->thread.Stop();
		this->systemPrompt->Release();
		this->requests.FreeAll(FreeAIRequest);
	}
};

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::LogTool log;
	IO::ConsoleWriter console;
	IO::ConsoleLogHandler logHdlr(console);
	log.AddLogHandler(logHdlr, IO::LogHandler::LogLevel::Raw);
	NN<IO::ConfigFile> cfg;
	NN<DB::DBTool> db;
	if (!IO::Java::JavaProperties::ParseAppProp().SetTo(cfg))
	{
		log.LogMessage(CSTR("Failed to parse application properties."), IO::LogHandler::LogLevel::Error);
		return 1;
	}
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	if (!DB::DBConfig::LoadFromConfig(clif, cfg, nullptr, log, false).SetTo(db))
	{
		log.LogMessage(CSTR("Failed to load database configuration."), IO::LogHandler::LogLevel::Error);
		cfg.Delete();
		return 1;
	}
	NN<Text::String> openAIUrl;
	NN<Text::String> openAIKey;
	NN<Text::String> schema;
	NN<Text::String> systemPrompt;
	UInt16 port;
	NN<Text::String> s;
	if (!cfg->GetValue(CSTR("spring.jpa.properties.hibernate.default_schema")).SetTo(schema))
	{
		log.LogMessage(CSTR("Failed to get schema from configuration."), IO::LogHandler::LogLevel::Error);
		db.Delete();
		cfg.Delete();
		return 1;
	}
	if (!cfg->GetValue(CSTR("spring.ai.openai.base-url")).SetTo(openAIUrl))
	{
		log.LogMessage(CSTR("Failed to get OpenAI URL from configuration."), IO::LogHandler::LogLevel::Error);
		db.Delete();
		cfg.Delete();
		return 1;
	}
	if (!cfg->GetValue(CSTR("spring.ai.openai.api-key")).SetTo(openAIKey))
	{
		log.LogMessage(CSTR("Failed to get OpenAI API key from configuration."), IO::LogHandler::LogLevel::Error);
		db.Delete();
		cfg.Delete();
		return 1;
	}
	if (!cfg->GetValue(CSTR("app.ai.system-prompt")).SetTo(systemPrompt))
	{
		log.LogMessage(CSTR("Failed to get system prompt from configuration."), IO::LogHandler::LogLevel::Error);
		db.Delete();
		cfg.Delete();
		return 1;
	}
	if (!cfg->GetValue(CSTR("server.port")).SetTo(s) || !s->ToUInt16(port))
	{
		log.LogMessage(CSTR("Failed to get server port from configuration."), IO::LogHandler::LogLevel::Error);
		db.Delete();
		cfg.Delete();
		return 1;
	}
	NN<Net::OpenAIClient> openAI;
	NEW_CLASSNN(openAI, Net::OpenAIClient(clif, nullptr, openAIUrl->ToCString(), openAIKey->ToCString()));
	if (openAI->IsError())
	{
		log.LogMessage(CSTR("Failed to initialize OpenAI client"), IO::LogHandler::LogLevel::Error);
		openAI.Delete();
		db.Delete();
		cfg.Delete();
		return 1;
	}
	NN<Text::String> modelName;
	if (!openAI->GetModelName(0).SetTo(modelName))
	{
		log.LogMessage(CSTR("Failed to get model name"), IO::LogHandler::LogLevel::Error);
		openAI.Delete();
		db.Delete();
		cfg.Delete();
		return 1;
	}
	log.LogMessage(Text::StringBuilderUTF8().Append(CSTR("Using model: "))->Append(modelName)->ToCString(), IO::LogHandler::LogLevel::Command);
	{
		AI::SimpleAIAgent agent(openAI, modelName->ToCString());
		AI::AIReadDBSkill readDBSkill(db->GetDBConn(), schema->ToCString());
		agent.AddSkill(readDBSkill);
		agent.SetMaxToolCalls(20);
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("web"));
		AIAgentHandler agentHandler(agent, log, systemPrompt, CSTRP(sbuff, sptr));
		Net::WebServer::WebListener listener(clif, nullptr, agentHandler, port, 60, 1, 4, CSTR("AIAgent/1.0"), false, Net::WebServer::KeepAlive::Default, true);
		if (listener.IsError())
		{
			log.LogMessage(CSTR("Failed to start web listener"), IO::LogHandler::LogLevel::Error);
		}
		else
		{
			log.LogMessage(CSTR("AIAgent running"), IO::LogHandler::LogLevel::Command);
			progCtrl->WaitForExit(progCtrl);
		}
	}
	openAI.Delete();
	db.Delete();
	cfg.Delete();
	return 0;
}