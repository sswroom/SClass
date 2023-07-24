#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/ServiceManager.h"
#include "Manage/Process.h"

OSInt IO::ServiceManager::ServiceComparator::Compare(ServiceItem* a, ServiceItem* b) const
{
	return a->name->CompareTo(b->name.Ptr());
}

IO::ServiceManager::ServiceManager()
{
	this->clsData = 0;
}

IO::ServiceManager::~ServiceManager()
{

}

Bool IO::ServiceManager::ServiceCreate(Text::CString svcName, Text::CString svcDesc, Text::CString cmdLine, IO::ServiceInfo::ServiceState stype)
{
	return false;
}

Bool IO::ServiceManager::ServiceDelete(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceSetDesc(Text::CString svcName, Text::CString svcDesc)
{
	return false;
}

Bool IO::ServiceManager::ServiceStart(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceStop(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceEnable(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceDisable(Text::CString svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceGetDetail(Text::CString svcName, ServiceDetail *svcDetail)
{
	return false;
}

UOSInt IO::ServiceManager::QueryServiceList(Data::ArrayList<ServiceItem*> *svcList)
{
	return 0;
}

void IO::ServiceManager::FreeServiceList(Data::ArrayList<ServiceItem*> *svcList)
{
}
