#ifndef _SM_DATA_ARRAYLISTUTIL
#define _SM_DATA_ARRAYLISTUTIL

#define LIST_CALL_FUNC(list, func) { UOSInt i = (list)->GetCount(); while (i-- > 0) func((list)->GetItem(i)); }
#define LIST_FREE_FUNC(list, func) { LIST_CALL_FUNC(list, func); (list)->Clear(); }
#define LIST_FREE_STRING(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItem(i)->Release(); (list)->Clear(); }
#define LIST_FREE_STRING_NO_CLEAR(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItem(i)->Release(); }
#define NNLIST_FREE_STRING(list) { UOSInt i = (list)->GetCount(); while (i-- > 0) (list)->GetItemNoCheck(i)->Release(); (list)->Clear(); }
#define NNLIST_CALL_FUNC(list, func) { UOSInt i = (list)->GetCount(); while (i-- > 0) func((list)->GetItemNoCheck(i)); }

#endif