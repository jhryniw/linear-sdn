#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include <stdlib.h>

#include <network_node.h>
#include <switch.h>
#include <controller.h>

#define MAX_NSW 7
#define MAXIP 1000

using namespace std;

static const char* CONT_USAGE = "usage: a2sdn cont nSwitch";
static const char* SWI_USAGE = "usage: a2sdn swi trafficFile [null|swj] [null|swk] IPlow-IPhigh";

struct input_t {
    bool isCont;
    int nSwitch;
    int swi;
    int swj;
    int swk;
    string trafficFile;
    int ipLow;
    int ipHigh;
};

// Prototypes
input_t ParseInput(const vector<string>& all_args);

int main(int argc, char** argv) {
    unique_ptr<NetworkNode> host;
    vector<string> all_args;
    input_t input;

    if (argc > 1) {
        all_args.assign(argv, argv + argc);
        input = ParseInput(all_args);
    } else {
        cout << "Too few arguments" << endl;
        exit(1);
    }

    if (input.isCont) {
        printf("Controller -- nSwitch: %d\n", input.nSwitch);
        host = unique_ptr<Controller>(new Controller(input.nSwitch));
    } else {
        printf("Switch %d -- Left: sw%d Right: sw%d tf: %s IPRange: %d-%d\n",
                input.swi, input.swj, input.swk, input.trafficFile.c_str(), input.ipLow, input.ipHigh);

        host = unique_ptr<Switch>(new Switch(input.swi, input.swj, input.swk, input.trafficFile, input.ipLow, input.ipHigh));
    }

    while (true) {
        host->loop();
    }
}

// Source: https://stackoverflow.com/questions/12774207/
inline bool file_exists (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

inline void validate_sw (int sw) {
    if (sw < 1 || sw > MAX_NSW) {
        printf("Invalid switch #%d. Must be in range [1-%d]\n", sw, MAX_NSW);
        exit(1);
    }
}

inline void validate_ip (int ip) {
    if (ip < 0 || ip > MAXIP) {
        printf("Invalid IP %d. Must be in range [0-%d]\n", ip, MAXIP);
        exit(1);
    }
}

void validate_ip_range(int ipl, int iph) {
    validate_ip(ipl);
    validate_ip(iph);

    if (ipl >= iph) {
        printf("Invalid lower bound for IP range %d-%d.\n", ipl, iph);
        exit(1);
    }
}

input_t ParseInput(const vector<string>& all_args) {
    bool isCont;
    int nSwitch;
    int swi, swj, swk;
    string trafficFile;
    int ipLow, ipHigh;

    try {

        if (all_args.at(1) == "cont") {
            isCont = true;
        } else if (sscanf(all_args.at(1).c_str(), "sw%d", &swi)) {
            isCont = false;
            validate_sw(swi);
        } else {
            cout << "Expected: [cont|swi] Got: " << all_args.at(1) << endl;
            exit(1);
        }

        if (isCont) {
            if (all_args.size() > 3) {
                cout << CONT_USAGE << "\nToo many arguments" << endl;
                exit(1);
            }

            if (!sscanf(all_args.at(2).c_str(), "%d", &nSwitch)) {
                cout << CONT_USAGE << "\nExpected: <nSwitch> Got: " << all_args.at(2) << endl;
                exit(1);
            }
        } else {
            if (all_args.size() > 6) {
                cout << SWI_USAGE << "\nToo many arguments" << endl;
                exit(1);
            }

            if (!file_exists(all_args.at(2))) {
                cout << SWI_USAGE << "\n" << all_args.at(2) << " is not a valid filename" << endl;
                exit(1);
            }

            if (all_args.at(3) == "null") {
                swj = Switch::NULL_SW;
            }
            else if (sscanf(all_args.at(3).c_str(), "sw%d", &swj)) {
                validate_sw(swj);
            }
            else {
                cout << SWI_USAGE << "\nExpected: [null|swj] Got: " << all_args.at(3) << endl;
                exit(1);
            }

            if (all_args.at(4) == "null") {
                swk = Switch::NULL_SW;
            }
            else if (sscanf(all_args.at(4).c_str(), "sw%d", &swk)) {
                validate_sw(swk);
            }
            else {
                cout << SWI_USAGE << "\nExpected: [null|swk] Got: " << all_args.at(4) << endl;
                exit(1);
            }

            if (sscanf(all_args.at(5).c_str(), "%d-%d", &ipLow, &ipHigh) == 2) {
                validate_ip_range(ipLow, ipHigh);
            }
            else {
                cout << SWI_USAGE << "\nExpected: IPlow-IPhigh Got: " << all_args.at(5) << endl;
                exit(1);
            }
        }
    } catch (const out_of_range&) {
        cout << "Too few arguments" << endl;
        if (isCont) {
            cout << CONT_USAGE << endl;
            exit(1);
        } else {
            cout << SWI_USAGE << endl;
        }
        exit(1);
    }

    return input_t { isCont, nSwitch, swi, swj, swk, trafficFile, ipLow, ipHigh };
}