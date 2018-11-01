#ifndef FIFO_H
#define FIFO_H

#include <iostream>
#include <string>
#include <memory>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <packet.h>

/**
 * Creates bidirectional communication between two nodes
 * src: The node id creating the fifo
 * dst: The node id to connect to
 */
class Port {
public:

    Port() = default;
    Port(int src, int dst);
    ~Port();

    /**
     * @return the read file descriptor
     */
    int rfd() const;

    /**
     * @return the write file descriptor
     */
    int wfd() const;

    /**
     * @return the source node id
     */
    int src() const;

    /**
     * @return the destination node id
     */
    int dst() const;

    /**
     * Read a packet incoming from the destination
     * @return
     */
    std::unique_ptr<Packet> readPacket();
    void writePacket(const Packet& packet);

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

#endif //FIFO_H
