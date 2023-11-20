#ifndef _SM_UI_GUIREALTIMELINECHART
#define _SM_UI_GUIREALTIMELINECHART
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIRealtimeLineChart : public GUIControl
	{
	private:
		static Int32 useCnt;

		NotNullPtr<Media::DrawEngine> eng;
		const UTF8Char *unit;
		UOSInt lineCnt;
		UOSInt sampleCnt;
		UInt32 *lineColor;
		UInt32 bgColor;
		UInt32 fontColor;

		Bool valueChanged;
		Double *chartVal;
		UOSInt chartOfst;
		Bool chartMaxChg;
		Double chartMax;
		Double chartMin;
		Sync::Mutex chartMut;
		void *clsData;

	public:
		static OSInt __stdcall RLCWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
	private:
		void OnPaint(NotNullPtr<Media::DrawImage> dimg);
		void Init(void *hInst);
		void Deinit(void *hInst);
	public:
		GUIRealtimeLineChart(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS);
		virtual ~GUIRealtimeLineChart();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		void AddSample(Double *samples);
		void SetLineColor(UOSInt lineId, UInt32 lineColor);
		void SetUnit(Text::CStringNN unit);
		void SetBGColor(UInt32 lineColor);
		void SetFontColor(UInt32 fontColor);
		void ClearChart();

	};
}
#endif
