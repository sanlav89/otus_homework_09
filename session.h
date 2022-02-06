#pragma once

#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include "handler.h"

namespace bulk {

using boost::asio::ip::tcp;
using buffer_t = char;
const int max_length = 1024;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, Handler *handler)
        : m_socket(std::move(socket))
        , m_handler(handler)
    {
    }

    ~Session()
    {
        delete m_handler;
    }

    void start()
    {
        do_read();
    }

private:

    tcp::socket m_socket;
    buffer_t m_buffer[max_length];
    Handler *m_handler;

    void do_read()
    {
        auto self(shared_from_this());
        m_socket.async_read_some(
            boost::asio::buffer(m_buffer, max_length),
            [this, self](boost::system::error_code error, std::size_t length)
            {
                if (!error) {
                    m_handler->receive(m_buffer, length);
                }
            }
        );
    }

};

}
