#include <iostream>
#include <fstream>
#include <regex>

#include <boost/filesystem.hpp>

#include "json.hpp"

using namespace boost::filesystem;
using json = nlohmann::json;

json library;

bool is_episode(const path file)
{
    std::regex episode(".* - \\d+.*\\.((mkv)|(mp4)|(avi))", std::regex_constants::icase);
    return std::regex_match(file.filename().string(), episode);
}

bool is_season_folder(const path directory)
{
    if (!is_directory(directory)) return false;

    std::regex season(".*s? ?\\d+$", std::regex_constants::icase);
    if(std::regex_match(directory.filename().string(), season))
    {
        return true;
    }
    else
    {
        directory_iterator end;
        for (directory_iterator dir_itr(directory); dir_itr != end; ++dir_itr)
        {
            if (is_episode(dir_itr->path()))
            {
                return true;
            }
        }
        return false;
    }
}

int get_season_folder_count(const path directory)
{
    int count = 0;
    directory_iterator end;
    for (directory_iterator dir_itr(directory); dir_itr != end; ++dir_itr)
    {
        if (is_season_folder(dir_itr->path()))
        {
            count++;
        }
    }
    return count;
}

bool is_show_folder(const path directory)
{
    directory_iterator end;
    for (directory_iterator dir_itr(directory); dir_itr != end; ++dir_itr)
    {
        if (is_season_folder(dir_itr->path()) || is_episode(dir_itr->path()))
        {
            return true;
        }
    }
    std::cout << "did not match: " + directory.filename().string() << std::endl;
    return false;
}

json parse_video_file(const path file)
{
    return {};
}

json scan_season(const path directory)
{
    json season;
    directory_iterator end;
    for (directory_iterator dir_itr(directory); dir_itr != end; ++dir_itr)
    {
        if (is_episode(dir_itr->path()))
        {
            std::string filename = dir_itr->path().filename().string();
            season[filename] = parse_video_file(dir_itr->path());
        }
    }
    return season;
}

json scan_show(const path show_dir)
{
    json show;
    directory_iterator end;
    int season_count = get_season_folder_count(show_dir);

    if (season_count > 1)
    {
        for (directory_iterator dir_itr(show_dir); dir_itr != end; ++dir_itr)
        {
            std::string name = dir_itr->path().filename().string();
            if (is_season_folder(dir_itr->path()))
            {
                show[name] = scan_season(dir_itr->path());
            }
        }
    }
    else if (season_count == 1)
    {
        for (directory_iterator dir_itr(show_dir); dir_itr != end; ++dir_itr)
        {
            if (is_season_folder(dir_itr->path()))
            {
                show = scan_season(dir_itr->path());
            }
        }
    }
    else if (season_count == 0 && is_season_folder(show_dir))
    {
        show = scan_season(show_dir);
    }

    return show;
}

void scan_library(const path dir_path)
{
    if (!exists(dir_path))
    {
        return;
    }

    directory_iterator end;
    for (directory_iterator dir_itr(dir_path); dir_itr != end; ++dir_itr)
    {
        if (is_show_folder(dir_itr->path()))
        {
            std::string folder_name = dir_itr->path().filename().string();
            library["shows"][folder_name] = scan_show(dir_itr->path());
        }
    }
}

int main(int argc, char **argv)
{
    library["shows"] = {};
    scan_library(argv[1]);

    std::ofstream out_file;
    out_file.open("shows.json");
    out_file << std::setw(4) << library << std::endl;
    out_file.close();
    return 0;
}