#include "log_init.hpp"

#include <boost/asio.hpp>
#include <zmq.h>

#include <iostream>

namespace io = boost::asio;
namespace sys = boost::system;

void on_recv(io::posix::stream_descriptor& stream, void* socket_p, const sys::error_code& ec)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << ec.message();
        return;
    }

    int ev{};
    size_t ev_size{sizeof(ev)};
    zmq_getsockopt(socket_p, ZMQ_EVENTS, &ev, &ev_size);

    if (ev & ZMQ_POLLIN)
    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        while (true)
        {
            int rc{zmq_msg_recv(&msg, socket_p, ZMQ_DONTWAIT)};
            if (rc == -1) break;

            std::string data((char*)zmq_msg_data(&msg), rc);
            BOOST_LOG_TRIVIAL(debug) << data;
        }
        zmq_msg_close(&msg);
    }

    // re async_wait
    stream.async_wait(
        io::posix::stream_descriptor::wait_read,
        std::bind(&on_recv, std::ref(stream), socket_p, std::placeholders::_1));
}

int main()
{
    log_init();

    io::io_context io_context;
    io::posix::stream_descriptor stream(io_context);

    auto ctx_p{zmq_ctx_new()};
    auto socket_p{zmq_socket(ctx_p, ZMQ_SUB)};

    char const* endpoint{"tcp://localhost:5563"};
    // char const* endpoint{"ipc:///tmp/zmq/0"};

    zmq_connect(socket_p, endpoint);
    zmq_setsockopt(socket_p, ZMQ_SUBSCRIBE, "B", 1);

    BOOST_LOG_TRIVIAL(info) << "Subscribing to channel 'B' at " << endpoint;

    int fd{};
    size_t fd_size{sizeof(fd)};
    zmq_getsockopt(socket_p, ZMQ_FD, &fd, &fd_size);

    stream.assign(fd);
    stream.async_wait(
        io::posix::stream_descriptor::wait_read,
        std::bind(&on_recv, std::ref(stream), socket_p, std::placeholders::_1));

#if defined(SIGQUIT)
    io::signal_set signals(io_context, SIGINT, SIGTERM, SIGQUIT);
#else
    io::signal_set signals(io_context, SIGINT, SIGTERM);
#endif
    signals.async_wait(
        [&](auto, auto)
        {
            BOOST_LOG_TRIVIAL(info) << "Signal received, stopping io_context";
            io_context.stop();
        });

    io_context.run();

    BOOST_LOG_TRIVIAL(info) << "Shutting down subscriber.";

    zmq_close(socket_p);
    zmq_ctx_destroy(ctx_p);
    return 0;
}
