# cpp
# testing UDP server
on 2nd termeinal

$ nc -u 127.0.0/1 5000 | cat 

$ echo "hello" | nc -u 127.0.0.1 5000
ACK
----------------
./a.out
-------------------a
test tcp server

$ nc 127.0.0.1 5000

then type message

lamda func
Meaning:

&udp_server::handle_receive → pointer to member function
this → object instance to call
placeholders::error → fill with error_code
placeholders::bytes_transferred → fill with received length

When UDP receives data, Asio internally does something like:
[this](error_code ec, size_t length)
{
    handle_receive(ec, length);
}


so it call 
this->handle_receive(ec,length);
---ld style:

class udp_server
{
public:

    void start()
    {
        socket_.async_receive_from(
            buffer_,
            endpoint_,
            boost::bind(
                &udp_server::handle_receive,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            ));
    }


private:

    void handle_receive(
        boost::system::error_code ec,
        std::size_t bytes)
    {
        if(!ec)
        {
            std::cout << bytes << std::endl;
        }
    }

    boost::asio::ip::udp::socket socket_;
};


------
Modern equivalent:

void start()
{
    socket_.async_receive_from(
        buffer_,
        endpoint_,
        [this](auto ec, auto bytes)
        {
            handle_receive(ec, bytes);
        });
}
--this new style
easier to read
can capture more variables
no boost::bind dependency
works better with C++14/17/20


boost::bind creates a callback object that remembers:
function address:
    udp_server::handle_receive

object:
    this

arguments:
    error
    bytes
Later Boost.Asio calls it:

packet arrives
      |
      v
boost::bind object
      |
      v
this->handle_receive(ec, length)


--------------------------ver 1----------lambda call handle_recives()
Lambda version 1 (wrapper)

socket_.async_receive_from(
    buffer,
    endpoint,

    [this](boost::system::error_code ec,
           std::size_t length)
    {
        handle_receive(ec, length);
    }
);
Old style:

async_receive
      |
      v
boost::bind
      |
      v
handle_receive()




-------------------------------ver 2 (function body moved into lambda)emove handle_receive() completely:
socket_.async_receive_from(
    buffer,
    endpoint,

    [this](boost::system::error_code ec,
           std::size_t length)
    {
        if(!ec)
        {
            std::cout << "RX "
                      << length
                      << " bytes\n";
        }
    }
);

packet arrives
      |
      v
lambda callback
      |
      v
process packet




Modern style A:

async_receive
      |
      v
lambda
      |
      v
handle_receive()


Modern style B:

async_receive
      |
      v
lambda
      |
      v
processing code
