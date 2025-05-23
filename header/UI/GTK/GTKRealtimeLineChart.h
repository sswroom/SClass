#ifndef _SM_UI_GTK_GTKREALTIMELINECHART
#define _SM_UI_GTK_GTKREALTIMELINECHART
#include "UI/GUIRealtimeLineChart.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKRealtimeLineChart : public GUIRealtimeLineChart
		{
		private:
			guint timerId;
			Optional<Media::ColorSess> colorSess;

		private:
			static gboolean SignalDraw(GtkWidget *widget, cairo_t *cr, gpointer data);
			static Int32 SignalTick(void *userObj);
		public:
			GTKRealtimeLineChart(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateIntervalMS, Optional<Media::ColorSess> colorSess);
			virtual ~GTKRealtimeLineChart();
		};
	}
}
#endif
