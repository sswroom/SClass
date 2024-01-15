#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DShow/DShowFilter.h"
#include "Media/DShow/DShowGraph.h"
#include <windows.h>
#include <dshow.h>

Media::DShow::DShowGraph::DShowGraph(Media::DShow::DShowManager *mgr, void *hwnd)
{
	IGraphBuilder *vrhGraph;
	this->mgr = mgr;
	this->hwnd = hwnd;
	this->graphBuilder = 0;
	this->pControl = 0;
	this->pEvent = 0;
	this->pSeek = 0;
	this->completed = false;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&vrhGraph);
	this->lastHR = hr;
	if (!FAILED(hr))
	{
		this->graphBuilder = vrhGraph;
		hr = vrhGraph->QueryInterface(IID_IMediaControl, &this->pControl);
		hr = vrhGraph->QueryInterface(IID_IMediaEvent, &this->pEvent);
		hr = vrhGraph->QueryInterface(IID_IMediaSeeking, &this->pSeek);
	}
}

Media::DShow::DShowGraph::~DShowGraph()
{
	if (this->graphBuilder)
	{
		if (this->pControl)
		{
			((IMediaControl*)this->pControl)->Release();
			this->pControl = 0;
		}
		if (this->pEvent)
		{
			((IMediaEvent*)this->pEvent)->Release();
			this->pEvent = 0;
		}
		if (this->pSeek)
		{
			((IMediaSeeking*)this->pSeek)->Release();
			this->pSeek = 0;
		}
		((IGraphBuilder*)this->graphBuilder)->Release();
		this->graphBuilder = 0;
	}
}

Bool Media::DShow::DShowGraph::AddFilter(DShowFilter *filter)
{
	IBaseFilter *f = (IBaseFilter*)filter->GetFilter();
	HRESULT hr = ((IGraphBuilder*)this->graphBuilder)->AddFilter(f, filter->GetName());
	this->lastHR = hr;
	return SUCCEEDED(hr);
}

Bool Media::DShow::DShowGraph::RenderFile(const WChar *fileName)
{
	HRESULT hr;
	hr = ((IGraphBuilder*)this->graphBuilder)->RenderFile(fileName, 0);
	this->lastHR = hr;
	return SUCCEEDED(hr);
}

Bool Media::DShow::DShowGraph::Run()
{
	HRESULT hr;
	hr = ((IMediaControl*)this->pControl)->Run();
	this->lastHR = hr;
	if (SUCCEEDED(hr))
	{
		this->completed = false;
	}
	return SUCCEEDED(hr);
}

Bool Media::DShow::DShowGraph::IsCompleted()
{
	return this->completed;
}

Bool Media::DShow::DShowGraph::SeekToTime(Data::Duration time)
{
	HRESULT hr;
	REFERENCE_TIME newTime = (REFERENCE_TIME)(time.GetSeconds() * 10000000LL + time.GetNS() / 100);
	if (this->pSeek)
	{
		hr = ((IMediaSeeking*)this->pSeek)->SetPositions(&newTime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);;
		this->lastHR = hr;
		if (SUCCEEDED(hr))
		{
			this->completed = false;
		}
		return SUCCEEDED(hr);
	}
	return false;
}

Bool Media::DShow::DShowGraph::CheckStatus()
{
	if (this->pEvent)
	{
		long evCode = 0;
		LONG_PTR param1 = 0;
		LONG_PTR param2 = 0;
	    HRESULT hr = S_OK;

		while (SUCCEEDED(((IMediaEvent*)this->pEvent)->GetEvent(&evCode, &param1, &param2, 0)))
		{
			switch (evCode)
			{
			case EC_COMPLETE:
			case EC_USERABORT:
				{
					this->completed = true;
				}
				break;

			case EC_ERRORABORT:
				if (pControl)
				{
					((IMediaControl*)this->pControl)->Stop();
				}
				break;
			}

			if (FAILED(((IMediaEvent*)this->pEvent)->FreeEventParams(evCode, param1, param2)))
				break;
		}
		return true;
	}
	else
	{
		return false;
	}
}
