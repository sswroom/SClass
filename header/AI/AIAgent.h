#ifndef _SM_AI_AIAGENT
#define _SM_AI_AIAGENT
#include "AI/AISession.h"
#include "AI/AITool.h"
#include "Data/ArrayListNN.hpp"

namespace AI
{
	class AIAgent
	{
	private:
		Data::ArrayListNN<AITool> tools;

	public:
		virtual ~AIAgent() = 0;

		virtual NN<AISession> CreateSession(Text::CStringNN systemPrompt) const = 0;
		virtual Optional<Text::String> Ask(NN<AISession> session, Text::CStringNN question) = 0;
		
		Bool AddTool(NN<AITool> tool);
		Optional<AITool> RemoveTool(Text::CStringNN name);
		UIntOS GetToolCount() const;
		Optional<AITool> GetTool(UIntOS index) const;
		Bool InvokeTool(Text::CStringNN name, NN<Data::VariObject> input, NN<Data::VariObject> output) const;
	};
}
#endif
