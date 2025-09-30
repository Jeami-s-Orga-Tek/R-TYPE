/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Protocol
*/

#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_


#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

#define kMagic 0xCAFE
#define kProtoVersion 1

namespace RtypeServer {
    
    struct NetHeader {
        uint16_t magic;
        uint8_t  version;
        uint8_t  type;
        uint32_t seq;
        uint32_t ack;
        uint32_t ackBits;
    };

    enum class MsgType : uint8_t {
        HELLO    = 1,
        WELCOME  = 2,
        PING     = 3,
        PONG     = 4,
        INPUT    = 10,
        SNAPSHOT = 11,
        EVENT    = 12,
    };

    struct HelloBody {
        uint32_t client_nonce = 0;
        uint8_t  proto = kProtoVersion;
        std::string name;
    };
    
    struct WelcomeBody {
        uint32_t playerId = 0;
        uint16_t roomId = 0;
        uint32_t baselineTick = 0;
    };

    class Protocol {
        public:
            Protocol();
            ~Protocol();
            void writeHeader(std::vector<uint8_t>& out, const NetHeader& h);
            NetHeader readHeader(const uint8_t* data, std::size_t size);
            void append_u8 (std::vector<uint8_t>& out, uint8_t v);
            void append_u16be(std::vector<uint8_t>& out, uint16_t v);
            void append_u32be(std::vector<uint8_t>& out, uint32_t v);
            void append_i16be(std::vector<uint8_t>& out, int16_t v);

            uint8_t  read_u8 (const uint8_t* data, std::size_t& off, std::size_t size);
            uint16_t read_u16be(const uint8_t* data, std::size_t& off, std::size_t size);
            uint32_t read_u32be(const uint8_t* data, std::size_t& off, std::size_t size);
            int16_t  read_i16be(const uint8_t* data, std::size_t& off, std::size_t size);
            void writeHelloBody  (std::vector<uint8_t>& out, const HelloBody& b);
            HelloBody readHelloBody(const uint8_t* data, std::size_t size, std::size_t& off);
            void writeWelcomeBody(std::vector<uint8_t>& out, const WelcomeBody& b);
            WelcomeBody readWelcomeBody(const uint8_t* data, std::size_t size, std::size_t& off);

        protected:
        private:
    };
}

#endif /* !PROTOCOL_HPP_ */
