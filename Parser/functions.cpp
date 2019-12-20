// SQLParser for functions

#include <stdio.h>
#include <string.h>
#include "sqlparser.h"
#include "listw.h"

// Parse function 
bool SqlParser::ParseFunction(Token *name)
{
    if(name == nullptr)
		return false;

	STATS_DECL

	Token *open = GetNextCharToken('(', L'(');

    if(open == nullptr)
		return false;

	bool exists = false;
    bool udt_exists = false;

    if(name->Compare("ABS", L"ABS", 3) == true)
		exists = ParseFunctionAbs(name, open);
    else
	if(name->Compare("ACOS", L"ACOS", 4) == true)
		exists = ParseFunctionAcos(name, open);
    else
	if(name->Compare("ADD_MONTHS", L"ADD_MONTHS", 10) == true)
		exists = ParseFunctionAddMonths(name);
    else
	if(name->Compare("ASCII", L"ASCII", 5) == true)
		exists = ParseFunctionAscii(name, open);
	else
	if(name->Compare("ASCIISTR", L"ASCIISTR", 8) == true)
		exists = ParseFunctionAsciistr(name, open);
    else
	if(name->Compare("ASIN", L"ASIN", 4) == true)
		exists = ParseFunctionAsin(name, open);
	else
	if(name->Compare("ATAN", L"ATAN", 4) == true)
		exists = ParseFunctionAtan(name, open);
	else
	if(name->Compare("ATAN2", L"ATAN2", 5) == true)
		exists = ParseFunctionAtan2(name);
    else
	if(name->Compare("AVG", L"AVG", 3) == true)
		exists = ParseFunctionAvg(name, open);
    else
	if(name->Compare("BIN_TO_NUM", L"BIN_TO_NUM", 10) == true)
		exists = ParseFunctionBinToNum(name, open);
    else
	if(name->Compare("BITAND", L"BITAND", 6) == true)
		exists = ParseFunctionBitand(name, open);
    else
	if(name->Compare("BITNOT", L"BITNOT", 6) == true)
		exists = ParseFunctionBitnot(name, open);
    else
	if(name->Compare("CAST", L"CAST", 4) == true)
		exists = ParseFunctionCast(name);
	else
	if(name->Compare("CEIL", L"CEIL", 4) == true)
		exists = ParseFunctionCeil(name, open);
    else
	if(name->Compare("CHR", L"CHR", 3) == true)
		exists = ParseFunctionChr(name, open);
    else
	if(name->Compare("COALESCE", L"COALESCE", 8) == true)
		exists = ParseFunctionCoalesce(name, open);
    else
	if(name->Compare("CONCAT", L"CONCAT", 6) == true)
		exists = ParseFunctionConcat(name, open);
	else
	if(name->Compare("CONVERT", L"CONVERT", 7) == true)
		exists = ParseFunctionConvert(name);
	else
	if(name->Compare("COS", L"COS", 3) == true)
		exists = ParseFunctionCos(name, open);
	else
	if(name->Compare("COSH", L"COSH", 4) == true)
		exists = ParseFunctionCosh(name, open);
    else
	if(name->Compare("COUNT", L"COUNT", 5) == true)
		exists = ParseFunctionCount(name, open);
    else
	if(name->Compare("DENSE_RANK", L"DENSE_RANK", 10) == true)
		exists = ParseFunctionDenseRank(name, open);
    else
	if(name->Compare("DECODE", L"DECODE", 6) == true)
		exists = ParseFunctionDecode(name, open);
    else
	if(name->Compare("DEREF", L"DEREF", 5) == true)
		exists = ParseFunctionDeref(name, open);
    else
	if(name->Compare("EMPTY_BLOB", L"EMPTY_BLOB", 10) == true)
		exists = ParseFunctionEmptyBlob(name, open);
	else
	if(name->Compare("EMPTY_CLOB", L"EMPTY_CLOB", 10) == true)
		exists = ParseFunctionEmptyClob(name, open);
	else
	if(name->Compare("EMPTY_DBCLOB", L"EMPTY_DBCLOB", 12) == true)
		exists = ParseFunctionEmptyDbclob(name, open);
	else
	if(name->Compare("EMPTY_NCLOB", L"EMPTY_NCLOB", 11) == true)
		exists = ParseFunctionEmptyNclob(name, open);
    else
	if(name->Compare("EXP", L"EXP", 3) == true)
		exists = ParseFunctionExp(name, open);
    else
	if(name->Compare("EXTRACT", L"EXTRACT", 7) == true)
		exists = ParseFunctionExtract(name, open);
    else
	if(name->Compare("FLOOR", L"FLOOR", 5) == true)
		exists = ParseFunctionFloor(name, open);
    else
	if(name->Compare("GREATEST", L"GREATEST", 8) == true)
		exists = ParseFunctionGreatest(name, open);
    else
	if(name->Compare("HEXTORAW", L"HEXTORAW", 8) == true)
		exists = ParseFunctionHextoraw(name, open);

	if(exists)
	{
		name->type = TOKEN_FUNCTION;
		name->open= open;
		name->close = GetLastToken();
        FUNC_STATS(name)

        if(_stats != nullptr)
			_stats->LogFuncCall(name, GetLastToken(), _option_cur_file);
		
		return exists;
	}

	if(name->Compare("INITCAP", L"INITCAP", 4) == true)
		exists = ParseFunctionInitcap(name, open);
    else
	if(name->Compare("INSTR", L"INSTR", 5) == true)
		exists = ParseFunctionInstr(name, open);
	else
	if(name->Compare("INSTRB", L"INSTRB", 6) == true)
		exists = ParseFunctionInstrb(name, open);
	else
	if(name->Compare("INTEGER", L"INTEGER", 7) == true ||
		name->Compare("INT", L"INT", 3) == true)
		exists = ParseFunctionInteger(name, open);
    else
	if(name->Compare("LAST_DAY", L"LAST_DAY", 8) == true)
		exists = ParseFunctionLastDay(name, open);
    else
	if(name->Compare("LEAST", L"LEAST", 5) == true)
		exists = ParseFunctionLeast(name, open);
    else
	if(name->Compare("LENGTH", L"LENGTH", 6) == true)
		exists = ParseFunctionLength(name, open);
	else
	if(name->Compare("LENGTHB", L"LENGTHB", 7) == true)
		exists = ParseFunctionLengthb(name, open);
    else
	if(name->Compare("LN", L"LN", 2) == true)
		exists = ParseFunctionLn(name, open);
    else
	if(name->Compare("LOG", L"LOG", 3) == true)
		exists = ParseFunctionLog(name, open);
    else
	if(name->Compare("LOWER", L"LOWER", 5) == true)
		exists = ParseFunctionLower(name, open);
	else
	if(name->Compare("LPAD", L"LPAD", 4) == true)
		exists = ParseFunctionLpad(name, open);
	else
	if(name->Compare("LTRIM", L"LTRIM", 5) == true)
		exists = ParseFunctionLtrim(name, open);
    else
	if(name->Compare("MOD", L"MOD", 3) == true)
		exists = ParseFunctionMod(name, open);
    else
	if(name->Compare("MONTHS_BETWEEN", L"MONTHS_BETWEEN", 14) == true)
		exists = ParseFunctionMonthsBetween(name, open);
    else
	if(name->Compare("NEXT_DAY", L"NEXT_DAY", 8) == true)
		exists = ParseFunctionNextDay(name, open);
    else
	if(name->Compare("NULLIF", L"NULLIF", 6) == true)
		exists = ParseFunctionNullif(name, open);
    else
	if(name->Compare("NVL", L"NVL", 3) == true)
		exists = ParseFunctionNvl(name, open);
	else
	if(name->Compare("NVL2", L"NVL2", 4) == true)
		exists = ParseFunctionNvl2(name, open);
    else
	if(name->Compare("POWER", L"POWER", 5) == true)
		exists = ParseFunctionPower(name, open);
    else
	if(name->Compare("RANK", L"RANK", 4) == true)
		exists = ParseFunctionRank(name, open);
    else
	if(name->Compare("REGEXP_SUBSTR", L"REGEXP_SUBSTR", 13) == true)
		exists = ParseFunctionRegexpSubstr(name, open);
	else
	if(name->Compare("REMAINDER", L"REMAINDER", 9) == true)
		exists = ParseFunctionRemainder(name, open);
    else
	if(name->Compare("REPLACE", L"REPLACE", 7) == true)
		exists = ParseFunctionReplace(name, open);
    else
	if(name->Compare("REVERSE", L"REVERSE", 7) == true)
		exists = ParseFunctionReverse(name, open);
    else
	if(name->Compare("ROUND", L"ROUND", 5) == true)
		exists = ParseFunctionRound(name, open);
    else
	if(name->Compare("ROW_NUMBER", L"ROW_NUMBER", 10) == true)
		exists = ParseFunctionRowNumber(name, open);
	else
	if(name->Compare("RPAD", L"RPAD", 4) == true)
		exists = ParseFunctionRpad(name, open);
	else
	if(name->Compare("RTRIM", L"RTRIM", 5) == true)
		exists = ParseFunctionRtrim(name, open);

	if(exists)
	{
		name->type = TOKEN_FUNCTION;
		name->open= open;
		name->close = GetLastToken();
        FUNC_STATS(name)

        if(_stats != nullptr)
			_stats->LogFuncCall(name, GetLastToken(), _option_cur_file);

		return exists;
	}

	if(name->Compare("SIGN", L"SIGN", 4) == true)
		exists = ParseFunctionSign(name, open);
	else
	if(name->Compare("SIN", L"SIN", 3) == true)
		exists = ParseFunctionSin(name, open);
	else
	if(name->Compare("SINH", L"SINH", 4) == true)
		exists = ParseFunctionSinh(name, open);
    else
	if(name->Compare("SOUNDEX", L"SOUNDEX", 7) == true)
		exists = ParseFunctionSoundex(name, open);
    else
	if(name->Compare("SQRT", L"SQRT", 4) == true)
		exists = ParseFunctionSqrt(name, open);
    else
	if(name->Compare("SUBSTR", L"SUBSTR", 6) == true)
		exists = ParseFunctionSubstr(name, open);
	else
	if(name->Compare("SUBSTR2", L"SUBSTR2", 7) == true)
		exists = ParseFunctionSubstr2(name, open);
	else
	if(name->Compare("SUBSTRB", L"SUBSTRB", 7) == true)
		exists = ParseFunctionSubstrb(name, open);
    else
	if(name->Compare("SUM", L"SUM", 3) == true)
		exists = ParseFunctionSum(name, open);
    else
	if(name->Compare("SYS_GUID", L"SYS_GUID", 8) == true)
		exists = ParseFunctionSysGuid(name, open);
	else
	if(name->Compare("TAN", L"TAN", 3) == true)
		exists = ParseFunctionTan(name, open);
	else
	if(name->Compare("TANH", L"TANH", 4) == true)
		exists = ParseFunctionTanh(name, open);
    else
	if(name->Compare("TO_CHAR", L"TO_CHAR", 7) == true)
		exists = ParseFunctionToChar(name, open);
	else
	if(name->Compare("TO_CLOB", L"TO_CLOB", 7) == true)
		exists = ParseFunctionToClob(name, open);
	else
	if(name->Compare("TO_DATE", L"TO_DATE", 7) == true)
		exists = ParseFunctionToDate(name);
    else
	if(name->Compare("TO_LOB", L"TO_LOB", 6) == true)
		exists = ParseFunctionToLob(name, open);
	else
	if(name->Compare("TO_NCHAR", L"TO_NCHAR", 8) == true)
		exists = ParseFunctionToNchar(name);
    else
	if(name->Compare("TO_NUMBER", L"TO_NUMBER", 9) == true)
		exists = ParseFunctionToNumber(name, open);
	else
	if(name->Compare("TO_SINGLE_BYTE", L"TO_SINGLE_BYTE", 14) == true)
		exists = ParseFunctionToSingleByte(name, open);
	else
	if(name->Compare("TO_TIMESTAMP", L"TO_TIMESTAMP", 12) == true)
		exists = ParseFunctionToTimestamp(name, open);
    else
	if(name->Compare("TRANSLATE", L"TRANSLATE", 9) == true)
		exists = ParseFunctionTranslate(name, open);
	else
	if(name->Compare("TRIM", L"TRIM", 4) == true)
		exists = ParseFunctionTrim(name, open);	
	else
	if(name->Compare("TRUNC", L"TRUNC", 5) == true)
		exists = ParseFunctionTrunc(name);	
    else
	if(name->Compare("UNISTR", L"UNISTR", 6) == true)
		exists = ParseFunctionUnistr(name, open);	
	else
	if(name->Compare("UPPER", L"UPPER", 5) == true)
		exists = ParseFunctionUpper(name, open);	
    else
	if(name->Compare("USERENV", L"USERENV", 7) == true)
		exists = ParseFunctionUserenv(name, open);	
    else
	if(name->Compare("VSIZE", L"VSIZE", 5) == true)
		exists = ParseFunctionVsize(name, open);	
    else
	if(name->Compare("XMLAGG", L"XMLAGG", 6) == true)
		exists = ParseFunctionXmlagg(name, open);	
	else
	if(name->Compare("XMLATTRIBUTES", L"XMLATTRIBUTES", 13) == true)
		exists = ParseFunctionXmlattributes(name, open);	
	else
	if(name->Compare("XMLCAST", L"XMLCAST", 7) == true)
		exists = ParseFunctionXmlcast(name, open);	
	else
	if(name->Compare("XMLCDATA", L"XMLCDATA", 8) == true)
		exists = ParseFunctionXmlcdata(name, open);	
	else
	if(name->Compare("XMLCOMMENT", L"XMLCOMMENT", 10) == true)
		exists = ParseFunctionXmlcomment(name, open);	
	else
	if(name->Compare("XMLCONCAT", L"XMLCONCAT", 9) == true)
		exists = ParseFunctionXmlconcat(name, open);	
	else
	if(name->Compare("XMLDIFF", L"XMLDIFF", 7) == true)
		exists = ParseFunctionXmldiff(name, open);	
    else
	if(name->Compare("XMLELEMENT", L"XMLELEMENT", 10) == true)
		exists = ParseFunctionXmlelement(name, open);	
    else
	if(name->Compare("XMLFOREST", L"XMLFOREST", 9) == true)
		exists = ParseFunctionXmlforest(name, open);	
    else
	if(name->Compare("XMLISVALID", L"XMLISVALID", 10) == true)
		exists = ParseFunctionXmlisvalid(name, open);	
	else
	if(name->Compare("XMLNAMESPACES", L"XMLNAMESPACES", 13) == true)
		exists = ParseFunctionXmlnamespaces(name, open);	
	else
	if(name->Compare("XMLPARSE", L"XMLPARSE", 8) == true)
		exists = ParseFunctionXmlparse(name, open);	
	else
	if(name->Compare("XMLPATCH", L"XMLPATCH", 8) == true)
		exists = ParseFunctionXmlpatch(name, open);	
	else
	if(name->Compare("XMLPI", L"XMLPI", 5) == true)
		exists = ParseFunctionXmlpi(name, open);	
	else
	if(name->Compare("XMLQUERY", L"XMLQUERY", 8) == true)
		exists = ParseFunctionXmlquery(name, open);	
    else
	if(name->Compare("XMLROOT", L"XMLROOT", 7) == true)
		exists = ParseFunctionXmlroot(name, open);	
    else
	if(name->Compare("XMLSEQUENCE", L"XMLSEQUENCE", 11) == true)
		exists = ParseFunctionXmlsequence(name, open);	
	else
	if(name->Compare("XMLSERIALIZE", L"XMLSERIALIZE", 12) == true)
		exists = ParseFunctionXmlserialize(name, open);	
    else
	if(name->Compare("XMLTRANSFORM", L"XMLTRANSFORM", 12) == true)
		exists = ParseFunctionXmltransform(name, open);	
	else
		udt_exists = ParseUnknownFunction(name, open);

	if(exists || udt_exists)
    {
		name->type = TOKEN_FUNCTION;
		name->open= open;
		name->close = GetLastToken();

		if(udt_exists)
        {
            UDF_FUNC_STATS(name)
            exists = true;
        }
        else
		{
            FUNC_STATS(name)

            if(_stats != nullptr)
				_stats->LogFuncCall(name, GetLastToken(), _option_cur_file);
		}
    }
	
	return exists;
}

// Unknown function (probably use-defined)
bool SqlParser::ParseUnknownFunction(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	// Handle comma separated expressions until )
	while(true)
	{
		Token *next = GetNextToken();

        if(next == nullptr || next->Compare(')', L')') == true)
			break;

		ParseExpression(next);

		// Must be comma
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	// Must be close (
	Token *close = GetNextCharToken(')', L')');

    return (close != nullptr) ? true : false;
}

// Parse function without parameters
bool SqlParser::ParseFunctionWithoutParameters(Token *name)
{
    if(name == nullptr)
		return false;

	STATS_DECL

	bool exists = false;

	if(name->Compare("CURRENT", L"CURRENT", 7) == true)
		exists = ParseFunctionCurrent(name);
	else
	if(name->Compare("CURRENT_DATE", L"CURRENT_DATE", 12) == true)
		exists = ParseFunctionCurrentDate(name);
	else
	if(name->Compare("CURRENT_SCHEMA", L"CURRENT_SCHEMA", 14) == true)
		exists = ParseFunctionCurrentSchema(name);
    else
	if(name->Compare("CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17) == true)
		exists = ParseFunctionCurrentTimestamp(name);
    else
	if(name->Compare("INTERVAL", L"INTERVAL", 8) == true)
		exists = ParseFunctionInterval(name);
	else
	if(name->Compare("LOCALTIMESTAMP", L"LOCALTIMESTAMP", 14) == true)
		exists = ParseFunctionLocaltimestamp(name);
    else
	if(name->Compare("NULL", L"NULL", 4) == true)
		exists = ParseFunctionNull(name);
    else
	if(name->Compare("SQLCODE", L"SQLCODE", 7) == true)
		exists = ParseFunctionSqlcode(name);
    else
	// SQL%ROWCOUNT
	if(name->Compare("SQL", L"SQL", 3) == true)
		exists = ParseFunctionSqlPercent(name);
	else
	if(name->Compare("SYSDATE", L"SYSDATE", 7) == true)
		exists = ParseFunctionSysdate(name);
    else
	if(name->Compare("SYSTIMESTAMP", L"SYSTIMESTAMP", 12) == true)
		exists = ParseFunctionSystimestamp(name);	
    else
	if(name->Compare("USER", L"USER", 4) == true)
		exists = ParseFunctionUser(name);

    if(exists)
	{
        FUNC_STATS(name)
		name->type = TOKEN_FUNCTION;
	}

	return exists;
}

// ABS
bool SqlParser::ParseFunctionAbs(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ GetNextCharToken(')', L')');

	return true;
}

// ACOS
bool SqlParser::ParseFunctionAcos(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// ADD_MONTHS
bool SqlParser::ParseFunctionAddMonths(Token *name)
{
    if(name == nullptr)
		return false;

	Token *date = GetNextToken();

	// Parse date
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *num = GetNextToken();

	// Parse number of months
	ParseExpression(num);

    /*Token *end_num =*/(void) GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// ASCII
bool SqlParser::ParseFunctionAscii(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	name->data_type = TOKEN_DT_STRING;
	
	return true;
}

// ASCIISTR
bool SqlParser::ParseFunctionAsciistr(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// ASIN
bool SqlParser::ParseFunctionAsin(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// ATAN
bool SqlParser::ParseFunctionAtan(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// ATAN2
bool SqlParser::ParseFunctionAtan2(Token *name)
{
    if(name == nullptr)
		return false;

	Token *num1 = GetNextToken();

	// Parse first numeric expression
	ParseExpression(num1);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *num2 = GetNextToken();

	// Parse second numeric expression
	ParseExpression(num2);

    /*Token *end_num2 */(void) GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// BIN_TO_NUM
bool SqlParser::ParseFunctionBinToNum(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *bit = GetNextToken();

    if(bit == nullptr)
		return false;

	// Parse first bit expression
	ParseExpression(bit);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *bitn = GetNextToken();

        if(bitn == nullptr)
			break;

		// Parse next bit expression
		ParseExpression(bitn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// BITAND
bool SqlParser::ParseFunctionBitand(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

		Token::Remove(name);
		Token::Change(comma, " &", L" &", 2);
	return true;
}

// BITNOT
bool SqlParser::ParseFunctionBitnot(Token *name, Token * /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	/*Token *comma */ (void) GetNextCharToken(',', L',');

	Token *exp2 = GetNextToken();

    if(exp2 == nullptr)
		return false;

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// CAST function
bool SqlParser::ParseFunctionCast(Token *cast)
{
    if(cast == nullptr)
		return false;

	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *as = GetNextWordToken("AS", L"AS", 2);

    if(as == nullptr)
		return false;

	Token *type = GetNextToken();

	// Parse cast data type
	ParseDataType(type, SQL_SCOPE_CASE_FUNC);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	// CAST AS VARCHAR
	if(type->Compare("VARCHAR", L"VARCHAR", 7) == true)
	{

        Token::Change(cast, "TO_CHAR", L"TO_CHAR", 7);
        Token::Remove(as, type);

	}
	else
	// CAST AS DATE
	if(type->Compare("DATE", L"DATE", 4) == true)
	{

        TokenStr format(", ", L", ", 2);

        // If string litral is specified try to recognize format, and convert to TO_DATE
        if(exp->type == TOKEN_STRING && RecognizeOracleDateFormat(exp, format) == true)
        {
            Token::Change(cast, "TO_DATE", L"TO_DATE", 7);

            AppendNoFormat(exp, &format);

            Token::Remove(as, type);
        }

	}

	return true;
}

// CEIL
bool SqlParser::ParseFunctionCeil(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// Get dollar sign if exists
	Token *sign = GetNextCharToken('$', L'$');
	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Remove dollar sign for CEIL
    Token::Remove(sign);

	return true;
}

// CHR
bool SqlParser::ParseFunctionChr(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	Token *usng = GetNextWordToken("USING", L"USING", 5);
    Token *nchar = nullptr;

    // USING NCHAR_CS in optional
	if (usng != nullptr) {
		nchar = GetNextWordToken("NCHAR_CS", L"NCHAR_CS", 8);
		Token::Remove(usng, nchar);
	}

	Token *close = GetNextCharToken(')', L')');

    // Convert to CHAR

    Token::Change(name, "CHAR", L"CHAR", 4);
    Prepend(close, " USING ASCII", L" USING ASCII", 12, name);

	return true;
}

// COALESCE
bool SqlParser::ParseFunctionCoalesce(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// CONCAT
bool SqlParser::ParseFunctionConcat(Token *concat, Token *open)
{
    if(concat == nullptr)
		return false;

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma1 = GetNextCharToken(',', L',');

    if(comma1 == nullptr)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	int params = 2;

    // Optional varying list of expressions
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);

        // Supports only 2 parameters in CONCAT, replace function with || operator
        Token::Change(comma, " ||", L" ||", 3);

		params++;
	}

	Token *close = GetNextCharToken(')', L')');

	// Replace function with || 
    if(params > 2)
	{
		Token::Remove(concat, open);
		Token::Remove(close);

		Token::Change(comma1, " ||", L" ||", 3);
	}

	return true;
}

// CONVERT
bool SqlParser::ParseFunctionConvert(Token *name)
{
    if(name == nullptr)
		return false;

    // CONVERT has a different meaning than in other databases
    Token *comma1 = GetNextCharToken(',', L',');

    if(comma1 == nullptr)
        return false;

    /*Token *dest_charset */ (void) GetNextToken();
    Token *comma2 = GetNextCharToken(',', L',');
    Token *source_charset = nullptr;

    if(comma2 != nullptr)
        source_charset = GetNextToken();

    /*Token *close */ (void) GetNextCharToken(')', L')');

    // Change syntax
    if(comma2 == nullptr)
    {
        Token::Change(comma1, " USING", L" USING", 6);
    }

	return true;
}

// COS
bool SqlParser::ParseFunctionCos(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// COSH
bool SqlParser::ParseFunctionCosh(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	Token *end_num = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

    Prepend(name, "(", L"(", 1);
    Token::Change(name, "EXP", L"EXP", 3);
    Append(close, " + EXP(-(", L" + EXP(-(", 9, name);
    AppendCopy(close, num, end_num);
    Append(close, ")))/2", L")))/2", 5);

	return true;
}

// AVG aggregate and analytic function
bool SqlParser::ParseFunctionAvg(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// Optional DISTINCT keyword
	/*Token *distinct */ (void) GetNext("DISTINCT", L"DISTINCT", 8);

	// Column, constant or expression
	Token *col = GetNextToken();

    if(col == nullptr)
		return false;

	ParseExpression(col);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

    if(over != nullptr)
		ParseAnalyticFunctionOverClause(over);

	return true;
}

// COUNT aggregate and analytic function
bool SqlParser::ParseFunctionCount(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// Optional DISTINCT keyword
	/*Token *distinct */ (void) GetNext("DISTINCT", L"DISTINCT", 8);

	// Column, constant or *
	Token *col = GetNextToken();

    if(col == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

    if(over != nullptr)
		ParseAnalyticFunctionOverClause(over);

	return true;
}

// SUM aggregate and analytic function
bool SqlParser::ParseFunctionSum(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// Optional DISTINCT keyword
	/*Token *distinct */ (void) GetNext("DISTINCT", L"DISTINCT", 8);

	// Column, constant or expression
	Token *col = GetNextToken();

    if(col == nullptr)
		return false;

	ParseExpression(col);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

    if(over != nullptr)
		ParseAnalyticFunctionOverClause(over);

	return true;
}

// CURRENT DATE, CURRENT SERVER, CURRENT SCHEMA, CURRENT SQLID, CURRENT TIME, CURRENT TIMESTAMP, CURRENT USER
bool SqlParser::ParseFunctionCurrent(Token *name)
{
    if(name == nullptr)
		return false;

	Token *second = GetNextToken();

    if(second == nullptr)
		return false;

	bool exists = false;

	// CURRENT SCHEMA
	if(second->Compare("SCHEMA", L"SCHEMA", 6) == true)
	{
        Token::Change(name, "CURRENT_SCHEMA", L"CURRENT_SCHEMA", 14);
        Token::Remove(second);

		exists = true;
	}
    else
	// CURRENT SQLID
	if(second->Compare("SQLID", L"SQLID", 5) == true)
	{
        Token::Change(name, "CURRENT_SQLID", L"CURRENT_SQLID", 13);
        Token::Remove(second);

		exists = true;
	}
	else
	// CURRENT TIMESTAMP
	if(second->Compare("TIMESTAMP", L"TIMESTAMP", 9) == true)
	{
        // Convert to SYSTIMESTAMP
        Token::Change(second, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);
        Token::Remove(name);

        // Convert to CURRENT_TIMESTAMP
        Token::Change(second, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
        Token::Remove(name);

		name->data_type = TOKEN_DT_DATETIME;
        name->nullable = false;

		exists = true;
	}
	else
	// CURRENT TIME
	if(second->Compare("TIME", L"TIME", 4) == true)
	{
        Token::Change(name, "CONVERT(TIME, GETDATE())", L"CONVERT(TIME, GETDATE())", 24);
        Token::Remove(second);

        Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);
        Token::Remove(second);

		exists = true;
	}
	else
	// CURRENT USER
	if(second->Compare("USER", L"USER", 4) == true)
	{
        Token::Remove(name);

		exists = true;
	}

	return exists;
}

// CURRENT_DATE
bool SqlParser::ParseFunctionCurrentDate(Token *name)
{
    if(name == nullptr)
		return false;

    // Convert to TRUNC(SYSDATE)
    Token::Change(name, "TRUNC(SYSDATE)", L"TRUNC(SYSDATE)", 14);

    // Convert CURRENT_DATE to NOW()
    Token::Change(name, "NOW()", L"NOW()", 5);

	return true;
}

// CURRENT_SCHEMA
bool SqlParser::ParseFunctionCurrentSchema(Token *name)
{
    if(name == nullptr)
		return false;

	return true;
}

// CURRENT_TIMESTAMP
bool SqlParser::ParseFunctionCurrentTimestamp(Token *name)
{
    if(name == nullptr)
		return false;

    // Convert to SYSTIMESTAMP
    Token::Change(name, "SYSTIMESTAMP", L"SYSTIMESTAMP", 12);

    // Convert CURRENT_DATE to NOW()
    Token::Change(name, "NOW()", L"NOW()", 5);

	name->data_type = TOKEN_DT_DATETIME;
	name->nullable = false;

	return true;
}

// INTERVAL
bool SqlParser::ParseFunctionInterval(Token *name)
{
    if(name == nullptr)
		return false;
	
    //variable or expression
	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// DAY i.e.
	Token *unit = GetNextToken();

	bool exp_num = exp->IsNumeric();

    // INTERVAL 'num' DAY, or NUMTODSINTERVAL(var, 'DAY')
    // Number constant must be quoted
    if(exp_num == true)
    {
        PrependNoFormat(exp, "'", L"'", 1);
        AppendNoFormat(exp, "'", L"'", 1);
    }
    // Variable or expression
    else
    {
        Prepend(exp, "NUMTODSINTERVAL(", L"NUMTODSINTERVAL(", 16, name);

        AppendNoFormat(exp, ",", L",", 1);
        PrependNoFormat(unit, "'", L"'", 1);
        AppendNoFormat(unit, "')", L"')", 2);

        Token::Remove(name);
    }

	return true;
}

// NULL value
bool SqlParser::ParseFunctionNull(Token *name)
{
    if(name == nullptr)
		return false;

	return true;
}

// DENSE_RANK function
bool SqlParser::ParseFunctionDenseRank(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

    if(over != nullptr)
		ParseAnalyticFunctionOverClause(over);
	
	return true;
}

// DECODE
bool SqlParser::ParseFunctionDecode(Token *decode, Token *open)
{
    if(decode == nullptr || open == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Evaluate expression
	ParseExpression(exp);

    // Replace with CASE
    Token::Change(decode, "CASE ", L"CASE ", 5);
    Token::Remove(open);

	ListW whens;

	bool null_exists = false;

	// Handle conditions
	while(true)
	{
		Token *comma1 = GetNextCharToken(',', L',');

        if(comma1 == nullptr)
			break;

		// WHEN condition (or ELSE if there is no corresponding THEN)
		Token *when = GetNextToken();

        if(when == nullptr)
			break;

		ParseExpression(when);

		Token *comma2 = GetNextCharToken(',', L',');

		// It is ELSE condition
        if(comma2 == nullptr)
		{
            Prepend(when, " ELSE ", L" ELSE ", 6, decode);
            Token::Remove(comma1);
			
			break;
		}

		// Check is WHEN condition contain NULL in DECODE
		if(when->Compare("NULL", L"NULL", 4) == true)
			null_exists = true;

		whens.Add(when);

		// Change commas to WHEN THEN
        Prepend(when, " WHEN ", L" WHEN ", 6, decode);
        Token::Remove(comma1);
        Token::Change(comma2, " THEN ", L" THEN ", 6, decode);

		Token *then = GetNextToken();

        if(then == nullptr)
			break;

		ParseExpression(then);
	} 

	Token *close = GetNextCharToken(')', L')');

    Token::Change(close, " END", L" END", 4, decode);

	// Check if we need to use searched form
	if(null_exists == true)
	{
		// Remove expression after case
		Token::Remove(exp);

		ListwItem *cur = whens.GetFirst();

		// Go through all WHEN conditions again
        while(cur != nullptr)
		{
			Token *when = (Token*)cur->value;

			PrependCopy(when, exp);

			// add 'exp IS' before NULL, and exp = in other cases
			if(when->Compare("NULL", L"NULL", 4) == true)
				Prepend(when, " IS ", L" IS ", 4);
			else
				Prepend(when, " = ", L" = ", 3);

			cur = cur->next;
		}
	}

	return true;
}

// DEREF
bool SqlParser::ParseFunctionDeref(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// EMPTY_BLOB()
bool SqlParser::ParseFunctionEmptyBlob(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	return true;	
}

// EMPTY_CLOB()
bool SqlParser::ParseFunctionEmptyClob(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	return true;	
}

// EMPTY_DBCLOB()
bool SqlParser::ParseFunctionEmptyDbclob(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
    // Convert to EMPTY_CLOB()
    Token::Change(name, "EMPTY_CLOB", L"EMPTY_CLOB", 10);

	return true;	
}

// EMPTY_NCLOB()
bool SqlParser::ParseFunctionEmptyNclob(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
    // Convert to EMPTY_CLOB()
    Token::Change(name, "EMPTY_CLOB", L"EMPTY_CLOB", 10);

	return true;	
}

// EXP
bool SqlParser::ParseFunctionExp(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// EXTRACT
bool SqlParser::ParseFunctionExtract(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *unit = GetNextToken();

	// FROM keyword after unit
	Token *from = GetNextWordToken("FROM", L"FROM", 4);
	
    if(from == nullptr)
		return false;

	Token *datetime = GetNextToken();

	// Parse datetime
	ParseExpression(datetime);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	if(Token::Compare(unit, "YEAR", L"YEAR", 4) == true)
	{
        // Convert to YEAR function
        Token::Change(name, "YEAR", L"YEAR", 4);
			
        Token::Remove(unit, false);
        Token::Remove(from, false);
	}
	else
	if(Token::Compare(unit, "MONTH", L"MONTH", 5) == true)
	{
        // Convert to MONTH function
        Token::Change(name, "MONTH", L"MONTH", 5);
			
        Token::Remove(unit, false);
        Token::Remove(from, false);
	}
	else
	if(Token::Compare(unit, "DAY", L"DAY", 3) == true)
	{
        // Convert to DAY function
        Token::Change(name, "DAY", L"DAY", 3);
			
        Token::Remove(unit, false);
        Token::Remove(from, false);
	}
	else
	if(Token::Compare(unit, "HOUR", L"HOUR", 4) == true)
	{
        // Convert to HOUR function
        Token::Change(name, "HOUR", L"HOUR", 4);
			
        Token::Remove(unit, false);
        Token::Remove(from, false);
	}
	else
	if(Token::Compare(unit, "MINUTE", L"MINUTE", 6) == true)
	{
        // Convert to MINUTE function
        Token::Change(name, "MINUTE", L"MINUTE", 6);
			
        Token::Remove(unit, false);
        Token::Remove(from, false);
	}
	else
	if(Token::Compare(unit, "SECOND", L"SECOND", 6) == true)
	{
        // Convert to SECOND function
        Token::Change(name, "SECOND", L"SECOND", 6);
			
        Token::Remove(unit, false);
        Token::Remove(from, false);
	}

	return true;
}

// FLOOR
bool SqlParser::ParseFunctionFloor(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// Get dollar sign if exists
	Token *sign = GetNextCharToken('$', L'$');
	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Remove dollar sign FLOOR
    if(sign != nullptr)
        Token::Remove(sign);

	return true;
}

// GREATEST
bool SqlParser::ParseFunctionGreatest(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// HEXTORAW
bool SqlParser::ParseFunctionHextoraw(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// INITCAP
bool SqlParser::ParseFunctionInitcap(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// INSTR
bool SqlParser::ParseFunctionInstr(Token *name, Token *open)
{
    if(name == nullptr)
		return false;

	Token *source = GetNextToken();

    if(source == nullptr)
		return false;

	// Parse source
	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *search = GetNextToken();

    if(search == nullptr)
		return false;

	// Parse search
	ParseExpression(search);

	Token *end_search = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
    Token *start = nullptr;
    Token *comma3 = nullptr;
    Token *instance = nullptr;
	
    // Start is optional
    if(comma2 != nullptr)
	{
		start = GetNextToken();

        if(start == nullptr)
			return false;

		// Parse start
		ParseExpression(start);

		comma3 = GetNextCharToken(',', L',');
	
        // Instance is optional
        if(comma3 != nullptr)
		{
			instance = GetNextToken();

            if(instance == nullptr)
				return false;

			// Parse instance
			ParseExpression(instance);
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // For 3 parameters, convert to LOCATE
    if(start != nullptr && instance == nullptr)
    {
        Token::Change(name, "LOCATE", L"LOCATE", 6);
        AppendCopy(open, search, end_search);
        Append(open, ", ", L", ", 2);

        Token::Remove(comma, end_search);
    }

	name->subtype = TOKEN_SUB_FUNC_STRING;

	return true;
}

// INSTRB
bool SqlParser::ParseFunctionInstrb(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *source = GetNextToken();

    if(source == nullptr)
		return false;

	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *search = GetNextToken();

    if(search == nullptr)
		return false;

	ParseExpression(search);

	Token *comma2 = GetNextCharToken(',', L',');
	
    if(comma2 == nullptr)
		return false;

	Token *start = GetNextToken();

    if(start == nullptr)
		return false;

	ParseExpression(start);

	Token *comma3 = GetNextCharToken(',', L',');
	
    if(comma3 == nullptr)
		return false;

	Token *instance = GetNextToken();

    if(instance == nullptr)
		return false;

	ParseExpression(instance);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// INT function
bool SqlParser::ParseFunctionInteger(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *close = GetNextCharToken(')', L')');

    // Convert to TO_NUMBER
    Token::Change(name, "TRUNC(TO_NUMBER", L"TRUNC(TO_NUMBER", 15);
    Append(close, ")", L")", 1);

    TOKEN_CHANGE(name, "CAST");
    PREPEND_FMT(close, " AS INT", name);

	return true;
}

// LAST_DAY
bool SqlParser::ParseFunctionLastDay(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *date = GetNextToken();

    if(date == nullptr)
		return false;

	// Parse date
	ParseExpression(date);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// LEAST
bool SqlParser::ParseFunctionLeast(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// LENGTH
bool SqlParser::ParseFunctionLength(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to CHAR_LENGTH
    Token::Change(name, "CHAR_LENGTH", L"CHAR_LENGTH", 11);

	return true;
}

// LENGTHB
bool SqlParser::ParseFunctionLengthb(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to LENGTH
    Token::Change(name, "LENGTH", L"LENGTH", 6);

	return true;
}

// LN
bool SqlParser::ParseFunctionLn(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// LOCALTIMESTAMP
bool SqlParser::ParseFunctionLocaltimestamp(Token *name)
{
    if(name == nullptr)
		return false;

	Token *open = GetNextCharToken('(', L'(');
    Token *prec = nullptr;

	// Prec is optional
    if(open != nullptr)
	{
		prec = GetNextToken();

        if(prec == nullptr)
			return false;

		// Parse prec
		ParseExpression(prec);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Remove prec
    if(prec != nullptr)
            Token::Remove(prec);

	return true;
}

// LOG
bool SqlParser::ParseFunctionLog(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num1 = GetNextToken();

    if(num1 == nullptr)
		return false;

	// Parse num1
	ParseExpression(num1);

	Token *comma = GetNextCharToken(',', L',');
    Token *num2 = nullptr;

    // Num2 is optional
    if(comma != nullptr)
	{
		num2 = GetNextToken();

        if(num2 == nullptr)
			return false;

		// Parse num2
		ParseExpression(num2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');


	return true;
}

// LOWER
bool SqlParser::ParseFunctionLower(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// LPAD
bool SqlParser::ParseFunctionLpad(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *len = GetNextToken();

	// Parse len
	ParseExpression(len);

	Token *end_len = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
    Token *pad = nullptr;
	
    // Pad is optional
    if(comma2 != nullptr)
	{
		pad = GetNextToken();

		// Parse pad
		ParseExpression(pad);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Add third parameter
    if(pad == nullptr)
        Append(end_len, ", ' '", L", ' '", 5);

	return true;
}

// LTRIM
bool SqlParser::ParseFunctionLtrim(Token *name, Token *open)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
    Token *set = nullptr;
    Token *end_set = nullptr;
	
    // Set is optional
    if(comma != nullptr)
	{
		set = GetNextToken();

		// Parse set
		ParseExpression(set);

		end_set = GetLastToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to TRIM
    if(set != nullptr)
    {
        Token::Change(name, "TRIM", L"TRIM", 4);
        Append(open, "LEADING ", L"LEADING ", 8, name);
        AppendCopy(open, set, end_set);
        Append(open, " FROM ", L"FROM ", 6, name);

        Token::Remove(comma, end_set);
    }

	name->data_type = TOKEN_DT_STRING;

	return true;
}

// MOD
bool SqlParser::ParseFunctionMod(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *dividend = GetNextToken();

    if(dividend == nullptr)
		return false;

	// Parse dividend
	ParseExpression(dividend);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *divisor = GetNextToken();

    if(divisor == nullptr)
		return false;

	// Parse divisor
	ParseExpression(divisor);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// MONTHS_BETWEEN
bool SqlParser::ParseFunctionMonthsBetween(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *date1 = GetNextToken();

    if(date1 == nullptr)
		return false;

	// Parse first date
	ParseExpression(date1);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *date2 = GetNextToken();

    if(date2 == nullptr)
		return false;

	// Parse second date
	ParseExpression(date2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;	
}

// NEXT_DAY
bool SqlParser::ParseFunctionNextDay(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *date = GetNextToken();

    if(date == nullptr)
		return false;

	// Parse date
	ParseExpression(date);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *weekday = GetNextToken();

    if(weekday == nullptr)
		return false;

	// Parse weekday
	ParseExpression(weekday);

	/*Token *close */ (void) GetNextCharToken(')', L')');
		
	return true;	
}

// NULLIF
bool SqlParser::ParseFunctionNullif(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// NVL
bool SqlParser::ParseFunctionNvl(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// First expression
	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	int num = 2;

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // 2 parameters
	if(num == 2)
	{
        // Convert to IFNULL
        Token::Change(name, "IFNULL", L"IFNULL", 6);
	}

	return true;	
}

// NVL2
bool SqlParser::ParseFunctionNvl2(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp1 = GetNextToken();

	// Parse first expression
	ParseExpression(exp1);

	/*Token *end_exp1 */ (void) GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *exp2 = GetNextToken();

	// Parse second expression
	ParseExpression(exp2);

	Token *comma2 = GetNextCharToken(',', L',');
	
    if(comma2 == nullptr)
		return false;

	Token *exp3 = GetNextToken();

	// Parse third expression
	ParseExpression(exp3);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// POWER
bool SqlParser::ParseFunctionPower(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *value = GetNextToken();

	// Parse value
	ParseExpression(value);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *power = GetNextToken();

	// Parse power
	ParseExpression(power);

	/*Token *close */ (void) GetNextCharToken(')', L')');
		
	return true;	
}

// RANK analytic function
bool SqlParser::ParseFunctionRank(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

    if(over != nullptr)
		ParseAnalyticFunctionOverClause(over);

	return true;
}

// ROW_NUMBER analytic function
bool SqlParser::ParseFunctionRowNumber(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');
	
	// OVER keyword
	Token *over = GetNextWordToken("OVER", L"OVER", 4);

    if(over != nullptr)
		ParseAnalyticFunctionOverClause(over);

	return true;
}

// REGEXP_SUBSTR
bool SqlParser::ParseFunctionRegexpSubstr(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma1 = GetNextCharToken(')', L')');

    if(comma1 == nullptr)
		return false;

	Token *pattern = GetNextToken();

	// Parse pattern
	ParseExpression(pattern);

	Token *comma2 = GetNextCharToken(')', L')');
    Token *start = nullptr;
    Token *comma3 = nullptr;
    Token *occurrence = nullptr;
    Token *comma4 = nullptr;
    Token *escape = nullptr;

	// Start is optional
    if(comma2 != nullptr)
	{
		start = GetNextToken();

		// Parse start
		ParseExpression(start);

		comma3 = GetNextCharToken(')', L')');

		// If start is specified, optional occurrence can be specified
        if(comma3 != nullptr)
		{
			occurrence = GetNextToken();

			// Parse occurence
			ParseExpression(occurrence);

			comma4 = GetNextCharToken(')', L')');

			// If occurrence is specified, optional escape can be specified
            if(comma4 != nullptr)
			{
				escape = GetNextToken();

				// Parse escape
				ParseExpression(escape);
			}
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// REMAINDER
bool SqlParser::ParseFunctionRemainder(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *dividend = GetNextToken();

	// Parse dividend
	ParseExpression(dividend);

	/*Token *end_dividend */ (void) GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *divisor = GetNextToken();

	// Parse divisor
	ParseExpression(divisor);

	/*Token *end_divisor */ (void) GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to MOD
    Token::Change(name, "MOD", L"MOD", 3);

	return true;
}

// REPLACE
bool SqlParser::ParseFunctionReplace(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *source = GetNextToken();

	// Parse source
	ParseExpression(source);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *search = GetNextToken();

	// Parse search
	ParseExpression(search);

	Token *end_search = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
    Token *replace = nullptr;

    // Replace is optional
    if(comma2 != nullptr)
	{
		replace = GetNextToken();

		// Parse replace
		ParseExpression(replace);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Add third parameter
    if(replace == nullptr)
        Append(end_search, ", ''", L", ''", 4);

	return true;
}

// REVERSE
bool SqlParser::ParseFunctionReverse(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// ROUND
bool SqlParser::ParseFunctionRound(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
    Token *exp2 = nullptr;
	
    // Second expression is optional
    if(comma != nullptr)
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// RPAD
bool SqlParser::ParseFunctionRpad(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *len = GetNextToken();

	// Parse len
	ParseExpression(len);

	Token *end_len = GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
    Token *pad = nullptr;
	
    // Pad is optional
    if(comma2 != nullptr)
	{
		pad = GetNextToken();

		// Parse pad
		ParseExpression(pad);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Add third parameter
    if(pad == nullptr)
        Append(end_len, ", ' '", L", ' '", 5);

	return true;
}

// RTRIM
bool SqlParser::ParseFunctionRtrim(Token *name, Token *open)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
    Token *set = nullptr;
    Token *end_set = nullptr;
	
    // Set is optional
    if(comma != nullptr)
	{
		set = GetNextToken();

		// Parse set
		ParseExpression(set);

		end_set = GetLastToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to TRIM
    if(set != nullptr)
    {
        Token::Change(name, "TRIM", L"TRIM", 4);
        Append(open, "TRAILING ", L"TRAILING ", 9, name);
        AppendCopy(open, set, end_set);
        Append(open, " FROM ", L"FROM ", 6, name);

        Token::Remove(comma, end_set);
    }

	name->data_type = TOKEN_DT_STRING;

	return true;
}

// SIGN
bool SqlParser::ParseFunctionSign(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// SIN
bool SqlParser::ParseFunctionSin(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// SINH
bool SqlParser::ParseFunctionSinh(Token *name, Token *open)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	Token *end_num = GetLastToken();
	Token *close = GetNextCharToken(')', L')');

    Token::Remove(name);
    Append(open, "EXP(", L"EXP(", 4, name);
    Append(end_num, ") - EXP(-(", L") - EXP(-(", 10, name);
    AppendCopy(end_num, num, end_num);
    Append(close, "))/2", L"))/2", 4);

	return true;
}

// SOUNDEX
bool SqlParser::ParseFunctionSoundex(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// SQLCODE
bool SqlParser::ParseFunctionSqlcode(Token *name)
{
    if(name == nullptr)
		return false;

	return true;
}

// SQL%ROWCOUNT
bool SqlParser::ParseFunctionSqlPercent(Token *name)
{
    if(name == nullptr)
		return false;

	Token *cent = GetNext('%', L'%');

    if(cent == nullptr)
		return false;

	Token *rowcount = GetNext("ROWCOUNT", L"ROWCOUNT", 8);

    // SQL%ROWCOUNT
    if(rowcount != nullptr)
	{
        TOKEN_CHANGE(cent, "@@");
        Token::Remove(name);
	}
	
	return true;
}

// SQRT
bool SqlParser::ParseFunctionSqrt(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// SUBSTR
bool SqlParser::ParseFunctionSubstr(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

    /*Token *string_end =*/(void) GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *pos = GetNextToken();

	// Parse pos
	ParseExpression(pos);

    /*Token *pos_end */(void) GetLastToken();
	Token *comma2 = GetNextCharToken(',', L',');
	
	// Length expression is optional
    if(comma2 != nullptr)
	{
		Token *length = GetNextToken();

		// Parse length
		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	name->data_type = TOKEN_DT_STRING;

	return true;
}

// SUBSTR2
bool SqlParser::ParseFunctionSubstr2(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *start = GetNextToken();

    if(start == nullptr)
		return false;

	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	
	// Length expression is optional
    if(comma2 != nullptr)
	{
		Token *length = GetNextToken();

        if(length == nullptr)
			return false;

		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// SUBSTRB
bool SqlParser::ParseFunctionSubstrb(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *start = GetNextToken();

    if(start == nullptr)
		return false;

	ParseExpression(start);

	Token *comma2 = GetNextCharToken(',', L',');
	
	// Length expression is optional
    if(comma2 != nullptr)
	{
		Token *length = GetNextToken();

        if(length == nullptr)
			return false;

		ParseExpression(length);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// SYS_GUID returns GUID without -
bool SqlParser::ParseFunctionSysGuid(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *close = GetNextCharToken(')', L')');

    // Convert to UUID and trim -
    Token::Change(name, "REPLACE(UUID", L"REPLACE(UUID", 12);
    AppendNoFormat(close, ", '-', '')", L", '-', '')", 10);

	return true;
}

// SYSDATE
bool SqlParser::ParseFunctionSysdate(Token *name)
{
    if(name == nullptr)
		return false;

	Token *open = GetNextCharToken('(', L'(');
    Token *close = nullptr;

    // SYSDATE() is only allowed
    if(open != nullptr)
		close = GetNextCharToken(')', L')');

    // Convert to SYSDATE
    if(open != nullptr)
		Token::Remove(open, close);
    else
    // Convert to SYSDATE()
    if(open == nullptr)
		Append(name, "()", L"()", 2);

	name->data_type = TOKEN_DT_DATETIME;
	name->nullable = false;

	return true;
}

// SYSTIMESTAMP
bool SqlParser::ParseFunctionSystimestamp(Token *name)
{
    if(name == nullptr)
		return false;

    // Convert to CURRENT_TIMESTAMP
    Token::Change(name, "CURRENT_TIMESTAMP", L"CURRENT_TIMESTAMP", 17);

	name->data_type = TOKEN_DT_DATETIME;

	return true;
}

// TAN
bool SqlParser::ParseFunctionTan(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	// Parse num
	ParseExpression(num);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// TANH
bool SqlParser::ParseFunctionTanh(Token *name, Token *open)
{
    if(name == nullptr)
		return false;

	Token *num = GetNextToken();

    if(num == nullptr)
		return false;

	ParseExpression(num);

	Token *close = GetNextCharToken(')', L')');

    Token::Change(name, "(EXP(2*", L"(EXP(2*", 7);
    Append(close, ") - 1) / (EXP(2*", L") - 1) / (EXP(2*", 16, name);
    AppendCopy(close, open, close);
    Append(close, ") + 1)", L") + 1)", 6);

	return true;
}

// TO_CHAR
bool SqlParser::ParseFunctionToChar(Token *name, Token *open)
{
    if(name == nullptr)
		return false;

	// First expression
	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
    Token *format = nullptr;

	// Optional format
    if(comma != nullptr)
	{
		format = GetNextToken();

		ParseExpression(format);
	}

	Token *close = GetNextCharToken(')', L')');

	// Format is specified and it is a string literal
    if(format != nullptr && format->type == TOKEN_STRING)
	{
		// $ and number groups separated by , 
		if(format->Compare("'FM$9,999,999'", L"'FM$9,999,999'", 14) == true)
		{
            // CONCAT('$', FORMAT(num, 0))
            Token::Change(name, "CONCAT('$', FORMAT", L"CONCAT('$', FORMAT", 18);
            Token::Change(format, "0", L"0", 1);
            Append(close, ")", L")", 1);
		}
		else
		// Number groups separated by , 
		if(format->Compare("'9,999,999'", L"'9,999,999'", 11) == true)
		{
            // FORMAT(num, 0)
            Token::Change(name, "FORMAT", L"FORMAT", 6);
            Token::Change(format, "0", L"0", 1);
		}
        else
		// Process each format specifier
		{
			TokenStr out;
			size_t i = 0;

			bool datetime = false;

			while(i < format->len)
			{
				// YYYY or RRRR part (4-digit year)
				if(format->Compare("YYYY", L"YYYY", i, 4) == true || 
					format->Compare("RRRR", L"RRRR", i, 4) == true)
				{
					datetime = true;

					i += 4;
					continue;
				}
				else
				// RR part (2-digit year rounded)
				if(format->Compare("RR", L"RR", i, 2) == true)
				{
                    out.Append("%y", L"%y", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// MM month (01-12)
				if(format->Compare("MM", L"MM", i, 2) == true)
				{
                    out.Append("%m", L"%m", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// MON month (Jan-Dec)
				if(format->Compare("MON", L"MON", i, 3) == true)
				{
                    out.Append("%b", L"%b", 2);

					datetime = true;

					i += 3;
					continue;
				}
				else
				// DD month (01-31)
				if(format->Compare("DD", L"DD", i, 2) == true)
				{
					out.Append("%d", L"%d", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// HH24 hour 
				if(format->Compare("HH24", L"HH24", i, 4) == true)
                {
					out.Append("%H", L"%H", 2);

					datetime = true;

					i += 4;
					continue;
				}
				else
				// MI minute 
				if(format->Compare("MI", L"MI", i, 2) == true)
                {
					out.Append("%i", L"%i", 2);

					datetime = true;

					i += 2;
					continue;
				}
				else
				// SS second 
				if(format->Compare("SS", L"SS", i, 2) == true)
                {
					out.Append("%s", L"%s", 2);

					datetime = true;

					i += 2;
					continue;
				}

				// No format matched just copy
				out.Append(format, i, 1);
				i++;
			}

			// A datetime format was specified
			if(datetime == true)
			{
                Token::Change(name, "DATE_FORMAT", L"DATE_FORMAT", 11);
			}
			
			// Set target format
			Token::ChangeNoFormat(format, out);
		}

        if(_stats != nullptr)
        {
            TokenStr st;
            st.Append(name);
            st.Append(open);
            st.Append("value, ", L"value, ", 7);
            st.Append(format);
            st.Append(")", L")", 1);

            FUNC_DTL_STATS(&st)
        }
	}

	name->data_type = TOKEN_DT_STRING;

	// If first argument is non-nullable, set the function result non-nullable
	if(exp->nullable == false)
		name->nullable = false;

	return true;
}

// TO_CLOB
bool SqlParser::ParseFunctionToClob(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
    Token *exp2 = nullptr;
	
	// Second expression is optional
    if(comma != nullptr)
	{
		exp2 = GetNextToken();

        if(exp2 == nullptr)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Second parameter is removed
    Token::Remove(comma);
    Token::Remove(exp2);

	return true;
}

// TO_DATE
bool SqlParser::ParseFunctionToDate(Token *name)
{
    if(name == nullptr)
		return false;

	// First expression
	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
    Token *format = nullptr;

	// Optional format
    if(comma != nullptr)
	{
		format = GetNextToken();

		ParseExpression(format);
	}

	Token *comma2 = GetNextCharToken(',', L',');
    Token *nls = nullptr;

	// Optional NLS settings
    if(comma2 != nullptr)
	{
		nls = GetNextToken();

		ParseExpression(nls);
	}

	Token *close = GetNextCharToken(')', L')');


    if(format != nullptr)
        TOKEN_CHANGE(name, "STR_TO_DATE");
    else
    {
        TOKEN_CHANGE(name, "CAST");
        PREPEND_NOFMT(close, " AS DATETIME");
    }

	// Format is specified and it is a string literal, process each part
    if(format != nullptr && format->type == TOKEN_STRING)
	{
		// Process each format specifier
        TokenStr out;
        size_t i = 0;

        while(i < format->len)
        {
            // YYYY or RRRR part (4-digit year)
            if(format->Compare("YYYY", L"YYYY", i, 4) == true ||
                format->Compare("RRRR", L"RRRR", i, 4) == true)
            {
               out.Append("%Y", L"%Y", 2);

                i += 4;
                continue;
            }
            else
            // RR part (2-digit year rounded)
            if(format->Compare("RR", L"RR", i, 2) == true)
            {
                out.Append("%y", L"%y", 2);

                i += 2;
                continue;
            }
            else
            // MM month (01-12)
            if(format->Compare("MM", L"MM", i, 2) == true)
            {
                out.Append("%m", L"%m", 2);

                i += 2;
                continue;
            }
            else
            // DD month (01-31)
            if(format->Compare("DD", L"DD", i, 2) == true)
            {
                out.Append("%d", L"%d", 2);

                i += 2;
                continue;
            }
            else
            // HH24 hour
            if(format->Compare("HH24", L"HH24", i, 4) == true)
            {
                out.Append("%H", L"%H", 2);

                i += 4;
                continue;
            }
            else
            // MI minutes
            if(format->Compare("MI", L"MI", i, 2) == true)
            {
                out.Append("%i", L"%i", 2);

                i += 2;
                continue;
            }
            else
            // SS seconds
            if(format->Compare("SS", L"SS", i, 2) == true)
            {
                out.Append("%s", L"%s", 2);

                i += 2;
                continue;
            }

            // No format matched just copy
            out.Append(format, i, 1);
            i++;
        }

        // Set target format
        Token::ChangeNoFormat(format, out);
	}

	name->data_type = TOKEN_DT_DATETIME;
		
	return true;
}

// TO_LOB
bool SqlParser::ParseFunctionToLob(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;	
}

// TO_NCHAR
bool SqlParser::ParseFunctionToNchar(Token *name)
{
    if(name == nullptr)
		return false;

	Token *exp1 = GetNextToken();

    if(exp1 == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp1);

	Token *comma = GetNextCharToken(',', L',');
    Token *exp2 = nullptr;
	
	// Second expression is optional
    if(comma != nullptr)
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// TO_NUMBER
bool SqlParser::ParseFunctionToNumber(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
    Token *exp2 = nullptr;
	
	// Second expression is optional
    if(comma != nullptr)
	{
		exp2 = GetNextToken();

        if(exp2 == nullptr)
			return false;

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// TO_SINGLE_BYTE
bool SqlParser::ParseFunctionToSingleByte(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// TO_TIMESTAMP
bool SqlParser::ParseFunctionToTimestamp(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
	
	// Second expression is optional
    if(comma != nullptr)
	{
		Token *exp2 = GetNextToken();

        if(exp2 == nullptr)
			return false;

		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// TRANSLATE
bool SqlParser::ParseFunctionTranslate(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

	// Parse string
	ParseExpression(string);

	Token *comma = GetNextCharToken(',', L',');
	
    if(comma == nullptr)
		return false;

	Token *to = GetNextToken();

	// Parse to
	ParseExpression(to);
		
	Token *comma2 = GetNextCharToken(',', L',');

    if(comma2 == nullptr)
		return false;

	Token *from = GetNextToken();

	// Parse from
	ParseExpression(from);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// TRIM
bool SqlParser::ParseFunctionTrim(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *type = GetNextToken();

    // Type is optional
	if(Token::Compare(type, "LEADING", L"LEADING", 7) == false &&
		Token::Compare(type, "TRAILING", L"TRAILING", 8) == false &&
		Token::Compare(type, "BOTH", L"BOTH", 4) == false &&
		Token::Compare(type, "L", L"L", 1) == false &&
		Token::Compare(type, "T", L"T", 1) == false &&
		Token::Compare(type, "B", L"B", 1) == false)
	{
		PushBack(type);
        type = nullptr;
	}

	Token *trim = GetNextToken();
    Token *from = nullptr;

	if(Token::Compare(trim, "FROM", L"FROM", 4))
	{
		from = trim;
        trim = nullptr;
	}
	else
        // Parse trim
		ParseExpression(trim);

    if(from == nullptr)
		from = GetNextWordToken("FROM", L"FROM", 4);
	
    Token *string = nullptr;

    // If FROM is not set then trim is also not set
    if(from == nullptr)
	{
		string = trim;
        trim = nullptr;
	}

    if(string == nullptr)
		string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

    /*Token *close */(void) GetNextCharToken(')', L')');

    // Does not support L, T, and B types
    if(Token::Compare(type, "L", L"L", 1))
        Token::Change(type, "LEADING", L"LEADING", 7);
    else
    if(Token::Compare(type, "T", L"T", 1))
        Token::Change(type, "TRAILING", L"TRAILING", 8);
    else
    if(Token::Compare(type, "B", L"B", 1))
        Token::Change(type, "BOTH", L"BOTH", 4);

	return true;
}

// TRUNC
bool SqlParser::ParseFunctionTrunc(Token *name)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *end_exp = GetLastToken();
	Token *comma = GetNextCharToken(',', L',');
    Token *unit = nullptr;

	// Truncation unit is optional
    if(comma != nullptr)
	{
		unit = GetNextToken();

		// Parse trancation unit
		ParseExpression(unit);
	}

    /*Token *close */(void) GetNextCharToken(')', L')');

	// Default datetime truncation
    if(unit == nullptr && exp->data_type == TOKEN_DT_DATETIME)
	{
        // Convert to DATE
        Token::Change(name, "DATE", L"DATE", 4);
	}
	else
	// Unit is specified and it is a string
    if(unit != nullptr && unit->type == TOKEN_STRING)
	{
		// Set return data type 
		name->data_type = TOKEN_DT_DATETIME;

		// 'DD' truncate to day
		if(unit->Compare("'DD'", L"'DD'", 4) == true)
		{
            // Convert to DATE
            Token::Change(name, "DATE", L"DATE", 4);

            Token::Remove(comma);
            Token::Remove(unit);
		}
		else
		// 'MM' truncate to month
		if(unit->Compare("'MM'", L"'MM'", 4) == true)
		{
            // DATE_FORMAT
            Token::Change(name, "DATE_FORMAT", L"DATE_FORMAT", 11);
            Token::ChangeNoFormat(unit, "'%Y-%m-01'", L"'%Y-%m-01'", 10);
		}
	}
	else
	{
        // Convert to TRUNCATE
        Token::Change(name, "TRUNCATE", L"TRUNCATE", 8);

        // Add second parameter
        if(unit == nullptr)
            Append(end_exp, ", 0", L", 0", 3);
	}

	return true;
}

// UNISTR
bool SqlParser::ParseFunctionUnistr(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	Token *end_string = GetLastToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

    // Convert to CHAR
    Token::Change(name, "CHAR", L"CHAR", 4);
    Append(end_string, " USING UCS2", L" USING UCS2", 11, name);

	return true;
}

// UPPER
bool SqlParser::ParseFunctionUpper(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *string = GetNextToken();

    if(string == nullptr)
		return false;

	// Parse string
	ParseExpression(string);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// USER
bool SqlParser::ParseFunctionUser(Token *name)
{
    if(name == nullptr)
		return false;

    // Add parentheses
    AppendNoFormat(name, "()", L"()", 2);

	name->nullable = false;

	return true;
}

// USERENV
bool SqlParser::ParseFunctionUserenv(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *parameter = GetNextToken();

    if(parameter == nullptr)
		return false;

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// VSIZE
bool SqlParser::ParseFunctionVsize(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', ')');

	return true;
}

// XMLAGG
bool SqlParser::ParseFunctionXmlagg(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *instance = GetNextToken();

    if(instance == nullptr)
		return false;

	// Parse instance
	ParseExpression(instance);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLATTRIBUTES
bool SqlParser::ParseFunctionXmlattributes(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	// Variable number of parameters
	while(true)
	{
		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		ParseExpression(expn);

		// Optional AS name
		Token *as = GetNextWordToken("AS", L"AS", 2);
	
        if(as != nullptr)
		{
			Token *name = GetNextToken();

            if(name == nullptr)
				return false;
		}

		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLCAST
bool SqlParser::ParseFunctionXmlcast(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	Token *as = GetNextWordToken("AS", L"AS", 2);

    if(as == nullptr)
		return false;

	/*Token *datatype */ (void) GetNextToken();
	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLCDATA
bool SqlParser::ParseFunctionXmlcdata(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLCOMMENT
bool SqlParser::ParseFunctionXmlcomment(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLCONCAT
bool SqlParser::ParseFunctionXmlconcat(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLDIFF
bool SqlParser::ParseFunctionXmldiff(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *doc = GetNextToken();

	// Parse first document
	ParseExpression(doc);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *doc2 = GetNextToken();

	// Parse second document
	ParseExpression(doc2);

	Token *comma2 = GetNextCharToken(',', L',');
    Token *integer = nullptr;
    Token *comma3 = nullptr;
    Token *string = nullptr;

	// Integer and string are optional
    if(comma2 != nullptr)
	{
		integer = GetNextToken();

		// Parse integer
		ParseExpression(integer);

		comma3 = GetNextCharToken(',', L',');

        if(comma3 == nullptr)
			return false;

		string = GetNextToken();

		// Parse string
		ParseExpression(string);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLELEMENT
bool SqlParser::ParseFunctionXmlelement(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *keyword = GetNextWordToken("NAME", L"NAME", 4);

    if(keyword == nullptr)
		return false;

	Token *element = GetNextToken();

	// Parse element
	ParseExpression(element);

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLFOREST
bool SqlParser::ParseFunctionXmlforest(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse first expression
	ParseExpression(exp);

	Token *as = GetNextWordToken("AS", L"AS", 2);
    Token *alias = nullptr;

	// Alias is optional
    if(as != nullptr)
		alias = GetNextToken();

	// Variable number of parameters
	while(true)
	{
		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;

		Token *expn = GetNextToken();

        if(expn == nullptr)
			break;

		// Parse next expression
		ParseExpression(expn);

		Token *asn = GetNextWordToken("AS", L"AS", 2);

		// Alias is optional
        if(asn != nullptr)
			/*Token *aliasn */ (void) GetNextToken();
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLISVALID
bool SqlParser::ParseFunctionXmlisvalid(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *instance = GetNextToken();

    if(instance == nullptr)
		return false;

	// Parse instance
	ParseExpression(instance);

	Token *comma = GetNextCharToken(',', L',');
    Token *url = nullptr;
    Token *comma2 = nullptr;
    Token *element = nullptr;

	// Url is optional
    if(comma != nullptr)
	{
		url = GetNextToken();

		// Parse url
		ParseExpression(url);

		comma2 = GetNextCharToken(',', L',');

		// Element is optional
        if(comma2 != nullptr)
		{
			element = GetNextToken();

			// Parse element
			ParseExpression(element);
		}
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLNAMESPACES
bool SqlParser::ParseFunctionXmlnamespaces(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;
	
	// DEFAULT or NO DEFAULT can be specified as single parameter
	Token *no = GetNextWordToken("NO", L"NO", 2); 
	Token *def = GetNextWordToken("DEFAULT", L"DEFAULT", 7); 
	
    if(no == nullptr && def != nullptr)
	{
		Token *uri = GetNextToken();

		ParseExpression(uri);
	}

	while(true)
	{
        if(def != nullptr)
			break;
		
		Token *uri = GetNextToken();

        if(uri == nullptr)
			break;

		ParseExpression(uri);

		Token *as = GetNextWordToken("AS", L"AS", 2);

        if(as == nullptr)
			break;

		Token *prefix = GetNextToken();

        if(prefix == nullptr)
			break;

		ParseExpression(prefix);

		Token *comma = GetNextCharToken(',', L',');

        if(comma == nullptr)
			break;
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLPARSE
bool SqlParser::ParseFunctionXmlparse(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

    // DOCUMENT is mandatory
	/*Token *document */ (void) GetNextWordToken("DOCUMENT", L"DOCUMENT", 8);

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLPATCH
bool SqlParser::ParseFunctionXmlpatch(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *doc = GetNextToken();

	// Parse first document
	ParseExpression(doc);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *doc2 = GetNextToken();

	// Parse second document
	ParseExpression(doc2);
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLPI
bool SqlParser::ParseFunctionXmlpi(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *keyword = GetNextWordToken("NAME", L"NAME", 4);

    if(keyword == nullptr)
		return false;

	Token *identifier = GetNextToken();

	// Parse identifier
	ParseExpression(identifier);

	Token *comma = GetNextCharToken(',', L',');

	// Expression is optional
    if(comma != nullptr)
	{
		Token *exp = GetNextToken();

		// Parse expression
		ParseExpression(exp);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLQUERY
bool SqlParser::ParseFunctionXmlquery(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp_cons = GetNextToken();

    if(exp_cons == nullptr)
		return false;

	ParseExpression(exp_cons);

	// Passing is optional
	Token *passing = GetNextWordToken("PASSING", L"PASSING", 7);
    Token *by = nullptr;
    Token *ref = nullptr;
    Token *by2 = nullptr;
    Token *ref2 = nullptr;
    Token *returning = nullptr;
    Token *sequence = nullptr;
    Token *by3 = nullptr;
    Token *ref3 = nullptr;
	
    if(passing != nullptr)
	{
		// By ref is optional
		by = GetNextWordToken("BY", L"BY", 2);
        if(by != nullptr)
			ref = GetNextWordToken("REF", L"REF", 3);

		// Variable number of arguments
		while(true)
		{
			Token *expn = GetNextToken();

            if(expn == nullptr)
				break;

			ParseExpression(expn);

			Token *as = GetNextWordToken("AS", L"AS", 2);

            if(as == nullptr)
				break;

			Token *identifier = GetNextToken();

            if(identifier == nullptr)
				break;

			// By ref is optional
			by2 = GetNextWordToken("BY", L"BY", 2);
            if(by2 != nullptr)
				ref2 = GetNextWordToken("REF", L"REF", 3);

			Token *comma = GetNextCharToken(',', L',');

            if(comma == nullptr)
				break;
		}
	}

	// Returning sequence is optional
	returning = GetNextWordToken("RETURNING", L"RETURNING", 9);
    if(returning != nullptr)
	{
		sequence = GetNextWordToken("SEQUENCE", L"SEQUNCE", 7);

		// By ref is optional
		by3 = GetNextWordToken("BY", L"BY", 2);
        if(by3 != nullptr)
			ref3 = GetNextWordToken("REF", L"REF", 3);
	}

	// Empty on empty is optional
	Token *empty = GetNextWordToken("EMPTY", L"EMPTY", 5);
    if(empty != nullptr)
	{
		/*Token *on */ (void) GetNextWordToken("ON", L"ON", 2);
		/*Token *empty2 */ (void) GetNextWordToken("EMPTY", L"EMPTY", 5);
	}
	
	/*Token *close */ (void) GetNextCharToken(')', L')');

    Token::Change(ref, "VALUE", L"VALUE", 5);
    Token::Remove(by2, ref2);
    Token::Change(sequence, "CONTENT", L"CONTENT", 7);
    Token::Remove(by3, ref3);
    Token::Change(empty, "NULL", L"NULL", 4);

	return true;
}

// XMLROOT
bool SqlParser::ParseFunctionXmlroot(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

	// Parse first expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *version = GetNextWordToken("VERSION", L"VERSION", 7);

    if(version == nullptr)
		return false;

	Token *no = GetNextWordToken("NO", L"NO", 2);
    Token *exp2 = nullptr;
    Token *value = nullptr;

    if(no != nullptr)
	{
		value = GetNextWordToken("VALUE", L"VALUE", 5);

        if(value == nullptr)
			return false;
	}
	else
	{
		exp2 = GetNextToken();

		// Parse second expression
		ParseExpression(exp2);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLSEQUENCE
bool SqlParser::ParseFunctionXmlsequence(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *comma = GetNextCharToken(',', L',');
    Token *fmt = nullptr;

	// Fmt is optional
    if(comma != nullptr)
	{
		fmt = GetNextToken();

		// Parse fmt
		ParseExpression(fmt);
	}

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLSERIALIZE
bool SqlParser::ParseFunctionXmlserialize(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *exp = GetNextToken();

    if(exp == nullptr)
		return false;

	// Parse expression
	ParseExpression(exp);

	Token *datatype = GetNextToken();

	// Parse datatype
	ParseDataType(datatype, SQL_SCOPE_XMLSERIALIZE_FUNC);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}

// XMLTRANSFORM
bool SqlParser::ParseFunctionXmltransform(Token *name, Token* /*open*/)
{
    if(name == nullptr)
		return false;

	Token *instance = GetNextToken();

	// Parse instance
	ParseExpression(instance);

	Token *comma = GetNextCharToken(',', L',');

    if(comma == nullptr)
		return false;

	Token *exp = GetNextToken();

	// Parse expression
	ParseExpression(exp);

	/*Token *close */ (void) GetNextCharToken(')', L')');

	return true;
}
