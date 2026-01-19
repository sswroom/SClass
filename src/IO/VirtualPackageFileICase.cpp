#include "Stdafx.h"
#include "IO/VirtualPackageFileICase.h"
#include "Sync/Interlocked.h"

IO::VirtualPackageFileICase::VirtualPackageFileICase(NN<const VirtualPackageFileICase> pkg) : VirtualPackageFile(pkg->GetSourceNameObj())
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

IO::VirtualPackageFileICase::VirtualPackageFileICase(NN<Text::String> fileName) : VirtualPackageFile(fileName)
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
	return this->namedItems.GetC(name);
}

void IO::VirtualPackageFileICase::PutItem(NN<Text::String> name, NN<PackFileItem> item)
{
	this->namedItems.PutNN(name, item);
}

void IO::VirtualPackageFileICase::RemoveItem(NN<Text::String> name)
{
	this->namedItems.RemoveNN(name);
}

NN<IO::PackageFile> IO::VirtualPackageFileICase::Clone() const
{
	NN<IO::PackageFile> pkg;
	NEW_CLASSNN(pkg, VirtualPackageFileICase(NN<const VirtualPackageFileICase>(*this)));
	return pkg;
}
