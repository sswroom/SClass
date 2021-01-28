namespace Media
{
	class DrawTool
	{
	public:
		DrawTool();
		~DrawTool();

		void DrawLineSimple(Media::StaticImage *img, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Int32 color);
		void FillRect(Media::StaticImage *img, Int32 x1, Int32  y1, Int32 x2, Int32 y2, Int32 color);
	};
};
