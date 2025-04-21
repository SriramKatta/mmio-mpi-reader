#include "parseCLA.hpp"

int main(int argc, char const *argv[])
{
    std::ifstream fin;
    parseCLA(argc, argv, fin);
    return 0;
}
