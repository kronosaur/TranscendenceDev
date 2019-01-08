//	Functions.h
//
//	Primitive function definitions

#define FN_MATH_ADD						1
#define FN_MATH_DIVIDE					2
#define FN_MATH_MULTIPLY				3
#define FN_MATH_SUBTRACT				4
#define FN_SET_SETQ						5
#define FN_EQUALITY_EQ					6
#define FN_SYSINFO_GLOBALS				7
#define FN_SYMTABLE_ADDENTRY			8
#define FN_SYMTABLE_DELETEENTRY			9
#define FN_SYMTABLE_LOOKUP				10
#define FN_SYMTABLE_LIST				11
#define FN_ITEMINFO_ISERROR				12
#define FN_SYSINFO_POOLUSAGE			13
#define FN_BLOCK_BLOCK					14
#define FN_BLOCK_ERRBLOCK				15
#define FN_LINKEDLIST_APPEND			16
#define FN_LINKEDLIST_REPLACE			17
#define FN_ATMTABLE_ADDENTRY			18
#define FN_ATMTABLE_DELETEENTRY			19
#define FN_ATMTABLE_LOOKUP				20
#define FN_ATMTABLE_LIST				21
#define FN_SET_SET						22
#define FN_VECTOR_SETELEMENT_OLD		23
#define FN_EQUALITY_LESSER				24
#define FN_EQUALITY_LESSER_EQ			25
#define FN_EQUALITY_GREATER				26
#define FN_EQUALITY_GREATER_EQ			27
#define FN_SYSINFO_TICKS				28
#define FN_MATH_RANDOM					29
#define FN_LOGICAL_AND					30
#define FN_LOGICAL_OR					31
#define FN_ENUM_WHILE					32
#define FN_LOGICAL_NOT					33
#define FN_ITEMINFO_ISATOM				34
#define FN_ITEMINFO_ISFUNCTION			35
#define FN_ITEMINFO_ASINT				36
#define FN_MATH_MODULUS					37
#define FN_ITEMINFO_ISINT				38
#define FN_ITEMINFO_HELP				39
#define FN_LINKEDLIST_REMOVE			40
#define FN_LINKEDLIST_REMOVE_NIL		41
#define FN_MATH_MIN						42
#define FN_MATH_MAX						43
#define FN_MATH_POWER					44
#define FN_MATH_SQRT					45
#define FN_MATH_ABSOLUTE				46
#define FN_FIND							47
#define FN_LOOKUP						48
#define FN_ITEMINFO_ISPRIMITIVE			49
#define FN_QUOTE						50
#define FN_ERROR						51
#define FN_SORT							52
#define FN_ITEM							53
#define FN_SET_ITEM						54
#define FN_RANDOM						55
#define FN_RANDOM_GAUSSIAN				56
#define FN_ITEM_TYPE					57
#define FN_EQUALITY_NEQ					58
#define FN_VECREATE_FILLED              59
#define FN_VECCREATE                    60
#define FN_VECTOR_ADD                   61
#define FN_VECTOR_SCALMUL               62
#define FN_VECTOR_DOT                   63
#define FN_VECTOR_EMUL                  64
#define FN_VECTOR_EADD					65
#define FN_ITEMINFO_ASDOUBLE            66
#define FN_VECTOR_SET					67
#define FN_VECTOR_GET					68
#define FN_MATH_ADD_NUMERALS			69
#define FN_MATH_DIVIDE_NUMERALS			70
#define FN_MATH_MULTIPLY_NUMERALS		71
#define FN_MATH_SUBTRACT_NUMERALS		72
#define FN_MATH_MODULUS_NUMERALS		73
#define FN_MATH_MIN_NUMERALS			74
#define FN_MATH_MAX_NUMERALS			75
#define FN_MATH_POWER_NUMERALS			76
#define FN_MATH_SQRT_NUMERALS			77
#define FN_MATH_ABSOLUTE_NUMERALS		78
#define FN_EQUALITY_EQ_NUMERALS			79
#define FN_EQUALITY_LESSER_NUMERALS		80
#define FN_EQUALITY_LESSER_EQ_NUMERALS	81
#define FN_EQUALITY_GREATER_NUMERALS	82
#define FN_EQUALITY_GREATER_EQ_NUMERALS	83
#define FN_EQUALITY_NEQ_NUMERALS		84
#define FN_VECTOR_EQ					85
#define FN_MATH_ROUND					86
#define FN_MATH_CEIL					87
#define FN_MATH_FLOOR					88
#define FN_ITEM_CONVERT_TO				89
#define FN_JOIN                         90
#define FN_LIST							91
#define FN_MAKE							92
#define FN_STRUCT						93
#define FN_STRUCT_APPEND				94
#define FN_MATH_ARCCOS					95
#define FN_MATH_ARCSIN					96
#define FN_MATH_ARCTAN					97
#define FN_MATH_COS						98
#define FN_MATH_SIN						99
#define FN_MATH_TAN						100
#define FN_MATH_EXP						101
#define FN_MATH_LOG						102

ICCItem *fnAppend (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnApply (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnAtmCreate (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnAtmTable (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnBlock (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnCat (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnCount (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnEnum (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnEquality (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnEqualityNumerals (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnEval (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnFilter (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnFind (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnForLoop (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnIf (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnItem (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnItemInfo (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnHelp (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnLambda (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnList (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnLink (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnLinkedList (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnLinkedListAppend (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnLogical (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnLoop (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnMap (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMatch (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMathFractions (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMathList (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMathListOld (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnMathOld(CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnMath (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnMathNumerals (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnRandom (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnRandomTable (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnRegEx (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSeededRandom (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSet (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnShuffle (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSort (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSpecial (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSplit (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnStrCapitalize (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnStrFind (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnStruct (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSubset (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSubst (CEvalContext *pCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnSwitch (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSymCreate (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSymTable (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnSysInfo (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnVecCreate (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnVecCreateOld (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnVector (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnVecMath (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);
ICCItem *fnVecIndex (CEvalContext *pCtx, ICCItem *pArguments, DWORD dwData);