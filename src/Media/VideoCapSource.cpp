#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"
#include "Media/VideoCapSource.h"

#include <windows.h>
#include <dshow.h>

Int32 Media::VideoCapSource::GetDeviceCount()
{
	Int32 retVal = 0;
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pEnum = NULL;

	CoInitialize(0);

	// Create the System Device Enumerator.
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, 
		reinterpret_cast<void**>(&pDevEnum));
	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(
			CLSID_VideoInputDeviceCategory,
			&pEnum, 0);

		//////////////////////////////
		if (SUCCEEDED(hr))
		{
			pEnum->Release();
		}
		pDevEnum->Release();
	}
	return retVal;
}

WChar *Media::VideoCapSource::GetDeviceName(WChar *buff, Int32 devNo)
{
	//////////////////////////////
	return 0;
}

Media::VideoCapSource::VideoCapSource(WChar *devName, Int32 freq, Int16 nbits, Int16 nChannels)
{
}

Media::VideoCapSource::VideoCapSource(Int32 devId, Int32 freq, Int16 nbits, Int16 nChannels)
{
}

Media::VideoCapSource::~VideoCapSource()
{
}
