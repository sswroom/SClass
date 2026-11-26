#ifndef _SM_AI_AICSYSTOOL
#define _SM_AI_AICSYSTOOL
#include "AI/AITool.h"
#include "Text/JSONBuilder.h"

namespace AI
{
	class AICSysTool : public AITool
	{
	private:
		UIntOS maxToolSize;

		static Bool GetNumber(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, OutParam<Double> value);
		static Bool GetUInt32(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, OutParam<UInt32> value);
		static Bool SetData(NN<Data::VariObject> output, NN<Text::JSONBuilder> json);
	public:
		AICSysTool();
		virtual ~AICSysTool();
		virtual Text::CStringNN GetName() const;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const;
		virtual void GetParameters(NN<Net::OpenAIToolParameters> parameters) const;
		virtual void SetMaxToolSize(UIntOS maxToolSize);
		virtual Bool Invoke(Text::CStringNN sessionId, NN<Data::VariObject> input, NN<Data::VariObject> output);
	};
}
#endif
