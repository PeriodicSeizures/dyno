#include "dyno.h"

#include <iostream>

using asio::ip::tcp;

namespace dyno {

    server::impl::impl(uint16_t port)
        : m_ctx(), m_acceptor(m_ctx, tcp::endpoint(tcp::v4(), port)), m_socketMove(m_ctx) {
        m_acceptor.listen();
    }

    void server::impl::do_accept() {
        m_acceptor.async_accept(m_socketMove, [this](const asio::error_code& ec) {
            if (!ec) {
                auto sess = std::make_shared<user_session>(std::move(m_socketMove));
                sess->start("dyno1.0");
                {
                    std::scoped_lock<std::mutex> scoped(m_mut);
                    m_sessions.push_back(sess);
                }
            } else {

            }
            do_accept();
        });
    }

    server::server(uint16_t port) : m_impl(new server::impl(port)) {
        m_impl->do_accept();
    }

    void server::run() {
        m_impl->m_thread = std::thread([this]() {
            m_impl->m_ctx.run();
        });
    }

    void server::stop() {
        m_impl->m_ctx.stop();

        if (m_impl->m_thread.joinable())
            m_impl->m_thread.join();
    }
}
