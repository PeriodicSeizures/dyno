#ifndef DYNO_H
#define DYNO_H

#include <cstdint>
#include <memory>
#include <queue>
#include <asio.hpp>
#include <thread>

namespace dyno {
    class user_session {
    private:
        asio::ip::tcp::socket m_socket;
        std::vector<char> m_writeVersionExchange;

        // Initial version exchange \r\n
        asio::streambuf m_readVersionExchange;

        // Write binary packet
        uint32_t m_tempWriteLength;
        std::queue<std::vector<std::byte>> m_writePacketQueue;

        // Read binary packet
        uint32_t m_readPacketLength;
        std::vector<std::byte> m_readPacket;

    public:
        using Ptr = std::shared_ptr<user_session>;

        void close();

    public:
        user_session(asio::ip::tcp::socket socket);

        void start(const std::string& software_version,
                   const std::string& comments = "");

    };



    class server {
    private:
        struct impl {
            friend server;

            std::thread m_thread;
            asio::io_context m_ctx;
            asio::ip::tcp::acceptor m_acceptor;
            asio::ip::tcp::socket m_socketMove; // instead of constructing a temp every time

            std::mutex m_mut;
            std::list<user_session::Ptr> m_sessions;

            explicit impl(uint16_t port);

            void do_accept();
        };

        std::unique_ptr<impl> m_impl;

    public:
        explicit server(uint16_t port = 22);

        void run();
        void stop();
    };


    // These are currently broad to cover the overall types
    // https://www.rfc-editor.org/rfc/rfc4251
    enum class message_type : uint8_t {
        tpt_generic = 1,
        tpt_algo = 20,
        tpt_kex = 30,

        user_generic = 50,
        user_aex = 60,

        conn_generic = 80,
        conn_chan = 90,

        local_ext = 192
    };
}

#endif //DYNOSSH_DYNOSSH_H
