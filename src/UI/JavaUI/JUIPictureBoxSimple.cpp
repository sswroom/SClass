#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageUtil.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIPictureBoxSimple.h"

UI::JavaUI::JUIPictureBoxSimple::JUIPictureBoxSimple(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIPictureBoxSimple(ui, parent, eng, hasBorder)
{
}

UI::JavaUI::JUIPictureBoxSimple::~JUIPictureBoxSimple()
{
}

OSInt UI::JavaUI::JUIPictureBoxSimple::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIPictureBoxSimple::SetImage(Optional<Media::StaticImage> currImage)
{
}

void UI::JavaUI::JUIPictureBoxSimple::SetImageDImg(Optional<Media::DrawImage> img)
{
}
