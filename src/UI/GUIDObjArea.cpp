#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIDObjArea.h"

void __stdcall UI::GUIDObjArea::DisplayThread(NotNullPtr<Sync::Thread> thread)
{
	UI::GUIDObjArea *me = (UI::GUIDObjArea*)thread->GetUserObj();
	me->mainEvt.Set();
	while (!thread->IsStopping())
	{
		if (me->drawUpdated)
		{
			Sync::MutexUsage dobjMutUsage(me->dobjMut);
			if (me->currDrawImg)
			{
				if (me->colorSess->Get10BitColor() && me->currScnMode == UI::GUIDDrawControl::SM_VFS)
				{
					UOSInt dbpl;
					UInt8 *destBuff = me->LockSurfaceBegin(me->currDrawImg->GetWidth(), me->currDrawImg->GetHeight(), &dbpl);
					if (destBuff)
					{
						UInt8 *tmpBuff = MemAlloc(UInt8, me->dispSize.CalcArea() << 2);
						me->currDrawImg->CopyBits(0, 0, tmpBuff, me->dispSize.x << 2, me->dispSize.x, me->dispSize.y, false);
	//					UOSInt w = me->surfaceW;
	//					UOSInt h = me->surfaceH;
	/*#if defined(HAS_ASM32)
						_asm
						{
							mov esi,tmpBuff
							mov edi,destBuff
							mov ebx,h
	dtlop:
							push edi
							mov ecx,w
	dtlop2:
							movzx eax,byte ptr [esi]
							shl eax,22
							movzx edx,byte ptr [esi+1]
							shl edx,12
							or eax,edx
							movzx edx,byte ptr [esi+2]
							shl edx,2
							or eax,edx
							movnti dword ptr [edi],eax
							add esi,4
							add edi,4
							dec ecx
							jnz dtlop2
							pop edi
							add edi,dbpl
							dec ebx
							jnz dtlop
						}
	#elif defined(HAS_GCCASM32)
						_asm
						{
							mov esi,tmpBuff
							mov edi,destBuff
							mov ebx,h
	dtlop:
							push edi
							mov ecx,w
	dtlop2:
							movzx eax,byte ptr [esi]
							shl eax,22
							movzx edx,byte ptr [esi+1]
							shl edx,12
							or eax,edx
							movzx edx,byte ptr [esi+2]
							shl edx,2
							or eax,edx
							movnti dword ptr [edi],eax
							add esi,4
							add edi,4
							dec ecx
							jnz dtlop2
							pop edi
							add edi,dbpl
							dec ebx
							jnz dtlop
						}
	#elif defined(HAS_GCCASM64)
						asm (
	"								mov tmpBuff,%rsi\n"
	"								mov destBuff,%rdi\n"
	"								movzx h,%rbx\n"
	"	dtlop:\n"
	"								push %rdi\n"
	"								movzx w,%rcx\n"
	"	dtlop2:\n"
	"								movzxb (%rsi),%rax\n"
	"								shl $22,%rax\n"
	"								movzxb 0x1(%rsi),%rdx\n"
	"								shl $12,%rdx\n"
	"								or %rdx,%rax\n"
	"								movzxb 0x2(%rsi),%rdx\n"
	"								shl $2,%rdx\n"
	"								or %rdx,%rax\n"
	"								movnti %eax,(%rdi)\n"
	"								add $4,%rsi\n"
	"								add $4,%rdi\n"
	"								dec %rcx\n"
	"								jnz dtlop2\n"
	"								pop %rdi\n"
	"								movzx dbpl,%rax\n"
	"								add %rax,%rdi\n"
	"								dec %rbx\n"
	"								jnz dtlop\n"
	);

	#endif*/

						me->LockSurfaceEnd();
						MemFree(tmpBuff);
					}
				}
				else
				{
					UOSInt dbpl;
					UInt8 *destBuff = me->LockSurfaceBegin(me->currDrawImg->GetWidth(), me->currDrawImg->GetHeight(), &dbpl);
					if (destBuff)
					{
						me->currDrawImg->CopyBits(0, 0, destBuff, dbpl, me->dispSize.x, me->dispSize.y, false);
						me->LockSurfaceEnd();
					}
				}
			}
			me->drawUpdated = false;
			me->processThread.Notify();

			me->DrawToScreen();
		}
		else
		{
			thread->Wait(100);
		}
	}
	me->mainEvt.Set();
}

void __stdcall UI::GUIDObjArea::ProcessThread(NotNullPtr<Sync::Thread> thread)
{
	UI::GUIDObjArea *me = (UI::GUIDObjArea*)thread->GetUserObj();
	NotNullPtr<Media::DrawImage> img;
	me->mainEvt.Set();
	while (!thread->IsStopping())
	{
		Sync::MutexUsage mutUsage(me->dobjMut);
		if (!me->drawUpdated && me->dobjHdlr && img.Set(me->currDrawImg))
		{
			Bool changed = me->dobjHdlr->Check(img);
			mutUsage.EndUse();
			if (changed)
			{
				me->drawUpdated = true;
				me->displayThread.Notify();
			}
			else
			{
				thread->Wait(10);
			}
		}
		else
		{
			mutUsage.EndUse();
			thread->Wait(100);
		}
	}
	me->mainEvt.Set();
}

void __stdcall UI::GUIDObjArea::OnUpdateSize(void *userObj)
{
	UI::GUIDObjArea *me = (UI::GUIDObjArea*)userObj;
	Sync::MutexUsage mutUsage(me->dobjMut);
	if (me->dobjHdlr)
	{
		me->dobjHdlr->SizeChanged(me->GetSizeP());
	}
}

UI::GUIDObjArea::GUIDObjArea(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> deng, Media::ColorManagerSess *colorSess) : UI::GUIDDrawControl(ui, parent, false, colorSess), displayThread(DisplayThread, this, CSTR("GUIDObjAreaDisp")), processThread(ProcessThread, this, CSTR("GUIDObjAreaProc"))
{
	this->deng = deng;
	this->colorSess = colorSess;
	this->dobjHdlr = 0;
	this->drawUpdated = false;
	this->currDrawImg = 0;
	this->HandleSizeChanged(OnUpdateSize, this);
	this->displayThread.Start();
	this->processThread.Start();
	
}

UI::GUIDObjArea::~GUIDObjArea()
{
	this->processThread.BeginStop();
	this->displayThread.BeginStop();
	this->processThread.WaitForEnd();
	this->displayThread.WaitForEnd();
	if (this->dobjHdlr)
	{
		DEL_CLASS(this->dobjHdlr);
	}
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->currDrawImg))
	{
		this->deng->DeleteImage(img);
		this->currDrawImg = 0;
	}
}

void UI::GUIDObjArea::SetHandler(UI::DObj::DObjHandler *dobjHdlr)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	if (this->dobjHdlr)
	{
		DEL_CLASS(this->dobjHdlr);
	}
	this->dobjHdlr = dobjHdlr;
	if (this->dobjHdlr)
	{
		this->dobjHdlr->SetColorSess(this->colorSess);
	}
}

Text::CStringNN UI::GUIDObjArea::GetObjectClass() const
{
	return CSTR("DObjArea");
}

OSInt UI::GUIDObjArea::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIDObjArea::OnSurfaceCreated()
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->currDrawImg))
	{
		this->deng->DeleteImage(img);
		this->currDrawImg = 0;
	}
	this->currDrawImg = this->deng->CreateImage32(this->dispSize, Media::AT_NO_ALPHA);
	this->drawUpdated = false;
}

void UI::GUIDObjArea::OnMouseWheel(Math::Coord2D<OSInt> scnPos, Int32 amount)
{
}

void UI::GUIDObjArea::OnMouseMove(Math::Coord2D<OSInt> scnPos)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	if (this->dobjHdlr)
	{
		this->dobjHdlr->OnMouseMove(scnPos);
	}
}

void UI::GUIDObjArea::OnMouseDown(Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	if (this->dobjHdlr)
	{
		this->dobjHdlr->OnMouseDown(scnPos, button);
	}
}

void UI::GUIDObjArea::OnMouseUp(Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	if (this->dobjHdlr)
	{
		this->dobjHdlr->OnMouseUp(scnPos, button);
	}
}

void UI::GUIDObjArea::OnMouseDblClick(Math::Coord2D<OSInt> scnPos, MouseButton button)
{
}
