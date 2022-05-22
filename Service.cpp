//
// Created by lev on 5/9/22.
//

#include "Service.h"
#include "measurements.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <utility>

Service::Service(std::shared_ptr<asio::ip::tcp::socket> sock, tbb::concurrent_unordered_set<std::string>& acceptor_unique_words) : m_sock(std::move(sock)), unique_words(acceptor_unique_words)
    {
    }

void Service::StartHandling() {
    asio::async_read(*m_sock.get(),
                     boost::asio::buffer(boost_msg_size, 4),
                     [this](const boost::system::error_code &ec, std::size_t bytes_transferred){
                         onMessageSizeReceived(ec, bytes_transferred);
                     }
    );
}

void Service::onMessageSizeReceived(const boost::system::error_code &ec, std::size_t bytes_transferred) {

    if (ec.value() != 0) {
        return;
    }
    uint32_t message_size = read_int32(boost_msg_size.data());
    char buffer_message[message_size];
    size_t read_len =  boost::asio::read(*m_sock, boost::asio::buffer(buffer_message, message_size));
    esw::Request request;
    if(!request.ParseFromArray(buffer_message, message_size)) {
        std::cerr << "Failed to parse Request" << std::endl;
        return;
    }
    processProtobuf(request);
    StartHandling();
}

void Service::processProtobuf(esw::Request &request)
{
    esw::Response response(esw::Response::default_instance());

    if (request.has_getcount()) {
        size_t unique_size = unique_words.size();
        unique_words.clear();
        response.set_status(esw::Response_Status_OK);
        response.set_counter(unique_size);

    } else if (request.has_postwords()) {
        std::string compressed_data = request.postwords().data();
        std::string decompressed_data = decompressGzip(compressed_data);
        addToSet(decompressed_data);
        response.set_status(esw::Response_Status_OK);

    } else {
        response.set_status(esw::Response_Status_ERROR);
        response.set_errmsg("Invalid request");
    }

    size_t output_msg_size = response.ByteSizeLong();
    char * output_array = new char[output_msg_size];
    response.SerializeToArray(output_array, output_msg_size);
    boost::endian::little_to_native_inplace(output_msg_size);
    char buffer_int32[4] = {
        (char)(output_msg_size >> 24 ) ,
        (char)(output_msg_size >> 16),
        (char)(output_msg_size >> 8),
        (char)(output_msg_size)
    };
    asio::write(*m_sock.get(), boost::asio::buffer(buffer_int32, 4));
    asio::write(*m_sock.get(), asio::buffer(output_array, output_msg_size));
}

void Service::addToSet(std::string& data){
    int pos = -1;
    for (size_t i = 0; i < data.length(); i++) {
        if(iswspace(data[i])) {
            if (pos + 1 < i) {
                unique_words.insert(data.substr(pos + 1, i - pos - 1));
            }
            pos = i;
        }
    }
    if (pos + 1 < data.length()) {
        unique_words.insert(data.substr(pos + 1));
    }
}

uint32_t read_int32(const char* buffer_int32)
{
    uint32_t num = 0;
    num |= (unsigned char)(buffer_int32[0]);
    num |= (unsigned char)(buffer_int32[1]) << 8;
    num |= (unsigned char)(buffer_int32[2]) << 16;
    num |= (unsigned char)(buffer_int32[3]) << 24;

    boost::endian::big_to_native_inplace(num);
    return num;
}

std::string decompressGzip(const std::string& data) {
    std::stringstream compressed(data);
    std::stringstream decompressed;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;

    out.push(boost::iostreams::gzip_decompressor());
    out.push(compressed);

    std::istream instream(&out);
    decompressed << instream.rdbuf();
    return decompressed.str();

}

