#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/MutexUsage.h"
#include "UI/GUIDObjArea.h"

void __stdcall UI::GUIDObjArea::DisplayThread(NN<Sync::Thread> thread)
{
	NN<UI::GUIDObjArea> me = thread->GetUserObj().GetNN<UI::GUIDObjArea>();
	me->mainEvt.Set();
	while (!thread->IsStopping())
	{
		if (me->drawUpdated)
		{
			NN<Media::DrawImage> currDrawImg;
			Sync::MutexUsage dobjMutUsage(me->dobjMut);
			if (me->currDrawImg.SetTo(currDrawImg))
			{
				if (me->colorSess->Get10BitColor() && me->currScnMode == UI::GUIDDrawControl::SM_VFS)
				{
					IntOS dbpl;
					UnsafeArray<UInt8> destBuff;
					if (me->LockSurfaceBegin(currDrawImg->GetWidth(), currDrawImg->GetHeight(), dbpl).SetTo(destBuff))
					{
						UInt8 *tmpBuff = MemAlloc(UInt8, me->dispSize.CalcArea() << 2);
						currDrawImg->CopyBits(0, 0, tmpBuff, me->dispSize.x << 2, me->dispSize.x, me->dispSize.y, false);
	//					UIntOS w = me->surfaceW;
	//					UIntOS h = me->surfaceH;
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
					IntOS dbpl;
					UnsafeArray<UInt8> destBuff;
					if (me->LockSurfaceBegin(currDrawImg->GetWidth(), currDrawImg->GetHeight(), dbpl).SetTo(destBuff))
					{
						currDrawImg->CopyBits(0, 0, destBuff, (UIntOS)dbpl, me->dispSize.x, me->dispSize.y, false);
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

void __stdcall UI::GUIDObjArea::ProcessThread(NN<Sync::Thread> thread)
{
	NN<UI::GUIDObjArea> me = thread->GetUserObj().GetNN<UI::GUIDObjArea>();
	NN<Media::DrawImage> img;
	me->mainEvt.Set();
	while (!thread->IsStopping())
	{
		Sync::MutexUsage mutUsage(me->dobjMut);
		NN<UI::DObj::DObjHandler> dobjHdlr;
		if (!me->drawUpdated && me->dobjHdlr.SetTo(dobjHdlr) && me->currDrawImg.SetTo(img))
		{
			Bool changed = dobjHdlr->Check(img);
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

void __stdcall UI::GUIDObjArea::OnUpdateSize(AnyType userObj)
{
	NN<UI::GUIDObjArea> me = userObj.GetNN<UI::GUIDObjArea>();
	Sync::MutexUsage mutUsage(me->dobjMut);
	NN<UI::DObj::DObjHandler> dobjHdlr;
	if (me->dobjHdlr.SetTo(dobjHdlr))
	{
		dobjHdlr->SizeChanged(me->GetSizeP());
	}
}

UI::GUIDObjArea::GUIDObjArea(NN<GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> deng, NN<Media::ColorManagerSess> colorSess) : UI::GUIDDrawControl(ui, parent, false, colorSess), displayThread(DisplayThread, this, CSTR("GUIDObjAreaDisp")), processThread(ProcessThread, this, CSTR("GUIDObjAreaProc"))
{
	this->deng = deng;
	this->colorSess = colorSess;
	this->dobjHdlr = nullptr;
	this->drawUpdated = false;
	this->currDrawImg = nullptr;
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
	this->dobjHdlr.Delete();
	NN<Media::DrawImage> img;
	if (this->currDrawImg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->currDrawImg = nullptr;
	}
}

void UI::GUIDObjArea::SetHandler(Optional<UI::DObj::DObjHandler> dobjHdlr)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	this->dobjHdlr.Delete();
	this->dobjHdlr = dobjHdlr;
}

Text::CStringNN UI::GUIDObjArea::GetObjectClass() const
{
	return CSTR("DObjArea");
}

IntOS UI::GUIDObjArea::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIDObjArea::OnSurfaceCreated()
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	NN<Media::DrawImage> img;
	if (this->currDrawImg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->currDrawImg = nullptr;
	}
	this->currDrawImg = this->deng->CreateImage32(this->dispSize, Media::AT_IGNORE_ALPHA);
	this->drawUpdated = false;
}

void UI::GUIDObjArea::OnMouseWheel(Math::Coord2D<IntOS> scnPos, Int32 amount)
{
}

void UI::GUIDObjArea::OnMouseMove(Math::Coord2D<IntOS> scnPos)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	NN<UI::DObj::DObjHandler> dobjHdlr;
	if (this->dobjHdlr.SetTo(dobjHdlr))
	{
		dobjHdlr->OnMouseMove(scnPos);
	}
}

void UI::GUIDObjArea::OnMouseDown(Math::Coord2D<IntOS> scnPos, MouseButton button)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	NN<UI::DObj::DObjHandler> dobjHdlr;
	if (this->dobjHdlr.SetTo(dobjHdlr))
	{
		dobjHdlr->OnMouseDown(scnPos, button);
	}
}

void UI::GUIDObjArea::OnMouseUp(Math::Coord2D<IntOS> scnPos, MouseButton button)
{
	Sync::MutexUsage mutUsage(this->dobjMut);
	NN<UI::DObj::DObjHandler> dobjHdlr;
	if (this->dobjHdlr.SetTo(dobjHdlr))
	{
		dobjHdlr->OnMouseUp(scnPos, button);
	}
}

void UI::GUIDObjArea::OnMouseDblClick(Math::Coord2D<IntOS> scnPos, MouseButton button)
{
}
