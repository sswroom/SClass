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
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"

class AIAgentHandler : public Net::WebServer::WebServiceHandler
{
private:
	NN<AI::AIAgent> agent;
	NN<Text::String> systemPrompt;

	static Bool __stdcall ReqFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
	{
		NN<AIAgentHandler> me = NN<AIAgentHandler>::ConvertFrom(svcHdlr);
		NN<Text::String> q;
		req->ParseHTTPForm();
		if (!req->GetHTTPFormStr(CSTR("q")).SetTo(q))
			return false;
		Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
		json.ObjectAddStr(CSTR("question"), q->ToCString());
		return me->ResponseJSONStr(req, resp, 0, json.Build());
	}

public:
	AIAgentHandler(NN<AI::AIAgent> agent, NN<Text::String> systemPrompt, Text::CStringNN webPath) : WebServiceHandler(webPath)
	{
		this->agent = agent;
		this->systemPrompt = systemPrompt->Clone();
		this->AddService(CSTR("/api/req"), Net::WebUtil::RequestMethod::HTTP_POST, ReqFunc);
	}
	
	virtual ~AIAgentHandler()
	{
		this->systemPrompt->Release();
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
		AI::AIReadDBSkill readDBSkill(db, schema->ToCString());
		agent.AddSkill(readDBSkill);
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("web"));
		AIAgentHandler agentHandler(agent, systemPrompt, CSTRP(sbuff, sptr));
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
	modelName->Release();
	openAI.Delete();
	db.Delete();
	cfg.Delete();
	return 0;
}