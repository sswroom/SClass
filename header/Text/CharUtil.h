#ifndef _SM_TEXT_CHARUTIL
#define _SM_TEXT_CHARUTIL

namespace Text
{
	class CharUtil
	{
	public:
		static Bool PtrIsWS(const UTF8Char **sptr); //Point to next char if true
		static Bool IsWS(const UTF8Char *sptr);
		static Bool IsPunctuation(UTF8Char c);
		static Bool IsAlphaNumeric(UTF8Char c);
		static Bool IsAlphabet(UTF8Char c);
	};
}
#endif
