#pragma once
namespace jh
{
	class DummyPacketBuilder
	{
	public:
		//static jh::SerializationBufferPtr BuildEchoPacket(DWORD len, ULONGLONG data);
		static PacketBufferRef BuildLoginRequestPacket();
		static PacketBufferRef BuildHeartbeatPacket(ULONGLONG timeStamp);
		static PacketBufferRef BuildMakeRoomRequestPacket();
		static PacketBufferRef BuildEnterRoomRequestPacket(USHORT roomNum, const WCHAR* roomName);
		static PacketBufferRef BuildLeaveRoomRequestPacket(USHORT roomNum, const WCHAR* roomName);
		static PacketBufferRef BuildChatRequestPacket(USHORT roomNum);
		static PacketBufferRef BuildRoomListRequestPacket();

		static PacketBufferRef BuildEchoPacket();
	};
}