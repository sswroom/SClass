#ifndef _SM_AI_AITOOL
#define _SM_AI_AITOOL
#include "Data/VariObject.h"
#include "Net/OpenAIClient.h"
#include "Text/StringBuilderUTF8.h"

namespace AI
{
	class AITool
	{
	public:
		virtual ~AITool() {};
		virtual Text::CStringNN GetName() const = 0;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const = 0;
		virtual void GetParameters(NN<Net::OpenAIToolParameters> parameters) const = 0;
		virtual Bool Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output) = 0;
	};

}
#endif
