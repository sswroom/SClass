#ifndef _SM_AI_AISESSION
#define _SM_AI_AISESSION
#include "Data/ArrayListStringNN.h"
#include "Net/OpenAIClient.h"

namespace AI
{
	class SimpleAIAgent;

	class AISession
	{
		friend class SimpleAIAgent;

	private:
		NN<Text::String> modelName;
		NN<Net::OpenAIChatCompletion> chat;
		Data::ArrayListStringNN sessionLines;

		AISession(Text::CStringNN modelName, Text::CStringNN systemPrompt);
		void AddTurn(Text::CStringNN question, Text::CStringNN answer);

	public:
		~AISession();
		NN<Text::String> GetText() const;
		UIntOS GetMessageCount() const;
		Optional<Net::OpenAIMessage> GetMessage(UIntOS index) const;
	};
}
#endif
