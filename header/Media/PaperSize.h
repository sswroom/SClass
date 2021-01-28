#ifndef _SM_MEDIA_PAPERSIZE
#define _SM_MEDIA_PAPERSIZE

namespace Media
{
	class PaperSize
	{
	public:
		typedef enum
		{
			PT_DEFAULT,
			PT_4A0,
			PT_2A0,
			PT_A0,
			PT_A1,
			PT_A2,
			PT_A3,
			PT_A4,
			PT_A5,
			PT_A6,
			PT_A7,
			PT_A8,
			PT_A9,
			PT_A10,
			PT_B0,
			PT_B1,
			PT_B2,
			PT_B3,
			PT_B4,
			PT_B5,
			PT_B6,
			PT_B7,
			PT_B8,
			PT_B9,
			PT_B10,
			PT_C0,
			PT_C1,
			PT_C2,
			PT_C3,
			PT_C4,
			PT_C5,
			PT_C6,
			PT_C7,
			PT_C8,
			PT_C9,
			PT_C10,
			PT_LETTER,
			PT_GOV_LETTER,
			PT_LEGAL,
			PT_JUNIOR_LEGAL,
			PT_ANSI_A,
			PT_ANSI_B,
			PT_ANSI_C,
			PT_ANSI_D,
			PT_ANSI_E,
			PT_CREDIT_CARD,
			PT_3R,
			PT_4R,
			PT_4D,
			PT_5R,
			PT_6R,
			PT_8R,
			PT_S8R,
			PT_10R,
			PT_S10R,
			PT_11R,
			PT_S11R,
			PT_12R,
			PT_S12R
		} PaperType;
	private:
		PaperType pType;
		Double widthMM;
		Double heightMM;

	public:
		PaperSize(PaperType pType);
		~PaperSize();

		Double GetWidthMM();
		Double GetHeightMM();
	};
};
#endif
