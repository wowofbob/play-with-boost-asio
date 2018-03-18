# play-with-boost-asio
Toy project: take boost::asio chat example and make it worse.

This is a modified version of boost chat server example.

Use `./build.sh chat_server` to build application.

Run it with `./app <port>`.

Now I'm only interested in server part of application. So there is no client provied. But you can use boost chat client example.

# What's different.

* Accepting side was abstracted away by `entrance` class. The name may be dump. The idea was to make some sort of entrance to
application which passes control further. For example, there might be `extrance`, then `prelude`, then `something-else`. This
will be useful when user connects to server and asks it about its description, version and so one. Then user sends a request
to go into some mode where there is only one request type going downstream and only one response type going upstream. Whatever.

* Recv/Send loop was abstracted away by `session` class. This class is intended to be a base class. Derived class must implement
callback methods like `on_start`, `on_read`, `on_write`, `on_write_error` and so on. Also, it is the user who decides how server
should behave. There are no loops there. To construct one, user should put `do_read` into `on_start`, then put `do_write` into
`on_read`, and to put `do_read` into `on_write`. This will be the simplest asynchronous read/write loop. Examples are in `chat_server.hpp`
file.

* Why inheritance. Well, there is no just inheritance used. `entrance` expects whatever with constructor of format `(socket, data&)`.
This is how it opens a `session`. Then, `session` abstracts read and write methods, while everything else is should be done by user.
That means it doesn't care about application data. If it would, then each callback would have been implemented as a function with that
data as a parameter. And I don't want to go in there.

* About names. If something has prefix `do_`, then it does something asynchronously; therwise synchronously.

# Summary

Self educational project.
