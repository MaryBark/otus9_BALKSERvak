#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "OutPuts2Console.h"

using boost::asio::ip::tcp;

class BulkClient
{
public:
    BulkClient(boost::asio::io_service& aIoService, tcp::resolver::iterator aEndpointIterator)
        : mIoService(aIoService)
        , mSocket(aIoService)
    {
        DoConnect(aEndpointIterator);
    }

    void write(const std::string& aMsg)
    {
        mIoService.post(
            [this, aMsg]()
            {
                bool writeInProgress = !mWriteMsgs.empty();
                mWriteMsgs.push_back(aMsg);
                if (!writeInProgress)
                {
                    DoWrite();
                }
            });
    }

    void close()
    {
        mIoService.post([this]() { mSocket.close(); });
    }

private:
    void DoConnect(tcp::resolver::iterator aEndpointIterator)
    {
        boost::asio::async_connect(mSocket, aEndpointIterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
          if (!ec)
          {
            DoWrite();
          }
        });
    }

    void DoWrite()
    {
        boost::asio::async_write(mSocket,
            boost::asio::buffer(mWriteMsgs.front().c_str(),
                mWriteMsgs.front().size()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                mWriteMsgs.pop_front();
                if (!mWriteMsgs.empty())
                    DoWrite();
            }
            else
                mSocket.close();
        });
    }

private:
    boost::asio::io_service& mIoService;
    tcp::socket mSocket;
    BulkQueue mWriteMsgs;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: bulk_client <host> <port>" << std::endl;
            return 1;
        }

        boost::asio::io_service ioService;

        tcp::resolver resolver(ioService);
        auto host = argv[1];
        auto port = argv[2];
        auto endpointIterator = resolver.resolve({ host, port });
        BulkClient client(ioService, endpointIterator);

        std::thread t([&ioService]()
            {
                ioService.run();
            });

        client.write("a");
        client.write("b");
        client.write("\n");
        client.write("c");
        client.write("\n");
        client.write("d");
        client.write("\n");
        client.write("\n");
        client.write("\n");
        client.write("f");
        client.write("\n");
        client.write("g");
        client.write("{");
        client.write("\n");
        client.write("h");
        client.write("\n");
        client.write("i");
        client.write("\n");
        client.write("j");
        client.write("\n");
        client.write("k");
        client.write("\n");
        client.write("\n");
        client.write("l");
        client.write("\n");

        client.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
