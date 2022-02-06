#pragma once

#include <memory>
#include "session.h"

namespace bulk {

class Server
{
public:
    Server(boost::asio::io_context& io_context, short port)
        : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        m_acceptor.async_accept(
                    [this](boost::system::error_code error, tcp::socket socket)
                    {
                        if (!error) {
                            std::make_shared<Session>(std::move(socket))->start();
                        }
                        do_accept();
                    }
        );
    }

    tcp::acceptor m_acceptor;
};

};
