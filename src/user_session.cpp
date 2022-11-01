#include "dyno.h"

namespace dyno {

    user_session::user_session(asio::ip::tcp::socket socket)
    : m_socket(std::move(socket)), m_readVersionExchange(255) {}

    void user_session::start(const std::string& software_version,
                             const std::string& comments) {

        const std::string ssh = "SSH-2.0-";

        m_writeVersionExchange.insert(m_writeVersionExchange.begin(),
                                      ssh.begin(), ssh.end());

        // Software version is required
        m_writeVersionExchange.insert(m_writeVersionExchange.begin(),
                software_version.begin(), software_version.end());

        if (!comments.empty()) {
            m_writeVersionExchange.push_back(' ');
            m_writeVersionExchange.insert(m_writeVersionExchange.begin(),
                                          comments.begin(), comments.end());
        }

        // CRLF
        m_writeVersionExchange.push_back('\r');
        m_writeVersionExchange.push_back('\n');

        //asio::async_write(m_socket,
        //                  asio::buffer(m_writeVersionExchange),
        //                  [this](const std::error_code& ec, size_t) {
        //    if (!ec) {
        //    } else {
        //        close();
        //    }
        //});

        asio::async_read_until(m_socket,
                               m_readVersionExchange,
                               "\r\n", [this](const std::error_code& ec, size_t) {
            if (!ec) {
                // verify the client
                auto constBuffer = m_readVersionExchange.data();

                std::string version;
                version.insert(version.begin(), asio::buffers_begin(constBuffer), asio::buffers_end(constBuffer));

                // now process the version string
                if (!version.starts_with("SSH-2.0-")) {
                    close();
                    return;
                }

                std::string_view software(version);
                auto f = software.find(" ", 8);
                software = software.substr(8, f == std::string::npos ? version.length() - 8 - 2: f - 8);

                // then there are comments
                if (f != std::string::npos) {
                    std::string_view comments(version);
                    comments = comments.substr(f - 8, software.length() - 8 - 2);
                }
            } else {
                close();
            }
        });

    }

    void user_session::close() {
        m_socket.close();
    }

}
