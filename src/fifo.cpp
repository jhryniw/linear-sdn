#include <fifo.h>

using namespace std;

Fifo::Fifo() :
        rfd(-1),
        wfd(-1)
{

}

Fifo::Fifo(int src, int dst)
{
    rfd = createFifo(src, dst, 'r');
    wfd = createFifo(src, dst, 'w');
}

Fifo::~Fifo() {
//    close(rfd);
//    close(wfd);
}

Packet Fifo::readPacket() {
    char msg_buf[128];
    read(rfd, msg_buf, 128);
    return Packet::decode(msg_buf);
}

void Fifo::writePacket(const Packet& packet) {
    string str_packet = packet.encode();
    write(wfd, str_packet.c_str(), str_packet.size());
}

int Fifo::createFifo(int src, int dst, char rw) {
    int ffd;
    char fifo_name[20];

    if (rw == 'r') {
        sprintf(fifo_name, "fifo-%d-%d", dst, src);

        if ((ffd = open(fifo_name, O_RDONLY | O_NONBLOCK)) == -1) {
            mkfifo(fifo_name, S_IRUSR | S_IWUSR);
            ffd = open(fifo_name, O_RDONLY | O_NONBLOCK);
        }

        cout << "Read: " << fifo_name << " " << ffd << endl;
    } else if (rw == 'w') {
        //O_WRONLY
        sprintf(fifo_name, "fifo-%d-%d", src, dst);

        if ((ffd = open(fifo_name, O_WRONLY | O_NONBLOCK)) == -1) {
            mkfifo(fifo_name, S_IRUSR | S_IWUSR);
            ffd = open(fifo_name, O_WRONLY | O_NONBLOCK);
        }

        cout << "Write: " << fifo_name << " " << ffd << endl;
    } else {
        // Error
        return -1;
    }

    return ffd;
}
