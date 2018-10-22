#include <fifo.h>

using namespace std;

Fifo::Fifo()
{

}

Fifo::Fifo(int src, int dst)
{
    r_fifo_ = createFifo(src, dst, 'r');
    w_fifo_ = createFifo(src, dst, 'w');
}

Fifo::~Fifo() {
    cout << "Closing FIFO" << endl;
    close(rfd());
    close(wfd());
}

int Fifo::rfd() const {
    return r_fifo_.fd;
}

int Fifo::wfd() const {
    return w_fifo_.fd;
}

int Fifo::dst() const {
    return r_fifo_.dst;
}

Packet Fifo::readPacket() {
    char msg_buf[128];
    read(rfd(), msg_buf, 128);
    return Packet::decode(msg_buf);
}

void Fifo::writePacket(const Packet& packet) {
    // Try opening write connection if it isn't open yet
    if (wfd() == -1) {
        w_fifo_ = createFifo(w_fifo_.src, w_fifo_.dst, 'w');
    }

    if (wfd() != -1) {
        string str_packet = packet.encode();
        write(wfd(), str_packet.c_str(), str_packet.size());
        printf("Sending packet type= %s srcIP= %d dstIP= %d\n", ToString(packet.type), packet.srcIP, packet.dstIP);
    }
}

Fifo::fifo_t Fifo::createFifo(int src, int dst, char rw) {
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

        cout << "Read: " << fifo_name << " " << fifo.fd << endl;
    } else if (rw == 'w') {
        sprintf(fifo_name, "fifo-%d-%d", src, dst);

        if ((fifo.fd = open(fifo_name, O_WRONLY | O_NONBLOCK)) == -1) {
            mkfifo(fifo_name, S_IRUSR | S_IWUSR);
            fifo.fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
        }

        cout << "Write: " << fifo_name << " " << fifo.fd << endl;
    } else {
        // Error
        printf("Cannot create fifo with rw flag \'%c\'", rw);
        return fifo;
    }

    return fifo;
}
