#ifndef _SM_AI_AIREADMDTOOL
#define _SM_AI_AIREADMDTOOL
#include "AI/AITool.h"

namespace AI
{
	class AIReadMDTool : public AITool
	{
	private:
		NN<Text::String> directory;

		static void AppendDirectoryPath(NN<Text::StringBuilderUTF8> sb, NN<Text::String> directory);
		static Bool IsMarkdownFileName(UnsafeArray<const UTF8Char> fileName, UIntOS fileNameLen);
	public:
		AIReadMDTool(Text::CStringNN directory);
		virtual ~AIReadMDTool();
		virtual Text::CStringNN GetName() const;
		virtual void GetDescription(NN<Text::StringBuilderUTF8> sb) const;
		virtual void GetParameters(NN<Net::OpenAIToolParameters> parameters) const;
		virtual Bool Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output);
	};
}
#endif
