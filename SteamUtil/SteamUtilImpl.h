//	SteamUtilImpl.h
//
//	Helper objects for SteamUtil
//	Copyright (c) 2015 by Kronosaur Productions. All Rights Reserved.

#pragma once

//	Call Thunks ----------------------------------------------------------------

//  This is the standard template, in which the result has an m_eResult member
//  with an error code.

template <class HANDLERTYPE, class RESULTTYPE> class TCallContext
	{
	protected:
		bool Invoke (SteamAPICall_t hSteamAPICall, CString *retsError = NULL)
			{
			m_CallResult.Set(hSteamAPICall, (HANDLERTYPE *)this, &TCallContext::Handler);

			while (m_CallResult.IsActive())
				{
				::Sleep(50);
				SteamAPI_RunCallbacks();
				}

            if (!m_bSuccess && retsError)
                *retsError = m_sError;

			return m_bSuccess;
			}

        void Handler (RESULTTYPE *pResult, bool bIOFailure)
            {
            if (bIOFailure)
                {
                m_bSuccess = false;
                m_sError = CONSTLIT("Unable to communicate with Steam service.");
                }
            else if (pResult->m_eResult != k_EResultOK)
                {
                m_bSuccess = false;
                switch (pResult->m_eResult)
                    {
                    case k_EResultInsufficientPrivilege:
                        m_sError = CONSTLIT("Sorry, you are not authorized to do that.");
                        break;

                    case k_EResultTimeout:
                        m_sError = CONSTLIT("Sorry, the operation timed out.");
                        break;

                    case k_EResultNotLoggedOn:
                        m_sError = CONSTLIT("Please log on to Steam and try again.");
                        break;

                    default:
                        m_sError = CONSTLIT("Unable to comply.");
                        break;
                    }
                }
            else
                {
                m_bSuccess = ((HANDLERTYPE *)this)->OnResult(pResult, &m_sError);
                }
            }

		CCallResult<HANDLERTYPE, RESULTTYPE> m_CallResult;
		bool m_bSuccess;
        CString m_sError;
	};

//  This template has m_bSuccess in the result.

template <class HANDLERTYPE, class RESULTTYPE> class TCallContext2
	{
	protected:
		bool Invoke (SteamAPICall_t hSteamAPICall, CString *retsError = NULL)
			{
			m_CallResult.Set(hSteamAPICall, (HANDLERTYPE *)this, &TCallContext2::Handler);

			while (m_CallResult.IsActive())
				{
				::Sleep(50);
				SteamAPI_RunCallbacks();
				}

            if (!m_bSuccess && retsError)
                *retsError = m_sError;

			return m_bSuccess;
			}

        void Handler (RESULTTYPE *pResult, bool bIOFailure)
            {
            if (bIOFailure)
                {
                m_bSuccess = false;
                m_sError = CONSTLIT("Unable to communicate with Steam service.");
                }
            else if (!pResult->m_bSuccess)
                {
                m_bSuccess = false;
                m_sError = CONSTLIT("Unable to comply.");
                }
            else
                {
                m_bSuccess = ((HANDLERTYPE *)this)->OnResult(pResult, &m_sError);
                }
            }

		CCallResult<HANDLERTYPE, RESULTTYPE> m_CallResult;
		bool m_bSuccess;
        CString m_sError;
	};

//  This template has no error result

template <class HANDLERTYPE, class RESULTTYPE> class TCallContext0
	{
	protected:
		bool Invoke (SteamAPICall_t hSteamAPICall, CString *retsError = NULL)
			{
			m_CallResult.Set(hSteamAPICall, (HANDLERTYPE *)this, &TCallContext0::Handler);

			while (m_CallResult.IsActive())
				{
				::Sleep(50);
				SteamAPI_RunCallbacks();
				}

            if (!m_bSuccess && retsError)
                *retsError = m_sError;

			return m_bSuccess;
			}

        void Handler (RESULTTYPE *pResult, bool bIOFailure)
            {
            if (bIOFailure)
                {
                m_bSuccess = false;
                m_sError = CONSTLIT("Unable to communicate with Steam service.");
                }
            else
                {
                m_bSuccess = ((HANDLERTYPE *)this)->OnResult(pResult, &m_sError);
                }
            }

		CCallResult<HANDLERTYPE, RESULTTYPE> m_CallResult;
		bool m_bSuccess;
        CString m_sError;
	};

class CDownloadLeaderboardScores : public TCallContext0<CDownloadLeaderboardScores, LeaderboardScoresDownloaded_t>
	{
	public:
		bool Call (SteamLeaderboard_t hLeaderboard, CAdventureHighScoreList *retScores)
			{
			m_pScores = retScores;
			return Invoke(SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, k_ELeaderboardDataRequestGlobal, 1, 100));
			}

		bool OnResult (LeaderboardScoresDownloaded_t *pResult, CString *retsError)
			{
			int i;

			for (i = 0; i < pResult->m_cEntryCount; i++)
				{
				LeaderboardEntry_t Entry;
				SteamUserStats()->GetDownloadedLeaderboardEntry(pResult->m_hSteamLeaderboardEntries, i, &Entry, NULL, 0);

				CString sUsername = CString(CString::csUTF8, SteamFriends()->GetFriendPersonaName(Entry.m_steamIDUser));
				m_pScores->InsertSimpleScore(sUsername, Entry.m_nScore);
				}

            return true;
			}

	private:
		CAdventureHighScoreList *m_pScores;
	};

class CFindOrCreateLeaderboard : public TCallContext0<CFindOrCreateLeaderboard, LeaderboardFindResult_t>
	{
	public:
		bool Call (const CString &sName, ELeaderboardSortMethod iSortMethod, ELeaderboardDisplayType iDisplayType, SteamLeaderboard_t *rethLeaderboard)
			{
			if (!Invoke(SteamUserStats()->FindOrCreateLeaderboard(sName.GetASCIIZPointer(), iSortMethod, iDisplayType)))
				return false;

			*rethLeaderboard = m_hLeaderboard;
			return true;
			}

		bool OnResult (LeaderboardFindResult_t *pResult, CString *retsError)
			{
            if (!pResult->m_bLeaderboardFound)
                return false;

			m_hLeaderboard = pResult->m_hSteamLeaderboard;
            return true;
			}

		SteamLeaderboard_t m_hLeaderboard;
	};

class CUGCGetUserContent : public TCallContext<CUGCGetUserContent, SteamUGCQueryCompleted_t>
    {
    public:
        struct SQuery
            {
            AppId_t nAppId;
            AccountID_t unAccountID;
            EUserUGCList eListType;
            TSortMap<CString, CString> RequiredTags;
            };

        struct SDetails
            {
            SDetails (void) :
                    dwUNID(0),
                    iType(extUnknown)
                { }

            DWORD dwUNID;
            EExtensionTypes iType;
            SteamUGCDetails_t Basic;
            TSortMap<CString, CString> Fields;
            };

        bool Call (const SQuery &Query, TArray<SDetails> &Results, CString *retsError = NULL)
            {
            int i;

            //  Store results here

            Results.DeleteAll();
            m_pResults = &Results;

            //  Loop until we've got all results
            
            m_bDone = false;
            int iPage = 1;
            while (!m_bDone)
                {
                m_Query = SteamUGC()->CreateQueryUserUGCRequest(Query.unAccountID, 
                        Query.eListType, 
                        k_EUGCMatchingUGCType_Items, 
                        k_EUserUGCListSortOrder_CreationOrderDesc, 
                        Query.nAppId, 
                        Query.nAppId, 
                        iPage);

                //  Add required tags

                for (i = 0; i < Query.RequiredTags.GetCount(); i++)
                    SteamUGC()->AddRequiredKeyValueTag(m_Query, Query.RequiredTags.GetKey(i), Query.RequiredTags[i]);

                //  Ask for key/value tags

                SteamUGC()->SetReturnKeyValueTags(m_Query, true);

                //  Invoke

                if (!Invoke(SteamUGC()->SendQueryUGCRequest(m_Query), retsError))
                    {
                    SteamUGC()->ReleaseQueryUGCRequest(m_Query);
                    return false;
                    }

                SteamUGC()->ReleaseQueryUGCRequest(m_Query);
                iPage++;
                }

            //  Done

            return true;
            }

    private:
        bool OnResult (SteamUGCQueryCompleted_t *pResult, CString *retsError)
            {
            DWORD i;

            if (m_pResults->GetCount() == 0)
                m_pResults->GrowToFit(pResult->m_unTotalMatchingResults);

            for (i = 0; i < pResult->m_unNumResultsReturned; i++)
                {
                SDetails *pDetails = m_pResults->Insert();
                if (!SteamUGC()->GetQueryUGCResult(m_Query, i, &pDetails->Basic))
                    {
                    //  LATER: Mark as an error.
                    m_bDone = true;
                    return false;
                    }

                int iTagCount = (int)SteamUGC()->GetQueryUGCNumKeyValueTags(m_Query, i);
                pDetails->Fields.GrowToFit(iTagCount);
                for (int j = 0; j < iTagCount; j++)
                    {
                    char szKey[256];
                    char szValue[256];
                    if (SteamUGC()->GetQueryUGCKeyValueTag(m_Query, i, j, szKey, sizeof(szKey), szValue, sizeof(szValue)))
                        {
                        CString sKey(szKey);
                        CString sValue(szValue);
                        pDetails->Fields.Insert(sKey, sValue);

                        //  If this is the UNID, then parse it

                        if (strEquals(sKey, CONSTLIT("unid")))
                            {
                            //  000000000011111
                            //  012345678901234
                            //  Transcendence:xxxxxxxx

                            const int UNID_OFFSET = 14;
                            pDetails->dwUNID = strParseIntOfBase(sValue.GetASCIIZPointer() + UNID_OFFSET, 16, 0);
                            }
                        else if (strEquals(sKey, CONSTLIT("type")))
                            {
                            if (strEquals(sValue, CONSTLIT("transcendenceAdventure")))
                                pDetails->iType = extAdventure;
                            else if (strEquals(sValue, CONSTLIT("transcendenceExpansion")))
                                pDetails->iType = extExtension;
                            else if (strEquals(sValue, CONSTLIT("transcendenceLibrary")))
                                pDetails->iType = extLibrary;
                            else
                                pDetails->iType = extUnknown;
                            }
                        }
                    }
                }

            //  If we've got all details, then we're done

            m_bDone = ((DWORD)m_pResults->GetCount() >= pResult->m_unTotalMatchingResults);

            return true;
            }

        TArray<SDetails> *m_pResults;
        UGCQueryHandle_t m_Query;
        bool m_bDone;

    friend TCallContext;
    };

class CUploadLeaderboardScore : public TCallContext2<CUploadLeaderboardScore, LeaderboardScoreUploaded_t>
	{
	public:
		bool Call (SteamLeaderboard_t hLeaderboard, int iScore)
			{
			return Invoke(SteamUserStats()->UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, iScore, NULL, 0));
			}

		bool OnResult (LeaderboardScoreUploaded_t *pResult, CString *retsError)
			{
            return true;
			}
	};
