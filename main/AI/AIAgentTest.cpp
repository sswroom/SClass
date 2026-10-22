#include "Stdafx.h"
#include "AI/AIReadDBSkill.h"
#include "AI/SimpleAIAgent.h"
#include "Core/Core.h"
#include "DB/DBConfig.h"
#include "DB/DBTool.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/Console.h"
#include "IO/ConsoleWriter.h"
#include "IO/Java/JavaProperties.h"
#include "Net/OpenAIClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClientFactory.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN propPath = CSTR("/home/sswroom/Temp/application.properties");
	Text::CStringNN dbSchema = CSTR("pbgapp");
	Text::CStringNN openAIURL = CSTR("http://127.0.0.1:1234/v1");
	Text::CStringNN openAIKey = CSTR("AnyKey");
	Text::CStringNN systemPrompt = CSTR("You are a helpful assistant.");
	Text::CStringNN question = CSTR("What is burial site SK-03?");
	NN<IO::ConfigFile> cfg;
	NN<DB::DBTool> db;
	if (!IO::Java::JavaProperties::Parse(propPath).SetTo(cfg))
	{
		printf("Failed to parse Java properties from %s\n", propPath.v.Ptr());
		return 1;
	}
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	IO::LogTool log;
	IO::ConsoleWriter console;
	IO::ConsoleLogHandler consoleLog(console);
	log.AddLogHandler(consoleLog, IO::LogHandler::LogLevel::Raw);
	if (!DB::DBConfig::LoadFromConfig(clif, cfg, nullptr, log, false).SetTo(db))
	{
		log.LogMessage(CSTR("Failed to load DB config"), IO::LogHandler::LogLevel::Error);
		cfg.Delete();
		return 1;
	}
	NN<Net::OpenAIClient> openAI;
	NEW_CLASSNN(openAI, Net::OpenAIClient(clif, nullptr, openAIURL, openAIKey));
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
		AI::AIReadDBSkill dbSkill(db->GetDBConn(), dbSchema);
		agent.AddSkill(dbSkill);
		NN<AI::AISession> session = agent.CreateSession(systemPrompt);
		NN<Text::String> answer;
		if (agent.Ask(session, question).SetTo(answer))
		{
			answer->Release();
			NN<Text::String> sessionText = session->GetText();
			log.LogMessage(sessionText->ToCString(), IO::LogHandler::LogLevel::Command);
			sessionText->Release();

			UTF8Char inputBuff[4096];
			UnsafeArray<UTF8Char> inputEnd;
			while (true)
			{
				log.LogMessage(CSTR("Ask another question (empty line or q to finish):"), IO::LogHandler::LogLevel::Command);
				inputEnd = IO::Console::GetLine(inputBuff);
				Text::CStringNN nextQuestion = CSTRP(inputBuff, inputEnd);
				if (nextQuestion.leng == 0 || nextQuestion.Equals(UTF8STRC("q")))
					break;
				if (agent.Ask(session, nextQuestion).SetTo(answer))
				{
					answer->Release();
					sessionText = session->GetText();
					log.LogMessage(sessionText->ToCString(), IO::LogHandler::LogLevel::Command);
					sessionText->Release();
				}
				else
				{
					log.LogMessage(CSTR("Failed to get an answer from the AI agent"), IO::LogHandler::LogLevel::Error);
				}
			}
		}
		else
		{
			log.LogMessage(CSTR("Failed to get an answer from the AI agent"), IO::LogHandler::LogLevel::Error);
		}
		session.Delete();
	}

	openAI.Delete();
	db.Delete();
	cfg.Delete();
	return 0;
}