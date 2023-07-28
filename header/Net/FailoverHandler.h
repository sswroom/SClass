#ifndef _SM_NET_FAILOVERHANDLER
#define _SM_NET_FAILOVERHANDLER
#include "Data/ArrayList.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	enum FailoverType
	{
		FT_MASTER_SLAVE,
		FT_ACTIVE_PASSIVE,
		FT_ROUND_ROBIN
	};

	template<class T> class FailoverHandler
	{
	private:
		Net::FailoverType foType;
		UOSInt lastIndex;
		Data::ArrayList<T *> channelList;
		Sync::Mutex mut;
	public:
		FailoverHandler(FailoverType foType);
		~FailoverHandler();

		T *GetCurrChannel();
		void GetOtherChannels(Data::ArrayList<T *> *chList);
		void SetCurrChannel(T *channel);
		void GetAllChannels(Data::ArrayList<T *> *chList);
		void AddChannel(T *channel);
	};

	template <class T> FailoverHandler<T>::FailoverHandler(Net::FailoverType foType)
	{
		this->foType = foType;
		this->lastIndex = 0;
	}

	template <class T> FailoverHandler<T>::~FailoverHandler()
	{
		T *channel;
		UOSInt i = this->channelList.GetCount();
		while (i-- > 0)
		{
			channel = this->channelList.GetItem(i);
			DEL_CLASS(channel);
		}
	}

	template <class T> T *FailoverHandler<T>::GetCurrChannel()
	{
		Sync::MutexUsage mutUsage(this->mut);
		if (this->channelList.GetCount() == 0)
		{
			return 0;
		}
		UOSInt initIndex;
		UOSInt currIndex;
		T *channel;
		switch (this->foType)
		{
		case Net::FT_ACTIVE_PASSIVE:
			initIndex = this->lastIndex;
			break;
		case Net::FT_MASTER_SLAVE:
			initIndex = 0;
			break;
		case Net::FT_ROUND_ROBIN:
			initIndex = (this->lastIndex + 1) % this->channelList.GetCount();
			break;
		default:
			return 0;
		}
		channel = this->channelList.GetItem(initIndex);
		if (!channel->ChannelFailure())
		{
			this->lastIndex = initIndex;
			return channel;
		}
		currIndex = (initIndex + 1) % this->channelList.GetCount();
		while (currIndex != initIndex)
		{
			channel = this->channelList.GetItem(currIndex);
			if (!channel->ChannelFailure())
			{
				this->lastIndex = currIndex;
				return channel;
			}
			currIndex = (currIndex + 1) % this->channelList.GetCount();
		}
		return 0;
	}

	template <class T> void FailoverHandler<T>::GetOtherChannels(Data::ArrayList<T *> *chList)
	{
		Sync::MutexUsage mutUsage(this->mut);
		T *channel;
		UOSInt j = this->channelList.GetCount();
		UOSInt i = (this->lastIndex + 1) % j;
		while (i != this->lastIndex)
		{
			channel = this->channelList.GetItem(i);
			if (!channel->ChannelFailure())
			{
				chList->Add(channel);
			}
			i = (i + 1) % j;
		}
	}

	template <class T> void FailoverHandler<T>::SetCurrChannel(T *channel)
	{
		Sync::MutexUsage mutUsage(this->mut);
		UOSInt i = this->channelList.IndexOf(channel);
		if (i != INVALID_INDEX)
		{
			this->lastIndex = i;
		}

	}

	template <class T> void FailoverHandler<T>::GetAllChannels(Data::ArrayList<T *> *chList)
	{
		Sync::MutexUsage mutUsage(this->mut);
		chList->AddAll(this->channelList);
	}

	template <class T> void FailoverHandler<T>::AddChannel(T *channel)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->channelList.Add(channel);
	}
}
#endif
