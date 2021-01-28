#ifndef _SM_TEXT_HKSCSFIX
#define _SM_TEXT_HKSCSFIX

namespace Text
{
	class HKSCSFix
	{
	private:
		Int32 *tab;
	public:
		HKSCSFix();
		~HKSCSFix();

		void FixString(UTF16Char *s);
		void FixString(UTF32Char *s);
		Bool IsError();
	};
}
#endif
