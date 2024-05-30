#ifndef _SM_TEXT_TEXTANALYZER
#define _SM_TEXT_TEXTANALYZER

namespace Text
{
	class TextAnalyzer
	{
	private:
		typedef struct
		{
			UnsafeArray<const UTF8Char> currPos;
		} TextSession;
	public:
		TextAnalyzer();
		~TextAnalyzer();

		void *BeginAnalyze(UnsafeArray<const UTF8Char> s);
		UnsafeArrayOpt<UTF8Char> NextWord(UnsafeArray<UTF8Char> sbuff, void *sess);
		void EndAnalyze(void *sess);
	};
}
#endif
