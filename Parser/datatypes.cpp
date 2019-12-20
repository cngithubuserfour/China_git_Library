// SQLParser for datatypes

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"

// Parse data type definition
bool SqlParser::ParseDataType(Token *name, int clause_scope)
{
    if(name == nullptr)
        return false;

    if(ParseBfileType(name) == true)
        return true;

    if(ParseBinaryDoubleType(name) == true)
        return true;

    if(ParseBinaryFloatType(name) == true)
        return true;

    if(ParseBlobType(name) == true)
        return true;

    if(ParseCharacterType(name) == true)
        return true;

    if(ParseCharType(name) == true)
        return true;

    if(ParseClobType(name) == true)
        return true;

    if(ParseDateType(name) == true)
        return true;

    if(ParseDecimalType(name) == true)
        return true;

    if(ParseDoubleType(name) == true)
        return true;

    if(ParseFloatType(name) == true)
        return true;

    if(ParseIntervalType(name) == true)
        return true;

    if(ParseIntType(name) == true)
        return true;

    if(ParseLongType(name) == true)
        return true;

    if(ParseNcharType(name) == true)
        return true;

    if(ParseNclobType(name) == true)
        return true;

    if(ParseNumberType(name, clause_scope) == true)
        return true;

    if(ParseNumericType(name) == true)
        return true;

    if(ParseNvarchar2Type(name, clause_scope) == true)
        return true;

    if(ParseRawType(name) == true)
        return true;

    if(ParseRealType(name) == true)
        return true;

    if(ParseRefcursor(name) == true)
        return true;

    if(ParseRowidType(name) == true)
        return true;

    if(ParseSmallintType(name) == true)
        return true;

    if(ParseTimestampType(name) == true)
        return true;

    if(ParseUrowidType(name) == true)
        return true;

    if(ParseVarcharType(name,clause_scope) == true)
        return true;

    if(ParseVarchar2Type(name, clause_scope) == true)
        return true;

    if(ParseXmltypeType(name) == true)
        return true;

    return false;
}

bool SqlParser::ParseTypedVariable(Token *var, Token *ref_type)
{
    if(var == nullptr || ref_type == nullptr)
        return false;

    // %TYPE and %ROWTYPE
    Token *cent = GetNextCharToken('%', L'%');

    Token *type = nullptr;
    Token *rowtype = nullptr;

    Token *like = nullptr;

    if(cent == nullptr && like == nullptr)
        return false;

    if(cent != nullptr)
    {
        type = GetNextWordToken("TYPE", L"TYPE", 4); 

        if(type == nullptr)
            rowtype = GetNextWordToken("ROWTYPE", L"ROWTYPE", 7); 

        if(type == nullptr && rowtype == nullptr)
            return false;

        if(type != nullptr)
            UDTYPE_STATS("%TYPE")
        else
            UDTYPE_STATS("%ROWTYPE")
    }

    UDTYPE_DTL_STATS_L(var)

    // Convert %TYPE
    if(type != nullptr)
    {
        // Read the data type from available meta information
        const char *meta_type = GetMetaType(ref_type);

        if(meta_type != nullptr)
        {
            Token::Change(ref_type, meta_type, nullptr, strlen(meta_type), type);
            Token::Remove(cent, type);
            return true;
        }

        // Guess functions
        TokenStr type_str(" ", L" ", 1);
        char data_type = GuessType(ref_type, type_str);

        if(data_type != 0)
            Append(var, &type_str, type);
        else
            Append(var, " VARCHAR(4000)", L" VARCHAR(4000)", 14, type);

        Comment("Use -meta option ", L"Use -meta option ", 17, ref_type, type);
    }
    else
    // Convert LIKE
    if(like != nullptr)
    {
        // Get table.column
        GetNextToken();

    }
    else
    // %ROWTYPE
    if(rowtype != nullptr)
    {
        // Save variable
        _spl_rowtype_vars.Add(var);

        Token::Remove(var, rowtype);

        Token *semi = GetNextCharToken(';', L';');

        if(semi != nullptr)
            Token::Remove(semi);

        // Return ; to the input to allow correct declaration parsing
        PushBack(semi);
    }

    return true;
}

// BFILE
bool SqlParser::ParseBfileType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("BFILE", L"BFILE", 5) == false)
        return false;

	STATS_ITM_DECL

    // Convet to VARCHAR(255)

    Token::Change(name, "VARCHAR(255)", L"VARCHAR(255)", 12);
    STATS_ITM_CONV_ERROR

    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BINARY_DOUBLE
bool SqlParser::ParseBinaryDoubleType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("BINARY_DOUBLE", L"BINARY_DOUBLE", 13) == false)
        return false;

    // Convert to DOUBLE in MySQL
    Token::Change(name, "DOUBLE", L"DOUBLE", 6);


	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BINARY_FLOAT
bool SqlParser::ParseBinaryFloatType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("BINARY_FLOAT", L"BINARY_FLOAT", 12) == false)
        return false;

    // Convert to FLOAT in MySQL
    Token::Change(name, "FLOAT", L"FLOAT", 5);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// BLOB
bool SqlParser::ParseBlobType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("BLOB", L"BLOB", 4) == false)
        return false;

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Num and semantics are optional in MySQL
    if(open != nullptr)
    {
        GetNextToken();
        GetNextCharToken(')', L')');
    }


    // Convert to LONGBLOB in MySQL
    Token::Change(name, "LONGBLOB", L"LONGBLOB", 8);


	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// CHARACTER & CHARACTER VARYING
bool SqlParser::ParseCharacterType(Token *name)
{
    if(name == nullptr)
        return false;

    bool character = false;
    bool character_in_braces = false;

    if(name->Compare("CHARACTER", L"CHARACTER", 9) == true)
        character = true;

    if(character == false && character_in_braces == false)
        return false;

    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    if(varying == nullptr)
        DTYPE_STATS(name)
    else
        DTYPE_STATS("CHARACTER VARYING")

    Token *open = GetNextCharToken('(', L'(');
    Token *size = nullptr;
    Token *semantics = nullptr;
    Token *close = nullptr;

    // Size is optional
    if(open != nullptr)
    {
        size = GetNextToken();

        semantics = GetNextWordToken("BYTE", L"BYTE", 4);

        if(semantics == nullptr)
        {
            semantics = GetNextWordToken("CHAR", L"CHAR", 4);
        }

        Token::Remove(semantics);

        close = GetNextCharToken(')', L')');
    }

    bool binary = false;

    GetNextWordToken("FOR", L"FOR", 3);

    // If MAX is specified, convert to CLOB types
    if(Token::Compare(size, "MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        Token::Remove(name);
        Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
        Token::Remove(open, close);
    }
    else
        // Check for CHARACTER VARYING FOR BIT DATA
        if(binary == true)
        {
            // Check for CHARACTER VARYING
            if(varying != nullptr)
            {
                if(open == nullptr)
                    Append(varying, "(1)", L"(1)", 3);
            }
            else
                // Check for CHAR
                if(varying == nullptr)
                {
                    int num = 1;

                    if(size != nullptr)
                        num = size->GetInt();

                    // If size is greater than 255 convert to VARCHAR in MySQL
                    if(num > 255)
                        Token::Change(name, "VARCHAR", L"VARCHAR", 7);
                }
        }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// CHAR and CHAR VARYING
bool SqlParser::ParseCharType(Token *name)
{
    if(name == nullptr)
        return false;

	STATS_ITM_DECL

    bool char_ = false;
    bool char_in_braces = false;

    if(name->Compare("CHAR", L"CHAR", 4) == true)
        char_ = true;

    if(char_ == false && char_in_braces == false)
        return false;

    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    if(varying == nullptr)
        DTYPE_STATS(name)
    else
        DTYPE_STATS("CHAR VARYING")

    Token *open = GetNextCharToken('(', L'(');
    Token *size = nullptr;
    Token *semantics = nullptr;
    Token *close = nullptr;

    // Size is optional
    if(open != nullptr)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();

        // For Oracle, size can be followed by length semantics CHAR or BYTE;
        semantics = GetNextWordToken("BYTE", L"BYTE", 4);

        if(semantics == nullptr)
        {
            semantics = GetNextWordToken("CHAR", L"CHAR", 4);
        }

        Token::Remove(semantics);

        close = GetNextCharToken(')', L')');
    }

    // If MAX is specified, convert to CLOB types
    if(size->Compare("MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        Token::Remove(name);
        Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
        Token::Remove(open, close);
    }
    else
    // Check for CHAR VARYING
    if(varying != nullptr)
    {
        // Size is mandatory in MySQL
        Append(varying, "(1)", L"(1)", 3);
    }
    else
    // Check for CHAR
    if(varying == nullptr)
    {
        int num = 1;

        if(size != nullptr)
            num = size->GetInt();

        // If size is greater than 255 convert to VARCHAR in MySQL
        if(num > 255)
            Token::Change(name, "VARCHAR", L"VARCHAR", 7);
        else
            STATS_ITM_CONV_NO_NEED
    }

    DTYPE_DTL_STATS_L(name)

    return true;
}

// CLOB
bool SqlParser::ParseClobType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("CLOB", L"CLOB", 4) == false)
        return false;

    DTYPE_STATS(name)

    // Convert to LONGTEXT in MySQL
    Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}


// DATE
bool SqlParser::ParseDateType(Token *name)
{
    if(name == nullptr)
        return false;

    bool date = false;
    bool date_in_braces = false;

    if(name->Compare("DATE", L"DATE", 4) == true)
        date = true;

    if(date == false && date_in_braces == false)
        return false;

    // Check for Oracle DATE
    Token::Change(name, "DATETIME", L"DATETIME", 8);
        
	name->data_type = TOKEN_DT_DATETIME;
    name->data_subtype = TOKEN_DT2_DATETIME_SEC;

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// DECIMAL and DEC
bool SqlParser::ParseDecimalType(Token *name)
{
    if(name == nullptr)
        return false;

	STATS_ITM_DECL

    bool decimal = false;


    if(name->Compare("DECIMAL", L"DECIMAL", 7) == true || name->Compare("DEC", L"DEC", 3) == true)
        decimal = true;

    if(decimal == false)
        return false;

    Token *open = GetNextCharToken('(', L'(');
    Token *close = nullptr;
    bool conv = false;

    // Precision and scale are optional
    if(open != nullptr)
    {
        Token *precision = GetNextNumberToken();

        // Scale is optional
        /*Token *comma =*/(void) GetNextCharToken(precision, ',', L',');

        close = GetNextCharToken(')', L')');

    }

    // Default precision and scale
    if(open == nullptr)
    {
        // NUMERIC(38,0) is default in Oracle
        Append(name, "(38,0)", L"(38,0)", 6);
        conv = true;
    }

    if(!conv)
        STATS_ITM_CONV_NO_NEED

    DTYPE_STATS(name)
    DTYPE_DTL_STATS_L(name)

    return true;
}

// DOUBLE PRECISION
bool SqlParser::ParseDoubleType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("DOUBLE", L"DOUBLE", 6) == false)
        return false;

	STATS_ITM_DECL
    DTYPE_STATS(name)

    bool conv = false;

    if(!conv)
        STATS_ITM_CONV_NO_NEED

    DTYPE_DTL_STATS_L(name)

    return true;
}

// FLOAT
bool SqlParser::ParseFloatType(Token *name)
{
    if(name == nullptr)
        return false;

    bool float_ = false;
    bool float_in_braces = false;
    bool conv = false;

    if(name->Compare("FLOAT", L"FLOAT", 5) == true)
        float_ = true;


    if(float_ == false && float_in_braces == false)
        return false;

	STATS_ITM_DECL
    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *close = nullptr;

    // Precision is optional
    if(open != nullptr)
    {
        /*Token *precision */ (void) GetNextToken();
        Token *comma = GetNextCharToken(',', L',');
        if(comma != nullptr)
        {
            /*Token *scale =*/ (void) GetNextToken();
        }
        close = GetNextCharToken(')', L')');
    }

    // Convert to DOUBLE in MySQL
    Token::Change(name, "DOUBLE", L"DOUBLE", 6);
    conv = true;

    if(open != nullptr)
        Token::Remove(open, close);

    if(!conv)
        STATS_ITM_CONV_NO_NEED

    DTYPE_DTL_STATS_L(name)

    return true;
}

// INTERVAL
bool SqlParser::ParseIntervalType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("INTERVAL", L"INTERVAL", 8) == false)
        return false;

 	STATS_ITM_DECL
    DTYPE_STATS(name)

    bool start = false;

    Token *first_unit = GetNextToken();
    Token *first_year = nullptr;
    Token *first_month = nullptr;
    Token *first_day = nullptr;
    Token *first_hour = nullptr;
    Token *first_minute = nullptr;
    Token *first_second = nullptr;
    Token *first_fraction = nullptr;
    Token *to = nullptr;
    Token *second_unit = nullptr;

    if(Token::Compare(first_unit, "YEAR", L"YEAR", 4) == true)
    {
        first_year = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "MONTH", L"MONTH", 5) == true)
    {
        first_month = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "DAY", L"DAY", 3) == true)
    {
        first_day = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "HOUR", L"HOUR", 4) == true)
    {
        first_hour = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "MINUTE", L"MINUTE", 6) == true)
    {
        first_minute = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "SECOND", L"SECOND", 6) == true)
    {
        first_second = first_unit;
        start = true;
    }
    else
    if(Token::Compare(first_unit, "FRACTION", L"FRACTION", 8) == true)
    {
        first_fraction = first_unit;
        start = true;
    }
    else
        PushBack(first_unit);

    Token *open = nullptr;
    Token *close = nullptr;

    if(start == true)
    {
        open = GetNextCharToken('(', L'(');

        // Precision is optional
        if(open != nullptr)
        {
            /*Token *precision */ (void) GetNextNumberToken();
            close = GetNextCharToken(')', L')');
        }

        to = GetNextWordToken("TO", L"TO", 2);

        if(to != nullptr)
            second_unit = GetNextToken();
    }

    Token *open2 = GetNextCharToken('(', L'(');
    Token *scale = nullptr;
    Token *close2 = nullptr;

    // Scale is optional
    if(open2 != nullptr)
    {
        scale = GetNextNumberToken();
        close2 = GetNextCharToken(')', L')');
    }

    // Convert to VARHCAR(30)
    Token::Remove(first_unit, second_unit);
    Token::Remove(open2, close2);
    Token::Change(name, "VARCHAR(30)", L"VARCHAR(30)", 11);

    STATS_ITM_CONV_ERROR

    DTYPE_DTL_STATS_L(name)

    return true;
}

// INT
bool SqlParser::ParseIntType(Token *name)
{
    if(name == nullptr)
        return false;

    bool int_ = false;
    bool int_in_braces = false;
    bool conv = false;

    if(name->Compare("INT", L"INT", 3) == true || name->Compare("INTEGER", L"INTEGER", 7) == true)
        int_ = true;

    if(int_ == false && int_in_braces == false)
        return false;

	STATS_ITM_DECL
    DTYPE_STATS(name)

    if(!conv)
        STATS_ITM_CONV_NO_NEED

    DTYPE_DTL_STATS_L(name)

	name->data_type = TOKEN_DT_NUMBER;
	name->data_subtype = TOKEN_DT2_INT;

    return true;
}

// LONG
bool SqlParser::ParseLongType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("LONG", L"LONG", 4) == false)
        return false;

    Token *unit = GetNextToken();

    Token *raw = nullptr;

    if(Token::Compare(unit, "RAW", L"RAW", 3) == true)
    {
        DTYPE_STATS("LONG RAW")
        raw = unit;
    }
    else
    {
        DTYPE_STATS(name)

        PushBack(unit);
        unit = nullptr;
    }
    
    // LONG in Oracle
    if(unit == nullptr)
    {
        // Convert to LONGTEXT in MySQL
        Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
    }
    else
        // LONG RAW in Oracle
        if(raw != nullptr)
        {
            // Convert to LONGBLOB in MySQL
            Token::Remove(name);
            Token::Change(unit, "LONGBLOB", L"LONGBLOB", 8);
        }


    STATS_ITM_DECL
    DTYPE_DTL_STATS_L(name)

    return true;
}

// NCHAR and NCHAR VARYING
bool SqlParser::ParseNcharType(Token *name)
{
    if(name == nullptr)
        return false;

    bool nchar = false;
    bool nchar_in_braces = false;

    if(name->Compare("NCHAR", L"NCHAR", 5) == true)
        nchar = true;

    if(nchar == false && nchar_in_braces == false)
        return false;

    Token *varying = GetNextWordToken("VARYING", L"VARYING", 7);

    if(varying == nullptr)
        DTYPE_STATS(name)
    else
        DTYPE_STATS("NCHAR VARYING")

    Token *open = GetNextCharToken('(', L'(');
    Token *size = nullptr;
    Token *close = nullptr;

    // Num is optional
    if(open != nullptr)
    {
        // For SQL Server, MAX can be specified as the size
        size = GetNextToken();
        close = GetNextCharToken(')', L')');
    }

    // If MAX is specified, convert to CLOB types
    if(size->Compare("MAX", L"MAX", 3) == true)
    {
        Token::Remove(name);
        Token::Change(varying, "LONGTEXT", L"LONGTEXT", 8);
        Token::Remove(open, close);
    }
    else
        // Check for NCHAR VARYING
        if(varying != nullptr)
        {
            if(open == nullptr)
                Append(varying, "(1)", L"(1)", 3);
        }
        else
            // Check for NCHAR
            if(varying == nullptr)
            {
            // If size is greater than 255 convert to NVARCHAR in MySQL
                int num = 1;

                if(size != nullptr)
                    num = size->GetInt();

                if(num > 255)
                    Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);
            }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// NCLOB
bool SqlParser::ParseNclobType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("NCLOB", L"NCLOB", 5) == false)
        return false;

    DTYPE_STATS(name)

    // Convert to LONGTEXT in MySQL
    Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}



// NUMBER
bool SqlParser::ParseNumberType(Token *name, int clause_scope)
{
    if(name == nullptr)
        return false;

    if(name->Compare("NUMBER", L"NUMBER", 6) == false)
        return false;

    DTYPE_STATS(name)

    // Precision and scale are optional
    Token *open = GetNextCharToken('(', L'(');
    Token *precision = nullptr;
    Token *comma = nullptr;
    Token *scale = nullptr;
    Token *close = nullptr;

    // Get precision and scale;
    if(open != nullptr)
    {
        // Precision can be specified as a number or *
        precision = GetNextToken();
        comma = GetNextCharToken(',', L',');

        if(comma != nullptr)
            scale = GetNextNumberToken(); 

        close = GetNextCharToken(')', L')');
    }

    // Floating-point number
    if(open == nullptr || (precision->Compare('*', L'*') == true && comma == nullptr))
    {
        // NUMBER without () in parameters and variables
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS ||
            clause_scope == SQL_SCOPE_VAR_DECL)
        {
            Token::Change(name, "DOUBLE", L"DOUBLE", 6);
        }
        else
        {
            Token::Change(name, "DOUBLE", L"DOUBLE", 6);

            if(open != nullptr)
                Token::Remove(open, close);
        }
    }
    else
    {
        int prec = -1;
        int sc = 0;

        bool prec_asterisk = false;

        if(precision->Compare('*', L'*') == true)
        {
            prec = 38;
            prec_asterisk = true;
        }
        else
            prec = precision->GetInt();

        if(scale != nullptr)
            sc = scale->GetInt();

            // Check for TINYINT range
            if(prec != -1 && prec < 3 && sc == 0)
            {
                Token::Change(name, "TINYINT", L"TINYINT", 7);
                Token::Remove(open, close);
            }
            else
                // Check for SMALLINT range
                if(prec >= 3 && prec < 5 && sc == 0)
                {
                    Token::Change(name, "SMALLINT", L"SMALLINT", 8);
                    Token::Remove(open, close);
                }
                else
                    // INT range
                    if(prec >= 5 && prec < 9 && sc == 0)
                    {
                        Token::Change(name, "INT", L"INT", 3);
                        Token::Remove(open, close);
                    }
                    else
                        if(prec >= 9 && prec < 19 && sc == 0)
                        {
                                Token::Change(name, "BIGINT", L"BIGINT", 6);
                                Token::Remove(open, close);
                        }
                        else
                            // DECIMAL
                        {
                            Token::Change(name, "DECIMAL", L"DECIMAL", 7);

                            if(prec_asterisk == true)
                                Token::Change(precision, "38", L"38", 2);
                        }
    }

	name->data_type = TOKEN_DT_NUMBER;
	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// NUMERIC
bool SqlParser::ParseNumericType(Token *name)
{
    if(name == nullptr)
        return false;

    bool numeric = false;
    bool numeric_in_braces = false;
    bool num = false;

    if(name->Compare("NUMERIC", L"NUMERIC", 7) == true)
        numeric = true;

    if(numeric == false && numeric_in_braces == false && num == false)
        return false;

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Precision and scale are optional
    if(open != nullptr)
    {
        Token *comma = GetNextCharToken(',', L',');
        Token *scale = nullptr;

        if(comma != nullptr)
            scale = GetNextNumberToken();


        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Default precision and scale
    if(open == nullptr)
    {
        // NUMERIC(38,0) is default in Oracle
        Append(name, "(38,0)", L"(38,0)", 6);
    }

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// NVARCHAR2
bool SqlParser::ParseNvarchar2Type(Token *name, int clause_scope)
{
    if(name == nullptr)
        return false;

    if(name->Compare("NVARCHAR2", L"NVARCHAR2", 9) == false)
        return false;

    DTYPE_STATS(name)

    // Size is mandatory in Oracle
    /*Token *open */ (void) GetNextCharToken('(', L'(');
    /*Token *size */ (void) GetNextNumberToken();
    /*Token *close */ (void) GetNextCharToken(')', L')');

    Token::Change(name, "NVARCHAR", L"NVARCHAR", 8);

    // When used in parameter list for a function or procedure add length
    if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
        APPEND_NOFMT(name, "(2000)");

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// RAW
bool SqlParser::ParseRawType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("RAW", L"RAW", 3) == false)
        return false;

    DTYPE_STATS(name)

    // Size is mandatory in Oracle
    /*Token *open */ (void) GetNextCharToken('(', L'(');
    Token *size = GetNextNumberToken();
    /*Token *close */(void) GetNextCharToken(')', L')');

    int num = 1;

    if(size != nullptr)
        num = size->GetInt();

    // If size is more than 255 convert to VARBINARY in MySQL
    if(num > 255)
        Token::Change(name, "VARBINARY", L"VARBINARY", 9);
    // If size is less than 255 convert to BINARY in MySQL
    else
        Token::Change(name, "BINARY", L"BINARY", 6);

    STATS_ITM_DECL
    DTYPE_DTL_STATS_L(name)

    return true;
}

// REAL
bool SqlParser::ParseRealType(Token *name)
{
    if(name == nullptr)
        return false;

    bool real = false;
    bool real_in_braces = false;

    if(name->Compare("REAL", L"REAL", 4) == true)
        real = true;

    if(real == false && real_in_braces == false)
        return false;

    DTYPE_STATS(name)

    // Convert Oracle REAL to FLOAT
    Token::Change(name, "FLOAT", L"FLOAT", 5);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// Oracle SYS_REFCURSOR
bool SqlParser::ParseRefcursor(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("SYS_REFCURSOR", L"SYS_REFCURSOR", 13) == false &&
        name->Compare("REFCURSOR", L"REFCURSOR", 9) == false )
        return false;
        
	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// ROWID
bool SqlParser::ParseRowidType(Token *name)
{
    if(name == nullptr)
        return false;

    // ROWID 10 bytes
    if(name->Compare("ROWID", L"ROWID", 5) == false)
        return false;

	STATS_ITM_DECL

    // Convert to CHAR(10)
    Token::Change(name, "CHAR(10)", L"CHAR(10)", 8);
    STATS_ITM_CONV_ERROR
        
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

// SMALLINT
bool SqlParser::ParseSmallintType(Token *name)
{
    if(name == nullptr)
        return false;

    bool smallint = false;
    bool smallint_in_braces = false;

    if(name->Compare("SMALLINT", L"SMALLINT", 8) == true)
        smallint = true;

    if(smallint == false && smallint_in_braces == false)
        return false;

    DTYPE_STATS(name)

    // Convert Oracle INTEGER to DECIMAL(38)
    Token::Change(name, "DECIMAL(38)", L"DECIMAL(38)", 11);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// TIMESTAMP
bool SqlParser::ParseTimestampType(Token *name)
{
    if(name == nullptr)
        return false;

    bool timestamp = false;
    bool timestamp_in_braces = false;
    bool conv = false;

    if(name->Compare("TIMESTAMP", L"TIMESTAMP", 9) == true)
        timestamp = true;

    if(timestamp == false && timestamp_in_braces == false)
        return false;

	STATS_ITM_DECL
    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');
    Token *close = nullptr;

    // Optional fractional precision
    if(open != nullptr)
    {
        /*Token *fraction */ (void) GetNextToken();
        close = GetNextCharToken(')', L')');
    }

    Token *without = GetNextWordToken("WITHOUT", L"WITHOUT", 7);
    // WITH TIME ZONE is mandatory parameter
    Token *with = GetNextWordToken("WITH", L"WITH", 4);

    // LOCAL is optional keyword in Oracle
    if(with != nullptr)
    {
        Token *local = GetNextWordToken("LOCAL", L"LOCAL", 5);

        // remove LOCAL keyword
        Token::Remove(local);
        conv = true;
    }

    Token *zone = nullptr;

    if(without != nullptr || with != nullptr)
    {
        /*Token *time */ (void) GetNextWordToken("TIME", L"TIME", 4);
        zone = GetNextWordToken("ZONE", L"ZONE", 4);

        //remove WITHOUT TIME ZONE
        if(without != nullptr)
        {
            Token::Remove(without, zone);
            conv = true;
        }
    }

    // Default fraction
    if(open == nullptr)
    {
        // TIMESTAMP(0) is default in MySQL
        Append(name, "(0)", L"(0)", 3);
        conv = true;
    }

    // Convert to DATETIME in MySQL
    Token::Change(name, "DATETIME", L"DATETIME", 8);

    // TIMESTAMP(6) is default in Oracle
    if(open == nullptr)
        Append(name, "(6)", L"(6)", 3);

    if(with != nullptr)
        Token::Remove(with, zone);

    conv = true;

    if(!conv)
        STATS_ITM_CONV_NO_NEED

    DTYPE_DTL_STATS_L(name)

    return true;
}

// UROWID
bool SqlParser::ParseUrowidType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("UROWID", L"UROWID", 6) == false)
        return false;

    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    // Size is optional in Oracle
    if(open != nullptr)
    {
        /*Token *size */ (void) GetNextNumberToken();
        /*Token *close */ (void) GetNextCharToken(')', L')');
    }

    // Convert to VARCHAR
    Token::Change(name, "VARCHAR", L"VARCHAR", 7);

    // Default size is 4000 in Oracle
   if(open == nullptr)
       Append(name, "(4000)", L"(4000)", 6);

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// VARCHAR
bool SqlParser::ParseVarcharType(Token *name, int clause_scope)
{
    if(name == nullptr)
        return false;

    bool varchar = false;
    bool varchar_in_braces = false;
	bool conv = false;

    if(name->Compare("VARCHAR", L"VARCHAR", 7) == true)
        varchar = true;
    else

    if(varchar == false && varchar_in_braces == false)
        return false;

	STATS_ITM_DECL
    DTYPE_STATS(name)

    Token *open = GetNextCharToken('(', L'(');

    Token *size = nullptr;

    Token *close = nullptr;



    // If MAX is specified, convert to CLOB types
    if(size->Compare("MAX", L"MAX", 3) == true)
    {
        // Change to LONGTEXT in MySQL
        Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);
        Token::Remove(open, close);
        conv = true;
    }
    // Size is mandatory in MySQL
    if(open == nullptr)
    {
        if(varchar_in_braces == true)
            Token::ChangeNoFormat(name, name, 1, name->len - 2);
        
        // When used in parameter list for a function or procedure add length
        if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
            AppendNoFormat(name, "(4000)", L"(4000)", 6);
        else
            Append(name, "(1)", L"(1)", 3);

		conv = true;
    }

	if(!conv)
        STATS_ITM_CONV_NO_NEED

    name->data_type = TOKEN_DT_STRING;

    DTYPE_DTL_STATS_L(name)

    return true;
}

// VARCHAR2
bool SqlParser::ParseVarchar2Type(Token *name, int clause_scope)
{
    if(name == nullptr)
        return false;

    if(name->Compare("VARCHAR2", L"VARCHAR2", 8) == false)
        return false;

    DTYPE_STATS(name)

    // Size is mandatory in  unless it is a parameter
    Token *open = GetNextCharToken('(', L'(');

    Token *size = nullptr;
    Token *semantics = nullptr;
    Token *close = nullptr;

    if(open != nullptr)
        size = GetNextNumberToken();

    // For Oracle, size can be followed by length semantics CHAR or BYTE
    if(size != nullptr)
        semantics = GetNextWordToken("BYTE", L"BYTE", 4);

    if(semantics == nullptr)
        semantics = GetNextWordToken("CHAR", L"CHAR", 4);

    if(open != nullptr)
        close = GetNextCharToken(')', L')');

    Token::Change(name, "VARCHAR", L"VARCHAR", 7);

    // Remove length semantics attribute
    if(semantics != nullptr)
        Token::Remove(semantics);

    // When used in parameter list for a function or procedure add length
    if(clause_scope == SQL_SCOPE_FUNC_PARAMS || clause_scope == SQL_SCOPE_PROC_PARAMS)
    {
        AppendNoFormat(name, "(4000)", L"(4000)", 6);
    }

    name->data_type = TOKEN_DT_STRING;

	STATS_ITM_DECL 
    DTYPE_DTL_STATS_L(name)

    return true;
}

// XMLTYPE
bool SqlParser::ParseXmltypeType(Token *name)
{
    if(name == nullptr)
        return false;

    if(name->Compare("XMLTYPE", L"XMLTYPE", 7) == false)
        return false;

    // Convert to LONGTEXT in MySQL
    Token::Change(name, "LONGTEXT", L"LONGTEXT", 8);

	STATS_ITM_DECL 
    DTYPE_STATS(name)
    DTYPE_DTL_STATS_0(name)

    return true;
}

