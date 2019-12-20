// SQLParser for SELECT statement

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// SELECT statement 
bool SqlParser::ParseSelectStatement(Token *select, int block_scope, int select_scope, int *result_sets,
										Token **list_end, ListW *exp_starts, ListW *out_cols, ListW *into_cols,
										int *appended_subquery_aliases, Token **from_end_out, Token **where_end_out)
{
    if(select == nullptr)
		return false;

	STATS_DECL

    Token *from = nullptr;
    Token *from_end = nullptr;

	ListWM from_table_end;

    Token *where_ = nullptr;
    Token *where_end = nullptr;

    Token *order = nullptr;

	int rowlimit = 0;

	// Row limit specified in SELECT list and SELECT options
    Token *rowlimit_slist = nullptr;
    Token *rowlimit_soptions = nullptr;
	
	bool rowlimit_percent = false;

	bool into = false;
	bool dummy_not_required = false;
	bool agg_func = false;
	bool agg_list_func = false;

    // Number of generated aliases for subqueries, start counter here if outer counter is NULL
	int app_subq_aliases = 0;

    if(appended_subquery_aliases != nullptr)
		app_subq_aliases = *appended_subquery_aliases;

	Enter(SQL_SCOPE_SELECT_STMT);

	// Check for common table expression CTE
	if(select->Compare("WITH", L"WITH", 4) == true)
	{
		if(ParseSelectCteClause(select) == true)
			GetNextWordToken("SELECT", L"SELECT", 6);
	}

	ListW out_cols_internal;

	// Fill list of columns for further use even if it is not requested by caller
    if(out_cols == nullptr)
		out_cols = &out_cols_internal;

	ParseSelectList(select, select_scope, &into, &dummy_not_required, &agg_func, &agg_list_func, 
		exp_starts, out_cols, into_cols, &rowlimit_slist, &rowlimit_percent);

    Token *select_list_end = GetLastToken();

    if(list_end != nullptr)
		*list_end = select_list_end;

	// FROM
	ParseSelectFromClause(select, false, &from, &from_end, &app_subq_aliases, dummy_not_required, &from_table_end);

	// Resolve data types for columns in select list
	SelectSetOutColsDataTypes(out_cols, &from_table_end);

	// WHERE
	ParseWhereClause(SQL_STMT_SELECT, &where_, &where_end, &rowlimit);

	// GROUP BY
	ParseSelectGroupBy();

	// HAVING clause
	ParseSelectHaving();

	// UNION ALL i.e. must go before ORDER BY and options that belong to the entire SELECT
	ParseSelectSetOperator(block_scope, select_scope);

	// Other clauses belong to the entire SELECT, not individual SELECT in set operators
	if(select_scope == SQL_SEL_SET_UNION)
		return true;

	Token *pre_order = GetLastToken();

	// ORDER BY
	ParseSelectOrderBy(&order);

	// Add row limitation that was defined in WHERE clause (rownum in Oracle)
	if(rowlimit > 0)
	{
        /*Token *last */ GetLastToken();
	}

	ParseSelectOptions(select, from_end, where_, order, &rowlimit_soptions, &rowlimit);

	// No assignment and it is a result set for outer SELECT in a procedure
    if(into == false && select_scope == 0 && block_scope == SQL_BLOCK_PROC)
	{
		// For Oracle add OPEN cur FOR

        TokenStr *cursor = new TokenStr();

        GenerateResultSetCursorName(cursor);

        Prepend(select, "OPEN ", L"OPEN ", 5);
        PrependNoFormat(select, cursor);
        Prepend(select, " FOR ", L" FOR ", 5);

        _spl_result_set_generated_cursors.Add(cursor);

		_spl_result_sets++;

        if(result_sets != nullptr)
			*result_sets = (*result_sets) + 1;
	}
	else 
	// Assignment and without FROM
    if(into == true && from == nullptr)
	{
        Token::Change(select, "SET", L"SET", 3);
	}

	// Convert row limits
    if(rowlimit_slist != nullptr || rowlimit_soptions != nullptr || rowlimit > 0)
		SelectConvertRowlimit(select, from, from_end, where_, where_end, pre_order, order, rowlimit_slist, rowlimit_soptions, rowlimit, rowlimit_percent);

	// Implement CONTINUE handler for NOT FOUND in Oracle
    if(into == true && agg_func == false)
		OracleContinueHandlerForSelectInto(select);

	if(select_scope == 0)
	{
		STATS_SET_DESC(SQL_STMT_SELECT_DESC)
        STMS_STATS(select)
	}

	Leave(SQL_SCOPE_SELECT_STMT);

    if(appended_subquery_aliases != nullptr)
		*appended_subquery_aliases = app_subq_aliases;

    if(from_end_out != nullptr)
		*from_end_out = from_end;

    if(where_end_out != nullptr)
		*where_end_out = where_end;

	return true;
}

// Parse subquery
bool SqlParser::ParseSubSelect(Token *open, int select_scope)
{
    if(open == nullptr)
		return false;

	bool exists = false;

	Token *next = GetNextToken();

    if(next == nullptr)
		return false;

	// One more level - ((SELECT ...) EXCEPT (SELECT ...))
	if(next->Compare('(', L'(') == true)
	{
		exists = ParseSubSelect(next, select_scope);

		// Make sure it was SELECT since ((t1 JOIN ... is also possible
		if(exists == true)
		{
			// Nested )
			/*Token *close */ (void) GetNextCharToken(')', L')');

			// (SELECT ...) EXCEPT SELECT is possible
			ParseSelectSetOperator(0, 0);
		}
	}
	else
	// SELECT statement
	if(next->Compare("SELECT", L"SELECT", 6) == true)
	{
        exists = ParseSelectStatement(next, 0, select_scope, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
	}

	// Not a select statement
	if(exists == false)
		PushBack(next);

	return exists; 
}

// Returns SELECT or WITH if it is the next input token
Token* SqlParser::GetNextSelectStartKeyword()
{
	Token *select = GetNext("SELECT", L"SELECT", 6);

    if(select != nullptr)
		return select;

	return GetNext("WITH", L"WITH", 4);
}

// Common table expression
bool SqlParser::ParseSelectCteClause(Token *with)
{
    if(with == nullptr)
		return false;

	// Comma-separated list of table expressions
	while(true)
	{
		Token *table = GetNextToken();

        if(table == nullptr)
			break;

		/*Token *open */ (void) GetNextCharToken('(', L'(');

		// Comma-separated list of colums
		while(true)
		{
			Token *column = GetNextToken();

            if(column == nullptr)
				break;

			Token *comma = GetNextCharToken(',', L',');

            if(comma == nullptr)
				break;
		}

		/*Token *close */ (void) GetNextCharToken(')', L')');

		// AS keyword
		/*Token *as */ (void) GetNextWordToken("AS", L"AS", 2);

		/*Token *open2 */ (void) GetNextCharToken('(', L'(');

		// SELECT statement
		Token *select = GetNextSelectStartKeyword();

        if(select == nullptr)
			break;

        ParseSelectStatement(select, 0, SQL_SEL_WITH_CTE, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

		/*Token *close2 */ (void) GetNextCharToken(')', L')');

		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	return true;
}

// Select list
bool SqlParser::ParseSelectList(Token *select, int select_scope, bool *select_into, bool *dummy_not_required,
								bool *agg_func, bool *agg_list_func, 
								ListW *exp_starts, ListW *out_cols, ListW *into_cols, 
								Token **rowlimit_slist, bool *rowlimit_percent)
{
	bool into_exists = false;
	bool dummy_not_req = false;
	bool agg_func_exists = false;
	bool agg_list_func_exists = false;

	// Parse predicate DISTINCT, TOP, FIRST etc.
	ParseSelectListPredicate(rowlimit_slist, rowlimit_percent);

	ListW cols;
	ListWM tsql_assignments;


	while(true)
	{
		// Column, expression, (subquery), (subquery) + exp and other complex expressions can be specified  
		Token *first = GetNextToken();

        if(first == nullptr)
			break;

		// Add the first position to the list
		cols.Add(first);

        if(exp_starts != nullptr)
			exp_starts->Add(first);

		ParseExpression(first);

		Token *first_end = GetLastToken();

		// Standalone column name 
		if(first == first_end && first->type == TOKEN_IDENT)
			first->subtype = TOKEN_SUB_COLUMN_NAME;

		// Check for an aggregate function
		if(IsAggregateFunction(first) == true)
			agg_func_exists = true;

		// Check for LIST aggregate function
		if(IsListAggregateFunction(first) == true)
			agg_list_func_exists = true;
		
		// Token following the expression
		Token *second = GetNextToken();

		// AS keyword between column and alias
		if(Token::Compare(second, "AS", L"AS", 2) == true)
			second = GetNextToken();

        if(second == nullptr)
			break;

		// If comma follows the expression parse next
		if(second->Compare(',', L',') == true)
		{
			// No alias specified, so the result set name is equal to column name
            if(out_cols != nullptr)
				out_cols->Add(first);

			continue;
		}

		// If INTO or FROM keyword, invalid alias or ; is next, end the select list
		if(second->Compare("INTO", L"INTO", 4) == true || second->Compare("FROM", L"FROM", 4) == true || 
			second->Compare(';', L';') == true || IsValidAlias(second) == false)
		{
			// No alias specified, so the result set name is equal to column name
            if(out_cols != nullptr)
				out_cols->Add(first);

			PushBack(second);
			break;
		}

		// Assign alias as output column
        if(out_cols != nullptr)
			out_cols->Add(second);

		// It was an alias, now must be a comma
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	Token *into = GetNextWordToken("INTO", L"INTO", 4);

	bool into_temp_table = false;

	// Parse into clause
    if(into != nullptr && !into_temp_table)
	{
		ListwItem *cur_select = cols.GetFirst();

		// Process each variable
		while(true)
		{
			Token *name = GetNextToken();

            if(name == nullptr)
				break;

            if(into_cols != nullptr)
				into_cols->Add(name);

            if(cur_select != nullptr)
				cur_select = cur_select->next;

			Token *comma = GetNextCharToken(',', L',');

            if(comma == nullptr || cur_select == nullptr)
				break;
		}

		Token *last = GetLastToken();

        // Remove INTO clause when converted to Oracle FOR
        if(select_scope == SQL_SEL_FOREACH)
			Token::Remove(into, last);

		into_exists = true;
	}

    if(select_into != nullptr)
		*select_into = into_exists;

    if(dummy_not_required != nullptr)
		*dummy_not_required = dummy_not_req;

    if(agg_func != nullptr)
		*agg_func = agg_func_exists;

    if(agg_list_func != nullptr)
		*agg_list_func = agg_list_func_exists;

	return true;
}

// Parse predicate DISTINCT, TOP, FIRST etc.
bool SqlParser::ParseSelectListPredicate(Token **rowlimit_slist, bool *rowlimit_percent)
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// DISTINCT in all databases
		if(next->Compare("DISTINCT", L"DISTINCT", 8) == true)
		{
			exists = true;
			continue;
		}

		// Not a predicate
		PushBack(next);
		break;
	}

	return exists;
}

// FROM clause
bool SqlParser::ParseSelectFromClause(Token *select, bool nested_from, Token **from_out, Token **from_end, 
									int *appended_subquery_aliases, bool dummy_not_required, ListWM *from_table_end)
{
    Token *from = nullptr;

	if(nested_from == false)
	{
		from = GetNextWordToken("FROM", L"FROM", 4);

		// FROM clause is absent
        if(from == nullptr)
		{
			Token *last = GetLastToken();

			if(dummy_not_required == false)
			{

				// For Oracle add FROM dual
                Append(last, " FROM ", L" FROM ", 6, select);
                AppendNoFormat(last, "dual", L"dual", 4);

			}

			return true;
		}
	}

	bool dummy_exists = false;

	int count = 0;

	while(true)
	{
		bool subquery = false;

		// Subquery or nested join
		Token *first = GetNextCharToken('(', L'(');
		
        if(first != nullptr)
		{
			bool subselect_exists = ParseSubSelect(first, SQL_SEL_FROM);

			// Parse nested join condition
			if(subselect_exists == false)
                ParseSelectFromClause(select, true, nullptr, nullptr, nullptr, false, nullptr);

			Token *close = GetNextCharToken(')', L')');

            if(close != nullptr)
				subquery = true;
		}
		// Table name
		else
		{
			// TABLE () table function
			Token *table_func = GetNextWordToken("TABLE", L"TABLE", 5);

            if(table_func != nullptr)
			{
				/*Token *open */ (void) GetNextCharToken('(', L'(');

				// Parse the table function
				ParseExpression(GetNextToken());

				/*Token *close */ (void) GetNextCharToken(')', L')');
			}
			else
				// Table name
				first = GetNextIdentToken(SQL_IDENT_OBJECT);

            if(first == nullptr && table_func == nullptr)
				break;

            if(first != nullptr)
			{
				// Check for Oracle dual
				if(first->Compare("dual", L"dual", 4) == true)
				{
					dummy_exists = true;
				}
            }
		}

        /*Token *first_end */(void) GetLastToken();

		// Token following the table name (candidate for alias)
		Token *second = GetNextToken();

		// AS keyword between table name and alias
		if(Token::Compare(second, "AS", L"AS", 2) == true)
			second = GetNextToken();

        if(second == nullptr)
		{
			count++;
			break;
		}

		// If comma follows the expression parse next
		if(second->Compare(',', L',') == true)
        {
			count++;
			continue;
		}

		// Check if the next is end of statement, subquery, or alias (not GO, WHERE, ORDER etc.)
		if(second->Compare(';', L';') == true || second->Compare(')', L')') == true || 
			IsValidAlias(second) == false)
        {
			count++;

			// Table name without alias
            if(from_table_end != nullptr)
				from_table_end->Add(first, first);

			PushBack(second);
			break;
		}

		// Second can point to alias or join keyword
		ParseJoinClause(first, second, subquery, from_table_end);

		count++;

		// Alias exists so comma must follow, otherwise FROM clause ended no matter what goes next
        if(GetNextCharToken(',', L',') == nullptr)
			break;
	}

	// Delete FROM if only dummy table is used
	if(count == 1 && dummy_exists == true)
	{
		Token *last = GetLastToken();

		// MySQL supports Oracle's dual
        Token::Remove(from, last);
	}

    if(from_out != nullptr)
		*from_out = from;

    if(from_end != nullptr)
		*from_end = GetLastToken();

	return true;
}

// Join clause in FROM clause of SELECT statement 
bool SqlParser::ParseJoinClause(Token *first, Token *second, bool first_is_subquery, ListWM *from_table_end)
{
    if(second == nullptr)
		return false;

	bool exists = false;

    Token *left_right_full = nullptr;
    Token *outer_inner = nullptr;
    Token *join = nullptr;
    Token *on = nullptr;

	exists = GetJoinKeywords(second, &left_right_full, &outer_inner, &join);

	// Second points to alias, try to get the first join keyword
	if(exists == false)
	{
        if(!first_is_subquery && from_table_end != nullptr)
			from_table_end->Add(second, first);

		Token *token = GetNext();

		exists = GetJoinKeywords(token, &left_right_full, &outer_inner, &join);

		if(!exists)
			PushBack(token);
	}

    if(join == nullptr)
		return false;

	// Second table name, or (SELECT ...)
	Token *third = GetNextToken();

    if(third == nullptr)
		return false;

	// Check for nested SELECT
	if(third->Compare('(', L'(') == true)
	{
		Token *select = GetNextSelectStartKeyword();

        if(select != nullptr)
            ParseSelectStatement(select, 0, SQL_SEL_JOIN, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);

		/*Token *close */ (void) GetNextCharToken(')', ')');
	}

    Token *fourth = nullptr;

	// If ON does not follows, it means optional alias for the second table in join
	on = GetNextWordToken("ON", L"ON", 2);

    if(on == nullptr)
	{
		// Alias
		fourth = GetNextIdentToken();

		on = GetNextWordToken("ON", L"ON", 2);
	}

	// Parse join condition
    if(on != nullptr)
        ParseBooleanExpression(SQL_BOOL_JOIN_ON, nullptr, nullptr);

	// List of join conditions can be specified JOIN ON ... JOIN ON ...
	Token *next = GetNextToken();

    if(ParseJoinClause(nullptr, next, false, nullptr) == false)
		PushBack(next);

	return true;
}

// Get JOIN clause keywords
bool SqlParser::GetJoinKeywords(Token *token, Token **left_right_full, Token **outer_inner, Token **join)
{
	bool exists = false;

	// LEFT | RIGHT | FULL [OUTER] JOIN
	if(TOKEN_CMP(token, "LEFT") || TOKEN_CMP(token, "RIGHT") || TOKEN_CMP(token, "FULL"))
	{
        if(left_right_full != nullptr)
			*left_right_full = token;

		Token *outer = TOKEN_GETNEXTW("OUTER");

        if(outer != nullptr && outer_inner != nullptr)
			*outer_inner = outer;

		Token *jn = TOKEN_GETNEXTW("JOIN");

        if(jn != nullptr && join != nullptr)
			*join = jn;

		exists = true;
	}
	else
	// INNER | OUTER JOIN
	if(TOKEN_CMP(token, "INNER") || TOKEN_CMP(token, "OUTER"))
	{
        if(outer_inner != nullptr)
			*outer_inner = token;

		Token *jn = TOKEN_GETNEXTW("JOIN");

        if(jn != nullptr && join != nullptr)
			*join = jn;

		exists = true;
	}
	else
	// JOIN
	if(TOKEN_CMP(token, "JOIN"))
	{
        if(join != nullptr)
			*join = token;

		exists = true;
	}

	return exists;
}

// WHERE clause in SELECT statement
bool SqlParser::ParseWhereClause(int stmt_scope, Token **where_out, Token **where_end_out, int *rowlimit)
{
	Token *where_ = GetNextWordToken("WHERE", L"WHERE", 5);

    if(where_ == nullptr)
		return false;

    if(where_out != nullptr)
		*where_out = where_;

	// WHERE CURRENT OF cursor
	if(ParseWhereCurrentOfCursor(stmt_scope) == true)
		return true;

	int count = 0;

	ParseBooleanExpression(SQL_BOOL_WHERE, where_, &count, rowlimit);

	// If Oracle rownum condition is set, and there are no conditions anymore remove WHERE
    if(rowlimit != nullptr && *rowlimit > 0 && count == 0)
		Token::Remove(where_);

    if(where_end_out != nullptr)
		*where_end_out = GetLastToken();
	
	return true;
}

// Oracle WHERE CURRENT OF cursor
bool SqlParser::ParseWhereCurrentOfCursor(int stmt_scope)
{
	Token *current = GetNextWordToken("CURRENT", L"CURRENT", 7);

    if(current == nullptr)
		return false;

	Token *of = GetNextWordToken("OF", L"OF", 2);
    Token *cursor = nullptr;

    if(of != nullptr)
		cursor = GetNextIdentToken();

    if(cursor == nullptr)
	{
		PushBack(current);
		return false;
	}

	// Save updatable cursor
	if(stmt_scope == SQL_STMT_UPDATE || stmt_scope == SQL_STMT_DELETE)
		_spl_updatable_current_of_cursors.Add(cursor);

	return true;
}

// GROUP BY clause in SELECT statement
bool SqlParser::ParseSelectGroupBy()
{
	Token *group = GetNextWordToken("GROUP", L"GROUP", 5);

    if(group == nullptr)
		return false;

	Token *by = GetNextWordToken("BY", L"BY", 2);

    if(by == nullptr)
		return false;

	// Parse group expressions
	while(true)
	{
		Token *exp = GetNextToken();

        if(exp == nullptr)
			break;

		// An GROUP BY expression 
		ParseExpression(exp);

		// Must be comma if next expression exists
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	return true;
}

// HAVING clause
bool SqlParser::ParseSelectHaving()
{
	Token *having = GetNextWordToken("HAVING", L"HAVING", 6);

    if(having == nullptr)
		return false;

	ParseBooleanExpression(SQL_BOOL_HAVING);

	return true;
}

// ORDER BY clause in SELECT statement
bool SqlParser::ParseSelectOrderBy(Token **order_out)
{
	Token *order = GetNextWordToken("ORDER", L"ORDER", 5);

    if(order == nullptr)
		return false;

	Token *by = GetNextWordToken("BY", L"BY", 2);

    if(by == nullptr)
		return false;

	// Parse order expressions
	while(true)
	{
		Token *exp = GetNextToken();

        if(exp == nullptr)
			break;

		// An ORDER expression 
		ParseExpression(exp);

		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// ASC or DESC
		if(next->Compare("ASC", L"ASC", 3) == true || next->Compare("DESC", L"DESC", 4) == true)
			next = GetNextToken();

		// Must be comma
		if(Token::Compare(next, ',', L',') == false)
		{
			PushBack(next);
			break;
		}
	}

    if(order_out != nullptr)
		*order_out = order;

	return true;
}

// UNION, UNION ALL, EXCEPT and MINUS operators
bool SqlParser::ParseSelectSetOperator(int block_scope, int /*select_scope*/)
{
	bool exists = false;

	while(true)
	{
		Token *op = GetNextToken();

        if(op == nullptr)
			break;

		// UNION and UNION ALL
		if(op->Compare("UNION", L"UNION", 5) == true)
		{
			/*Token *all */ (void) GetNextWordToken("ALL", L"ALL", 3);

			exists = true;
			break;
		}
        else
		// MINUS in Oracle
		if(op->Compare("MINUS", L"MINUS", 5) == true)
		{
			exists = true;
			break;
		}

		PushBack(op);
		break;
	}

	// If a set operator exists next must a SELECT statement
	if(exists == true)
	{
		// SELECT or (SELECT ...) can be specified
		Token *open = GetNextCharToken('(', L'(');
		
        if(open != nullptr)
		{
			ParseSubSelect(open, SQL_SEL_SET_UNION);

			/*Token *close */ (void) GetNextCharToken(')', L')');
		}
		else
		{
			Token *select = GetNextWordToken("SELECT", L"SELECT", 6);

            if(select != nullptr)
                ParseSelectStatement(select, block_scope, SQL_SEL_SET_UNION, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		}
	}

	return exists;
}

// SELECT statements options at the end of the statement
bool SqlParser::ParseSelectOptions(Token * /*select*/, Token * /*from_end*/, Token * /*where_*/, Token * /*order*/, 
									Token **rowlimit_soptions, int *rowlimit)
{
	bool exists = false;

	while(true)
	{
		Token *option = GetNextToken();

        if(option == nullptr)
			break;

        // Oracle, MySQL FOR UPDATE
		if(option->Compare("FOR", L"FOR", 3) == true)
		{
			Token *update = GetNextWordToken("UPDATE", L"UPDATE", 6);
            Token *read = nullptr;

            if(update == nullptr)
				read = GetNextWordToken("READ", L"READ", 4);

            if(update != nullptr)
			{
				// OF column, ... clause
				Token *of = GetNextWordToken("OF", L"OF", 2);

                while(of != nullptr)
				{
					Token *col = GetNextIdentToken();

                    if(col == nullptr)
						break;

					Token *comma = GetNextCharToken(',', L',');

                    if(comma == nullptr)
						break;
				}

				// Oracle SKIP LOCKED
				Token *skip = GetNextWordToken("SKIP", L"SKIP", 4);
                Token *locked = nullptr;

                if(skip != nullptr)
					locked = GetNextWordToken("LOCKED", L"LOCKED", 6);

				// MySQL does not support SKIP LOCKED, comment it
                if(skip != nullptr && locked != nullptr)
					Comment(skip, locked);
			}
			else
            if(read != nullptr)
			{
				Token *only = GetNextWordToken("ONLY", L"ONLY", 4);

                if(only != nullptr)
					Token::Remove(option, only);
			}			

			exists = true;
			continue;
		}
        else
		// MySQL LIMIT num option
		if(option->Compare("LIMIT", L"LIMIT", 5) == true)
		{
			Token *num = GetNextToken();

			// ROWNUM is used in Oracle
            if(num != nullptr)
				Token::Remove(option, num);

            if(rowlimit_soptions != nullptr)
				*rowlimit_soptions = num;

			exists = true;
			continue;
		}

		PushBack(option);
		break;
	}

	return exists;
}

// Resolve data types for columns in select list
void SqlParser::SelectSetOutColsDataTypes(ListW *out_cols, ListWM *from_table_end)
{
    if(out_cols == nullptr || from_table_end == nullptr)
		return;

	ListwItem *col_item = out_cols->GetFirst();

    while(col_item != nullptr)
	{
		Token *col = (Token*)col_item->value;

        if(col == nullptr)
			break;

		// Standalone column name
		if(col->subtype == TOKEN_SUB_COLUMN_NAME)
		{
			ListwmItem *from_item = from_table_end->GetFirst();

			// Find a table containing this column, and define its data type
            while(from_item != nullptr)
			{
				Token *table = (Token*)from_item->value2;

				col->datatype_meta = GetMetaType(table, col);

                if(col->datatype_meta != nullptr)
				{
					col->table = table;
					break;
				}

				from_item = from_item->next;
			}
		}
		else
		// All columns selected from single table
		if(TOKEN_CMPC(col, '*') && from_table_end->GetCount() == 1)
		{
			// Save the table name
			col->table = (Token*)from_table_end->GetFirst()->value2;
		}

		col_item = col_item->next;
	}
}

// Convert row limits specified in SELECT
void SqlParser::SelectConvertRowlimit(Token *select, Token *from, Token *from_end, Token *where_, 
										Token *where_end, Token *pre_order, Token *order, 
										Token *rowlimit_slist, Token *rowlimit_soptions, int rowlimit, 
										bool rowlimit_percent)
{
	// One of limits should be set
    if(rowlimit_slist == nullptr && rowlimit_soptions == nullptr && rowlimit == 0)
		return;

	int limit = rowlimit;

    if(rowlimit_slist != nullptr)
		limit = rowlimit_slist->GetInt();
	else
    if(rowlimit_soptions != nullptr)
		limit = rowlimit_soptions->GetInt();

	// 100 percent
	if(limit == 100 && rowlimit_percent == true)
		return;

	Token *end = GetLastToken();

	// ROWNUM in Oracle
    // No ORDER BY, just add a condition to WHERE
    if(order == nullptr)
    {
        // No WHERE clause too
        if(where_ == nullptr)
            end = Append(from_end, " WHERE ", L" WHERE ", 7, select);
        else
            end = Append(where_end, " AND ", L" AND ", 5, where_);
    }
    // ORDER BY exists so a subquery must be used
    else
    {
        Prepend(select, "SELECT * FROM (", L"SELECT * FROM (", 15);
        Append(end, ") WHERE ", L") WHERE ", 8, select);
    }

    AppendNoFormat(end, "rownum <= ", L"rownum <= ", 10);

    if(rowlimit_slist != nullptr || rowlimit_soptions != nullptr)
        AppendCopy(end, Nvl(rowlimit_slist, rowlimit_soptions));
    else
        Append(end, rowlimit);

    // Row limit in percent
    if(rowlimit_percent == true)
    {
        Append(end, "/100 * ", L"/100 * ", 7);
        Append(end, "\n (SELECT COUNT(*) ", L"\n (SELECT COUNT(*) ", 19, select);

        // Copy FROM, WHERE, UNION, GROUP BY, HAVING COUNT, but not ORDER BY
        AppendCopy(end, from, pre_order);

        Append(end, ")", L")", 1);
    }
}
