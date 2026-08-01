#pragma once

namespace jh
{
	class PacketBuilder
	{
	public:
		static PacketBufferRef BuildEchoPacket(WORD len, ULONGLONG data);
	};
}
