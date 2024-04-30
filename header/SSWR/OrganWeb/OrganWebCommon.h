#ifndef _SM_SSWR_ORGANWEB_ORGANWEBCOMMON
#define _SM_SSWR_ORGANWEB_ORGANWEBCOMMON
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListInt64.h"
#include "Data/Comparator.h"
#include "Data/FastMapNN.h"
#include "Data/Timestamp.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganWeb
	{
		typedef enum
		{
			POT_UNKNOWN,
			POT_USERFILE,
			POT_GROUP,
			POT_SPECIES
		} PickObjType;

		typedef enum
		{
			GF_NONE = 0,
			GF_ADMIN_ONLY = 1
		} GroupFlags;

		typedef enum
		{
			SF_NONE = 0,
			SF_HAS_MYPHOTO = 1,
			SF_HAS_WEBPHOTO = 8
		} SpeciesFlags;

		enum class DataFileType
		{
			Unknown,
			GPSTrack,
			Temperature
		};

		enum class UserType
		{
			Admin,
			User
		};

		enum class FileType
		{
			Unknown,
			Image,
			Video,
			Audio
		};

		enum class LocType
		{
			Unknown,
			GPSTrack,
			PhotoExif,
			UserInput
		};

		typedef struct
		{
			Int32 bookId;
			Int32 speciesId;
			NN<Text::String> dispName;
		} BookSpInfo;

		class BookInfo
		{
		public:
			Int32 id;
			NN<Text::String> title;
			NN<Text::String> author;
			NN<Text::String> press;
			Int64 publishDate;
			Optional<Text::String> url;
			Int32 userfileId;

			Data::ArrayListNN<BookSpInfo> species;
		};

		typedef struct
		{
			Int32 id;
			Int32 parentId;
			Optional<Text::String> cname;
			Optional<Text::String> ename;
			Double lat;
			Double lon;
			Int32 cateId;
			Int32 locType;
		} LocationInfo;

		typedef struct
		{
			Int64 fromDate;
			Int64 toDate;
			Int32 locId;
			Int32 cateId;
		} TripInfo;
		
		typedef struct
		{
			Int32 id;
			FileType fileType;
			NN<Text::String> oriFileName;
			Int64 fileTimeTicks;
			Double lat;
			Double lon;
			Int32 webuserId;
			Int32 speciesId;
			Int64 captureTimeTicks;
			NN<Text::String> dataFileName;
			UInt32 crcVal;
			Int32 rotType;
			Int32 prevUpdated;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
			Optional<Text::String> descript;
			Optional<Text::String> location;
			Optional<Text::String> camera;
			LocType locType;
		} UserFileInfo;

		typedef struct
		{
			Int32 id;
			Int32 crcVal;
			NN<Text::String> imgUrl;
			NN<Text::String> srcUrl;
			NN<Text::String> location;
			Int32 prevUpdated;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
		} WebFileInfo;

		struct DataFileInfo
		{
			Int32 id;
			DataFileType fileType;
			Data::Timestamp startTime;
			Data::Timestamp endTime;
			NN<Text::String> oriFileName;
			NN<Text::String> dataFileName;
			Int32 webuserId;
		};

		typedef struct
		{
			Int32 id;
			NN<Text::String> userName;
			Optional<Text::String> pwd;
			NN<Text::String> watermark;
			UserType userType;
			Data::ArrayListInt64 userFileIndex;
			Data::ArrayListNN<UserFileInfo> userFileObj;
			Data::FastMapNN<Int32, Data::FastMapNN<Int64, TripInfo>> tripCates;
			Data::FastMapNN<Data::Timestamp, DataFileInfo> gpsDataFiles;
			Data::FastMapNN<Data::Timestamp, DataFileInfo> tempDataFiles;
			Int32 unorganSpId;
		} WebUserInfo;

		class SpeciesInfo
		{
		public:
			Int32 speciesId;
			NN<Text::String> engName;
			NN<Text::String> chiName;
			NN<Text::String> sciName;
			UInt32 sciNameHash;
			Int32 groupId;
			NN<Text::String> descript;
			NN<Text::String> dirName;
			Optional<Text::String> photo;
			NN<Text::String> idKey;
			Int32 cateId;
			SpeciesFlags flags;
			Int32 photoId;
			Int32 photoWId;
			Optional<Text::String> poiImg;

			Data::ArrayListNN<BookSpInfo> books;
			Data::ArrayListNN<UserFileInfo> files;
			Data::FastMapNN<Int32, WebFileInfo> wfiles;
		};

		class GroupInfo
		{
		public:
			Int32 id;
			Int32 groupType;
			NN<Text::String> engName;
			NN<Text::String> chiName;
			NN<Text::String> descript;
			Int32 parentId;
			Int32 photoGroup;
			Int32 photoSpecies;
			Optional<Text::String> idKey;
			Int32 cateId;
			GroupFlags flags;

			UOSInt photoCount;
			UOSInt myPhotoCount;
			UOSInt totalCount;
			Optional<SpeciesInfo> photoSpObj;
			Data::ArrayListNN<SpeciesInfo> species;
			Data::ArrayListNN<GroupInfo> groups;
		};

		typedef struct
		{
			Int32 id;
			NN<Text::String> chiName;
			NN<Text::String> engName;
		} GroupTypeInfo;

		struct CategoryInfo
		{
			Int32 cateId;
			NN<Text::String> chiName;
			NN<Text::String> dirName;
			NN<Text::String> srcDir;
			Int32 flags;
			Data::FastMapNN<Int32, GroupTypeInfo> groupTypes;
			Data::ArrayListNN<GroupInfo> groups;
		};

		typedef struct
		{
			UInt32 scnWidth;
			Bool isMobile;
			Optional<WebUserInfo> user;
			Data::ArrayListInt32 *pickObjs;
			PickObjType pickObjType;
		} RequestEnv;

		struct PeakInfo
		{
			Int32 id;
			NN<Text::String> refId;
			NN<Text::String> district;
			Double mapX;
			Double mapY;
			Double markedHeight;
			Int32 csys;
			Int32 status;
			Optional<Text::String> name;
			Optional<Text::String> type;
		};

		class SpeciesSciNameComparator : public Data::Comparator<NN<SpeciesInfo>>
		{
		public:
			virtual ~SpeciesSciNameComparator(){};

			virtual OSInt Compare(NN<SpeciesInfo> a, NN<SpeciesInfo> b) const
			{
				if (a->sciNameHash > b->sciNameHash)
				{
					return 1;
				}
				else if (a->sciNameHash < b->sciNameHash)
				{
					return -1;
				}
				else
				{
					return a->sciName->CompareToFast(b->sciName->ToCString());
				}
			}
		};

		class UserFileTimeComparator : public Data::Comparator<NN<UserFileInfo>>
		{
		public:
			virtual ~UserFileTimeComparator(){};
			virtual OSInt Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const
			{
				if (a->webuserId > b->webuserId)
				{
					return 1;
				}
				else if (a->webuserId < b->webuserId)
				{
					return -1;
				}
				else if (a->captureTimeTicks > b->captureTimeTicks)
				{
					return 1;
				}
				else if (a->captureTimeTicks < b->captureTimeTicks)
				{
					return -1;
				}
				else
				{
					return 0;
				}
			}
		};

		class UserFileDescComparator : public Data::Comparator<NN<UserFileInfo>>
		{
		private:
			NN<RequestEnv> env;
		public:
			UserFileDescComparator(NN<RequestEnv> env)
			{
				this->env = env;
			}
			virtual ~UserFileDescComparator(){}
			virtual OSInt Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const
			{
				Bool aDesc = false;
				Bool bDesc = false;
				NN<Text::String> aStr;
				NN<Text::String> bStr;
				NN<WebUserInfo> user;
				if (env->user.SetTo(user))
				{
					if (a->descript.SetTo(aStr) && aStr->leng > 0 && (user->userType == UserType::Admin || a->webuserId == user->id))
						aDesc = true;
					if (b->descript.SetTo(bStr) && bStr->leng > 0 && (user->userType == UserType::Admin || b->webuserId == user->id))
						bDesc = true;
				}
				if (aDesc && bDesc && a->descript.SetTo(aStr) && b->descript.SetTo(bStr))
				{
					OSInt ret = aStr->CompareTo(bStr);
					if (ret != 0)
						return ret;
				}
				else if (aDesc)
				{
					return -1;
				}
				else if (bDesc)
				{
					return 1;
				}
				if (a->captureTimeTicks > b->captureTimeTicks)
				{
					return 1;
				}
				else if (a->captureTimeTicks < b->captureTimeTicks)
				{
					return -1;
				}
				else
				{
					return 0;
				}
			}
		};
	}
}
#endif
