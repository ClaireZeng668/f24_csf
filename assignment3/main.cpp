//main cache simulator implementation

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "cache.h"

int main(int argc, char *argv[]) {
    //check if correct number of arguments provided
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <sets> <blocks> <block_size> <write-allocate|no-write-allocate> <write-through|write-back> <lru|fifo>\n";
        return 1;
    }

    //parse command-line arguments
    int sets = std::stoi(argv[1]);
    int blocks = std::stoi(argv[2]);
    int blockSize = std::stoi(argv[3]);
    std::string writeAllocate = argv[4];
    std::string writePolicy = argv[5];
    std::string evictionPolicy = argv[6];

    Cache cacheSim(sets, blocks, blockSize, writeAllocate, writePolicy, evictionPolicy);

    //read memory access trace from standard input
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        char type;
        std::string address;
        int size;

        if (!(iss >> type >> address >> size)) {
            std::cerr << "Error: Invalid trace format\n";
            return 1;
        }

        unsigned int memAddress = std::stoul(address, nullptr, 16);

        if (type == 'l') {
            cacheSim.load(memAddress);
        } else if (type == 's') {
            cacheSim.store(memAddress);
        } else {
            std::cerr << "Error: Unknown operation type\n";
            return 1;
        }
    }

    //pront summary statistics
    cacheSim.printSummary();

    return 0;
}