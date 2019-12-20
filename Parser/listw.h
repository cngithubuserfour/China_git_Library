// ListW - Stores a list of pointers (Wrapped list)

#ifndef sqlines_listw_h
#define sqlines_listw_h

class ListwItem
{
public:
	void *value;

	ListwItem *prev;
	ListwItem *next;

	ListwItem()
	{
		value = NULL;
		prev = NULL;
		next = NULL;
	}
};

class ListW
{
	// The first element in the list
	ListwItem *first;
	// The last element in the list
	ListwItem *last;

	// Total number of items in the list
	int count;

public:
	// Constructor
	ListW()
	{
		first = NULL;
		last = NULL;
		count = 0;
	}

	// Destructor
	~ListW()
	{
		DeleteAll();
	}

	// Delete all elements from the list
	void DeleteAll()
	{
		ListwItem *current = first;
		ListwItem *next = NULL;

		// remove list elements
		while(current)
		{
			next = current->next;
			delete current;
			current = next;
		}

		first = NULL;
		last = NULL;
		count = 0;
	}

    // Delete the last item
    void DeleteLast()
    {
        DeleteSince(GetLast());
    }

	// Delete elements since the specified item including
	void DeleteSince(ListwItem *since)
	{
		if(since == NULL)
			return;

		ListwItem *current = since;
		ListwItem *next = NULL;

		if(since == first)
			first = NULL;

		last = since->prev;

		if(last != NULL)
			last->next = NULL;

		// remove list elements
		while(current)
		{
			next = current->next;
			delete current;
			current = next;

			count--;
		}
	}

	// Add a new item to the list
	void Add(void* value)
	{
		ListwItem *item = new ListwItem();

		item->value = value;

		// check if it is the first item in the list
		if(!first)
		{
			first = item;
			item->prev = NULL;
		}
		else
		// otherwise link the previous element with the new and vice versa
		{
			last->next = item;
			item->prev = last;
		}

		last = item;
		item->next = NULL;

		// increase the number of items in the list
		count++;
	}

	// Get the first value
	ListwItem* GetFirst()
	{
		return first;
	}

	// Get the last value
	ListwItem* GetLast()
	{
		return last;
	}

	// Get the total number of items in the list
	int GetCount() 
	{ 
		return count; 
	}
};

#endif // sqlines_listw_h
