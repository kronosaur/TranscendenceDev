//	DefPrimitives.h
//
//	Default primitives

static PRIMITIVEPROCDEF g_DefPrimitives[] =
	{
		{	"<",				fnEqualityNumerals,		FN_EQUALITY_LESSER,
			"(< [x1 x2 ... xn]) -> True if x1 < x2 < ... < xn",
			NULL,	0,	},

		{	"ls",				fnEquality,		FN_EQUALITY_LESSER,
			"(ls [x1 x2 ... xn]) -> True if x1 < x2 < ... < xn",
			NULL,	0,	},

		{	"<=",				fnEqualityNumerals,		FN_EQUALITY_LESSER_EQ,
			"(<= [x1 x2 ... xn]) -> True if x1 <= x2 <= ... <= xn",
			NULL,	0,	},

		{	"leq",				fnEquality,		FN_EQUALITY_LESSER_EQ,
			"(leq [x1 x2 ... xn]) -> True if x1 <= x2 <= ... <= xn",
			NULL,	0,	},

		{	">",				fnEqualityNumerals,		FN_EQUALITY_GREATER,
			"(> [x1 x2 ... xn]) -> True if x1 > x2 > ... > xn",
			NULL,	0,	},

		{	"gr",				fnEquality,		FN_EQUALITY_GREATER,
			"(gr [x1 x2 ... xn]) -> True if x1 > x2 > ... > xn",
			NULL,	0,	},

		{	">=",				fnEqualityNumerals,		FN_EQUALITY_GREATER_EQ,
			"(>= [x1 x2 ... xn]) -> True if x1 >= x2 >= ... >= xn",
			NULL,	0,	},

		{	"geq",				fnEquality,		FN_EQUALITY_GREATER_EQ,
			"(geq [x1 x2 ... xn]) -> True if x1 >= x2 >= ... >= xn",
			NULL,	0,	},

		{	"add",				fnMathListOld,		FN_MATH_ADD,
			"(add x1 [x2 ... xn]) -> sum (after converting arguments to int32)",
			"v*",	0,	},

		{	"+",				fnMathList,		FN_MATH_ADD,
			"(+ x1 [x2 ... xn]) -> sum (int32 or real)",
			"v*",	0,	},

		{	"abs",				fnMath,			FN_MATH_ABSOLUTE,
			"(abs x) -> absolute value (int32 or real)",
			"n",	0,	},

		{	"acos",				fnMathNumerals,	FN_MATH_ARCCOS,
			"(acos x ['degrees]) -> inverse cosine (real)",
			"n*",	0,	},

		{	"and",				fnLogical,		FN_LOGICAL_AND,
			"(and [exp1 exp2 ... expn]) -> True/Nil\n\n"

			"Returns Nil if any argument is Nil, otherwise returns last argument",
			NULL,	0,	},

		{	"append",			fnAppend,		0,
			"(append [list1 list2 ...]) -> concatenated list",
			"*",	0,	},

		{	"apply",			fnApply,		0,
			"(apply function [arg1 arg2 ... argn] argList) -> Result of function with arguments arg1... followed by contents of argList",
			NULL,	0,	},

		{	"asin",				fnMathNumerals,	FN_MATH_ARCSIN,
			"(asin x ['degrees]) -> inverse sine (real)",
			"n*",	0,	},

		{	"atan",				fnMathNumerals,	FN_MATH_ARCTAN,
			"(atan y [x] ['degrees]) -> inverse tangent (real)",
			"n*",	0,	},
	
		{	"atmAddEntry",		fnAtmTable,		FN_ATMTABLE_ADDENTRY,	"",		NULL,	PPFLAG_SIDEEFFECTS,	},
		{	"atmDeleteEntry",	fnAtmTable,		FN_ATMTABLE_DELETEENTRY,"",		NULL,	PPFLAG_SIDEEFFECTS,	},
		{	"atmList",			fnAtmTable,		FN_ATMTABLE_LIST,		"",		NULL,	0,	},
		{	"atmLookup",		fnAtmTable,		FN_ATMTABLE_LOOKUP,		"",		NULL,	0,	},
		{	"atmAtomTable",		fnAtmCreate,	0,						"",		NULL,	0,	},

		{	"block",			fnBlock,		FN_BLOCK_BLOCK,
			"(block [localsList exp1 exp2 ... expn]) -> value of expn\n\n"
			
			"Evaluates a series of expressions and returns the value of the last one.\n"
			"LocalsList is a list of local variables and/or lists consisting of a local variable and the initial value to assign it."
			"If an expression evaluates to an error, returns that error and doesn't evaluate the rest.",
			NULL,	0,	},

		{	"cat",				fnCat,			0,
			"(cat [string1 string2 ... stringn]) -> concatenated string",		NULL,	0,	},

		{	"ceil",				fnMathNumerals,		FN_MATH_CEIL,
			"(ceil x) -> x rounded up, i.e. towards positive infinity (real)",
			"n",	0,	},

		{	"convertTo",		fnItem,			FN_ITEM_CONVERT_TO,
			"(convertTo dataType value) -> result\n\n"
			
			"dataType: 'error|'int32|'list|'nil|'real|'string|'true",
			"sv",	0,	},

		{	"cos",				fnMathNumerals,	FN_MATH_COS,
			"(cos x ['degrees]) -> cosine (real)",
			"n*",	0,	},

		{	"count",			fnCount,		0,
			"(count list|string|struct) -> number of items or characters",
			NULL,	0,	},

		{	"divide",			fnMathOld,			FN_MATH_DIVIDE,
			"(divide x y) -> x / y after converting arguments to int32, rounded toward 0",		NULL,	0,	},

		{	"/",				fnMathNumerals,			FN_MATH_DIVIDE,
			"(/ x y) -> x / y (real)",		
			"nn",	0,	},

		{	"enum",				fnEnum,			0,
			"(enum list itemVar exp) -> value\n\n"

			"Iterate itemVar over list evaluating exp. Returns the last value of exp.",
			NULL,	0,	},

		{	"enumWhile",		fnEnum,			FN_ENUM_WHILE,
			"(enumWhile list condition itemVar exp) -> value\n\n"

			"Iterate itemVar over list evaluating exp while condition is True. Returns the last value of exp.",
			NULL,	0,	},

		{	"errBlock",			fnBlock,		FN_BLOCK_ERRBLOCK,
			"(errBlock localsList [exp1 exp2 ... expn] expErr) -> value of expn or expErr if error occurs\n\n"
			
			"Like block, except it evaluates and returns the last expression only if one of the other expressions evaluates to an error.\n"
			"LocalsList must contain at least one variable. If the first variable is not assigned within localsList, it will be set\n"
			"to the error while evaluating expErr.",
			NULL,	0,	},

		{	"error",			fnSpecial,		FN_ERROR,
			"(error msg) -> error",
			"s",	0,	},

		{	"eq",				fnEquality,		FN_EQUALITY_EQ,
			"(eq [x1 x2 ... xn]) -> True if all arguments are equal",
			NULL,	0,	},

		{	"=",				fnEqualityNumerals, FN_EQUALITY_EQ,
			"(= [x1 x2 ... xn]) -> True if all arguments are equal",
			NULL, 0, },

		{	"eval",				fnEval,			0,
			"(eval exp) -> result\n\n"
			
			"Evaluates the expression. If it's a string, result is the value of the corresponding symbol.\n"
			"If it's a quoted expression, the expression is evaluated.\n"
			"If it's a list, each item is evaluated, then it's evaluated as a function call.",
			NULL,	0,	},
		
		{	"filter",			fnFilter,		0,
			"(filter list var boolean-exp) -> filtered list\n\n"
			
			"Iterates var over list, evaluating boolean-exp. Returns a list containing the items for which boolean-exp is non-Nil.",
			"lqu",	0,	},

		{	"exp",				fnMathNumerals,	FN_MATH_EXP,
			"(exp x) -> e^x (real)",
			"n",	0,	},

		{	"find",				fnFind,			FN_FIND,
			"(find string target) -> position of target in string (0-based)\n"
			"(find source-list target ['ascending|'descending] [keyIndex]) -> position of target in source-list (0-based)\n\n"
			
			"The 'ascending or 'descending argument makes the function more efficient if the list is long and sorted in the specified direction.\n"
			"Specifying an integer keyIndex causes it to compare that item of each list within source-list with the target instead.\n",
			"vv*",	0,	},

		{	"floor",			fnMathNumerals,		FN_MATH_FLOOR,
			"(floor x) -> x rounded down, i.e. towards negative infinity (real)",
			"n",	0,	},

		{	"fncHelp",			fnItemInfo,		FN_ITEMINFO_HELP,
			"(fncHelp symbol) -> help on function symbol (works on deprecated functions)",
			NULL,	0,	},

		{	"for",				fnForLoop,		0,
			"(for var from to exp) -> value of last expression",
			NULL,	0,	},

		{	"help",				fnHelp,			0,
			"(help) -> this help\n"
			"(help '*) -> all functions\n"
			"(help 'partial-string) -> all functions starting with partial-string\n"
			"(help 'function-name) -> help on function-name\n",
			"*",	0,	},

		{	"hex",				fnItem,			FN_HEX,
			"(hex x [fieldSize]) -> hex-string\n"
			"(hex hex-string) -> value",
			"v*",	0,	},

		{	"if",				fnIf,			0,						"(if condition exp1 [exp2]) -> exp1 if condition True, otherwise exp2",		NULL,	0,	},
		{	"int",				fnItemInfo,		FN_ITEMINFO_ASINT,		"(int x) -> x as an integer",						NULL,	0,	},
		{	"double",			fnItemInfo,		FN_ITEMINFO_ASDOUBLE,	"(double x) -> x as a double",						NULL,	0,  },
		{	"isAtom",			fnItemInfo,		FN_ITEMINFO_ISATOM,		"(isAtom exp) -> True if exp is not a list",		NULL,	0,	},
		{	"isError",			fnItemInfo,		FN_ITEMINFO_ISERROR,	"(isError exp) -> True if exp is an error",			NULL,	0,	},
		{	"isInt",			fnItemInfo,		FN_ITEMINFO_ISINT,		"(isInt exp) -> True if exp is an integer",			NULL,	0,	},
		{	"isFunction",		fnItemInfo,		FN_ITEMINFO_ISFUNCTION,	"(isFunction exp) -> True if exp is a function",	NULL,	0,	},
		{	"isPrimitive",		fnItemInfo,		FN_ITEMINFO_ISPRIMITIVE,"(isPrimitive exp) -> True if exp is a primitive",	NULL,	0,	},

		{	"@",				fnItem,			FN_ITEM,
			"(@ list index) -> item index from list (0-based)\n"
			"(@ struct key) -> value corresponding to key from struct",
			"v*",	0,	},

		{	"item",				fnItem,			FN_ITEM,
			"DEPRECATED: Alias of @",
			"vv",	0,	},

		{	"join",				fnItem,			FN_JOIN,
			"(join list [separator]) -> Concatenates items of list into a string, with separator in between each.\n"
            "(join list 'oxfordComma) -> 'a' ; 'a and b' ; 'a, b, and c' ; etc.",
			"v*",	0,	},

		{	"lambda",			fnLambda,		0,
			"(lambda args-list exp) -> lambda function",
			NULL,	0,	},

		{	"log",				fnMathNumerals,	FN_MATH_LOG,
			"(log x [base]) -> Logarithm of x (real). Base defaults to e.",
			"n*",	0,	},

		{	"lookup",			fnFind,			FN_LOOKUP,
			"(lookup source target ['ascending|'descending] [keyIndex]) -> Like find, but returns the matching item rather than the index.",
			"vv*",	0,	},

		{	"loop",				fnLoop,			0,
			"(loop condition exp) -> Evaluate exp until condition is Nil",
			NULL,	0,	},

		{	"link",				fnLink,			0,						"",		"s",	0,	},
		{	"list",				fnList,			FN_LIST,
			"(list [i1 i2 ... in]) -> list",
			"*",	0,	},

		{	"lnkAppend",		fnLinkedListAppend,	0,	
			"(lnkAppend list item) -> Modifies list by appending item to the end and returns the result.",
			"uv",	PPFLAG_SIDEEFFECTS,	},

		{	"lnkRemove",		fnLinkedList,	FN_LINKEDLIST_REMOVE,
			"(lnkRemove list index) -> Modifies list by removing the specified item (0-based) and returns the result.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"lnkRemoveNil",		fnLinkedList,	FN_LINKEDLIST_REMOVE_NIL,
			"(lnkRemoveNil list) -> Modifies list by removing all Nil items and returns the result.",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"lnkReplace",		fnLinkedList,	FN_LINKEDLIST_REPLACE,
			"(lnkReplace list index item) -> list",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"make",				fnList,			FN_MAKE,
			"(make 'duplicates count entry) -> list of n entries\n"
			"(make 'sequence count) -> list from 1 to count\n"
			"(make 'sequence start end [inc]) -> list from start to end, incrementing by inc",
			"s*",	0,	},

		{	"map",				fnMap,			0,
			"(map list ['excludeNil|'original|'reduceMax|'reduceMin|'reduceAverage|'reduceSum] var exp) -> list",
			"l*qu",	0,	},

		{	"match",			fnMatch,			0,
			"(match list var boolean-exp) -> first item that matches",
			"lqu",	0,	},

		{	"max",				fnMathList,			FN_MATH_MAX,
			"(max x1 x2 ... xn) -> z",
			"v*",	0,	},

		{	"min",				fnMathList,			FN_MATH_MIN,
			"(min x1 x2 ... xn) -> z",
			"v*",	0,	},

		{	"modulo",			fnMath,			FN_MATH_MODULUS,
			"(modulo ['degrees] x y) -> z",
			"*",	0,	},

		{	"mod",			fnMathNumerals,			FN_MATH_MODULUS_NUMERALS,
			"(mod ['degrees] x y) -> z",
			"*",	0,	},

		{	"multiply",			fnMathListOld,		FN_MATH_MULTIPLY,
			"(multiply x1 x2 ... xn) -> z",
			"v*",	0,	},

		{	"*",				fnMathList,			FN_MATH_MULTIPLY,
			"(* x1 x2 ... xn) -> z",
			"v*",	0,	},

		{	"neq",				fnEquality,		FN_EQUALITY_NEQ,
			"(neq x1 x2 ... xn) -> True if any arguments are not equal",
			NULL,	0,	},

		{	"!=",				fnEqualityNumerals,	FN_EQUALITY_NEQ,
			"(!= x1 x2 ... xn) -> True if any arguments are not equal",
			NULL, 0, },

		{	"not",				fnLogical,		FN_LOGICAL_NOT,
			"(not exp) -> True/Nil",
			NULL,	0,	},

		{	"or",				fnLogical,		FN_LOGICAL_OR,
			"(or exp1 exp2 ... expn) -> True/Nil\n\n"

			"Returns first non-Nil argument",
			NULL,	0,	},

		{	"power",			fnMathFractions,		FN_MATH_POWER,
			"(power x y) -> z",
			"vv",	0,	},

		{	"pow",				fnMathNumerals,		FN_MATH_POWER_NUMERALS,
			"(pow x y) -> z",
			"nn",	0,	},

		{	"regex",			fnRegEx,			0,
			"(regex source pattern ['offset|'subex]) -> result",
			"ss*",	0,	},

		{	"round",			fnMathNumerals,		FN_MATH_ROUND,
			"(round ['stochastic] x) -> y",
			"*n",	0,	},

		{	"quote",			fnSpecial,			FN_QUOTE,
			"(quote exp) -> unevaluated exp",
			"u",	0,	},

		{	"random",			fnRandom,		FN_RANDOM,
			"(random from to)\n"
			"(random list)\n"
			"(random) -> random number between 0 and 1",
			"*",	0,	},

		{	"randomGaussian",		fnRandom,		FN_RANDOM_GAUSSIAN,
			"(randomGaussian low mid high) -> random number between low and high",
			"iii",	0,	},

		{	"randomTable",		fnRandomTable,		0,
			"(randomTable chance1 exp1 chance2 exp2 ... chancen expn) -> exp",
			NULL,	0,	},

		{	"seededRandom",		fnSeededRandom,		0,
			"(seededRandom seed from to)\n(seededRandom seed list)",
			"i*",	0,	},

		{	"set",				fnSet,			FN_SET_SET,
			"(set string value) -> value",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"set@",				fnItem,			FN_SET_ITEM,
			"(set@ list-var index value) -> list\n"
			"(set@ struct-var key value) -> struct\n"
			"(set@ struct-var struct) -> merged structs",
			"uv*",	PPFLAG_SIDEEFFECTS,	},

		{	"setItem",			fnItem,			FN_SET_ITEM,
			"DEPRECATED: Alias of set@",
			"uvv",	PPFLAG_SIDEEFFECTS,	},

		{	"setq",				fnSet,			FN_SET_SETQ,
			"(setq variable value)",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"shuffle",			fnShuffle,		0,
			"(shuffle list) -> shuffled list",
			"l",	0,	},

		{	"sin",				fnMathNumerals,	FN_MATH_SIN,
			"(sin x ['degrees]) -> z",
			"n*",	0,	},

		{	"sort",				fnSort,			FN_SORT,
			"(sort list-of-atoms ['ascending|'descending]) -> sorted list\n"
			"(sort list-of-lists ['ascending|'descending] [index to sort on]) -> sorted list\n"
			"(sort list-of-structs ['ascending|'descending] [field to sort on]) -> sorted list\n",
			"v*",	0,	},

		{	"split",			fnSplit,		0,
			"(split string [characters]) -> list",
			"s*",	0,	},

		{	"sqrt",				fnMath,			FN_MATH_SQRT,
			"(sqrt x) -> integer z",
			"v",	0,	},

		{	"sqrtn",				fnMathNumerals,			FN_MATH_SQRT_NUMERALS,
			"(sqrtn x) -> real z",
			"n",	0,	},

		{	"strCapitalize",	fnStrCapitalize,0,
			"(strCapitalize string) -> string",
			NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"strFind",			fnStrFind,		0,
			"(strFind string target) -> pos of target in string (0-based)",
			"ss",	0,	},

		{	"struct",			fnStruct,		FN_STRUCT,
			"(struct key1 value1 [ key2 value2 ...]) -> struct\n"
            "(struct (key1 value1) [ (key2 value2) ...]) -> struct\n"
            "(struct struct1 [struct2 ...]) -> struct\n"
            "{ key1:value1 [key2:value2 ...] } -> struct\n\n"
			
			"Creates a data structure. Keys must be strings, but this is assumed when using {} syntax. Use (@ struct key) to access data.",
			"*",	0,	},

		{	"structAppend",		fnStruct,		FN_STRUCT_APPEND,
			"(structAppend key1 value1 [ key2 value2 ...]) -> struct\n"
            "(structAppend (key1 value1) [ (key2 value2) ...]) -> struct\n"
            "(structAppend struct1 [struct2 ...]) -> struct\n\n"
			
			"Same as struct except values of the same key are appended into a list.",
			"*",	0,	},

		{	"subset",			fnSubset,		0,
			"(subset list pos [count]) -> list\n"
			"(subset string pos [count]) -> string",
			"vv*",	0,	},

		{	"subst",			fnSubst,		0,
			"(subst string arg1 arg2 ... argn) -> string",
			"s*",	PPFLAG_SIDEEFFECTS,	},

		{	"subtract",			fnMathOld,			FN_MATH_SUBTRACT,
			"(subtract x1 x2 ... xn) -> z",
			NULL,	0,	},

		{	"-",				fnMathList,			FN_MATH_SUBTRACT,
			"(- x1 x2 ... xn) -> z\n (- x) -> -x",
			"v*",	0,	},

		{	"switch",			fnSwitch,		0,
			"(switch [cond1 exp1 ... condn expn] [defaultexp]) -> value\n\n"

			"Evaluates conditions until one returns non-Nil, then evaluates and returns the corresponding expression.",
			NULL,	0,	},

		//{	"symDeleteEntry",	fnSymTable,		FN_SYMTABLE_DELETEENTRY,"",		NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"sysGlobals",		fnSysInfo,		FN_SYSINFO_GLOBALS,		"(sysGlobals) -> list of global symbols",		NULL,	0,	},
		{	"sysPoolUsage",		fnSysInfo,		FN_SYSINFO_POOLUSAGE,	"(sysPoolUsage) -> list of resource usage",		NULL,	0,	},
		{	"sysSelfTest",		fnSysInfo,		FN_SYSINFO_SELF_TEST,	"(sysSelfTest) -> True/Nil",		NULL,	0,	},
		{	"sysTicks",			fnSysInfo,		FN_SYSINFO_TICKS,		"(sysTicks) -> int",		NULL,	0,	},

		{	"tan",				fnMathNumerals,	FN_MATH_TAN,
			"(tan x ['degrees]) -> z",
			"n*",	0,	},

		{	"typeOf",			fnItem,			FN_ITEM_TYPE,
			"(typeOf item) -> type\n\n"

			"type:\n\n"

			"   'error\n"
			"   'function\n"
			"   'int32\n"
			"   'list\n"
			"   'nil\n"
			"   'primitive\n"
			"   'real\n"
			"   'string\n"
			"   'struct\n"
			"   'true",

			"v",	0,	},

		{	"vecSetElement",	fnVector,		FN_VECTOR_SETELEMENT_OLD,	"",		NULL,	PPFLAG_SIDEEFFECTS,	},

		{	"vecCreate",		fnVecCreateOld,	0,	
			"(vecCreate) -> empty vector", 
			NULL, 0, },

		{ "vFilled", fnVecCreate, FN_VECREATE_FILLED, "(vFilled scalar shapelist) -> vector filled with scalar's value", NULL, 0, },
		{ "vector", fnVecCreate, FN_VECCREATE, "(vector contentlist) -> vector form of contentlist", NULL, 0, },
		{ "v+", fnVecMath, FN_VECTOR_ADD, "(v+ vec1 vec2) -> result of vector addition of vec1 and vec2", NULL, 0, },
		{ "vDot", fnVecMath, FN_VECTOR_DOT, "(vDot vec1 vec2) -> result of vector dot product of vec1 and vec2", NULL, 0, },
		{ "v*", fnVecMath, FN_VECTOR_SCALMUL, "(v* scalar vec1) -> result of scalar multiplication of scalar and vec1", NULL, 0, },
		{ "v^", fnVecMath, FN_VECTOR_EMUL, "(v^ vec1 vec2) -> result of element-wise multiplication of vec1 and vec2", NULL, 0, },
		{ "v<-", fnVector, FN_VECTOR_SET, "(v<- vec1 indexlist datalist) -> set the elements of vec1 with datalist based on the indices in indexlist", NULL, PPFLAG_SIDEEFFECTS, },
		{ "v->", fnVector, FN_VECTOR_GET, "(v-> vec1 indexlist) -> get the elements of vec1 based on indexlist", NULL, 0, },
		{ "v=", fnVecMath, FN_VECTOR_EQ, "(v= vec1 vec2) -> compare vec1 and vec2 for equality", NULL, 0, }
	};

#define DEFPRIMITIVES_COUNT		(sizeof(g_DefPrimitives) / sizeof(g_DefPrimitives[0]))
