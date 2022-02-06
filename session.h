#pragma once

#include <memory>
#include <iostream>
#include <boost/asio.hpp>

namespace bulk {

using boost::asio::ip::tcp;
using buffer_t = char;
const int max_length = 1024;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket)
        : m_socket(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:

    tcp::socket m_socket;
    buffer_t m_buffer[max_length];

    void do_read()
    {
        auto self(shared_from_this());
        m_socket.async_read_some(
                    boost::asio::buffer(m_buffer, max_length),
                    [this, self](boost::system::error_code error, std::size_t length)
                    {
                        if (!error) {
                            std::cout << "receive "
                                      << length << "="
                                      << std::string{m_buffer, length} << std::endl;
                            do_write(length);
                        }
                    }
        );
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(
                    m_socket,
                    boost::asio::buffer(m_buffer, length),
                    [this, self](boost::system::error_code error, std::size_t)
                    {
                        if (!error) {
                            do_read();
                        }
                    }
        );
    }
};

}
