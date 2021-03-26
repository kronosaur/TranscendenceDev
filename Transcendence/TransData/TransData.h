//	TransData.h
//
//	Include file for TranData app

#ifndef INCL_TRANSDATA
#define INCL_TRANSDATA

#ifndef INCL_TSE
#include "TSE.h"
#endif

#include "ItemBenchmarks.h"
#include "Utilities.h"

class CHost : public CUniverse::IHost
	{
	public:
		virtual void ConsoleOutput (const CString &sLine) override { printf("%s\n", sLine.GetASCIIZPointer()); }
		virtual void DebugOutput (const CString &sLine) override { printf("%s\n", sLine.GetASCIIZPointer()); }
		virtual bool FindFont (const CString &sFont, const CG16bitFont **retpFont = NULL) const;
		virtual void LogOutput (const CString &sLine) const override { printf("%s\n", (LPSTR)sLine); ::kernelDebugLogString(sLine); }

	private:
		void InitFonts (void) const;

		mutable TArray<CG16bitFont> m_Fonts;
	};

class CConsole : public ILog
	{
	public:
		virtual void Print (const CString &sLine) const { ClearLine(); printf("%s\n", (LPSTR)sLine); }
		virtual void Progress (const CString &sLine, int iPercent = -1) const { ClearLine(); printf("%s\r", (LPSTR)sLine); m_iClearCount = sLine.GetLength(); }

	private:
		void ClearLine (void) const
			{
			if (m_iClearCount) 
				{
				printf("%s\r", (LPSTR)strRepeat(CONSTLIT(" "), m_iClearCount));
				m_iClearCount = 0;
				}
			}

		mutable int m_iClearCount = 0;
	};

class ITransDataCommand
	{
	public:
		ITransDataCommand (ILog &Console) :
				m_Console(Console)
			{ }

		virtual ~ITransDataCommand (void) { }
		virtual bool Run (void) { return true; }
		virtual bool SetOptions (const CXMLElement &CmdLine) { return true; }

	protected:
		bool Error (const CString &sError) const { m_Console.Print(strPatternSubst(CONSTLIT("ERROR: %s"))); return false; }
		ILog &GetConsole (void) { return m_Console; }
		void Print (const CString &sLine) const { m_Console.Print(sLine); }
		void Progress (const CString &sLine) const { m_Console.Progress(sLine); }

	private:
		ILog &m_Console;
	};

//	Used by sim tables

class ItemInfo
	{
	public:
		struct SFoundDesc
			{
			const CItemType *pItemType = NULL;
			int iSystemLevel = -1;
			const CDesignType *pFoundOn = NULL;
			int iTotalCount = 0;						//	Total item count
			TSortMap<int, int> Games;					//	Game samples found on
			};

		void AddFoundOnEntry (const CItem &Item, const CDesignType &FoundOnType, int iSystemLevel, int iSampleIndex)
			{
			bool bNew;
			auto *pFoundOn = FoundOn.SetAt(ItemInfo::MakeFoundOnKey(*Item.GetType(), FoundOnType, iSystemLevel), &bNew);
			if (bNew)
				{
				pFoundOn->pItemType = Item.GetType();
				pFoundOn->pFoundOn = &FoundOnType;
				pFoundOn->iSystemLevel = iSystemLevel;
				}

			pFoundOn->iTotalCount += Item.GetCount();

			auto *pCount = pFoundOn->Games.SetAt(iSampleIndex, &bNew);
			if (bNew)
				*pCount = 1;
			else
				*pCount += 1;
			}

		static CString MakeFoundOnKey (const CItemType &ItemType, const CDesignType &Type, int iSystemLevel)
			{ return strPatternSubst(CONSTLIT("%08x/%08x/%02d"), ItemType.GetUNID(), Type.GetUNID(), iSystemLevel); }

		ItemInfo (void) { }

		CItemType *pType = NULL;						//	Item type
		int iTotalCount = 0;							//	Total times this item type has appeared
		double rTotalCount = 0.0;
		TSortMap<CString, SFoundDesc> FoundOn;			//	Stats for where items were found
	};

const int MAX_FREQUENCY_COUNT = 12;

class StationInfo
	{
	public:
		StationInfo (void) { }

		CStationType *pType = NULL;
		CString sCategory;

		int iFreqCount[MAX_FREQUENCY_COUNT] = { 0 };	//	For each count, then number of times that
													//	number of stations have appeared in the
													//	system instance.

		int iTotalCount = 0;						//	Total times this station type has appeared
													//	the system instance.
		double rTotalCount = 0.0;

		int iSystemCount = 0;						//	Number of system instances with this
													//	station type

		int iTempCount = 0;							//	Temp count for a specific system instance
	};

struct SDesignTypeInfo
	{
	double rPerGameMedianCount = 0.0;				//	Median encountered per game
	double rPerGameMeanCount = 0.0;					//	Mean encountered per game
	int iPerGameMinCount = 0;						//	Minimum ever encountered in a game
	int iPerGameMaxCount = 0;						//	Maximum ever encountered in a game
	CString sDistribution;							//	"1 (5%); 2 (10%); 3 (70%); 4 (10%); 5 (5%)"
	};

typedef TSortMap<DWORD, SDesignTypeInfo> CDesignTypeStats;

//	Functions

const char *FrequencyChar (int iFreq);
void GetCCTransDataLibrary (SPrimitiveDefTable *retpTable);
void ShowHelp (CXMLElement *pCmdLine);
void MarkItemsKnown (CUniverse &Universe);
bool OutputImage (CG32bitImage &Image, const CString &sFilespec);
ALERROR LoadDesignTypeStats (DWORD dwAventureUNID, CDesignTypeStats *retStats);

void Decompile (const CString &sDataFile, CXMLElement *pCmdLine);
void DebugMarkers (CXMLElement *pCmdLine);
void DoEffectPerformanceTest (CUniverse &Universe, CXMLElement *pCmdLine);
void DoRandomNumberTest (void);
void DoSmokeTest (CUniverse &Universe, CXMLElement *pCmdLine);
void DoTradeSim (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateArmorTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateAttributeList (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateDiagnostics (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateEffectExplorer (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateEffectImage (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateEncounterCount (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateEncounterFrequency (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateEncounterTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateEntitiesTable (const CString &sDataFile, CXMLElement *pCmdLine);
void GenerateExtensionTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateImageChart (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateItemFrequencyTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateItemTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateLanguageTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateLootSim (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateRandomItemTables (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateReference (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateScript (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateShieldStats (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateShipImage (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateShipImageChart (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateShipTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateSimTables (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateSnapshot (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateStationFrequencyTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateStationPlaceSim (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateStats (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateStdArmorTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateStdShieldTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateSystemCount (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateSystemImages (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateSystemLabelCount (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateSystemTest (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateTopology (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateTopologyMap (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateTradeTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateTypeDependencies (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateTypeIslands (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateTypeTable (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateWeaponEffectChart (CUniverse &Universe, CXMLElement *pCmdLine);
void GenerateWordList (const CString &sDataFile, CXMLElement *pCmdLine);
void GenerateWorldImageChart (CUniverse &Universe, CXMLElement *pCmdLine);
void HexarcTest (CUniverse &Universe, CXMLElement *pCmdLine);
void PerformanceTest (CUniverse &Universe, CXMLElement *pCmdLine);
void Run (CUniverse &Universe, CXMLElement *pCmdLine);
void RunBattleSim(CUniverse &Universe, CXMLElement *pCmdLine);
void RunEncounterSim (CUniverse &Universe, CXMLElement *pCmdLine);
void RunFile (const CString &sFilespec, bool bNoLogo);
void WordGenerator (CXMLElement *pCmdLine);

//	Don't care about warning of depecrated functions (e.g., sprintf)
#pragma warning(disable: 4996)

#endif
