#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

class message
{
public:

  enum { header_length = 4 };

  message()
    : body_length_(0)
    , data_(header_length, 0)
  {
  }

  const char* data() const
  {
    return data_.data();
  }

  char* data()
  {
    return data_.data();
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data() + header_length;
  }

  char* body()
  {
    return data() + header_length;
  }

  std::size_t get_body_length() const
  {
    return body_length_;
  }

  void set_body_length(std::size_t new_length)
  {
    // Set new length.
    body_length_ = new_length;
    // When new length is more then max
    if (body_length_ > max_body_length_)
    {
      // Limit it to max.
      body_length_ = max_body_length_;
    }
    // When new length is more then size of internal buffer
    if (length() > data_.size())
    {
      // Resize internal buffer.
      data_.resize(length());
    }
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data(), header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length_)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[header_length + 1] = "";
    std::sprintf(header, "%4d", body_length_);
    std::memcpy(data(), header, header_length);
  }

  static std::size_t get_max_body_length()
  {
    return max_body_length_;
  }

  static void set_max_body_length(std::size_t new_max_length)
  {
    max_body_length_ = new_max_length;
  }
  
private:
  
  std::size_t body_length_;

  std::vector<char> data_;

  static std::size_t max_body_length_;

};

#endif // _MESSAGE_HPP_
