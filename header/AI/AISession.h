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
		NN<Text::String> sessionId;
		NN<Text::String> modelName;
		NN<Net::OpenAIChatCompletion> chat;
		Data::ArrayListStringNN sessionLines;
		Data::ArrayListStringNN attachments;

		static NN<Text::String> NewAISessionId();
		void AddTurn(Text::CStringNN question, Text::CStringNN answer);
		void AddAttachment(NN<Text::String> attachment);

		AISession(Text::CStringNN modelName, Text::CStringNN systemPrompt);
	public:
		~AISession();
		Text::CStringNN GetSessionId() const;
		NN<Text::String> GetText() const;
		UIntOS GetMessageCount() const;
		Optional<Net::OpenAIMessage> GetMessage(UIntOS index) const;
		UIntOS GetAttachmentCount() const;
		Optional<Text::String> GetAttachment(UIntOS index) const;
	};
}
#endif
