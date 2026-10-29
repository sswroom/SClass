#ifndef _SM_AI_AIAGENTHANDLER
#define _SM_AI_AIAGENTHANDLER
#include "Data/FastMapNN.hpp"
#include "Net/WebServer/WebServiceHandler.h"
#include "Sync/Thread.h"

namespace AI
{
	class AIAgent;
	class AISession;
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

		static Text::CStringNN RequestStatusGetName(RequestStatus status);
		static void __stdcall ProcessThread(NN<Sync::Thread> t);
		static void __stdcall FreeAIRequest(NN<AIRequest> req);

		static Bool __stdcall ReqFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall SessFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);

	public:
		AIAgentHandler(NN<AI::AIAgent> agent, NN<IO::LogTool> log, NN<Text::String> systemPrompt, Text::CStringNN webPath);
		virtual ~AIAgentHandler();
	};
}
#endif