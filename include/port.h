#ifndef PORT_H
#define PORT_H

#include <cstring>
#include <iostream>
#include <string>
#include <memory>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <utils.h>
#include <packet.h>

/**
 * Bidirectional communication channel between two nodes
 */
class Port {
public:

    Port();
    Port(int src, int dst);
    virtual ~Port() = default;

    /**
     * @return the source node id
     */
    int src() const { return src_; };

    /**
     * Change the source node id
     */
    void setSrc(int src) { src_ = src; };

    /**
     * @return the destination node id
     */
    int dst() const { return dst_; };

    /**
     * Change the destination node id
     */
    void setDst(int dst) { dst_ = dst; };

    /**
     * @return the read file descriptor
     */
    virtual int rfd() const = 0;

    /**
     * @return the write file descriptor
     */
    virtual int wfd() const = 0;

    /**
     * Read an incoming packet from the destination
     */
    virtual std::unique_ptr<Packet> readPacket();

    /**
     * Send an outgoing packet to the destination
     */
    virtual void writePacket(const Packet& packet);

private:
    int src_; /** The source node id */
    int dst_; /** the destination node id */
};


class FifoPort : public Port {
public:

    FifoPort() = default;
    FifoPort(int src, int dst);
    ~FifoPort() override;

    int rfd() const override;

    int wfd() const override;

    void writePacket(const Packet& packet) override;

private:
    struct fifo_t {
        int src;
        int dst;
        int fd = -1;
    };

    fifo_t r_fifo_;
    fifo_t w_fifo_;

    /**
     * Creates one FIFO for reading or writing
     * The src and dst should not be different between calls
     * @return a FIFO struct containing an opened file descriptor
     */
    fifo_t createFifo(int src, int dst, char rw);
};


class SocketPort : public Port {
public:

    SocketPort();
    explicit SocketPort(int sfd);
    SocketPort(int src, int dst, const std::string& dst_ip, int dst_port);
    ~SocketPort() override;

    int rfd() const override;

    int wfd() const override;

    std::unique_ptr<Packet> readPacket() override;

    void writePacket(const Packet& packet) override;

private:
    int sfd_;

    /**
     * Attempt connection with server
     * @param str_server_ip server ip as a string (e.g 127.0.0.1)
     * @param server_port the server's listening port number
     */
    void connect(const std::string& str_server_ip, int server_port);
};

#endif //PORT_H
