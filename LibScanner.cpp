#include <iostream>
#include <boost/filesystem.hpp>

#include "json.hpp"

using namespace boost::filesystem;
using json = nlohmann::json;

void scan_library(const path &dir_path)
{
    if (!exists(dir_path))
    {
        return;
    }

    directory_iterator end;
    for (directory_iterator dir_itr(dir_path); dir_itr != end; ++dir_itr)
    {

        if (is_directory(dir_itr->status()))
        {
            // found a sub directory, recurse
            std::cout << dir_itr->path().filename();
            std::cout << std::endl;
            scan_library(dir_itr->path());
        }
        else
        {
            // found a file
            std::cout << dir_itr->path().filename();
            std::cout << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    scan_library(argv[1]);
    return 0;
}