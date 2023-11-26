#include "Stdafx.h"
#include "IO/VirtualPackageFileFast.h"
#include "Sync/Interlocked.h"

IO::VirtualPackageFileFast::VirtualPackageFileFast(NotNullPtr<const VirtualPackageFileFast> pkg) : VirtualPackageFile(pkg->GetSourceNameObj())
{
	this->pkgFiles.PutAll(pkg->pkgFiles);
	this->parent = pkg->parent;
	this->items.AddAll(pkg->items);
	this->namedItems.PutAll(pkg->namedItems);
	IO::PackFileItem *item;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->items.GetCount();
	while (i < j)
	{
		item = this->items.GetItem(i);
		Sync::Interlocked::IncrementI32(item->useCnt);
		i++;
	}
}

IO::VirtualPackageFileFast::VirtualPackageFileFast(NotNullPtr<Text::String> fileName) : VirtualPackageFile(fileName)
{
}

IO::VirtualPackageFileFast::VirtualPackageFileFast(Text::CStringNN fileName) : VirtualPackageFile(fileName)
{
}

IO::VirtualPackageFileFast::~VirtualPackageFileFast()
{
}

Optional<const IO::PackFileItem> IO::VirtualPackageFileFast::GetItemByName(Text::CStringNN name) const
{
	return this->namedItems.GetC(name);
}

void IO::VirtualPackageFileFast::PutItem(NotNullPtr<Text::String> name, NotNullPtr<PackFileItem> item)
{
	this->namedItems.PutNN(name, item.Ptr());
}

void IO::VirtualPackageFileFast::RemoveItem(NotNullPtr<Text::String> name)
{
	this->namedItems.RemoveNN(name);
}

NotNullPtr<IO::PackageFile> IO::VirtualPackageFileFast::Clone() const
{
	NotNullPtr<IO::VirtualPackageFileFast> pkg;
	NEW_CLASSNN(pkg, VirtualPackageFileFast(NotNullPtr<const VirtualPackageFileFast>(*this)));
	return pkg;
}
