#ifndef _SM_SSWR_ORGANWEB_ORGANWEBCOMMON
#define _SM_SSWR_ORGANWEB_ORGANWEBCOMMON
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListInt64.h"
#include "Data/Comparator.h"
#include "Data/FastMap.h"
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

		typedef struct
		{
			Int32 bookId;
			Int32 speciesId;
			NotNullPtr<Text::String> dispName;
		} BookSpInfo;

		class BookInfo
		{
		public:
			Int32 id;
			NotNullPtr<Text::String> title;
			NotNullPtr<Text::String> author;
			NotNullPtr<Text::String> press;
			Int64 publishDate;
			Text::String *url;
			Int32 userfileId;

			Data::ArrayList<BookSpInfo*> species;
		};

		typedef struct
		{
			Int32 id;
			Int32 parentId;
			Text::String *cname;
			Text::String *ename;
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
			NotNullPtr<Text::String> oriFileName;
			Int64 fileTimeTicks;
			Double lat;
			Double lon;
			Int32 webuserId;
			Int32 speciesId;
			Int64 captureTimeTicks;
			NotNullPtr<Text::String> dataFileName;
			UInt32 crcVal;
			Int32 rotType;
			Int32 prevUpdated;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
			Text::String *descript;
			Text::String *location;
		} UserFileInfo;

		typedef struct
		{
			Int32 id;
			Int32 crcVal;
			NotNullPtr<Text::String> imgUrl;
			NotNullPtr<Text::String> srcUrl;
			NotNullPtr<Text::String> location;
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
			Text::String *oriFileName;
			Text::String *dataFileName;
			Int32 webuserId;
		};

		typedef struct
		{
			Int32 id;
			NotNullPtr<Text::String> userName;
			Text::String *pwd;
			NotNullPtr<Text::String> watermark;
			UserType userType;
			Data::ArrayListInt64 userFileIndex;
			Data::ArrayList<UserFileInfo*> userFileObj;
			Data::FastMap<Int32, Data::FastMap<Int64, TripInfo*>*> tripCates;
			Data::FastMap<Data::Timestamp, DataFileInfo*> gpsDataFiles;
			Data::FastMap<Data::Timestamp, DataFileInfo*> tempDataFiles;
			Int32 unorganSpId;
		} WebUserInfo;

		class SpeciesInfo
		{
		public:
			Int32 speciesId;
			NotNullPtr<Text::String> engName;
			NotNullPtr<Text::String> chiName;
			NotNullPtr<Text::String> sciName;
			UInt32 sciNameHash;
			Int32 groupId;
			NotNullPtr<Text::String> descript;
			NotNullPtr<Text::String> dirName;
			Text::String *photo;
			NotNullPtr<Text::String> idKey;
			Int32 cateId;
			SpeciesFlags flags;
			Int32 photoId;
			Int32 photoWId;
			Text::String *poiImg;

			Data::ArrayList<BookSpInfo*> books;
			Data::ArrayList<UserFileInfo*> files;
			Data::FastMap<Int32, WebFileInfo*> wfiles;
		};

		class GroupInfo
		{
		public:
			Int32 id;
			Int32 groupType;
			NotNullPtr<Text::String> engName;
			NotNullPtr<Text::String> chiName;
			NotNullPtr<Text::String> descript;
			Int32 parentId;
			Int32 photoGroup;
			Int32 photoSpecies;
			Text::String *idKey;
			Int32 cateId;
			GroupFlags flags;

			UOSInt photoCount;
			UOSInt myPhotoCount;
			UOSInt totalCount;
			SpeciesInfo *photoSpObj;
			Data::ArrayList<SpeciesInfo*> species;
			Data::ArrayList<GroupInfo *> groups;
		};

		typedef struct
		{
			Int32 id;
			Text::String *chiName;
			Text::String *engName;
		} GroupTypeInfo;

		struct CategoryInfo
		{
			Int32 cateId;
			Text::String *chiName;
			Text::String *dirName;
			NotNullPtr<Text::String> srcDir;
			Int32 flags;
			Data::FastMap<Int32, GroupTypeInfo *> groupTypes;
			Data::ArrayList<GroupInfo*> groups;
		};

		typedef struct
		{
			UInt32 scnWidth;
			Bool isMobile;
			WebUserInfo *user;
			Data::ArrayListInt32 *pickObjs;
			PickObjType pickObjType;
		} RequestEnv;

		class SpeciesSciNameComparator : public Data::Comparator<SpeciesInfo*>
		{
		public:
			virtual ~SpeciesSciNameComparator(){};

			virtual OSInt Compare(SpeciesInfo *a, SpeciesInfo *b) const
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

		class UserFileTimeComparator : public Data::Comparator<UserFileInfo*>
		{
		public:
			virtual ~UserFileTimeComparator(){};
			virtual OSInt Compare(UserFileInfo *a, UserFileInfo *b) const
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

		class UserFileDescComparator : public Data::Comparator<UserFileInfo*>
		{
		private:
			RequestEnv *env;
		public:
			UserFileDescComparator(RequestEnv *env)
			{
				this->env = env;
			}
			virtual ~UserFileDescComparator(){}
			virtual OSInt Compare(UserFileInfo *a, UserFileInfo *b) const
			{
				Bool aDesc = false;
				Bool bDesc = false;
				NotNullPtr<Text::String> bStr;
				if (env->user != 0)
				{
					if (a->descript != 0 && a->descript->leng > 0 && (env->user->userType == UserType::Admin || a->webuserId == env->user->id))
						aDesc = true;
					if (b->descript != 0 && b->descript->leng > 0 && (env->user->userType == UserType::Admin || b->webuserId == env->user->id))
						bDesc = true;
				}
				if (aDesc && bDesc && bStr.Set(b->descript))
				{
					OSInt ret = a->descript->CompareTo(bStr);
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
