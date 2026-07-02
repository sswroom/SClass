namespace Map
{
	class SmartLabel
	{
	public:
		struct LabelSession;
		typedef enum
		{
			LF_ROTATE = 1,
			LF_SMART = 2,
			LF_ALIGN = 4
		} LabelFlags;
		typedef void (CALLBACKFUNC DrawFunc)(AnyType userObj, const WChar *str, Int32 x, Int32 y, Int32 scaleW, Int32 scaleH, Int32 style, LabelFlags flags);
		typedef void (CALLBACKFUNC SizeFunc)(AnyType userObj, const WChar *str, Int32 scaleW, Int32 scaleH, Int32 style, Int32 *width, Int32 *height, LabelFlags flags);

	private:
		Int32 NewLabel(NN<LabelSession> sess, Int32 priority);
		void SwapLabel(NN<LabelSession> sess, Int32 index, Int32 index2);
		Int32 LabelOverlapped(UnsafeArray<Int32> points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry);

	public:
		SmartLabel();
		~SmartLabel();

		NN<LabelSession> BeginDraw(Int32 nLabels, NN<MapView> view, DrawFunc func1, SizeFunc func2, AnyType userObj);
		void EndDraw(NN<LabelSession> sess);
		void DrawImage(NN<LabelSession> sess, Media::DrawImage *img);

		Bool AddPointLabel(NN<LabelSession> sess, Int32 priority, Int32 fontStyle, WChar *label, Int32 *points, Int32 nPoints, LabelFlags flags, Double mapRate);
		Bool AddPolylineLabel(NN<LabelSession> sess, Int32 priority, Int32 fontStyle, WChar *label, Int32 *points, Int32 nPoints, LabelFlags flags, Double mapRate);
		Bool AddPolygonLabel(NN<LabelSession> sess, Int32 priority, Int32 fontStyle, WChar *label, Int32 *points, Int32 nPoints, LabelFlags flags, Double mapRate);
	};
}
