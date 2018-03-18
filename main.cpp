#include "entrance.hpp"
#include "message.hpp"

#include <iostream>

class dummy_app {
public:

  typedef int application_t;
  
  dummy_app(boost::asio::ip::tcp::socket, application_t) {};

  void start()
  {
    std::cout << __FUNCTION__ << std::endl;
  }

};


std::size_t message::max_body_length_ = 512;

int main() {

  message::set_max_body_length(1024);
  
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 3000);
  entrance<dummy_app> door(io_service, endpoint);
  io_service.run();

}
