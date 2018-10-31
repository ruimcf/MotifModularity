#include "CommunityPartition.test.h"

CommunityPartition Test::partition;

void Test::startTest(){
    partition.init(10, 3);
    partition.randomPartition(3);
    std::cout << partition.toStringPartitionByCommunity() << std::endl;
}

int main(int argc, char ** argv)
{
    Test::startTest();

    return 0;
};

