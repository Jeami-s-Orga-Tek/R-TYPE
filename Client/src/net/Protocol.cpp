/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Protocol
*/


#include "net/Protocol.hpp"
#include <cstring>
#include <boost/endian/conversion.hpp>
#include <vector>
#include <string>
#include <stdexcept>

namespace RtypeServer {
    Protocol::Protocol() {}
    Protocol::~Protocol() {}

void Protocol::writeHeader(std::vector<uint8_t>& out, const NetHeader& h)
{
    append_u16be(out, h.magic);
    append_u8(out, h.version);
    append_u8(out, h.type);
    append_u32be(out, h.seq);
    append_u32be(out, h.ack);
    append_u32be(out, h.ackBits);
}

NetHeader Protocol::readHeader(const uint8_t* data, std::size_t size)
{
    NetHeader h{};

    if (size < sizeof(NetHeader))
        throw std::runtime_error("packet too small for header");
    std::size_t off = 0;
    h.magic = read_u16be(data, off, size);
    h.version = read_u8(data, off, size);
    h.type = read_u8(data, off, size);
    h.seq = read_u32be(data, off, size);
    h.ack = read_u32be(data, off, size);
    h.ackBits = read_u32be(data, off, size);
    return h;
}

void Protocol::append_u8 (std::vector<uint8_t>& out, uint8_t v){ out.push_back(v); }

void Protocol::append_u16be(std::vector<uint8_t>& out, uint16_t v)
{
    uint16_t be = boost::endian::native_to_big(v);
    auto* p = reinterpret_cast<const uint8_t*>(&be);
    out.insert(out.end(), p, p+2);
}

void Protocol::append_u32be(std::vector<uint8_t>& out, uint32_t v)
{
    uint32_t be = boost::endian::native_to_big(v);
    auto* p = reinterpret_cast<const uint8_t*>(&be);
    out.insert(out.end(), p, p+4);
}

void Protocol::append_i16be(std::vector<uint8_t>& out, int16_t v)
{
    int16_t be = boost::endian::native_to_big(v);
    auto* p = reinterpret_cast<const uint8_t*>(&be);
    out.insert(out.end(), p, p+2);
}

uint8_t  Protocol::read_u8 (const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+1 > size) throw std::runtime_error("read_u8 OOB");
    return data[off++];
}

uint16_t Protocol::read_u16be(const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+2 > size) throw std::runtime_error("read_u16 OOB");
    uint16_t be; std::memcpy(&be, data+off, 2); off += 2;
    return boost::endian::big_to_native(be);
}

uint32_t Protocol::read_u32be(const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+4 > size) throw std::runtime_error("read_u32 OOB");
    uint32_t be; std::memcpy(&be, data+off, 4); off += 4;
    return boost::endian::big_to_native(be);
}

int16_t  Protocol::read_i16be(const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+2 > size) throw std::runtime_error("read_i16 OOB");
    int16_t be; std::memcpy(&be, data+off, 2); off += 2;
    return boost::endian::big_to_native(be);
}

void Protocol::writeHelloBody  (std::vector<uint8_t>& out, const HelloBody& b)
{
    append_u32be(out, b.client_nonce);
    append_u8(out, b.proto);
    uint8_t len = static_cast<uint8_t>(std::min<std::size_t>(255, b.name.size()));
    append_u8(out, len);
    out.insert(out.end(), b.name.data(), b.name.data()+len);
}

HelloBody Protocol::readHelloBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    HelloBody b{};
    b.client_nonce = read_u32be(data, off, size);
    b.proto = read_u8(data, off, size);
    uint8_t len = read_u8(data, off, size);
    if (off + len > size) throw std::runtime_error("hello name OOB");
    b.name.assign(reinterpret_cast<const char*>(data+off), len);
    off += len;
    return b;
}

void Protocol::writeWelcomeBody(std::vector<uint8_t>& out, const WelcomeBody& b)
{
    append_u32be(out, b.playerId);
    append_u16be(out, b.roomId);
    append_u32be(out, b.baselineTick);
}

WelcomeBody Protocol::readWelcomeBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    WelcomeBody b{};
    b.playerId = read_u32be(data, off, size);
    b.roomId = read_u16be(data, off, size);
    b.baselineTick = read_u32be(data, off, size);
    return b;
}

void Protocol::writeString(std::vector<uint8_t>& out, const std::string& str)
{
    uint16_t len = static_cast<uint16_t>(std::min<std::size_t>(65535, str.size()));
    append_u16be(out, len);
    out.insert(out.end(), str.data(), str.data() + len);
}

std::string Protocol::readString(const uint8_t* data, std::size_t size, std::size_t& off)
{
    uint16_t len = read_u16be(data, off, size);
    if (off + len > size) throw std::runtime_error("string OOB");
    std::string str(reinterpret_cast<const char*>(data + off), len);
    off += len;
    return str;
}

void Protocol::writeCreateRoomBody(std::vector<uint8_t>& out, const CreateRoomBody& b)
{
    writeString(out, b.roomName);
    append_u8(out, b.maxPlayers);
}

CreateRoomBody Protocol::readCreateRoomBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    CreateRoomBody b{};
    b.roomName = readString(data, size, off);
    b.maxPlayers = read_u8(data, off, size);
    return b;
}

void Protocol::writeJoinRoomBody(std::vector<uint8_t>& out, const JoinRoomBody& b)
{
    writeString(out, b.roomName);
}

JoinRoomBody Protocol::readJoinRoomBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    JoinRoomBody b{};
    b.roomName = readString(data, size, off);
    return b;
}

void Protocol::writeMessageBody(std::vector<uint8_t>& out, const MessageBody& b)
{
    writeString(out, b.message);
}

MessageBody Protocol::readMessageBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    MessageBody b{};
    b.message = readString(data, size, off);
    return b;
}

void Protocol::writeRoomCreatedBody(std::vector<uint8_t>& out, const RoomCreatedBody& b)
{
    append_u32be(out, b.roomId);
    writeString(out, b.roomName);
    append_u8(out, b.success);
}

RoomCreatedBody Protocol::readRoomCreatedBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    RoomCreatedBody b{};
    b.roomId = read_u32be(data, off, size);
    b.roomName = readString(data, size, off);
    b.success = read_u8(data, off, size);
    return b;
}

void Protocol::writeRoomJoinedBody(std::vector<uint8_t>& out, const RoomJoinedBody& b)
{
    append_u32be(out, b.roomId);
    writeString(out, b.roomName);
    append_u8(out, b.success);
    append_u8(out, b.playerCount);
}

RoomJoinedBody Protocol::readRoomJoinedBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    RoomJoinedBody b{};
    b.roomId = read_u32be(data, off, size);
    b.roomName = readString(data, size, off);
    b.success = read_u8(data, off, size);
    b.playerCount = read_u8(data, off, size);
    return b;
}

void Protocol::writeRoomLeftBody(std::vector<uint8_t>& out, const RoomLeftBody& b)
{
    append_u32be(out, b.roomId);
    append_u8(out, b.success);
}

RoomLeftBody Protocol::readRoomLeftBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    RoomLeftBody b{};
    b.roomId = read_u32be(data, off, size);
    b.success = read_u8(data, off, size);
    return b;
}

void Protocol::writeRoomListBody(std::vector<uint8_t>& out, const RoomListBody& b)
{
    append_u16be(out, static_cast<uint16_t>(b.rooms.size()));
    for (const auto& room : b.rooms) {
        append_u32be(out, room.roomId);
        writeString(out, room.roomName);
        append_u8(out, room.currentPlayers);
        append_u8(out, room.maxPlayers);
    }
}

RoomListBody Protocol::readRoomListBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    RoomListBody b{};
    uint16_t roomCount = read_u16be(data, off, size);
    b.rooms.reserve(roomCount);
    for (uint16_t i = 0; i < roomCount; ++i) {
        RoomInfoEntry entry{};
        entry.roomId = read_u32be(data, off, size);
        entry.roomName = readString(data, size, off);
        entry.currentPlayers = read_u8(data, off, size);
        entry.maxPlayers = read_u8(data, off, size);
        b.rooms.push_back(entry);
    }
    return b;
}

void Protocol::writeRoomMessageBody(std::vector<uint8_t>& out, const RoomMessageBody& b)
{
    append_u32be(out, b.senderId);
    writeString(out, b.senderName);
    writeString(out, b.message);
}

RoomMessageBody Protocol::readRoomMessageBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    RoomMessageBody b{};
    b.senderId = read_u32be(data, off, size);
    b.senderName = readString(data, size, off);
    b.message = readString(data, size, off);
    return b;
}

void Protocol::writeRoomErrorBody(std::vector<uint8_t>& out, const RoomErrorBody& b)
{
    append_u8(out, b.errorCode);
    writeString(out, b.errorMessage);
}

RoomErrorBody Protocol::readRoomErrorBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    RoomErrorBody b{};
    b.errorCode = read_u8(data, off, size);
    b.errorMessage = readString(data, size, off);
    return b;
}

} // namespace RtypeServer
