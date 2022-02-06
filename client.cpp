#include <iostream>
#include <thread>
#include <boost/asio.hpp>

namespace ba = boost::asio;

int main()
{
    std::locale::global(std::locale(""));

    try {

        auto task = [](
                const std::string &msg,
                const std::string &ip,
                short port
                )
        {
            ba::io_context io_context;
            ba::ip::tcp::endpoint ep(ba::ip::address::from_string(ip), port);
            ba::ip::tcp::socket sock(io_context);
            sock.connect(ep);
            ba::write(sock, ba::buffer(msg.c_str(), msg.size()));
        };

        auto f1 = std::async(std::launch::async, task,
                             "1\n2\n3\n4\n5\n6\n{\na\nb\nc\nd\n}\n89\n",
                             "127.0.0.1", 9000);
        auto f3 = std::async(std::launch::async, task,
                             "1\n", "127.0.0.2", 9000);

    } catch(const boost::system::system_error& ex) {
        std::cout << "boost exception! " << ex.what() << std::endl;
    } catch(const std::exception& ex) {
        std::cout << "std::exception! " << ex.what() << std::endl;
    }

    return 0;
}
