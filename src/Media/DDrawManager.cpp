#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include <windows.h>
#include <ddraw.h>

Int32 __stdcall Media::DDrawManager::DDEnumMonCall(void *guid, Char *driverDesc, Char *driverName, void *context, void *hMonitor)
{
	Media::DDrawManager *me = (Media::DDrawManager*)context;
	LPDIRECTDRAW7 lpDD;
	if (hMonitor == 0 && me->defDD != 0)
	{
		return 1;
	}
	if (me->monMap->Get((OSInt)hMonitor) != 0)
	{
		return 1;
	}

	if (DirectDrawCreateEx( (GUID FAR *)guid, (VOID**)&lpDD, IID_IDirectDraw7, NULL ) != DD_OK )
	{
	}
	else
	{
		if (hMonitor == 0)
		{
			me->defDD = lpDD;
		}
		else
		{
			me->monMap->Put((OSInt)hMonitor, lpDD);
		}
	}

	return 1;
}

void Media::DDrawManager::ReleaseAll()
{
	if (this->defDD)
	{
		((LPDIRECTDRAW7)this->defDD)->Release();
		this->defDD = 0;
	}
	Data::ArrayList<void*> *monList;
	UOSInt i;
	monList = this->monMap->GetValues();
	i = monList->GetCount();
	while (i-- > 0)
	{
		((LPDIRECTDRAW7)monList->GetItem(i))->Release();
	}
	this->monMap->Clear();
}

Media::DDrawManager::DDrawManager()
{
	NEW_CLASS(this->monMap, Data::Int64Map<void*>());
	this->defDD = 0;
	this->RecheckMonitor();
}

Media::DDrawManager::~DDrawManager()
{
	this->ReleaseAll();
	DEL_CLASS(this->monMap);
}

Bool Media::DDrawManager::IsError()
{
	return this->defDD == 0;
}

void *Media::DDrawManager::GetDD7(void *hMonitor)
{
	this->RecheckMonitor();
	void *ret = this->monMap->Get((OSInt)hMonitor);
	if (ret)
		return ret;
	return this->defDD;
}

void Media::DDrawManager::ReleaseDD7(void *hMonitor)
{
	void *ret = this->monMap->Remove((OSInt)hMonitor);
	if (ret)
	{
		((LPDIRECTDRAW7)ret)->Release();
	}
}

void Media::DDrawManager::RecheckMonitor()
{
	DirectDrawEnumerateExA((LPDDENUMCALLBACKEXA)DDEnumMonCall, this, DDENUM_ATTACHEDSECONDARYDEVICES);
}

void Media::DDrawManager::Reinit()
{
	this->ReleaseAll();
	this->RecheckMonitor();
}
