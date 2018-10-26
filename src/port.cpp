#include <port.h>

using namespace std;

Port::Port(int src, int dst)
{
    r_fifo_ = createFifo(src, dst, 'r');
    w_fifo_ = createFifo(src, dst, 'w');
}

Port::~Port() {
    close(rfd());
    close(wfd());
}

int Port::rfd() const {
    return r_fifo_.fd;
}

int Port::wfd() const {
    return w_fifo_.fd;
}

int Port::dst() const {
    return r_fifo_.dst;
}

int Port::src() const {
    return r_fifo_.src;
}

unique_ptr<Packet> Port::readPacket() {
    char msg_buf[128];
    read(rfd(), msg_buf, 128);
    return Packet::decode(msg_buf);
}

void Port::writePacket(const Packet& packet) {
    // Try opening write connection if it isn't open yet
    if (wfd() == -1) {
        w_fifo_ = createFifo(w_fifo_.src, w_fifo_.dst, 'w');
    }

    if (wfd() != -1) {
        string str_packet = packet.encode();
        write(wfd(), str_packet.c_str(), str_packet.size());
    }
}

Port::fifo_t Port::createFifo(int src, int dst, char rw) {
    fifo_t fifo;
    char fifo_name[20];

    fifo.src = src;
    fifo.dst = dst;

    if (src == -1 || dst == -1) {
        return fifo;
    }

    if (rw == 'r') {
        sprintf(fifo_name, "fifo-%d-%d", dst, src);

        if ((fifo.fd = open(fifo_name, O_RDONLY | O_NONBLOCK)) == -1) {
            mkfifo(fifo_name, S_IRUSR | S_IWUSR);
            fifo.fd = open(fifo_name, O_RDONLY | O_NONBLOCK);
        }
    } else if (rw == 'w') {
        sprintf(fifo_name, "fifo-%d-%d", src, dst);

        if ((fifo.fd = open(fifo_name, O_WRONLY | O_NONBLOCK)) == -1) {
            mkfifo(fifo_name, S_IRUSR | S_IWUSR);
            fifo.fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
        }
    } else {
        // Error
        printf("Cannot create fifo with rw flag \'%c\'", rw);
        return fifo;
    }

    return fifo;
}
