#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/ImageUtil.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/Java/JavaPictureBox.h"

void UI::Java::JavaPictureBox::UpdatePreview()
{
}

UI::Java::JavaPictureBox::JavaPictureBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Bool allowResize) : UI::GUIPictureBox(ui, parent, eng, hasBorder, allowResize)
{
}

UI::Java::JavaPictureBox::~JavaPictureBox()
{
}

OSInt UI::Java::JavaPictureBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
