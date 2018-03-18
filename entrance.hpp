#ifndef _ENTRANCE_HPP_
#define _ENTRANCE_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <utility>

template <class Session>
class entrance
{
public:
  
  entrance
    ( boost::asio::io_service&              io_service
    , const boost::asio::ip::tcp::endpoint& endpoint
    )
    : acceptor_ ( io_service, endpoint )
    , socket_   ( io_service )
  {
    do_accept();
  }
 
private:

  void do_accept()
  {
    acceptor_.async_accept
      ( socket_
      , [this](boost::system::error_code ec)
        {
          if (!ec)
          {
	    std::make_shared<Session>(std::move(socket_), app_data_)->start();
          }
	  else
	  {
	    // Silently ignore error.
	  }
          do_accept();
        }
      );
  }

  boost::asio::ip::tcp::acceptor  acceptor_;
  boost::asio::ip::tcp::socket    socket_;
  typename Session::application_t app_data_;
};

#endif//_ENTRANCE_HPP_
