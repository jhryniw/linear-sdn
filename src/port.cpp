#include <port.h>

using namespace std;

Port::Port() :
    src_(-1),
    dst_(-1) {}

Port::Port(int src, int dst) :
    src_(src),
    dst_(dst)
{

}

unique_ptr<Packet> Port::readPacket() {
    char msg_buf[128];
    read(rfd(), msg_buf, 128);
    return Packet::decode(msg_buf);
}

void Port::writePacket(const Packet& packet) {
    if (wfd() != -1) {
        string str_packet = packet.encode();
        write(wfd(), str_packet.c_str(), str_packet.size());
    }
}

/************************************************************/

FifoPort::FifoPort(int src, int dst) :
    Port(src, dst)
{
    r_fifo_ = createFifo(src, dst, 'r');
    w_fifo_ = createFifo(src, dst, 'w');
}

FifoPort::~FifoPort() {
    close(rfd());
    close(wfd());
}

int FifoPort::rfd() const {
    return r_fifo_.fd;
}

int FifoPort::wfd() const {
    return w_fifo_.fd;
}

void FifoPort::writePacket(const Packet& packet) {
    // Try opening write connection if it isn't open yet
    if (wfd() == -1) {
        w_fifo_ = createFifo(w_fifo_.src, w_fifo_.dst, 'w');
    }

    Port::writePacket(packet);
}

FifoPort::fifo_t FifoPort::createFifo(int src, int dst, char rw) {
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

/************************************************************/

SocketPort::SocketPort() :
    Port(),
    sfd_(-1) {}

SocketPort::SocketPort(int sfd) :
    Port(),
    sfd_(sfd) {}

SocketPort::SocketPort(int src, int dst, const std::string& dst_ip, int dst_port) :
    Port(src, dst) {

    sfd_ = socket(AF_INET, SOCK_STREAM, 0);
    connect(dst_ip, dst_port);
}

SocketPort::~SocketPort() {
    close(sfd_);
}

int SocketPort::rfd() const {
    return sfd_;
}

int SocketPort::wfd() const {
    return sfd_;
}

unique_ptr<Packet> SocketPort::readPacket() {
    ssize_t n;
    char msg_buf[128];

    // n > 0  Packet received successfully
    // n = 0  Peer closed connection gracefully
    // n < 0  Error occured
    if ((n = recv(rfd(), msg_buf, 128, 0)) > 0) {
        return Packet::decode(msg_buf);
    } else if (n < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            // Ignore these errors
            return nullptr;
        } else {
            perror("recv error");
            fflush(stdout);
        }
    }

    // Generate a close packet
    return unique_ptr<Packet>(new Packet(PacketType::CLOSE, dst(), src()));
}

void SocketPort::writePacket(const Packet &packet) {
    if (wfd() != -1) {
        string str_packet = packet.encode();
        send(wfd(), str_packet.c_str(), str_packet.size(), MSG_NOSIGNAL);
    }
}

void SocketPort::connect(const std::string& str_ip, int port) {
    char err_str[256];
    in_addr_t server_ip = 0;

    if (inet_pton(AF_INET, str_ip.c_str(), &server_ip) < 1) {
        sprintf(err_str, "Invalid server IP %s", str_ip.c_str());
        perror(err_str);
        fflush(stdout);
    }

    sockaddr_in serv;
    memset((char *)&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr = { .s_addr = server_ip };
    serv.sin_port = static_cast<unsigned short>(port);

    cout << "Connecting to server ";
    cout.flush();
    if (::connect(sfd_, (sockaddr *)&serv, sizeof(serv)) < 0) {
        sprintf(err_str, "Could not connect to %s", str_ip.c_str());
        perror(err_str);
        fflush(stdout);
    }
    cout << "done" << endl;
}
