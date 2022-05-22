//
// Created by lev on 5/8/22.
//

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include "Acceptor.h"
#include <unordered_set>
#include <tbb/concurrent_unordered_set.h>

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

using namespace boost;

class Server {
public:
    Server();

    void Start(unsigned short port_num,
               unsigned int thread_pool_size);

    void Stop();
private:
    asio::io_service m_ios;
    std::unique_ptr<asio::io_service::work> m_work;
    std::unique_ptr<Acceptor> acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;
    tbb::concurrent_unordered_set<std::string> unique_words;

};


#endif //SERVER_SERVER_H
