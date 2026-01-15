#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/ImageUtil.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/JavaUI/JUIPictureBox.h"

void UI::JavaUI::JUIPictureBox::UpdatePreview()
{
}

UI::JavaUI::JUIPictureBox::JUIPictureBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIPictureBox(ui, parent, eng, hasBorder, allowResize)
{
}

UI::JavaUI::JUIPictureBox::~JUIPictureBox()
{
}

IntOS UI::JavaUI::JUIPictureBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
