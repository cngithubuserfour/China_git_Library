// SQLParser for language elements

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "str.h"

// Convert an identifier
void SqlParser::ConvertIdentifier(Token *token, int type, int scope)
{
    if(token == nullptr || token->len == 0)
		return;
	
	token->type = TOKEN_IDENT;

	// Check for NEW or OLD column reference in trigger
	if(ConvertTriggerNewOldColumn(token) == true)
		return;

	// Convert table, view, procedure, function etc. name
	if(type == SQL_IDENT_OBJECT)
	{
		ConvertObjectIdentifier(token, scope);
		return;
	}
	else
	// Convert a parameter 
	if(type == SQL_IDENT_PARAM)
	{
		ConvertParameterIdentifier(token);
		return;
	}
	else
	// Convert a local variable
	if(type == SQL_IDENT_VAR)
	{
		ConvertVariableIdentifier(token);
		return;
	}

	// Check for a cursor parameter reference NOW PROCESSED AS LOCAL BLOCK VAR
	//if(_spl_current_declaring_cursor != NULL && ConvertCursorParameter(token) == true)
	//	return;

	// Check for record variable reference
	if(ConvertRecordVariable(token) == true)
		return;

	// Check for implicit record variable reference
	if(ConvertImplicitRecordVariable(token))
		return;

	// Check for implicit field reference (without record name)
	if(ConvertImplicitForRecordVariable(token) == true)
		return;

	// Check for local temporary table column reference
	if(ConvertLocalTableColumn(token) == true)
		return;

	// Convert column name
	if(type == SQL_IDENT_COLUMN || type == SQL_IDENT_COLUMN_SINGLE)
	{
		ConvertColumnIdentifier(token, scope);
		return;
	}


	else
	{
	// OLD CODE BELOW

	const char *cur = token->str;

    if(cur == nullptr)
		return;
	// Oracle pseudo-column (nextval i.e)
	if(ConvertOraclePseudoColumn(token) == true)
		return;


	else
	// Change quote to backtick for MySQL (" is not suppored by default in MySQL)

	if(*cur == '"' || *cur == '[')
	{
		char *ident = new char[token->len + 1];

		*ident = '`';

		char open = *cur;
		char close = (*cur == '[') ? ']' : *cur;

		// Copy the value within quotes
		size_t i = 1;
		for(i = 1; i < token->len - 1; i++)
		{
			// Open or close quote is faced inside, can be qualified identifier
			if(cur[i] == open || cur[i] == close)
				ident[i] = '`';
			else
				ident[i] = cur[i];
		}

		ident[i] = '`';
		ident[i + 1] = 0;

		token->t_str = ident;
		token->t_len = token->len;
	}
			
	}		
}

// Convert table name
void SqlParser::ConvertObjectIdentifier(Token *token, int scope)
{
    if(token == nullptr)
		return;

	// Check for local table variable
	if(ConvertTableVariable(token) == true)
		return;

	// Check for session temporary table
	if(ConvertSessionTemporaryTable(token) == true)
		return;

	// Check if object name mapping is set
	if(MapObjectName(token) == true)
		return;

	TokenStr ident;
	size_t len = 0;

	// Package function or procedure, add a prefix
    if(_spl_package != nullptr && (scope == SQL_SCOPE_FUNC || scope == SQL_SCOPE_PROC))
		PrefixPackageName(ident);

	// Get the number of parts in quailified identifier
	int parts = GetIdentPartsCount(token);

	// Object contains a schema
	if(parts > 1)
		ConvertSchemaName(token, ident, &len);
	else
	// Set explicit schema if defined by the option
	if(!_option_set_explicit_schema.empty())
	{
		TokenStr schema;
		schema.Append(_option_set_explicit_schema.c_str(), L"", _option_set_explicit_schema.length());
		schema.Append(".", L".", 1);
		schema.Append(ident);
		ident.Set(schema);
	}

	ConvertObjectName(token, ident, &len);

	// If identifier was modified set the target value
	if(Token::Compare(token, &ident) == false)
		Token::ChangeNoFormat(token, ident);
}

// Convert column name
void SqlParser::ConvertColumnIdentifier(Token *token, int /*scope*/)
{
    if(token == nullptr)
		return;

	TokenStr ident;
	size_t len = 0;

	// Get the number of parts in quailified identifier
	int parts = GetIdentPartsCount(token);

	// Identifier contains a schema
	if(parts > 2)
		ConvertSchemaName(token, ident, &len);

	// Identifier contains a table
	if(parts > 1)
	{
		ConvertObjectName(token, ident, &len);

		ident.Append(token, len, 1);
		len = len + 1;
	}

	ConvertObjectName(token, ident, &len);

	// If identifier was modified set the target value
	if(Token::Compare(token, &ident) == false)
		Token::ChangeNoFormat(token, ident);
}

// Convert single (not qualified) column identifier
void SqlParser::ConvertColumnIdentifierSingle(Token *token, int /*scope*/)
{
    if(token == nullptr || token->str == nullptr)
		return;

	TokenStr ident;
    bool changed = false;

    for(size_t i = 0; i < token->len; i++)
    {
        ident.Append(token, i, 1);
    }

    if(changed)
        Token::ChangeNoFormat(token, ident); 
}

// Oracle PL/SQL cursor parameter reference NOW PROCESSED AS LOCAL BLOCK VAR
bool SqlParser::ConvertCursorParameter(Token *token)
{
    if(token == nullptr || _spl_current_declaring_cursor == nullptr)
		return false;

	bool exists = false;

	// Use variables instead of cursor parameters 
	if(_spl_cursor_params.GetCount() > 0)
	{
        for(ListwmItem *i = _spl_cursor_params.GetFirst(); i != nullptr; i = i->next)
		{
			Token *cursor_name = (Token*)i->value2;

			if(Token::Compare(cursor_name, _spl_current_declaring_cursor) == true)
			{
				Token *param = (Token*)i->value3;

				if(Token::Compare(param, token) == true)
				{
					// The target value of parameter already contains variable name
					Token::Change(token, param);

					exists = true;
					break;
				}
			}			
		}
	}

	return exists;
}

// %ROWTYPE record variable reference rec.field 
bool SqlParser::ConvertRecordVariable(Token *token)
{
    if(token == nullptr)
		return false;

	bool exists = false;

	ListwItem *item = _spl_rowtype_vars.GetFirst();

	// Compare identifier prefix with existing record name
    while(item != nullptr)
	{
		Token *rec = (Token*)item->value;

		// Check for rec. prefix
        if(rec != nullptr && Token::Compare(rec, token, rec->len) == true &&
			token->Compare(".", L".", rec->len, 1) == true)
		{
			// Change to rec_field in MySQL
			TokenStr ident;

			// Append record name _ and field name 
			ident.Append(token, 0, rec->len);
			ident.Append("_", L"_", 1);
			ident.Append(token, rec->len + 1, token->len - rec->len - 1);

			Token::ChangeNoFormat(token, ident);


			// Save referenced record fields (once only)
            if(Find(_spl_rowtype_fields, rec, token) == nullptr)
				_spl_rowtype_fields.Add(rec, token);

			exists = true;
			break;
		}

		item = item->next;
	}

	return exists;
}

// Implicit record (cursor name) variable reference rec.field 
bool SqlParser::ConvertImplicitRecordVariable(Token *token)
{
    if(token == nullptr)
		return false;

	bool exists = false;

	ListwmItem *item = _spl_implicit_rowtype_vars.GetFirst();

	// Compare identifier prefix with existing record name
    while(item != nullptr)
	{
		exists = ConvertImplicitRecordVariable(token, (Token*)item->value);

		if(exists)
			break;

		item = item->next;
	}

	return exists;
}

bool SqlParser::ConvertImplicitRecordVariable(Token *token, Token *rec)
{
	// Check for rec. prefix
    if(rec != nullptr && Token::Compare(rec, token, rec->len) && token->Compare(".", L".", rec->len, 1))
	{
		// Change to rec_field in MySQL
		TokenStr ident;

		// Append record name _ and field name 
		ident.Append(token, 0, rec->len);
		ident.Append("_", L"_", 1);
		ident.Append(token, rec->len + 1, token->len - rec->len - 1);

		Token::ChangeNoFormat(token, ident);

		// Save referenced record fields (once only)
        if(Find(_spl_implicit_rowtype_fields, rec, token) == nullptr)
			_spl_implicit_rowtype_fields.Add(rec, token);

		return true;
	}

	return false;
}

// Check for implicit field reference (without record name)
bool SqlParser::ConvertImplicitForRecordVariable(Token *token)
{

	bool exists = false;

    ListwmItem *i = Find(_spl_implicit_record_fields, nullptr, token);

	// Field found
    if(i != nullptr && i->value != nullptr && i->value2 != nullptr)
	{
		Token *rec = (Token*)i->value;

		// Make sure name is not qualified
		if(Token::Compare(token, rec, rec->len) == false)
		{
			// There can be conflicts with column names, check the current scope
			if(IsScope(SQL_SCOPE_INSERT_VALUES) == true ||
				// Assignment but not SET var = (SELECT ...)
				(IsScope(SQL_SCOPE_ASSIGNMENT_RIGHT_SIDE) == true && IsScope(SQL_SCOPE_SELECT_STMT) == false))
			{
				TokenStr new_name(rec);
				new_name.Append(".", L".", 1);
				new_name.Append(token);

				Token::ChangeNoFormat(token, new_name);
			}
		}

		exists = true;
	}

	return exists;
}

// Check for NEW or OLD column reference in trigger
bool SqlParser::ConvertTriggerNewOldColumn(Token *token)
{
    if(token == nullptr || _spl_scope != SQL_SCOPE_TRIGGER)
		return false;

	bool exists = false;

    if(_spl_old_correlation_name == nullptr && Token::Compare(token, "OLD", L"OLD", 1, 3) == true &&
			token->Compare(".", L".", 4, 1) == true)
	{

		// Save referenced old columns (once only)
        if(Find(_spl_tr_old_columns, token) == nullptr)
			_spl_tr_old_columns.Add(token);

		exists = true;
	}		

	return exists;
}

// Convert declared local temporary table
bool SqlParser::ConvertTableVariable(Token *token)
{
    if(token == nullptr)
		return false;

	bool exists = false;

	ListwItem *item = _spl_declared_local_tables.GetFirst();

	// Compare identifier with existing declared local table (table variable)
    while(item != nullptr)
	{
		Token *table = (Token*)item->value;

        if(table != nullptr && Token::Compare(table, token) == true)
		{
			// If the variable was changed, set new name to all references
			if(table->IsTargetSet() == true)
				Token::Change(token, table);	

			exists = true;
			break;
		}

		item = item->next;
	}

	return exists;
}

// Check for session temporary table
bool SqlParser::ConvertSessionTemporaryTable(Token *token)
{
    if(token == nullptr)
		return false;

	bool exists = false;

	return exists;
}

// Check for local temporary table column reference
bool SqlParser::ConvertLocalTableColumn(Token *token)
{
    if(token == nullptr)
		return false;

	bool exists = false;

	ListwItem *item = _spl_declared_local_tables.GetFirst();

	// Compare identifier prefix with existing local temporary name
    while(item != nullptr)
	{
		Token *table = (Token*)item->value;

		// Check for table. prefix
        if(table != nullptr && Token::Compare(table, token, table->len) == true &&
			token->Compare(".", L".", table->len, 1) == true)
		{
			// If the table variable was changed, set new name to all references
			if(table->IsTargetSet() == true)
			{
				TokenStr ident(table);

				// Append referenced column name
				ident.Append(token, table->len, token->len - table->len);

				Token::ChangeNoFormat(token, ident);
			}

			exists = true;
			break;
		}

		item = item->next;
	}

	return exists;
}

// Convert a procedure, function parameter
void SqlParser::ConvertParameterIdentifier(Token *token)
{
    if(token == nullptr && token->len > 0)
		return;

}

void SqlParser::ConvertParameterIdentifier(Token *ref, Token *decl)
{
    if(ref == nullptr || decl == nullptr || decl->t_len == 0)
		return;

	// Set value of the referenced parameter to the value changed at the declaration
	Token::Change(ref, decl);

	// Propagate attributes
	ref->type = decl->type;
	ref->data_type = decl->data_type;
	ref->data_subtype = decl->data_subtype;
}

// Convert a local variable
void SqlParser::ConvertVariableIdentifier(Token *token)
{
    if(token == nullptr && token->len > 0)
		return;

	// Check if a variable with this name already declared
	Token *var = GetVariable(token);

    if(var != nullptr && var->t_len != 0)
	{
		ConvertVariableIdentifier(token, var);
		return;
	}
	
}

void SqlParser::ConvertVariableIdentifier(Token *ref, Token *decl)
{
    if(ref == nullptr || decl == nullptr || decl->t_len == 0)
		return;

	// Set value of the referenced variable to the value changed at the declaration
	Token::Change(ref, decl);

	// Propagate attributes
	ref->type = decl->type;
	ref->data_type = decl->data_type;
	ref->data_subtype = decl->data_subtype;
}

// Oracle pseudo-column (nextval i.e.)
bool SqlParser::ConvertOraclePseudoColumn(Token *token)
{
    if(token == nullptr)
		return false;

	CONV_STATS
	size_t len = token->len;

	// .nextval reference
	if(len > 8 && token->Compare(".nextval", L".nextval", len - 8, 8) == true)
	{
		// NextVal('seqname') user-defined function in MySQL
		Prepend(token, "NextVal('", L"NextVal('", 9); 
		Token::Change(token, token->str, token->wstr, len - 8);
		Append(token, "')", L"')", 2);
		
		token->subtype = TOKEN_SUB_IDENT_SEQNEXTVAL;

        CONV_DESC(SEQUENCE_NEXTVAL_DESC)
        SEQ_REF_STATS("seqname.NEXTVAL")
        SEQ_REF_DTL_STATS(token)
		return true;
	}
	else
	// .currval reference
	if(len > 8 && token->Compare(".currval", L".currval", len - 8, 8) == true)
    {
		// LastVal('seqname') user-defined function in MySQL
		Prepend(token, "LastVal('", L"LastVal('", 9); 
		Token::Change(token, token->str, token->wstr, len - 8);
		Append(token, "')", L"')", 2);

        CONV_DESC(SEQUENCE_CURRVAL_DESC)
        SEQ_REF_STATS("seqname.CURRVAL")
        SEQ_REF_DTL_STATS(token)
		return true;
	}

	return false;
}

// Column constraint or propertirs
bool SqlParser::ParseColumnConstraints(Token *column, Token *type, Token *type_end)
{
    if(column == nullptr)
		return false;

	bool exists = false;

	int num = 0;

    Token *null = nullptr;

	// Parse until , or )
	while(true)
	{
		Token *cns = GetNextToken();

        if(cns == nullptr)
			break;

		if(cns->Compare(',', L',') == true || cns->Compare(';', L';') == true || 
			cns->Compare(')', L')') == true)
		{
			PushBack(cns);
			break;
		}
		else
		// Inline named constraint can be specified in Oracle
		if(ParseInlineColumnConstraint(type, type_end, cns, num) == true)
		{
			num++;
		}
		else
		// NULL constraint
		if(cns->Compare("NULL", L"NULL", 4) == true)
		{
			null = cns;
			num++;
		}
		else
		// Check for NOT NULL constraint, NOT LOGGED/COMPACT
		if(cns->Compare("NOT", L"NOT", 3) == true)
		{
			Token *next = GetNextToken();

            if(TOKEN_CMP(next, "NULL") == true)
            {
                if(_obj_scope == SQL_SCOPE_TABLE)
                    CREATE_TAB_STMS_STATS("NOT NULL constraints")
            }

			num++;
		}
		else
		// Inline CHECK constraint
		if(cns->Compare("CHECK", L"CHECK", 5) == true)
		{
			ParseCheckConstraint(cns);
			num++;
		}
		// Oracle ENABLE contraint state
		else
		if(cns->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			// Remove for other databases
			Token::Remove(cns);

			num++;
        }
    }

	return exists;
}

// Data type attribute for variable or parameter UNSIGNED i.e
bool SqlParser::ParseVarDataTypeAttribute()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// Not an attribute
		PushBack(next);
		break;
	}

	return exists;
}

// DEFAULT
bool SqlParser::ParseDefaultExpression(Token *type, Token *token )
{
    if(token == nullptr)
		return false;
		
	Token *first = GetNextToken();

    if(first == nullptr)
		return false;

    Token *default_end = nullptr;

	ParseExpression(first);
	default_end = GetLastToken();

	Token *new_default = token;
	Token *new_default_first = first;

	// Oracle requires TO_TIMESTAMP around datetime literals i.e.
	FormatDefaultExpression(type, new_default_first, new_default);

	return true;
}
// Format DEFAULT expression
bool SqlParser::FormatDefaultExpression(Token *datatype, Token *exp, Token *default_)
{
    if(datatype == nullptr || exp == nullptr)
		return false;

	// Oracle requires TO_TIMESTAMP around datetime literals i.e.
	// A datetime source data type with default string literal
	if(datatype->data_type == TOKEN_DT_DATETIME && exp->type == TOKEN_STRING)
	{
		TokenStr format;

		if(RecognizeOracleDateFormat(exp, format) == true)
		{
			// Use TO_DATE for dates, and TO_TIMESTAMP for datetimes
			if(datatype->data_subtype == TOKEN_DT2_DATE)
				Prepend(exp, "TO_DATE(", L"TO_DATE(", 8, default_);
			else
				Prepend(exp, "TO_TIMESTAMP(", L"TO_TIMESTAMP(", 13, default_);

			AppendNoFormat(exp, ", ", L", ", 2);
			Append(exp, &format);
			AppendNoFormat(exp, ")", L")", 1);
		}
	}

	return true;
}

// Standalone column constraint
bool SqlParser::ParseStandaloneColumnConstraints(Token *alter, ListW &pkcols,
												 ListWM *inline_indexes)
{
	bool exists = false;

	// In Oracle constraint can be enclosed with () i.e. ALTER TABLE ADD (PRIMARY KEY (c1,c2))
	Token *open = GetNext('(', L'(');

    if(open != nullptr)
	{
		Token::Remove(open);
	}

	while(true)
	{
		bool exists_now = false;

		Token *cns = GetNextToken();

        if(cns == nullptr)
			break;

		// Check for constraint name
		if(cns->Compare("CONSTRAINT", L"CONSTRAINT", 10) == true)
		{
            /*Token *cns_name*/(void) GetNextIdentToken();

			// Now get constraint type keyword
			cns = GetNextToken();
		}

		// Parse PRIMARY KEY, UNIQUE in database and INDEX in MySQL 
		if(ParseKeyConstraint(alter, cns, pkcols, inline_indexes) == true)
			exists_now = true;
		else
		// Check for FOREIGN KEY
		if(ParseForeignKey(cns) == true)
			exists_now = true;
		else
		// CHECK constraint
		if(ParseCheckConstraint(cns) == true)
			exists_now = true;

		// Check for constraint option
		if(exists_now == true)
		{
			ParseConstraintOption();
			exists = true;

			continue;
		}
		
		// Not a constraint
		PushBack(cns);
		break;
	}

    if(open != nullptr)
	{
		Token *close = GetNext(')', L')');

        if(close != nullptr)
		{
			Token::Remove(close);
		}
	}


	return exists;
}

// Inline named constraint can be specified in Oracle
bool SqlParser::ParseInlineColumnConstraint(Token *type, Token *type_end, Token *constraint, int num)
{
	// CONSTRAINT keyword before the name
    if(constraint == nullptr || Token::Compare(constraint, "CONSTRAINT", L"CONSTRAINT", 10) == false)
		return false;

	// Constraint name
	Token *name = GetNextToken();

    if(name == nullptr)
		return false;

	// Constraint type keyword
	Token *cns = GetNextToken();

    if(cns == nullptr)
		return false;

	bool exists = false;

	ListW pkcols;

	// Check for FOREIGN KEY
	if(ParseForeignKey(cns) == true)
		exists = true;
	else
	// CHECK constraint
	if(ParseCheckConstraint(cns) == true)
		exists = true;
	else
	if(cns->Compare("DEFAULT", L"DEFAULT", 7) == true)
	{
		exists = ParseDefaultExpression(type, cns);
	}

	// Check for constraint option
	if(exists == true)
		ParseConstraintOption();

	return exists;
}

// Parse PRIMARY KEY, UNIQUE constraint clause, and INDEX/KEY in MySQL 
bool SqlParser::ParseKeyConstraint(Token * /*alter*/, Token *cns, ListW &pkcols,
                                    ListWM *inline_indexes)
{
    if(cns == nullptr)
		return false;

	bool primary = false;
	bool unique = false; 

	// MySQL UNIQUE INDEX clause
	bool unique_index = false;
	bool index = false;

    Token *index_name = nullptr;

	// PRIMARY KEY
	if(cns->Compare("PRIMARY", L"PRIMARY", 7) == true)
	{
		/*Token *key */ (void) GetNextWordToken("KEY", L"KEY", 3);
		primary = true;

        if(_stmt_scope == SQL_STMT_ALTER_TABLE)
            ALTER_TAB_STMS_STATS("Add PRIMARY KEY")
	}
	else
	// UNIQUE
	if(cns->Compare("UNIQUE", L"UNIQUE", 6) == true)
	{
		unique = true;

		// Check for MySQL UNIQUE KEY or UNIQUE INDEX
		Token *index = GetNextWordToken("INDEX", L"INDEX", 5);

        if(index == nullptr)
			index = GetNextWordToken("KEY", L"KEY", 3);

        if(index != nullptr)
		{
			unique_index = true;
			index_name = GetNextIdentToken();

		}

        if(_stmt_scope == SQL_STMT_ALTER_TABLE)
            ALTER_TAB_STMS_STATS("Add UNIQUE constraint")
	}
	else
	// MySQL INDEX or KEY for inline non-unique index
	if(cns->Compare("INDEX", L"INDEX", 5) == true || cns->Compare("KEY", L"KEY", 3) == true)
	{
		index = true;
		index_name = GetNextIdentToken();

	}
	else
		return false;

	// In SQL Server PRIMARY and UNIQUE key can be CLUSTERED or NONCLUSTERED
	if(primary == true || unique == true)
	{
		// CLUSTERED or NONCLUSTERED keyword can go for SQL Server
		Token *clustered = GetNext("CLUSTERED", L"CLUSTERED", 9);
        Token *nonclustered = nullptr;
		
        if(clustered == nullptr)
			nonclustered = GetNext("NONCLUSTERED", L"NONCLUSTERED", 12);

	}

	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

    Book *cr_table_end = nullptr;

	bool greenplum_distributed = false;


	int count = 0;

	while(true)
	{
		Token *col = GetNextIdentToken();

        if(col == nullptr)
			break;

		if(primary == true)
			pkcols.Add(col);

		// For Greenplum, add column to DISTRIBUTED BY in CREATE TABLE
		if(greenplum_distributed == true)
		{
			if(count > 0)
				Append(cr_table_end->book, ", ", L", ", 2);

			AppendCopy(cr_table_end->book, col);
		}

		count++;

		// For SQL Server PRIMARY KEY, for MySQL index next can be ASC DESC 
		Token *asc = GetNext("ASC", L"ASC", 3); 
        Token *desc = nullptr;

        if(asc == nullptr)
			desc = GetNext("DESC", L"DESC", 4);


		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		if(next->Compare(',', L',') == true)
			continue;

		// Leave if not comma
		PushBack(next);
		break;
	}

    /*Token *close =*/(void) GetNextCharToken(')', L')');

	// Parse index options
	ParseKeyIndexOptions();


	// Check for inline non-unique index in MySQL
    if(unique == false && index == true && inline_indexes != nullptr)
	{
		inline_indexes->Add(cns, GetLastToken());

		// Inline indexes will be moved, so delete comma at the end of previous column
		Token *prev = GetPrevToken(cns);

		if(Token::Compare(prev, ',', L',') == true)
			Token::Remove(prev);
	}


	return true;
}

// Parse index options
bool SqlParser::ParseKeyIndexOptions()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// Oracle USING INDEX clause
		if(next->Compare("USING", L"USING", 5) == true)
		{
			Token *key_option = GetNextToken();

			// Oracle USING INDEX
			if(Token::Compare(key_option, "INDEX", L"INDEX", 5))
			{				

				if (ParseCreateIndexOptions() == true)
				{
					Token::Remove(next, key_option);
				}
				else {
					Token *name = GetNextToken(key_option);
					// Remove USING INDEX
					Token::Remove(next, name);
				}

			}

			exists = true;
			continue;
		}
		else
		// Oracle ENABLE
		if(next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			// Remove ENABLE
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle NOVALIDATE
		if(next->Compare("NOVALIDATE", L"NOVALIDATE", 10) == true)
		{
			// Remove NOVALIDATE
			Token::Remove(next);

			exists = true;
			continue;
        }

		PushBack(next);
		break;
	}

	return exists;
}

// Parse FOREIGN KEY constraint clause
bool SqlParser::ParseForeignKey(Token *foreign)
{
    if(foreign == nullptr || foreign->Compare("FOREIGN", L"FOREIGN", 7) == false)
		return false;

	/*Token *key */ (void) GetNextWordToken("KEY", L"KEY", 3);
	Token *open = GetNextCharToken('(', L'(');

	// Get foreign key columns
    if(open != nullptr)
	{
		// Get columns until )
		while(true)
		{			
			// Column name
			Token *next = GetNextIdentToken();

            if(next == nullptr)
				break;

			Token *comma = GetNext(',', L',');

            if(comma == nullptr)
				break;
		}

		/*Token *close */ (void) GetNext(')', L')');
	}

	Token *references = GetNextWordToken("REFERENCES", L"REFERENCES", 10);

    if(references != nullptr)
	{
		Token *pk_table = GetNextIdentToken(SQL_IDENT_OBJECT);
		Token *open = GetNextCharToken('(', L'(');

		// Get primary/unique key columns
        if(pk_table != nullptr && open != nullptr)
		{
			// Get columns until )
			while(true)
			{			
				// Column name
				Token *next = GetNextIdentToken();

                if(next == nullptr)
					break;

				Token *comma = GetNext(',', L',');

                if(comma == nullptr)
					break;
			}

			/*Token *close */ (void) GetNext(')', L')');
		}
	}

	// Parse actions
	while(true)
	{
		Token *on = GetNextWordToken("ON", L"ON", 2);

        if(on == nullptr)
			break;

		Token *action = GetNextToken();

        if(action == nullptr)
			break;

		bool on_delete = false;
		bool on_update = false;

		// ON DELETE
		if(action->Compare("DELETE", L"DELETE", 6) == true)
			on_delete = true;
		else
		if(action->Compare("UPDATE", L"UPDATE", 6) == true)
			on_update = true;

		if(on_delete == true || on_update == true)
		{
			Token *what = GetNextToken();
			
			// NO ACTION, SET NULL and CASCADE
            if(what == nullptr)
				break;

			// NO ACTION
			if(what->Compare("NO", L"NO", 2) == true)
			{
				Token *action = GetNextToken();

				// Oracle does not support ON DELETE NO ACTION and ON UPDATE NO ACTION clause, but this 
				// is the default behavior (do not allow to delete parent rows if child rows exist)
				Token::Remove(on, action);
			}
			else
			// SET NULL
			if(what->Compare("SET", L"SET", 3) == true)
				/*Token *null */ (void) GetNextToken();

			continue;
		}

		PushBack(action);
		break;
	}

	// Parse foreign key options
	ParseKeyIndexOptions();

    if(_stmt_scope == SQL_STMT_ALTER_TABLE)
        ALTER_TAB_STMS_STATS("Add FOREIGN KEY")

	return true;
}

// CHECK constraint
bool SqlParser::ParseCheckConstraint(Token *check)
{
    if(check == nullptr || check->Compare("CHECK", L"CHECK", 5) == false)
		return false;

	/*Token *open */ (void) GetNextCharToken('(', L'(');

	// CHECK expression
	ParseBooleanExpression(SQL_BOOL_CHECK);

	/*Token *close*/ (void) GetNextCharToken(')', L')');

    if(_stmt_scope == SQL_STMT_ALTER_TABLE)
        ALTER_TAB_STMS_STATS("Add CHECK constraint")

	return true;
}

// Constraint option 
bool SqlParser::ParseConstraintOption()
{
	bool exists = false;

	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		// Oracle ENABLE
		if(next->Compare("ENABLE", L"ENABLE", 6) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle DISABLE
		if(next->Compare("DISABLE", L"DISABLE", 7) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle VALIDATE
		if(next->Compare("VALIDATE", L"VALIDATE", 8) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		else
		// Oracle NOVALIDATE
		if(next->Compare("NOVALIDATE", L"NOVALIDATE", 10) == true)
		{
			Token::Remove(next);

			exists = true;
			continue;
		}
		//NOT DEFERRABLE INITIALLY IMMEDIATE NORELY VALIDATE in Oracle
		else 	
		if (next->Compare("NOT", L"NOT", 3) == true) {
			(void)GetNextWordToken("DEFERRABLE", L"DEFERRABLE", 10);
			(void)GetNextWordToken("INITIALLY", L"INITIALLY", 9);
			(void)GetNextWordToken("IMMEDIATE", L"IMMEDIATE", 9);
			(void)GetNextWordToken("NORELY", L"NORELY", 6);
			Token *validate = GetNextWordToken("VALIDATE", L"VALIDATE", 8);
			if (validate != nullptr) {
				Token::Remove(next, validate);
				exists = true;
				continue;
			}
		}
		// Not a constraint option
		PushBack(next);
		break;
	}

	return exists;
}

// Get and parse the next token as an expression
Token* SqlParser::ParseExpression()
{
    Token *first = GetNext();

    if(ParseExpression(first))
        return first;

    PushBack(first);
    return nullptr;
}

// Parse an expression
bool SqlParser::ParseExpression(Token *first, int prev_operator)
{
    if(first == nullptr)
		return false;

	bool exists = false;

	// If expression starts with open (, parse recursively except a subquery
	if(first->Compare('(', L'(') == true)
	{
		Token *next = GetNextToken();

		// Subquery
		if(Token::Compare(next, "SELECT", L"SELECT", 6) == true)
		{
			bool select_pattern = ParseSelectExpressionPattern(first, next); 

			if(!select_pattern)
				ParseSelectStatement(next, 0, SQL_SEL_EXP, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			
			_exp_select++;
		}
		else
		{
		ParseExpression(next);

		// Propagate data type from expression to (
		first->data_type = next->data_type;			
		}

		// Next should be )
		/*Token *close*/ (void) GetNextCharToken(')', L')');

		exists = true;
	}
	else
	// Datetime literals
	if(ParseDatetimeLiteral(first) == true)
		exists = true;
    else
    // Named variable and expression: param => expr (Oracle)
    if(ParseNamedVarExpression(first) == true)
        exists = true;
	else
	// CASE expression (check CASE and IF before function as it can contain ( before first expression)
	if(ParseCaseExpression(first) == true)
		exists = true;
	else
	// Oracle built-in package
	if(ParseOraclePackage(first) == true)
		exists = true;
	else
	// Function with ()
	if(ParseFunction(first) == true)
		exists = true;
	else
	// Function without parentheses
	if(ParseFunctionWithoutParameters(first) == true)
		exists = true;
	else
	//// Boolean literal True/False
	//if(ParseBooleanLiteral(first))
	//	exists = true;
	//else
	// This can be an identifier 
	if(first->type == TOKEN_WORD || first->type == TOKEN_IDENT)
	{
        // Check if a variable exists with this name
        Token *var = GetVariable(first);

        // Change only if it was changed at the declaration
        if(var != nullptr && var->t_len != 0)
            ConvertVariableIdentifier(first, var);

        Token *param = nullptr;

        // Check if a parameter exists with this name
        if(var == nullptr)
            param = GetParameter(first);

        // Change only if it was changed at the declaration
        if(param != nullptr && param->t_len != 0)
            ConvertParameterIdentifier(first, param);

        if(var == nullptr && param == nullptr)
            ConvertIdentifier(first, SQL_IDENT_COLUMN);

		exists = true;
	}
	else
	// Other tokens that can start expression
	if(first->type == TOKEN_IDENT || first->type == TOKEN_STRING)
		exists = true;

	if(exists == false)
		return false;

	// Parse operators (they will be processed recursively by calling ParseExpression on the second operand)
	bool op_exists = false;

	// String concatenation || operator in Oracle
	if(ParseStringConcatenation(first, prev_operator) == true)
		op_exists = true;

	// Addition operator + 
    if(op_exists == false && ParseAdditionOperator(first) == true)
		op_exists = true;

	// Subtraction operator - 
	if(op_exists == false && ParseSubtractionOperator(first) == true)
		op_exists = true;

	// Multiplication operator * 
	if(op_exists == false && ParseMultiplicationOperator(first) == true)
		op_exists = true;

	// Division operator / 
	if(op_exists == false && ParseDivisionOperator(first) == true)
		op_exists = true;

	// Percent operator % 
	if(op_exists == false && ParsePercentOperator(first) == true)
		op_exists = true;

	return true;
}

// Parse datetime literal
bool SqlParser::ParseDatetimeLiteral(Token *token)
{
    if(token == nullptr)
		return false;

	bool exists = false;

	// TIMESTAMP '2013-01-01 11:11:10' or DATE '2016-07-11'
	if(TOKEN_CMP(token, "TIMESTAMP") == true || TOKEN_CMP(token, "DATE") == true)
	{
		Token *string = GetNextStringToken();

        if(string != nullptr)
			exists = true;
	}

	return exists;
}

// Named variable and expression: param => expr (Oracle)
bool SqlParser::ParseNamedVarExpression(Token *token)
{
    if(token == nullptr)
		return false;

    Token *equal = GetNext('=', L'=');

    Token *arrow = GetNext(equal, '>', L'>');

    if(arrow == nullptr)
    {
        PushBack(equal);
        return false;
    }

    ParseExpression();

	return true;
}

// Parse a boolean expression
bool SqlParser::ParseBooleanExpression(int scope, Token *stmt_start, int *conditions_count, int *rowlimit,
										Token *prev_open, bool *bool_operator_no_exists)
{
	int count = 0;

	// Typical error check boolean expressions involving = operator
//	if(ParseBooleanErrorCheckPattern() == true)
//		return true;

	// Boolean expression can be optionally in braces
	Token *open = GetNextCharToken('(', L'(');

	bool spec_and = false;
	bool bool_op_not_exists = false;

    if(open != nullptr)
	{
        /*Token *select =*/(void) GetNextWordToken("SELECT", L"SELECT", 6);

			// Use recursion as multiple open ( can be specified
			if(spec_and == false)
				ParseBooleanExpression(scope, stmt_start, conditions_count, rowlimit, open, &bool_op_not_exists);

			// Possible AND or OR between nested () conditions
			ParseBooleanAndOr(scope, stmt_start, &count, rowlimit);

        /*Token *close =*/(void) GetNextCharToken(')', L')');
		
	}

    /*Token *pre_first =*/ (void) GetLastToken();
    Token *first = nullptr;

	// First can be already parsed
    if(open == nullptr)
	{
		first = GetNextToken();

        if(first == nullptr)
			return false;

		// Check EXISTS () and NOT EXISTS expression
		if(first->Compare("EXISTS", L"EXISTS", 6) == true || first->Compare("NOT", L"NOT", 3) == true)
		{
			PushBack(first);
            first = nullptr;
		}
		else
			ParseExpression(first);
	}

    Token *op = nullptr;
    Token *not_ = nullptr;

	bool unary = false;

	if(spec_and == false)
	{
		// Get boolean operator
		op = GetNextToken();

		// Check for ) than means subexpression, not boolean expression - (exp) > exp2 i.e.
		if(Token::Compare(op, ')', L')') == true ||
			// Expression ended
			Token::Compare(op, ';', L';') == true) 
		{
            if(bool_operator_no_exists != nullptr)
				*bool_operator_no_exists = true;

			PushBack(op);
			return true;
		}

		if(Token::Compare(op, ',', L',') == true)
		{
			// List of columns in the left part of (a, b, c) IN ... Open ( is taken at higher level
            if(prev_open != nullptr)
			{
				while(Token::Compare(op, ',', L',') == true)
				{
					Token *exp = GetNextToken();

					ParseExpression(exp);

					op = GetNextToken();

					if(Token::Compare(op, ')', L')') == true)
					{
						PushBack(op);
						return true;
					}
				}
			}
			// End of boolean condition
			else
			{
				PushBack(op);
				return true;
			}
		}

		// Negation operator
		if(Token::Compare(op, "NOT", L"NOT", 3) == true)
		{
			not_ = op;
			op = GetNextToken();

				if(!TOKEN_CMP(op, "LIKE") && !TOKEN_CMP(op, "IN") && !TOKEN_CMP(op, "BETWEEN") && !TOKEN_CMP(op, "EXISTS") && !TOKEN_CMP(op, "MATCHES"))
				{
					ParseExpression(op);
					op = GetNextToken();
				}
		}

        if(op == nullptr)
			return false;
	}

	if(spec_and == true || TOKEN_CMPC(op, ')'))
		unary = true;
	else
	// Possible boolean function or expression without following operator (check for terminating word)
	if(scope == SQL_BOOL_IF && op->Compare("THEN", L"THEN", 4) == true)
	{
		PushBack(op);

		unary = true;
		count++;		
	}
	else
	// IS NULL or IS NOT NULL
	if(op->Compare("IS", L"IS", 2) == true)
	{
		Token *not_ = GetNextWordToken("NOT", L"NOT", 3);
		Token *null = GetNextWordToken("NULL", L"NULL", 4); 

        if(not_ != nullptr || null != nullptr)
		{
			unary = true;
			count++;
		}
	}
	else
	// BETWEEN a AND b
	if(op->Compare("BETWEEN", L"BETWEEN", 7) == true)
	{
		Token *exp1 = GetNextToken();

		ParseExpression(exp1);

		Token *and_ = GetNextWordToken("AND", L"AND", 3);
		
		Token *exp2 = GetNextToken();

		ParseExpression(exp2);

        if(exp1 != nullptr && and_ != nullptr && exp2 != nullptr)
		{
			unary = true;
			count++;
		}
	}
	else
	// IN predicate
	if(op->Compare("IN", L"IN", 2) == true)
	{
		if(ParseInPredicate(op) == true)
		{
			unary = true;
			count++;
		}
	}
	else
	// EXISTS predicate
	if(op->Compare("EXISTS", L"EXISTS", 6) == true)
	{
		// Parentheses () are required for EXISTS expression
		Token *open = GetNextCharToken('(', L'(');

        if(open != nullptr)
		{
            /*Token *next =*/(void) GetNextToken();


			/*Token *close*/ (void) GetNextCharToken(')', L')');
		}

		unary = true;
		count++;
	}
	else
	// LIKE predicate
	if(op->Compare("LIKE", L"LIKE", 4) == true)
	{
		// Optionally enclosed in parentheses () 
		Token *open = GetNextCharToken('(', L'(');

		/*Token *pattern */ (void) GetNextToken();

		/*Token *close*/ (void) GetNextCharToken(open, ')', L')');

		unary = true;
		count++;
    }

    Token *op2 = nullptr;

	// Check for <= <> != >= 2 symbol operators
	if(unary == false)
	{
		if(op->chr != 0 || op->wchr != 0)
		{
			op2 = GetNextCharToken('=', L'=');

            if(op2 == nullptr)
				op2 = GetNextCharToken('>', L'>');
		}
		// Unknown or no operator
		else
		{
			PushBack(op);
			unary = true;
		}
	}
	
	// Second operand
	if(unary == false)
	{
		Token *second = GetNextToken();

        if(second == nullptr)
			return false;

		bool ora_join = false;

		// Check for (+) join condition
		if(scope == SQL_BOOL_WHERE)
			ora_join = ParseOracleOuterJoin(first, second);

		if(!ora_join)
			ParseExpression(second);

		count++;

		// Oracle rownum condition in WHERE clause removed for other databases
		if(ParseOracleRownumCondition(first, op, second, rowlimit) == true)
			count--;
	}

	// Possible AND or OR 
	ParseBooleanAndOr(scope, stmt_start, &count, rowlimit);

    if(conditions_count != nullptr)
		*conditions_count = count;

	return true;
}

// AND or OR in boolean expression
bool SqlParser::ParseBooleanAndOr(int scope, Token *stmt_start, int *conditions_count, int *rowlimit)
{
	bool exists = false;

	while(true)
	{
		// Possible AND or OR 
		Token *next = GetNextToken();

        if(next == nullptr)
			break;

		if(Token::Compare(next, "AND", L"AND", 3) == false && Token::Compare(next, "OR", L"OR", 2) == false)
		{
			PushBack(next);
			break;
		}
		
		int count = 0;

		// Process recursively
		ParseBooleanExpression(scope, stmt_start, &count, rowlimit);

        if(conditions_count != nullptr)
			*conditions_count = (*conditions_count + count);

		exists = true;
	}

	return exists;
}

// IN predicate
bool SqlParser::ParseInPredicate(Token *in)
{
    if(in == nullptr)
		return false;

	/*Token *open */ (void) GetNextCharToken('(', '(');

		while(true)
		{
			Token *exp = GetNextToken();

            if(exp == nullptr)
				break;

			ParseExpression(exp);

			Token *comma = GetNextCharToken(',', ',');

            if(comma == nullptr)
				break;
		}

	Token *close = GetNextCharToken(')', ')');

    if(close == nullptr)
		return false;

	return true;
}

// Parser a procedural block
bool SqlParser::ParseBlock(int type, bool frontier, int scope, int *result_sets)
{
	// Process tokens until the end of the block
	while(true)
	{
		Token *token = GetNextToken();

        if(token == nullptr)
			break;

		// Entered a nested block
		if(token->Compare("BEGIN", L"BEGIN", 5) == true)
		{
			Token *next = GetNext();

			bool block = true;

			// Make sure it is not BEGIN WORK, BEGIN TRANSACTION or BEGIN;
			if(Token::Compare(next, "WORK", L"WORK", 4 )== true ||
				Token::Compare(next, "TRANSACTION", L"TRANSACTION", 11) == true || 
				Token::Compare(next, "TRAN", L"TRAN", 4) == true || 
				Token::Compare(next, ';', L';') == true)
				block = false;

			PushBack(next);

			if(block == true)
			{
                _spl_begin_blocks.Add(token);

				ParseBlock(SQL_BLOCK_BEGIN, true, scope, result_sets);

                _spl_begin_blocks.DeleteLast();

				Token *end = GetNext("END", L"END", 3);

                if(end != nullptr)
				{
					Token *semi = GetNext(';', L';');

                    if(semi == nullptr)
					{
						// while MySQL require BEGIN END;
						APPEND_NOFMT(end, ";");
					}
					else
						PushBack(semi);
				}

				continue;
			}
        }

		// If frontier was set, check the closing sequence for the block
		if(frontier == true)
		{
			// END keyword
			if(token->Compare("END", L"END", 3) == true)
			{
				PushBack(token);
				break;
			}
			else
			// For procedural block check for next CREATE PROCEDURE, FUNCTION, TRIGGER and
			// exit to avoid stack overflow in case of parser issues and deep nested calls
			if(type == SQL_BLOCK_PROC && token->Compare("CREATE", L"CREATE", 6) == true)
			{
				Token *next = GetNextToken();

				if(Token::Compare(next, "PROCEDURE", L"PROCEDURE", 9) || Token::Compare(next, "FUNCTION", L"FUNCTION", 8) ||
					Token::Compare(next, "TRIGGER", L"TRIGGER", 7))
				{
					PushBack(token);
					break;
				}

				PushBack(next);
			}
			else
			// IF block can be terminated with ELSEIF, ELSIF, ELSE or ENDIF keyword
			if(type == SQL_BLOCK_IF && (token->Compare("ELSEIF", L"ELSEIF", 6) || 
				token->Compare("ELSIF", L"ELSIF", 5) || token->Compare("ELSE", L"ELSE", 4) || token->Compare("ENDIF", L"ENDIF", 5)))
			{
				PushBack(token);
				break;
			}
			else
			// CASE block can be terminated with WHEN or ELSE keyword
			if(type == SQL_BLOCK_CASE && (token->Compare("WHEN", L"WHEN", 4) || token->Compare("ELSE", L"ELSE", 4)))
			{
				PushBack(token);
				break;
			}
			else
			// REPEAT block terminated with UNTIL
			if(type == SQL_BLOCK_REPEAT && token->Compare("UNTIL", L"UNTIL", 5) == true)
			{
				PushBack(token);
				break;
			}
			else 
			// Oracle EXCEPTION block terminated by WHEN for next condition
			if(type == SQL_BLOCK_EXCEPTION  && token->Compare("WHEN", L"WHEN", 4) == true)
			{
				PushBack(token);
				break;
            }
			else 
			// Sybase ADS TRY ... CATCH
			if((type == SQL_BLOCK_TRY  && TOKEN_CMP(token, "CATCH")) || (type == SQL_BLOCK_CATCH  && TOKEN_CMP(token, "END")))
			{
				PushBack(token);
				break;
			}

			// Parse high-level token
			Parse(token, scope, result_sets);
			continue;
		}
		else
        {
			// Parse the first statement and exit
			Parse(token, scope, result_sets);
			break;
		}
	}

	return true;
}

// Oracle PL/SQL assignment statement variable := exp
bool SqlParser::ParseAssignmentStatement(Token *variable)
{
    if(variable == nullptr)
		return false;

	// Must be a word or identifier
	if(variable->type != TOKEN_WORD && variable->type != TOKEN_IDENT)
		return false;

	Token *colon = GetNextCharToken(':', L':');
	Token *equal = GetNextCharToken('=', L'=');

    if(equal == nullptr)
		return false;


	// Try to find variable declaration
    /*Token *decl_var =*/ (void) GetVariableOrParameter(variable);

	// SET var = value in MySQL
	Prepend(variable, "SET ", L"SET ", 4, _declare_format);

	Token::Remove(colon);

	Token *next = GetNextToken();

	// Assignment expression
	ParseExpression(next);

	return true;
}

// String concatenation operator || in Oracle
bool SqlParser::ParseStringConcatenation(Token *first, int prev_operator)
{
    if(first == nullptr)
		return false;
	
	Token *first_end = GetLastToken();

	Token *bar = GetNextCharToken('|', L'|');

    if(bar == nullptr)
		return false;

	Token *bar2 = GetNextCharToken('|', L'|');

    if(bar2 == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second, SQL_OPERATOR_CONCAT);

	// Change to CONCAT function in MySQL
	// If it is first expression add CONCAT( before 
	if(prev_operator != SQL_OPERATOR_CONCAT)
		Prepend(first, "CONCAT(", L"CONCAT(", 7);

	Token::Change(bar, ",", L",", 1);
	Token::Remove(bar2);

	// If the first expression is nullable, around with IFNULL
	if(first->nullable == true)
	{
		Prepend(first, "IFNULL(", L"IFNULL(", 7);
		Append(first_end, ", '')", L", '')", 5);

		// Set not null to first token to avoid nested IFNULL when multiple concatenations specified
		first->nullable = false;
	}

	// If the second expression is nullable, around with IFNULL
	if(second->nullable == true)
	{
		Prepend(second, "IFNULL(", L"IFNULL(", 7);
		Append(GetLastToken(), ", '')", L", '')", 5);
	}

	// If it is first expression add ) to close CONCAT function
	if(prev_operator != SQL_OPERATOR_CONCAT)
		Append(GetLastToken(), ")", L")", 1);

	return true;
}

// Multiplication operator *
bool SqlParser::ParseMultiplicationOperator(Token *first)
{
    if(first == nullptr)
		return false;
	
	Token *asterisk = GetNextCharToken('*', L'*');

    if(asterisk == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second);

	return true;
}

// Addition operator +
bool SqlParser::ParseAdditionOperator(Token *first)
{
    if(first == nullptr)
		return false;
	
    /*Token *first_end =*/(void) GetLastToken();

	Token *plus = GetNextPlusMinusAsOperatorToken('+', L'+');

    if(plus == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second, SQL_OPERATOR_PLUS);

    /*Token *second_end =*/(void) GetLastToken();

	return true;
}

// Check for a +/- chain of interval expressions: for example, + expr MONTH - expr DAYS + ...
bool SqlParser::ParseAddSubIntervalChain(Token *first, int prev_operator)
{
	bool exists = false;

	// Get + or -
	Token *plus = GetNextPlusMinusAsOperatorToken('+', L'+');
    Token *minus = nullptr;

    if(plus == nullptr)
		minus = GetNextPlusMinusAsOperatorToken('-', L'-');

    if(plus != nullptr)
	{
		PushBack(plus);
		ParseAdditionOperator(first);
		exists = true;
	}
	else
    if(minus != nullptr)
	{
		PushBack(minus);
		ParseSubtractionOperator(first);
		exists = true;
	}

	return exists;
}

// Division operator /
bool SqlParser::ParseDivisionOperator(Token *first)
{
    if(first == nullptr)
		return false;
	
	/*Token *first_end */ (void) GetLastToken();

	Token *div = GetNextCharToken('/', L'/');

    if(div == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second);

	return true;
}

// Parse % operator or attribute 
bool SqlParser::ParsePercentOperator(Token *first)
{
    if(first == nullptr)
		return false;
	
	Token *cent = GetNextCharToken('%', L'%');

    if(cent == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	// cursor%NOTFOUND in Oracle
	if(TOKEN_CMP(second, "NOTFOUND") == true)
	{
		// NOT_FOUND = 1 in MySQL
		TOKEN_CHANGE(second, "NOT_FOUND = 1");
		Token::Remove(first, cent);
	}
    else
    // cursor%FOUND in Oracle
	if(TOKEN_CMP(second, "FOUND") == true)
	{
		// NOT_FOUND = 0 in MySQL
		TOKEN_CHANGE(second, "NOT_FOUND = 0");
		Token::Remove(first, cent);
	}

	return true;
}

// Subtraction operator -
bool SqlParser::ParseSubtractionOperator(Token *first)
{
    if(first == nullptr)
		return false;
	
    /*Token *first_end =*/(void) GetLastToken();

	Token *minus = GetNextPlusMinusAsOperatorToken('-', L'-');

    if(minus == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	// Parse the second and other expressions recursively
	ParseExpression(second);

    /*Token *second_end*/ (void) GetLastToken();

	return true;
}

// CASE as expression
bool SqlParser::ParseCaseExpression(Token *first)
{
    if(first == nullptr || Token::Compare(first, "CASE", L"CASE", 4) == false)
		return false;
    return true;
}

// Analytic function OVER (PARTITION part, ... ORDER BY c1 ASC, ...) clause
bool SqlParser::ParseAnalyticFunctionOverClause(Token *over)
{
    if(over == nullptr)
		return false;

	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

	// Optional PARTITION clause
	Token *partition = GetNextWordToken("PARTITION", L"PARTITION", 9);
	/*Token *by */ (void) GetNextWordToken(partition, "BY", L"BY", 2);

    if(partition != nullptr)
	{
		// Comma separated list of columns
		while(true)
		{
			Token *col = GetNextToken();

            if(col == nullptr)
				break;

			Token *comma = GetNextCharToken(',', L',');

            if(comma == nullptr)
				break;
		}
	}


	/*Token *close*/ (void) GetNextCharToken(')', L')');

	return true;
}
