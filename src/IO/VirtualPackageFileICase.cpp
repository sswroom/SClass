#include "Stdafx.h"
#include "IO/VirtualPackageFileICase.h"
#include "Sync/Interlocked.h"

IO::VirtualPackageFileICase::VirtualPackageFileICase(NotNullPtr<const VirtualPackageFileICase> pkg) : VirtualPackageFile(pkg->GetSourceNameObj())
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

IO::VirtualPackageFileICase::VirtualPackageFileICase(NotNullPtr<Text::String> fileName) : VirtualPackageFile(fileName)
{
}

IO::VirtualPackageFileICase::VirtualPackageFileICase(Text::CStringNN fileName) : VirtualPackageFile(fileName)
{
}

IO::VirtualPackageFileICase::~VirtualPackageFileICase()
{
}

Optional<const IO::PackFileItem> IO::VirtualPackageFileICase::GetItemByName(Text::CStringNN name) const
{
	return this->namedItems.Get(name);
}

void IO::VirtualPackageFileICase::PutItem(NotNullPtr<Text::String> name, NotNullPtr<PackFileItem> item)
{
	this->namedItems.PutNN(name, item.Ptr());
}

void IO::VirtualPackageFileICase::RemoveItem(NotNullPtr<Text::String> name)
{
	this->namedItems.RemoveNN(name);
}

NotNullPtr<IO::PackageFile> IO::VirtualPackageFileICase::Clone() const
{
	NotNullPtr<IO::PackageFile> pkg;
	NEW_CLASSNN(pkg, VirtualPackageFileICase(NotNullPtr<const VirtualPackageFileICase>(*this)));
	return pkg;
}
