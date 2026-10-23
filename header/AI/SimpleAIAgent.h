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
		UIntOS maxToolCalls;

		static UIntOS GetUTF8PrefixLength(UnsafeArray<const UTF8Char> text, UIntOS maxLen);
		static Bool SetArgs(NN<Data::VariObject> input, NN<Text::JSONObject> args);
		Optional<Text::String> SendQuestion(NN<AISession> session, Text::CStringNN question);

	public:
		SimpleAIAgent(NN<Net::OpenAIClient> openAI, Text::CStringNN modelName);
		virtual ~SimpleAIAgent();

		virtual NN<AISession> CreateSession(Text::CStringNN systemPrompt) const;
		virtual Optional<Text::String> Ask(NN<AISession> session, Text::CStringNN question);
		void SetMaxToolCalls(UIntOS maxToolCalls);
	};
}
#endif
