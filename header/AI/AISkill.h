#ifndef _SM_AI_AISKILL
#define _SM_AI_AISKILL
#include "Data/VariObject.h"
#include "Text/StringBuilderUTF8.h"

namespace AI
{
	class AISkill
	{
	public:
		virtual ~AISkill() = 0;
		virtual Text::CStringNN GetName() const = 0;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const = 0;
		virtual Bool Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output) = 0;
	};

}
#endif
