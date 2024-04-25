#ifndef _SM_UI_GUIREALTIMELINECHART
#define _SM_UI_GUIREALTIMELINECHART
#include "Media/DrawEngine.h"
#include "Sync/Mutex.h"
#include "UI/GUIControl.h"

namespace UI
{
	class GUIRealtimeLineChart : public GUIControl
	{
	protected:
		NN<Media::DrawEngine> eng;
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

		void OnPaint(NN<Media::DrawImage> dimg);
	public:
		GUIRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt);
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
