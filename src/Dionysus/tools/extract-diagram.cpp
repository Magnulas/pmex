#include <topology/persistence-diagram.h>

#include <string>
#include <map>
#include <iostream>

#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>

typedef PersistenceDiagram<>                    PDgm;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " FILENAME" << std::endl;
        return 0;
    }
    std::string infilename = argv[1];
    
    std::ifstream ifs(infilename.c_str());
    boost::archive::binary_iarchive ia(ifs);
    
    std::map<Dimension, PDgm> dgms;
    ia >> dgms;
    for (std::map<Dimension, PDgm>::const_iterator cur = dgms.begin(); cur != dgms.end(); ++cur)
        for (PDgm::const_iterator pcur = cur->second.begin(); pcur != cur->second.end(); ++pcur)
            std::cout << cur->first << " " << pcur->x() << " " << pcur->y() << std::endl;
}
