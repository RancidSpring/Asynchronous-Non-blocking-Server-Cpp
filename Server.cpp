//
// Created by lev on 5/8/22.
//

#include "Server.h"
#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

Server::Server() {
    m_work.reset(new asio::io_service::work(m_ios));
}

void Server::Start(unsigned short port_num, unsigned int thread_pool_size) {
    assert(thread_pool_size > 0);
    acc.reset(new Acceptor(m_ios, port_num, unique_words));
    acc->Start();

    for (unsigned int i = 0; i < thread_pool_size; i++) {
        std::unique_ptr<std::thread> th(
                new std::thread([this]()
                                { m_ios.run(); }));
        m_thread_pool.push_back(std::move(th));
    }
}

void Server::Stop() {
    acc->Stop();
    m_ios.stop();

    for (auto &th : m_thread_pool){
        th->join();
    }
}

