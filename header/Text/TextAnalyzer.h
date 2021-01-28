#ifndef _SM_TEXT_TEXTANALYZER
#define _SM_TEXT_TEXTANALYZER

namespace Text
{
	class TextAnalyzer
	{
	private:
		typedef struct
		{
			const UTF8Char *currPos;
		} TextSession;
	public:
		TextAnalyzer();
		~TextAnalyzer();

		void *BeginAnalyze(const UTF8Char *s);
		UTF8Char *NextWord(UTF8Char *sbuff, void *sess);
		void EndAnalyze(void *sess);
	};
}
#endif
