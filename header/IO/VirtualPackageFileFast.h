#ifndef _SM_IO_VIRTUALPACKAGEFILEFAST
#define _SM_IO_VIRTUALPACKAGEFILEFAST
#include "Data/FastStringMapNN.hpp"
#include "IO/VirtualPackageFile.h"

namespace IO
{
	class VirtualPackageFileFast : public VirtualPackageFile
	{
	private:
		Data::FastStringMapNN<PackFileItem> namedItems;

		VirtualPackageFileFast(NN<const VirtualPackageFileFast> pkg);
	public:
		VirtualPackageFileFast(NN<Text::String> fileName);
		VirtualPackageFileFast(Text::CStringNN fileName);
		virtual ~VirtualPackageFileFast();

		virtual Optional<const PackFileItem> GetItemByName(Text::CStringNN name) const;
		virtual void PutItem(NN<Text::String> name, NN<PackFileItem> item);
		virtual void RemoveItem(NN<Text::String> name);
		virtual NN<PackageFile> Clone() const;
	};
}
#endif
