#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/ImageUtil.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JavaPictureBox.h"

void UI::JavaUI::JavaPictureBox::UpdatePreview()
{
}

UI::JavaUI::JavaPictureBox::JavaPictureBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIPictureBox(ui, parent, eng, hasBorder, allowResize)
{
}

UI::JavaUI::JavaPictureBox::~JavaPictureBox()
{
}

OSInt UI::JavaUI::JavaPictureBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
