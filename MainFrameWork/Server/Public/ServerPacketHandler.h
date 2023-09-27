#pragma once

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];


// TODO
enum : uint16
{
	PKT_S_TIME = 1,
	PKT_S_LOGIN = 2,
	PKT_S_OPENLEVEL = 3,
	PKT_S_LEVELSTATE = 4,
	PKT_S_CREATEOBJECT = 5,
	PKT_S_CHARACTERNAME = 6,
	PKT_S_MATRIX = 7,
	PKT_S_ANIMATION = 8,
};

//TODO
bool Handle_INVALID_Server(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_TIME_Server(PacketSessionRef& session, Protocol::S_TIME& pkt);
bool Handel_S_OPENLEVEL_Server(PacketSessionRef& session, Protocol::S_OPEN_LEVEL& pkt);
bool Handel_S_LEVELSTATE_Server(PacketSessionRef& session, Protocol::S_LEVEL_STATE& pkt);
bool Handel_S_CREATEOBJECT_Server(PacketSessionRef& session, Protocol::S_CREATE_OBJCECT& pkt);
bool Handel_S_CHARACTERNAME_Server(PacketSessionRef& session, Protocol::S_CHARACTER_NAME& pkt);
bool Handel_S_MATRIX_Server(PacketSessionRef& session, Protocol::S_MATRIX& pkt);
bool Handel_S_ANIMATION_Server(PacketSessionRef& session, Protocol::S_ANIMATION& pkt);


class CServerPacketHandler
{
public:
	// TODO
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID_Server;

		GPacketHandler[PKT_S_TIME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_TIME>(Handle_S_TIME_Server, session, buffer, len); };
		GPacketHandler[PKT_S_OPENLEVEL] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_OPEN_LEVEL>(Handel_S_OPENLEVEL_Server, session, buffer, len); };
		GPacketHandler[PKT_S_LEVELSTATE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LEVEL_STATE>(Handel_S_LEVELSTATE_Server, session, buffer, len); };
		GPacketHandler[PKT_S_CREATEOBJECT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CREATE_OBJCECT>(Handel_S_CREATEOBJECT_Server, session, buffer, len); };
		GPacketHandler[PKT_S_CHARACTERNAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHARACTER_NAME>(Handel_S_CHARACTERNAME_Server, session, buffer, len); };
		GPacketHandler[PKT_S_MATRIX] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_MATRIX>(Handel_S_MATRIX_Server, session, buffer, len); };
		GPacketHandler[PKT_S_ANIMATION] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ANIMATION>(Handel_S_ANIMATION_Server, session, buffer, len); };
		
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

	// TODO
	static SendBufferRef MakeSendBuffer(Protocol::S_TIME& pkt) { return MakeSendBuffer(pkt, PKT_S_TIME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_OPEN_LEVEL& pkt) { return MakeSendBuffer(pkt, PKT_S_OPENLEVEL); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEVEL_STATE& pkt) { return MakeSendBuffer(pkt, PKT_S_LEVELSTATE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CREATE_OBJCECT& pkt) { return MakeSendBuffer(pkt, PKT_S_CREATEOBJECT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHARACTER_NAME& pkt) { return MakeSendBuffer(pkt, PKT_S_CHARACTERNAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MATRIX& pkt) { return MakeSendBuffer(pkt, PKT_S_MATRIX); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ANIMATION& pkt) { return MakeSendBuffer(pkt, PKT_S_ANIMATION); }


private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};

