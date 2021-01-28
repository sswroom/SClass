namespace Media
{
	class ImageWin
	{
	public:
		ImageWin();
		~ImageWin();

		Image *CreateImage(HBITMAP hbmp);
		HBITMAP CreateHBITMAP(StaticImage *img);
	};
};
