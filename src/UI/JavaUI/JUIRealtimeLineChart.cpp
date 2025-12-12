#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JUIRealtimeLineChart.h"

UI::JavaUI::JUIRealtimeLineChart::JUIRealtimeLineChart(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, UOSInt lineCnt, UOSInt sampleCnt, UInt32 updateInterval) : UI::GUIRealtimeLineChart(ui, parent, eng, lineCnt, sampleCnt)
{
}

UI::JavaUI::JUIRealtimeLineChart::~JUIRealtimeLineChart()
{
}
