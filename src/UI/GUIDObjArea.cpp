#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIDObjArea.h"

UInt32 __stdcall UI::GUIDObjArea::DisplayThread(void *userObj)
{
	UI::GUIDObjArea *me = (UI::GUIDObjArea*)userObj;
	me->displayRunning = true;
	me->mainEvt->Set();
	while (!me->displayToStop)
	{
		if (me->drawUpdated)
		{
			if (me->colorSess->Get10BitColor() && me->currScnMode == UI::GUIDDrawControl::SM_VFS)
			{
				UOSInt dbpl;
				UInt8 *destBuff = me->LockSurfaceBegin(me->currDrawImg->GetWidth(), me->currDrawImg->GetHeight(), &dbpl);
				if (destBuff)
				{
					UInt8 *tmpBuff = MemAlloc(UInt8, me->surfaceW * me->surfaceH << 2);
					me->currDrawImg->CopyBits(0, 0, tmpBuff, me->surfaceW << 2, me->surfaceW, me->surfaceH, false);
					UOSInt w = me->surfaceW;
					UOSInt h = me->surfaceH;
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
					me->currDrawImg->CopyBits(0, 0, destBuff, dbpl, me->surfaceW, me->surfaceH, false);
					me->LockSurfaceEnd();
				}
			}
			me->drawUpdated = false;
			me->processEvt->Set();

			me->DrawToScreen();
		}
		else
		{
			me->displayEvt->Wait(100);
		}
	}
	me->displayRunning = false;
	me->mainEvt->Set();
	return 0;
}

UInt32 __stdcall UI::GUIDObjArea::ProcessThread(void *userObj)
{
	UI::GUIDObjArea *me = (UI::GUIDObjArea*)userObj;
	me->processRunning = true;
	me->mainEvt->Set();
	while (!me->processToStop)
	{
		me->dobjMut->Lock();
		if (!me->drawUpdated && me->dobjHdlr && me->currDrawImg)
		{
			Bool changed = me->dobjHdlr->Check(me->currDrawImg);
			me->dobjMut->Unlock();
			if (changed)
			{
				me->drawUpdated = true;
				me->displayEvt->Set();
			}
			else
			{
				me->processEvt->Wait(10);
			}
		}
		else
		{
			me->dobjMut->Unlock();
			me->processEvt->Wait(100);
		}
	}
	me->processRunning = false;
	me->mainEvt->Set();
	return 0;
}

UI::GUIDObjArea::GUIDObjArea(GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng, Media::ColorManagerSess *colorSess) : UI::GUIDDrawControl(ui, parent, false, colorSess)
{
	this->deng = deng;
	this->colorSess = colorSess;
	NEW_CLASS(this->dobjMut, Sync::Mutex());
	this->dobjHdlr = 0;
	this->drawUpdated = false;
	this->displayToStop = false;
	this->displayRunning = false;
	NEW_CLASS(this->displayEvt, Sync::Event(true));
	this->processToStop = false;
	this->processRunning = false;
	NEW_CLASS(this->processEvt, Sync::Event(true));
	NEW_CLASS(this->mainEvt, Sync::Event(true));
	this->currDrawImg = 0;
	Sync::Thread::Create(DisplayThread, this);
	Sync::Thread::Create(ProcessThread, this);
	while (!this->displayRunning)
	{
		this->displayRunning = true;
	}
	
}

UI::GUIDObjArea::~GUIDObjArea()
{
	this->processToStop = true;
	this->processEvt->Set();
	this->displayToStop = true;
	this->displayEvt->Set();
	while (this->displayRunning)
	{
		this->mainEvt->Wait(100);
	}
	while (this->processRunning)
	{
		this->mainEvt->Wait(100);
	}
	DEL_CLASS(this->dobjMut);
	DEL_CLASS(this->mainEvt);
	DEL_CLASS(this->displayEvt);
	DEL_CLASS(this->processEvt);
	if (this->dobjHdlr)
	{
		DEL_CLASS(this->dobjHdlr);
	}
	if (this->currDrawImg)
	{
		this->deng->DeleteImage(this->currDrawImg);
		this->currDrawImg = 0;
	}
}

void UI::GUIDObjArea::SetHandler(UI::DObj::DObjHandler *dobjHdlr)
{
	this->dobjMut->Lock();
	if (this->dobjHdlr)
	{
		DEL_CLASS(this->dobjHdlr);
	}
	this->dobjHdlr = dobjHdlr;
	if (this->dobjHdlr)
	{
		this->dobjHdlr->SetColorSess(this->colorSess);
	}
	this->dobjMut->Unlock();
}

Text::CString UI::GUIDObjArea::GetObjectClass()
{
	return CSTR("DObjArea");
}

OSInt UI::GUIDObjArea::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIDObjArea::OnSurfaceCreated()
{
	this->dobjMut->Lock();
	if (this->currDrawImg)
	{
		this->deng->DeleteImage(this->currDrawImg);
		this->currDrawImg = 0;
	}
	this->currDrawImg = this->deng->CreateImage32(this->surfaceW, this->surfaceH, Media::AT_NO_ALPHA);
	this->drawUpdated = false;
	this->dobjMut->Unlock();
}

void UI::GUIDObjArea::OnMouseWheel(OSInt x, OSInt y, Int32 amount)
{
}

void UI::GUIDObjArea::OnMouseMove(OSInt x, OSInt y)
{
	this->dobjMut->Lock();
	if (this->dobjHdlr)
	{
		this->dobjHdlr->OnMouseMove(x, y);
	}
	this->dobjMut->Unlock();
}

void UI::GUIDObjArea::OnMouseDown(OSInt x, OSInt y, MouseButton button)
{
	this->dobjMut->Lock();
	if (this->dobjHdlr)
	{
		this->dobjHdlr->OnMouseDown(x, y, button);
	}
	this->dobjMut->Unlock();
}

void UI::GUIDObjArea::OnMouseUp(OSInt x, OSInt y, MouseButton button)
{
	this->dobjMut->Lock();
	if (this->dobjHdlr)
	{
		this->dobjHdlr->OnMouseUp(x, y, button);
	}
	this->dobjMut->Unlock();
}

void UI::GUIDObjArea::OnMouseDblClick(OSInt x, OSInt y, MouseButton button)
{
}
