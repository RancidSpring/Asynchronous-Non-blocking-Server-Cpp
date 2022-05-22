//
// Created by lev on 5/8/22.
//

#include "Acceptor.h"
#include "Service.h"


Acceptor::Acceptor(asio::io_service &ios, unsigned short port_num, tbb::concurrent_unordered_set<std::string>& server_unique_words) : m_ios(ios),
                                                               m_acceptor(m_ios,
                                                                          asio::ip::tcp::endpoint(
                                                                                  asio::ip::tcp::v6(),
                                                                                  port_num)),
                                                               m_isStopped(false),
                                                               unique_words(server_unique_words)
    {
    std::cout << "Server full address is: " << asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port_num) << std::endl;
    }

void Acceptor::Start()
{
    m_acceptor.listen();
    InitAccept();
}

void Acceptor::Stop() {
        m_isStopped.store(true);
}

void Acceptor::InitAccept() {
        std::shared_ptr<asio::ip::tcp::socket>
                sock(new asio::ip::tcp::socket(m_ios));

        m_acceptor.async_accept(*sock.get(),
                                [this, sock](
                                        const boost::system::error_code &error)
                                {
                                    onAccept(error, sock);
                                });
}

void Acceptor::onAccept(const boost::system::error_code &ec, std::shared_ptr<asio::ip::tcp::socket> sock) {
    if (ec.value() == 0) {
        (new Service(sock, unique_words))->StartHandling();
    }
    else {
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();
    }

    if (!m_isStopped.load()) {
        InitAccept();
    }
    else {
        m_acceptor.close();
    }
}