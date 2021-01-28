#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DShow/DShowEVRFilter.h"
#include "Media/DShow/DShowGraph.h"
#include "Media/DShow/DShowManager.h"
#include "Media/DShow/DShowVMR9Filter.h"
#include "Media/DShow/DShowVideoRecvFilter.h"
#include <windows.h>

Media::DShow::DShowManager::DShowManager()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);
}

Media::DShow::DShowManager::~DShowManager()
{
	CoUninitialize();
}

Media::DShow::DShowGraph *Media::DShow::DShowManager::CreateGraph(void *hwnd)
{
	Media::DShow::DShowGraph *graph;
	NEW_CLASS(graph, Media::DShow::DShowGraph(this, hwnd));
	return graph;
}

Media::DShow::DShowEVRFilter *Media::DShow::DShowManager::CreateEVR(void *hwnd)
{
	Media::DShow::DShowEVRFilter *filter;
	NEW_CLASS(filter, Media::DShow::DShowEVRFilter(this, hwnd));
	return filter;
}

Media::DShow::DShowVMR9Filter *Media::DShow::DShowManager::CreateVMR9(void *hwnd)
{
	Media::DShow::DShowVMR9Filter *filter;
	NEW_CLASS(filter, Media::DShow::DShowVMR9Filter(this, hwnd));
	return filter;
}

Media::DShow::DShowVideoRecvFilter *Media::DShow::DShowManager::CreateVideoRecvFilter(Media::DShow::DShowVideoRecvFilter::VFrame32Hdlr hdlr, void *userObj)
{
	Media::DShow::DShowVideoRecvFilter *filter;
	NEW_CLASS(filter, Media::DShow::DShowVideoRecvFilter(this, hdlr, userObj));
	return filter;
}
