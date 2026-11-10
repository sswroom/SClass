#ifndef _SM_AI_AINOWTOOL
#define _SM_AI_AINOWTOOL
#include "AI/AITool.h"

namespace AI
{
	class AINowTool : public AITool
	{
	public:
		AINowTool();
		virtual ~AINowTool();
		virtual Text::CStringNN GetName() const;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const;
		virtual void GetParameters(NN<Net::OpenAIToolParameters> parameters) const;
		virtual Bool Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output);
	};
}
#endif
