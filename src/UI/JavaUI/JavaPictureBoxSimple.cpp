#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageUtil.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaPictureBoxSimple.h"

UI::JavaUI::JavaPictureBoxSimple::JavaPictureBoxSimple(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder) : UI::GUIPictureBoxSimple(ui, parent, eng, hasBorder)
{
}

UI::JavaUI::JavaPictureBoxSimple::~JavaPictureBoxSimple()
{
}

OSInt UI::JavaUI::JavaPictureBoxSimple::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaPictureBoxSimple::SetImage(Optional<Media::StaticImage> currImage)
{
}

void UI::JavaUI::JavaPictureBoxSimple::SetImageDImg(Optional<Media::DrawImage> img)
{
}
