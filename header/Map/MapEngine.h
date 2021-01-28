/*
void ReleaseLine(MapVars *map, MyStyle *style)
void ReleaseFont(MapVars *map, MyStyle *style)
void ReleaseLabel(MapVars *map, MyLabel *label)
Int32 LabelOverlapped(Int32 *points, Int32 nPoints, Int32 tlx, Int32 tly, Int32 brx, Int32 bry)
Int32 NewLabel(MapVars *map, Int32 priority)
void AddLabel(MapVars *map, WChar *label, Int32 lblSize, Int32 nPoint, Int32 *points, Int32 priority, Int32 recType, Int32 fontStyle, Int32 flags)
void SwapLabel(MyLabel *mapLabels, Int32 index, Int32 index2)
void FntDrawChars(MapVars *map, WChar *label, Int32 lblSize, Int32 scnPosX, Int32 scnPosY, Int32 scaleW, Int32 scaleH, Int32 fontStyle, Int32 align)
void FntGetSize(MapVars *map, Int32 *size, WChar *label, Int32 lblSize, Int32 fontStyle, Int32 scaleW, Int32 scaleH)
void MapAfterDraw(MapVars *map)
void SelectPen(MapVars *map, MyLine *lines, Int32 index, Int32 maxScale)
*/
namespace Map
{
	class MapEngine
	{
	public:
		static void *Init(Int32 w, Int32 h, Int32 xPos, Int32 yPos, Int32 bgColor, Int32 scale, Int32 nLineStyle, Int32 nFontStyle, Int32 nLabels);
		static void InitLine(void *m, Int32 lineNum, OSInt nStyles);
		static void InitFont(void *m, Int32 fontNum, OSInt nStyles);
		static void SetLine(void *m, Int32 lineNum, OSInt index, Int32 lineType, Int32 width, Int32 color, WChar *styles);
		static void SetFont(void *m, Int32 fontNum, OSInt index, Int32 fontType, WChar *fontName, Int32 size, Int32 thickness, Int32 color);
		static void EndDraw(void *m);
		static Bool SaveImg(void *m, WChar *fileName, Int32 imgFormat);
		static Int32 GetImg(void *m, UInt8 *buff, Int32 imgFormat);
		static void Close(void *m);
		static void DrawLine(void *m, WChar *fileName , Int32 maxScale, Int32 lineStyle, Bool *drawn);
		static void FillLine(void *m, WChar *fileName, Int32 maxScale, Int32 lineStyle, Int32 fillColor, Bool *drawn);
		static void DrawString(void *m, WChar *fileName, Int32 priority, Int32 fontStyle, Int32 flags, Bool *drawn);
		static void DrawPoints(void *m, WChar *fileName, WChar *imgFile, Bool *drawn);
		static void DrawMarker(void *m, Int32 xpos, Int32 ypos, Int32 color);
	};
}
