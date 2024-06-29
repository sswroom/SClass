#ifndef _SM_TEXT_TEXTANALYZER
#define _SM_TEXT_TEXTANALYZER

namespace Text
{
	class TextAnalyzer
	{
	public:
		struct TextSession;
	public:
		TextAnalyzer();
		~TextAnalyzer();

		NN<TextSession> BeginAnalyze(UnsafeArray<const UTF8Char> s);
		UnsafeArrayOpt<UTF8Char> NextWord(UnsafeArray<UTF8Char> sbuff, NN<TextSession> sess);
		void EndAnalyze(NN<TextSession> sess);
	};
}
#endif
