namespace Media
{
	class ImageWin
	{
	public:
		ImageWin();
		~ImageWin();

		Optional<RasterImage> CreateImage(HBITMAP hbmp);
		HBITMAP CreateHBITMAP(NN<StaticImage> img);
	};
}
