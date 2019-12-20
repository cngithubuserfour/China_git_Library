// Post processing

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Post conversion when all tokens processed
void SqlParser::Post()
{
	Book *bookmark = _bookmarks.GetFirstNoCurrent();

	// Iterate through bookmarks
    while(bookmark != nullptr)
	{
		bookmark = bookmark->next;
	}
}
