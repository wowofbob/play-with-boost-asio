#ifndef _SESSION_HPP_
#define _SESSION_HPP_

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "message.hpp"

#define LOG { std::cout << __FUNCTION__ << std::endl; }

class session
  : public std::enable_shared_from_this<session>
{
public:

  session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket))
  {
    LOG;
  }

  virtual ~session()
  {
    LOG;
  }
  
  void start()
  {
    LOG;
    on_start(); //room_.join(shared_from_this());
    do_read();  //do_read_header();
  }

protected:

  virtual void on_start() = 0;
  virtual void on_read(message const& msg) = 0;
  virtual void on_write() = 0;
  virtual void on_finish() = 0;
  
  virtual void on_read_header_error(boost::system::error_code ec) {}
  virtual void on_read_body_error(boost::system::error_code ec) {}
  virtual void on_write_error(boost::system::error_code ec) {}

  void read()
  {
    LOG;
    std::size_t bytes_read;
    boost::system::error_code ec;

    read_header(bytes_read, ec);
    if (ec || bytes_read != message::header_length)
    {
      on_read_header_error(ec);
      return;
    }

    read_body(bytes_read, ec);
    if (ec || bytes_read != read_msg_.get_body_length())
    {
      on_read_body_error(ec);
      return;
    }

    on_read(read_msg_);
  }

  void write(message const& msg)
  {
    LOG;
    boost::system::error_code ec;
    std::size_t bytes_written =
      boost::asio::write
        ( socket_
        , boost::asio::buffer(msg.data(), msg.length())
        , ec
        );
    if (ec || bytes_written != msg.length())
    {
      on_write_error(ec);
    }
    else
    {
      on_write();
    }
  }
  
  void do_read()
  {
    LOG;
    do_read_header();
  }
  
  void do_write(message const& msg)
  {
    LOG;
    auto self(shared_from_this());
    boost::asio::async_write
      ( socket_
      , boost::asio::buffer(msg.data(), msg.length())
      , [this, self](boost::system::error_code ec, std::size_t )
        {
          if (!ec)
          {
            on_write();
	    // write_msgs_.pop_front();
            // if (!write_msgs_.empty())
            // {
            //   do_write();
            // }
          }
          else
          {
	    on_write_error(ec); //room_.leave(shared_from_this());
          }
        });
  }

  template<class Session, class Application>
  void give_control_to(Application& app)
  {
    LOG;
    on_finish();
    std::make_shared<Session>(std::move(socket_), app)->start();
  }
  
private:

  void read_header(std::size_t& bytes_read, boost::system::error_code& ec)
  {
    bytes_read =
      boost::asio::read
        ( socket_
        , boost::asio::buffer(read_msg_.data(), message::header_length)
        , ec
	);
  }

  void read_body(std::size_t& bytes_read, boost::system::error_code& ec)
  {
    bytes_read =
      boost::asio::read
        ( socket_
	, boost::asio::buffer(read_msg_.body(), read_msg_.get_body_length())
	, ec
	);
  }

  void do_read_header()
  {
    LOG;
    auto self(shared_from_this());
    boost::asio::async_read
      ( socket_
      , boost::asio::buffer(read_msg_.data(), message::header_length)
      , [this, self](boost::system::error_code ec, std::size_t)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            on_read_header_error(ec); //room_.leave(shared_from_this());
          }
        });
  }

  void do_read_body()
  {
    LOG;
    auto self(shared_from_this());
    boost::asio::async_read
      ( socket_
      , boost::asio::buffer(read_msg_.body(), read_msg_.get_body_length())
      , [this, self](boost::system::error_code ec, std::size_t)
        {
          if (!ec)
          {
            on_read(read_msg_); //room_.deliver(read_msg_);
            // do_read_header();
          }
          else
          {
	    on_read_body_error(ec); //room_.leave(shared_from_this());
          }
        });
  }

  boost::asio::ip::tcp::socket socket_;
  message read_msg_;
};


#endif//_SESSION_HPP_
