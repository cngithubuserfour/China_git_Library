// ListT<T> class - Stores a list of T values, linked to each other in a chain 

#ifndef sqlines_listt_h
#define sqlines_listt_h

template <class T> class ListT
{
	// The first element in the list
	T *first;
	// The last element in the list
	T *last;
	// The current element (for list client classes that iterate through the list)
	T *current;
	// Next element (it is set in advance as current can be removed after retrieval)
	T *nextl;

	// Total number of items in the list
	int count;
	// Specifies whether items in the list are external or not (should be deleted in destructor or not)
	bool referencedItems;

public:

	// Next and previous when the list is included into another list
	ListT<T> *next; 
	ListT<T> *prev;

public:
	// Template constructor
	ListT()
	{
		first = NULL;
		last = NULL;
		current = NULL;
		nextl = NULL;
		count = 0;
		referencedItems = false;

		next = NULL;
		prev = NULL;
	}

	// Template destructor
	~ListT()
	{
		if(!referencedItems)	
			DeleteAll();
	}

	// Delete the current item 
	void DeleteCurrent(bool destroyObject = true)
	{
		if(!current)
			return;

		Delete(current, destroyObject);

		current = NULL;
	}

	// Delete the item (must be in list) 
	void Delete(T *del, bool destroyObject = true)
	{
		if(!del)
			return;

		// set the link with the previous item
		if(del->prev)
			del->prev->next = del->next;
		else
			// the deleted is the first item, so set the first to the next item
			first = del->next;

		// set the previous link in the next item
		if(del->next)
			del->next->prev = del->prev;

		// check if the item is the last item
		if(del == last)
		{
			if(del->prev)
				last = del->prev;
			else
				last = NULL;
		}

		if(destroyObject)
			delete del;

		if(count)
			count--;
	}

	// Delete all elements from the list
	void DeleteAll()
	{
		T *current = first;
		T *next = NULL;

		// remove list elements
		while(current)
		{
			next = current->next;
			delete current;
			current = next;
		}

		first = NULL;
		last = NULL;
		current = NULL;
		count = 0;
	}

	// Sets whether items are just referenced by the list, and must not be deleted 
	void SetReferencedItems(bool value)
	{
		referencedItems = value;
	}

	// Add a new item to the list
	void Add(T* value)
	{
		// check if it is the first item in the list
		if(!first)
		{
			first = value;
			value->prev = NULL;
		}
		else
			// otherwise link the previous element with the new and vice versa
		{
			last->next = value;
			value->prev = last;
		}

		last = value;
		value->next = NULL;

		// make the newly added value as current so DeleteCurrent can be applied after Add but without GetFirst/Next
		current = value;

		// increase the number of items in the list
		count++;
	}

	// Append a new item to the list after the specified item
	void Append(T* existing, T* value)
	{
		if(!existing || !value)
			return;

		T* prevNext = existing->next;

		// set links
		existing->next = value;
		value->prev = existing; 
		value->next = prevNext;

		if(prevNext)
			prevNext->prev = value;

		// check if we insert after current last item
		if(existing == last)
			last = value;

		// increase the number of items in the list
		count++;
	}

	// Prepend a new item in the list before the specified item
	void Prepend(T* existing, T* value)
	{
		if(!existing || !value)
			return;

		T* prevExisting = existing->prev;

		// if previous does not exist, we are adding new first
		if(!prevExisting)
			first = value;  
		else
			prevExisting->next = value;

		// set links for existing and prepended items
		value->next = existing;
		value->prev = existing->prev;
		existing->prev = value;

		// increase the number of items in the list
		count++;
	}

	// Get the first value
	T* GetFirst()
	{
		if(!first)
			return NULL;

		current = first;
		nextl = first->next;

		return first;
	}

	// Get the first value without using current field (this is required for nested/concurrent iteration)
	T* GetFirstNoCurrent()
	{
		return first;
	}

	// Get the last value without using current field (this is required for nested/concurrent iterations)
	T* GetLastNoCurrent()
	{
		return last;
	}

	// Get the next value
	T* GetNext()
	{
		current = nextl;

		// next is set in advance because current can be deleted after retrieval
		if(nextl)
			nextl = current->next;

		return current;
	}

	// Get the total number of items in the list
	int GetCount() { return count; }
};

#endif // sqlines_listt_h
