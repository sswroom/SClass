#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "Math/Math_C.h"
#include "Media/GTKDrawEngine.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKRealtimeLineChart.h"

gboolean UI::GTK::GTKRealtimeLineChart::SignalDraw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	UI::GTK::GTKRealtimeLineChart *me = (UI::GTK::GTKRealtimeLineChart *)data;
	UInt32 w = (UInt32)gtk_widget_get_allocated_width((GtkWidget*)me->hwnd.OrNull());
	UInt32 h = (UInt32)gtk_widget_get_allocated_height((GtkWidget*)me->hwnd.OrNull());
	NN<Media::DrawImage> scn = NN<Media::GTKDrawEngine>::ConvertFrom(me->eng)->CreateImageScn(cr, Math::Coord2D<OSInt>(0, 0), Math::Coord2D<OSInt>((OSInt)w, (OSInt)h), me->colorSess);
	me->OnPaint(scn);
	me->eng->DeleteImage(scn);
	return FALSE;
}

Int32 UI::GTK::GTKRealtimeLineChart::SignalTick(void *userObj)
{
	UI::GTK::GTKRealtimeLineChart *me = (UI::GTK::GTKRealtimeLineChart *)userObj;
	if (me->valueChanged)
	{
		me->valueChanged = false;
		me->Redraw();
	}
	return 1;
}

UI::GTK::GTKRealtimeLineChart::GTKRealtimeLineChart(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateInterval, Optional<Media::ColorSess> colorSess) : UI::GUIRealtimeLineChart(ui, parent, eng, lineCnt, sampleCnt)
{
	this->colorSess = colorSess;
	this->hwnd = (ControlHandle*)gtk_drawing_area_new();
	parent->AddChild(*this);
	this->Show();
	g_signal_connect(G_OBJECT(this->hwnd.OrNull()), "draw", G_CALLBACK(SignalDraw), this);

	this->timerId = g_timeout_add(updateInterval, SignalTick, this);
}

UI::GTK::GTKRealtimeLineChart::~GTKRealtimeLineChart()
{
	g_source_remove(this->timerId);
}
