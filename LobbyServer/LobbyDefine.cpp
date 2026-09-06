#include "pch.h"

bool LobbyServerConfig::GetMainContent(jh_utility::Parser& parser, const WCHAR* categoryName)
{
	return LobbyServerConfig::GetMainContent(parser, categoryName);
}

bool LobbyServerConfig::GetContents(jh_utility::Parser& parser, const WCHAR* categoryName)
{
	if (wcscmp(categoryName, LOBBY_DATA_CATEGORY_NAME) == 0)
	{
		bool succeeded = parser.SetReadingCategory(categoryName);
		succeeded &= parser.GetValue(L"maxRoomCount", m_usMaxRoomCnt);
		succeeded &= parser.GetValue(L"maxRoomUserCount", m_usMaxRoomUserCnt);
		return succeeded;
	}
	
	return false;
}
