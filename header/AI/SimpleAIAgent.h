#ifndef _SM_AI_SIMPLEAIAGENT
#define _SM_AI_SIMPLEAIAGENT
#include "AI/AIAgent.h"
#include "AI/AISession.h"
#include "Net/OpenAIClient.h"

namespace AI
{
	class SimpleAIAgent : public AIAgent
	{
	private:
		NN<Net::OpenAIClient> openAI;
		NN<Text::String> modelName;

		Optional<Text::String> SendQuestion(NN<AISession> session, Text::CStringNN question);

	public:
		SimpleAIAgent(NN<Net::OpenAIClient> openAI, Text::CStringNN modelName);
		virtual ~SimpleAIAgent();
		NN<AISession> CreateSession(Text::CStringNN systemPrompt) const;
		Optional<Text::String> Ask(NN<AISession> session, Text::CStringNN question);
	};
}
#endif
