#ifndef _SM_TEXT_UNICODE
#define _SM_TEXT_UNICODE

namespace Text
{
	class Unicode
	{
	public:
		struct Block
		{
			UInt32 beginCode;
			UInt32 endCode;
			const UTF8Char *name;
			UIntOS nameLen;
		};

	private:
		static Block blocks[];
	public:
		static Optional<Block> GetBlock(UInt32 code);
	};
}
#endif
