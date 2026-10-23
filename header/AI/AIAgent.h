#ifndef _SM_AI_AIAGENT
#define _SM_AI_AIAGENT
#include "AI/AISession.h"
#include "AI/AISkill.h"
#include "Data/ArrayListNN.hpp"

namespace AI
{
	class AIAgent
	{
	private:
		Data::ArrayListNN<AISkill> skills;

	public:
		virtual ~AIAgent() = 0;

		virtual NN<AISession> CreateSession(Text::CStringNN systemPrompt) const = 0;
		virtual Optional<Text::String> Ask(NN<AISession> session, Text::CStringNN question) = 0;

		Bool AddSkill(NN<AISkill> skill);
		Optional<AISkill> RemoveSkill(Text::CStringNN name);
		UIntOS GetSkillCount() const;
		Optional<AISkill> GetSkill(UIntOS index) const;
		Bool InvokeSkill(Text::CStringNN name, NN<Data::VariObject> input, NN<Data::VariObject> output) const;
	};
}
#endif
