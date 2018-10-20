#include <switch.h>

using namespace std;

Switch::Switch(int id, int swj, int swk, string tf_path, int ip_low, int ip_high) :
    NetworkNode(id, 4),
    traffic_file_(tf_path),
    ip_low_(ip_low),
    ip_high_(ip_high)
{
    setPort(0, CONT_PORT);
    setPort(1, swj);
    setPort(2, swk);
}

Switch::~Switch()
{
    traffic_file_.close();
}

void Switch::list()
{
    cout << "Switch Information" << endl;
}

void Switch::exit()
{
    list();
}

void Switch::processPacket(const Packet& packet)
{

}