//
// Created by lev on 5/8/22.
//
#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H


#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <unordered_set>
#include <tbb/concurrent_unordered_set.h>

using namespace boost;

class Acceptor {
public:
    Acceptor(asio::io_service &ios, unsigned short port_num, tbb::concurrent_unordered_set<std::string>& unique_words);

    //The Start() method is intended to instruct an object of the Acceptor class to start listening and accepting incoming connection requests.
    void Start();

    // Stop accepting incoming connection requests.
    void Stop();
private:
    void InitAccept();

    void onAccept(const boost::system::error_code &ec,
                 std::shared_ptr<asio::ip::tcp::socket> sock);

    asio::io_service &m_ios;
    asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_isStopped;
    tbb::concurrent_unordered_set<std::string> &unique_words;
    std::mutex mtx;
};


#endif //SERVER_ACCEPTOR_H
