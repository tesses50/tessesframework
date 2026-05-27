#include "TessesFramework/Uuid.hpp"
#include <iostream>

int main(int argc, char **argv) {
    // e794499c
    using namespace Tesses::Framework;

    Uuid uuid = Uuid::Generate();
    /*uuid.time_low = 0xe794499c;
    uuid.time_mid = 0x823c;
    uuid.time_hi_and_version = 0x304c;
    uuid.clock_seq_hi_and_reserved = 0xa4;

    uuid.clock_seq_low = 0x59;
    uuid.node[0] = 0x8f;
    uuid.node[1] = 0xdc;
    uuid.node[2] = 0xd9;
    uuid.node[3] = 0x60;
    uuid.node[4] = 0xb4;
    uuid.node[5] = 0xac;*/

    std::cout << uuid.ToString(
                     Tesses::Framework::UuidStringifyConfig::LowercaseNoCurly)
              << std::endl;
}