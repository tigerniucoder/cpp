#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <vector>


class UdpSocket
{
public:

    UdpSocket(boost::asio::io_context& io,
              uint16_t port)
        :
        socket_(io,
                boost::asio::ip::udp::endpoint(
                    boost::asio::ip::udp::v4(),
                    port))
    {
        std::cout << "UDP socket bind port "
                  << port << std::endl;
    }


    void start()
    {
        receive();
    }


private:

    void receive()
    {

        socket_.async_receive_from(
            boost::asio::buffer(recv_buffer_),
            sender_endpoint_,
// lamda funciton stye B shorter, handle recve in side lamda fun body
            [this](boost::system::error_code ec,
                   std::size_t length)
            {

                if (!ec)
                {

                    std::string msg(
                        recv_buffer_.data(),
                        length);


                    std::cout
                        << "RX from "
                        << sender_endpoint_.address()
                        << ":"
                        << sender_endpoint_.port()
                        << " -> "
                        << msg
                        << std::endl;


                    send_reply("ACK");


                }
                else
                {
                    std::cout
                        << "Receive error: "
                        << ec.message()
                        << std::endl;
                }


                // keep receiving
                receive();
            });
    }



    void send_reply(const std::string& msg)
    {

        auto data =
            std::make_shared<std::string>(msg);


        socket_.async_send_to(
            boost::asio::buffer(*data),
            sender_endpoint_,


            [data](boost::system::error_code ec,
                   std::size_t)
            {

                if(ec)
                {
                    std::cout
                    << "Send error "
                    << ec.message()
                    << std::endl;
                }
                else
                {
                    std::cout
                    << "TX ACK"
                    << std::endl;
                }

            });
    }



private:

    boost::asio::ip::udp::socket socket_;

    boost::asio::ip::udp::endpoint sender_endpoint_;

    std::array<char,2048> recv_buffer_;

};



int main()
{

    try
    {

        boost::asio::io_context io;


        UdpSocket udp(io,5000);


        udp.start();


        std::cout
        << "UDP server running..."
        << std::endl;


        io.run();

    }
    catch(std::exception& e)
    {

        std::cerr
        << "Exception: "
        << e.what()
        << std::endl;

    }


    return 0;
}
