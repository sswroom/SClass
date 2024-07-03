#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/WIAManager.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#include <wia.h>

#if defined(__WiaDevMgr2_FWD_DEFINED__) && (_MSC_VER >= 1400)
#define DEVMGRTYPE IWiaDevMgr2
#define DEVITEMTYPE IWiaItem2
#else
#define DEVMGRTYPE IWiaDevMgr
#define DEVITEMTYPE IWiaItem
#endif

Media::WIAManager::WIAManager()
{
	HRESULT hr;
	this->pWiaDevMgr = 0;
	NEW_CLASS(this->devNames, Data::ArrayListStrUTF8());
	NEW_CLASS(this->devIds, Data::ArrayListStrUTF8());
	hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

#if defined(__WiaDevMgr2_FWD_DEFINED__) && (_MSC_VER >= 1400)
	hr = CoCreateInstance( CLSID_WiaDevMgr2, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr2, &this->pWiaDevMgr);
#else
	hr = CoCreateInstance( CLSID_WiaDevMgr, NULL, CLSCTX_LOCAL_SERVER, IID_IWiaDevMgr, &this->pWiaDevMgr);
#endif
	if (hr == S_OK)
	{
		IEnumWIA_DEV_INFO *enumDevInfo;
		DEVMGRTYPE *devMgr = (DEVMGRTYPE*)this->pWiaDevMgr;
		hr = devMgr->EnumDeviceInfo( WIA_DEVINFO_ENUM_LOCAL, &enumDevInfo );
		if (hr == S_OK)
		{
			IWiaPropertyStorage *pWiaPropertyStorage;
			
			while ((hr = enumDevInfo->Next( 1, &pWiaPropertyStorage, 0 )) == S_OK)
			{
				PROPSPEC PropSpec[2] = {0};
				PROPVARIANT PropVar[2] = {0};

				const ULONG c_nPropertyCount = sizeof(PropSpec)/sizeof(PropSpec[0]);

				PropSpec[0].ulKind = PRSPEC_PROPID;
				PropSpec[0].propid = WIA_DIP_DEV_ID;

				PropSpec[1].ulKind = PRSPEC_PROPID;
				PropSpec[1].propid = WIA_DIP_DEV_NAME;

				HRESULT hr = pWiaPropertyStorage->ReadMultiple( c_nPropertyCount, PropSpec, PropVar );
				if (SUCCEEDED(hr))
				{
					if (VT_BSTR == PropVar[0].vt && VT_BSTR == PropVar[1].vt)
					{
						this->devIds->Add(Text::StrToUTF8New(PropVar[0].bstrVal));
						this->devNames->Add(Text::StrToUTF8New(PropVar[1].bstrVal));
					}

					FreePropVariantArray( c_nPropertyCount, PropVar );
				}


				pWiaPropertyStorage->Release();
			}
			enumDevInfo->Release();
		}
	}
}

Media::WIAManager::~WIAManager()
{
	if (this->pWiaDevMgr)
	{
		((DEVMGRTYPE*)this->pWiaDevMgr)->Release();
	}
	this->devNames->DeleteAll();
	this->devIds->DeleteAll();
	DEL_CLASS(this->devNames);
	DEL_CLASS(this->devIds);
	CoUninitialize();
}


UOSInt Media::WIAManager::GetDeviceCount()
{
	return this->devNames->GetCount();
}

UnsafeArrayOpt<const UTF8Char> Media::WIAManager::GetDeviceName(UOSInt index)
{
	return this->devNames->GetItem(index);
}

Optional<Media::WIADevice> Media::WIAManager::CreateDevice(UOSInt index)
{
	UnsafeArray<const UTF8Char> devId;
	if (!this->devIds->GetItem(index).SetTo(devId))
		return 0;
	DEVITEMTYPE *devItem;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(devId);
#if defined(__WiaDevMgr2_FWD_DEFINED__) && (_MSC_VER >= 1400)
	HRESULT hr = ((DEVMGRTYPE*)this->pWiaDevMgr)->CreateDevice(0, (BSTR)wptr.Ptr(), &devItem);
#else
	HRESULT hr = ((DEVMGRTYPE*)this->pWiaDevMgr)->CreateDevice((BSTR)wptr.Ptr(), &devItem);
#endif
	Text::StrDelNew(wptr);
	if (hr == S_OK)
	{
		Media::WIADevice *dev;
		NEW_CLASS(dev, Media::WIADevice(devItem));
		return dev;
	}
	return 0;
}

Media::WIADevice::WIADevice(void *pWiaItem)
{
	this->pWiaItem = pWiaItem;
}

Media::WIADevice::~WIADevice()
{
	((DEVITEMTYPE*)this->pWiaItem)->Release();
}