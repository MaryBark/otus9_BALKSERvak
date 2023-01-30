#ifndef SERVER_H
#define SERVER_H


//class server
//{
//public:
//    server();
//};


#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#include "AllProzessors.h"

class Server
{
public:
    Server(boost::asio::io_service& aIoService, const tcp::endpoint& aEndpoint, std::size_t aBulkSize);

private:
    void DoAccept();

    tcp::acceptor mAcceptor;
    tcp::socket mSocket;
    std::size_t mBulkSize;
    std::shared_ptr<CommandProcessor> mCommandProcessor;
};



#endif // SERVER_H
