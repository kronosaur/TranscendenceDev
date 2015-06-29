//	SteamUtilImpl.h
//
//	Helper objects for SteamUtil
//	Copyright (c) 2015 by Kronosaur Productions. All Rights Reserved.

#pragma once

//	Call Thunks ----------------------------------------------------------------

template <class HANDLERTYPE, class RESULTTYPE> class TCallContext
	{
	protected:
		bool Invoke (SteamAPICall_t hSteamAPICall)
			{
			m_CallResult.Set(hSteamAPICall, (HANDLERTYPE *)this, &HANDLERTYPE::OnResult);

			while (m_CallResult.IsActive())
				{
				::Sleep(50);
				SteamAPI_RunCallbacks();
				}

			return m_bSuccess;
			}

		CCallResult<HANDLERTYPE, RESULTTYPE> m_CallResult;
		bool m_bSuccess;
	};

class CDownloadLeaderboardScores : public TCallContext<CDownloadLeaderboardScores, LeaderboardScoresDownloaded_t>
	{
	public:
		bool Call (SteamLeaderboard_t hLeaderboard, CAdventureHighScoreList *retScores)
			{
			m_pScores = retScores;
			return Invoke(SteamUserStats()->DownloadLeaderboardEntries(hLeaderboard, k_ELeaderboardDataRequestGlobal, 1, 100));
			}

		void OnResult (LeaderboardScoresDownloaded_t *pResult, bool bIOFailure)
			{
			int i;

			if (m_bSuccess = (!bIOFailure))
				{
				for (i = 0; i < pResult->m_cEntryCount; i++)
					{
					LeaderboardEntry_t Entry;
					SteamUserStats()->GetDownloadedLeaderboardEntry(pResult->m_hSteamLeaderboardEntries, i, &Entry, NULL, 0);

					CString sUsername = CString(CString::csUTF8, SteamFriends()->GetFriendPersonaName(Entry.m_steamIDUser));
					m_pScores->InsertSimpleScore(sUsername, Entry.m_nScore);
					}
				}
			}

	private:
		CAdventureHighScoreList *m_pScores;
	};

class CFindOrCreateLeaderboard : public TCallContext<CFindOrCreateLeaderboard, LeaderboardFindResult_t>
	{
	public:
		bool Call (const CString &sName, ELeaderboardSortMethod iSortMethod, ELeaderboardDisplayType iDisplayType, SteamLeaderboard_t *rethLeaderboard)
			{
			if (!Invoke(SteamUserStats()->FindOrCreateLeaderboard(sName.GetASCIIZPointer(), iSortMethod, iDisplayType)))
				return false;

			*rethLeaderboard = m_hLeaderboard;
			return true;
			}

		void OnResult (LeaderboardFindResult_t *pResult, bool bIOFailure)
			{
			if (m_bSuccess = (pResult->m_bLeaderboardFound && !bIOFailure))
				m_hLeaderboard = pResult->m_hSteamLeaderboard;
			}

		SteamLeaderboard_t m_hLeaderboard;
	};

class CUploadLeaderboardScore : public TCallContext<CUploadLeaderboardScore, LeaderboardScoreUploaded_t>
	{
	public:
		bool Call (SteamLeaderboard_t hLeaderboard, int iScore)
			{
			return Invoke(SteamUserStats()->UploadLeaderboardScore(hLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, iScore, NULL, 0));
			}

		void OnResult (LeaderboardScoreUploaded_t *pResult, bool bIOFailure)
			{
			m_bSuccess = (pResult->m_bSuccess && !bIOFailure);
			}
	};
