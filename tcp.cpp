#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <memory>

using boost::asio::ip::tcp;

class TcpServer
{
public:
    TcpServer(boost::asio::io_context& io, uint16_t port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)),
          socket_(io)
    {
    }

    void start()
    {
        accept();
    }

private:

    void accept()
    {
        std::cout << "Waiting for client...\n";

        acceptor_.async_accept(
            socket_,
            [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::cout << "Client connected from "
                              << socket_.remote_endpoint().address().to_string()
                              << std::endl;

                    receive();
                }
            });
    }

    void receive()
    {
        socket_.async_read_some(
            boost::asio::buffer(buffer_),

            [this](boost::system::error_code ec,
                   std::size_t length)
            {
                if (!ec)
                {
                    std::string msg(buffer_.data(), length);

                    std::cout << "RX: " << msg << std::endl;

                    send("ACK\n");
                }
            });
    }

    void send(const std::string& text)
    {
        auto data = std::make_shared<std::string>(text);

        boost::asio::async_write(
            socket_,
            boost::asio::buffer(*data),

            [this, data](boost::system::error_code ec,
                         std::size_t)
            {
                if (!ec)
                {
                    std::cout << "ACK sent\n";

                    receive();
                }
            });
    }

private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::array<char,1024> buffer_;
};

int main()
{
    boost::asio::io_context io;

    TcpServer server(io,5000);

    server.start();

    io.run();
}
