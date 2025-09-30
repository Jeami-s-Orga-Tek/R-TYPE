/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Protocol
*/

#include "Protocol.hpp"
#include <boost/endian/conversion.hpp>
#include <cstring>
#include <stdexcept>

RtypeServer::Protocol::Protocol()
{
    
}

RtypeServer::Protocol::~Protocol()
{
}

void RtypeServer::Protocol::writeHeader(std::vector<uint8_t>& out, const NetHeader& h)
{
    out.resize(sizeof(NetHeader));
    NetHeader be{
        boost::endian::native_to_big(h.magic),
        h.version,
        h.type,
        boost::endian::native_to_big(h.seq),
        boost::endian::native_to_big(h.ack),
        boost::endian::native_to_big(h.ackBits),
    };
    std::memcpy(out.data(), &be, sizeof(be));

}

RtypeServer::NetHeader RtypeServer::Protocol::readHeader(const uint8_t* data, std::size_t size)
{
    if (size < sizeof(NetHeader)) 
        throw std::runtime_error("packet too small");
    NetHeader be{};
    std::memcpy(&be, data, sizeof(be));
    NetHeader h{
        boost::endian::big_to_native(be.magic),
        be.version,
        be.type,
        boost::endian::big_to_native(be.seq),
        boost::endian::big_to_native(be.ack),
        boost::endian::big_to_native(be.ackBits),
    };
    return h;
}

void RtypeServer::Protocol::append_u8 (std::vector<uint8_t>& out, uint8_t v){ out.push_back(v); }

void RtypeServer::Protocol::append_u16be(std::vector<uint8_t>& out, uint16_t v)
{
    uint16_t be = boost::endian::native_to_big(v);
    auto* p = reinterpret_cast<const uint8_t*>(&be);
    out.insert(out.end(), p, p+2);
}
void RtypeServer::Protocol::append_u32be(std::vector<uint8_t>& out, uint32_t v)
{
    uint32_t be = boost::endian::native_to_big(v);
    auto* p = reinterpret_cast<const uint8_t*>(&be);
    out.insert(out.end(), p, p+4);

}
void RtypeServer::Protocol::append_i16be(std::vector<uint8_t>& out, int16_t v)
{
    int16_t be = boost::endian::native_to_big(v);
    auto* p = reinterpret_cast<const uint8_t*>(&be);
    out.insert(out.end(), p, p+2);
}

uint8_t  RtypeServer::Protocol::read_u8 (const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+1 > size)
        throw std::runtime_error("read_u8 OOB");
    return data[off++];

}

uint16_t RtypeServer::Protocol::read_u16be(const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+4 > size)
        throw std::runtime_error("read_u32 OOB");
    uint32_t be; std::memcpy(&be, data+off, 4); off += 4;
    return boost::endian::big_to_native(be);

}

uint32_t RtypeServer::Protocol::read_u32be(const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+4 > size)
        throw std::runtime_error("read_u32 OOB");
    uint32_t be; std::memcpy(&be, data+off, 4); off += 4;
    return boost::endian::big_to_native(be);

}

int16_t  RtypeServer::Protocol::read_i16be(const uint8_t* data, std::size_t& off, std::size_t size)
{
    if (off+2 > size)
        throw std::runtime_error("read_i16 OOB");
    int16_t be; std::memcpy(&be, data+off, 2); off += 2;
    return boost::endian::big_to_native(be);
}

void RtypeServer::Protocol::writeHelloBody  (std::vector<uint8_t>& out, const HelloBody& b)
{
    append_u32be(out, b.client_nonce);
    append_u8(out, b.proto);
    uint8_t len = static_cast<uint8_t>(std::min<std::size_t>(255, b.name.size()));
    append_u8(out, len);
    out.insert(out.end(), b.name.data(), b.name.data()+len);

}

RtypeServer::HelloBody RtypeServer::Protocol::readHelloBody(const uint8_t* data, std::size_t size, std::size_t& off)
{
    HelloBody b{};
    b.client_nonce = read_u32be(data, off, size);
    b.proto = read_u8(data, off, size);
    uint8_t len = read_u8(data, off, size);
    if (off + len > size) 
        throw std::runtime_error("hello name OOB");
    b.name.assign(reinterpret_cast<const char*>(data+off), len);
    off += len;
    return b;

}

void RtypeServer::Protocol::writeWelcomeBody(std::vector<uint8_t>& out, const WelcomeBody& b)
{
    append_u32be(out, b.playerId);
    append_u16be(out, b.roomId);
    append_u32be(out, b.baselineTick);
}

RtypeServer::WelcomeBody RtypeServer::Protocol::readWelcomeBody(const uint8_t* data, std::size_t size, std::size_t& off)
{       
    WelcomeBody b{};
    b.playerId = read_u32be(data, off, size);
    b.roomId = read_u16be(data, off, size);
    b.baselineTick = read_u32be(data, off, size);
    return b;

}

