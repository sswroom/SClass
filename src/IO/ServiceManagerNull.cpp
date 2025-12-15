#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/ServiceManager.h"
#include "Manage/Process.h"

OSInt IO::ServiceManager::ServiceComparator::Compare(NN<ServiceItem> a, NN<ServiceItem> b) const
{
	return a->name->CompareTo(b->name);
}

IO::ServiceManager::ServiceManager()
{
}

IO::ServiceManager::~ServiceManager()
{

}

Bool IO::ServiceManager::ServiceCreate(Text::CStringNN svcName, Text::CString svcDesc, Text::CStringNN cmdLine, IO::ServiceInfo::ServiceState stype)
{
	return false;
}

Bool IO::ServiceManager::ServiceDelete(Text::CStringNN svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceSetDesc(Text::CStringNN svcName, Text::CStringNN svcDesc)
{
	return false;
}

Bool IO::ServiceManager::ServiceStart(Text::CStringNN svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceStop(Text::CStringNN svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceEnable(Text::CStringNN svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceDisable(Text::CStringNN svcName)
{
	return false;
}

Bool IO::ServiceManager::ServiceGetDetail(Text::CStringNN svcName, NN<ServiceDetail> svcDetail)
{
	return false;
}

UOSInt IO::ServiceManager::QueryServiceList(NN<Data::ArrayListNN<ServiceItem>> svcList)
{
	return 0;
}

void IO::ServiceManager::FreeServiceList(NN<Data::ArrayListNN<ServiceItem>> svcList)
{
}
