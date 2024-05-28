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

NN<Media::DShow::DShowGraph> Media::DShow::DShowManager::CreateGraph(void *hwnd)
{
	NN<Media::DShow::DShowGraph> graph;
	NEW_CLASSNN(graph, Media::DShow::DShowGraph(this, hwnd));
	return graph;
}

NN<Media::DShow::DShowEVRFilter> Media::DShow::DShowManager::CreateEVR(void *hwnd)
{
	NN<Media::DShow::DShowEVRFilter> filter;
	NEW_CLASSNN(filter, Media::DShow::DShowEVRFilter(this, hwnd));
	return filter;
}

NN<Media::DShow::DShowVMR9Filter> Media::DShow::DShowManager::CreateVMR9(void *hwnd)
{
	NN<Media::DShow::DShowVMR9Filter> filter;
	NEW_CLASSNN(filter, Media::DShow::DShowVMR9Filter(this, hwnd));
	return filter;
}

NN<Media::DShow::DShowVideoRecvFilter> Media::DShow::DShowManager::CreateVideoRecvFilter(Media::DShow::DShowVideoRecvFilter::VFrame32Hdlr hdlr, void *userObj)
{
	NN<Media::DShow::DShowVideoRecvFilter> filter;
	NEW_CLASSNN(filter, Media::DShow::DShowVideoRecvFilter(this, hdlr, userObj));
	return filter;
}
