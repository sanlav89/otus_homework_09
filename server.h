#pragma once

#include <memory>
#include "session.h"
#include "logger.h"

namespace bulk {

class Server
{
public:
    Server(boost::asio::io_context& io_context, short port, std::size_t bulkSize)
        : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
        , m_bulkSize(bulkSize)
        , m_console(new logger::Console(std::cout))
        , m_logfile(new logger::LogFile)
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
                    Handler *handler{new Handler(m_bulkSize, m_cmdsStatic)};
                    handler->registerLogger(m_console);
                    handler->registerLogger(m_logfile);
                    std::make_shared<Session>(std::move(socket), handler)->start();
                }
                do_accept();
            }
        );
    }

    tcp::acceptor m_acceptor;
    std::size_t m_bulkSize;
    logger::LogPtr m_console;
    logger::LogPtr m_logfile;
    std::queue<Cmd> m_cmdsStatic;

};

};
