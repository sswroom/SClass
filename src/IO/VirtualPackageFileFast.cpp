#include "Stdafx.h"
#include "IO/VirtualPackageFileFast.h"
#include "Sync/Interlocked.h"

IO::VirtualPackageFileFast::VirtualPackageFileFast(NN<const VirtualPackageFileFast> pkg) : VirtualPackageFile(pkg->GetSourceNameObj())
{
	this->pkgFiles.PutAll(pkg->pkgFiles);
	this->parent = pkg->parent;
	this->items.AddAll(pkg->items);
	this->namedItems.PutAll(pkg->namedItems);
	NN<IO::PackFileItem> item;
	Data::ArrayIterator<NN<IO::PackFileItem>> it = this->items.Iterator();
	while (it.HasNext())
	{
		item = it.Next();
		Sync::Interlocked::IncrementI32(item->useCnt);
	}
}

IO::VirtualPackageFileFast::VirtualPackageFileFast(NN<Text::String> fileName) : VirtualPackageFile(fileName)
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

void IO::VirtualPackageFileFast::PutItem(NN<Text::String> name, NN<PackFileItem> item)
{
	this->namedItems.PutNN(name, item.Ptr());
}

void IO::VirtualPackageFileFast::RemoveItem(NN<Text::String> name)
{
	this->namedItems.RemoveNN(name);
}

NN<IO::PackageFile> IO::VirtualPackageFileFast::Clone() const
{
	NN<IO::VirtualPackageFileFast> pkg;
	NEW_CLASSNN(pkg, VirtualPackageFileFast(NN<const VirtualPackageFileFast>(*this)));
	return pkg;
}
