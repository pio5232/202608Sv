#pragma once

namespace jh
{
	class UserManager
	{
	public:
		UserManager();
		~UserManager();

		void Init();

		UserRef CreateUser(ULONGLONG sessionId);
		void RemoveUser(ULONGLONG sessionId);

		UserRef GetUserByUserId(ULONGLONG userId);
		UserRef GetUserBySessionId(ULONGLONG sessionId);
	private:

		SRWLOCK									m_lock;
		// sessionID to userID
		std::unordered_map<ULONGLONG, UserRef>	m_sessionIdToUserUMap;
		
		// userID to userPtr
		std::unordered_map<ULONGLONG, UserRef>	m_userIdToUserUMap;
	};
}