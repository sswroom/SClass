#ifndef _SM_TEXT_CHARUTIL
#define _SM_TEXT_CHARUTIL

namespace Text
{
	class CharUtil
	{
	public:
		struct UnicodeBlock
		{
			UInt32 firstCode;
			UInt32 lastCode;
			Bool dblWidth;
			const Char *name;
		};

	private:
		static UnicodeBlock ublk[];
	public:
		static Bool PtrIsWS(const UTF8Char **sptr); //Point to next char if true
		static Bool IsWS(const UTF8Char *sptr);
		static Bool IsPunctuation(UTF8Char c);
		static Bool IsAlphaNumeric(UTF8Char c);
		static Bool IsAlphabet(UTF8Char c);
		static Bool UTF8CharValid(const UTF8Char *sptr);
		static const UnicodeBlock *GetUnicodeBlock(UTF32Char c);
		static Bool IsDoubleSize(UTF32Char c);
	};
}
#endif
