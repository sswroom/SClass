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
		UnsafeArrayOpt<const UTF8Char> unit;
		UIntOS lineCnt;
		UIntOS sampleCnt;
		UInt32 *lineColor;
		UInt32 bgColor;
		UInt32 fontColor;

		Bool valueChanged;
		Double *chartVal;
		UIntOS chartOfst;
		Bool chartMaxChg;
		Double chartMax;
		Double chartMin;
		Sync::Mutex chartMut;
		void *clsData;

		void OnPaint(NN<Media::DrawImage> dimg);
	public:
		GUIRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UIntOS lineCnt, UIntOS sampleCnt);
		virtual ~GUIRealtimeLineChart();

		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);

		void AddSample(Double *samples);
		void SetLineColor(UIntOS lineId, UInt32 lineColor);
		void SetUnit(Text::CStringNN unit);
		void SetBGColor(UInt32 lineColor);
		void SetFontColor(UInt32 fontColor);
		void ClearChart();
	};
}
#endif
