#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <cstdio>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <network_node.h>

class Controller : public NetworkNode {
public:

    Controller(int nSwitch);

    virtual void list();
    virtual void exit();
    virtual void processPacket(const Packet& packet);

};

#endif //CONTROLLER_H
