//
// Created by lev on 5/9/22.
//

#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <unordered_set>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <tbb/concurrent_unordered_set.h>
#include "measurements.pb.h"


using namespace boost;

class Service : public std::enable_shared_from_this<Service>
{
public:
    Service(std::shared_ptr<asio::ip::tcp::socket> sock, tbb::concurrent_unordered_set<std::string>& acceptor_unique_words);

    void StartHandling();

private:
    void onMessageSizeReceived(const boost::system::error_code &ec, std::size_t bytes_transferred);
    void processProtobuf(esw::Request &request);
    void addToSet(std::string& data);

private:
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    boost::array<char, 4> boost_msg_size;
    tbb::concurrent_unordered_set<std::string> &unique_words;
};

uint32_t read_int32(const char* buffer_int32);
std::string decompressGzip(const std::string& data);

#endif //SERVER_SERVICE_H
