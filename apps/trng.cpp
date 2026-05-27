#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {

        std::cout << "USAGE: " << argv[0] << " times" << std::endl;
        std::cout << "USAGE: " << argv[0] << " times max" << std::endl;
        std::cout << "USAGE: " << argv[0] << " times min max" << std::endl;
        return 0;
    }
    Tesses::Framework::Random random;
    int times = std::atoi(argv[1]);
    if (argc > 2) {
        int32_t min = 0;
        int32_t max = std::stoi(argv[2]);
        if (argc > 3) {
            min = max;
            max = std::stoi(argv[3]);
        }
        for (int i = 0; i < times; i++) {
            std::cout << random.Next(min, max) << std::endl;
        }
    } else {
        for (int i = 0; i < times; i++) {
            std::cout << random.Next() << std::endl;
        }
    }
}