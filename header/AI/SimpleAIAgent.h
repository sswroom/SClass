#ifndef _SM_AI_SIMPLEAIAGENT
#define _SM_AI_SIMPLEAIAGENT
#include "AI/AIAgent.h"
#include "Net/OpenAIClient.h"

namespace AI
{
	class SimpleAIAgent : public AIAgent
	{
	private:
		NN<Net::OpenAIClient> openAI;
		NN<Text::String> modelName;

	public:
		SimpleAIAgent(NN<Net::OpenAIClient> openAI, Text::CStringNN modelName);
		virtual ~SimpleAIAgent();
		Optional<Text::String> Ask(Text::CStringNN systemPrompt, Text::CStringNN question) const;
	};
}
#endif
