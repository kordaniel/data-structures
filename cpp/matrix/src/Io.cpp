#include "Io.hpp"

#include <iostream>
#include <fstream>
#include <system_error>


namespace IO
{

    FilePath
    PathToNormalizedFilePath(const fs::path& path)
    {
        fs::path canonicalPath = fs::weakly_canonical(path).make_preferred();
        return { canonicalPath.parent_path(), canonicalPath.filename() };
    }

    std::optional<std::vector<FilePath>>
    FilesInDirectory(std::string_view path)
    {
        std::vector<FilePath> files;

        try
        {
            for (const auto& e : fs::directory_iterator(path)) {
                files.emplace_back(PathToNormalizedFilePath(e.path()));
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << "[ERROR]: " << e.what() << '\n';
            return std::nullopt;
        }

        return files;
    }

    std::optional<std::vector<std::string>>
    ReadLinesTextFile(const std::string& fpath)
    {
        std::string fileLine;
        std::vector<std::string> fileLines;
        std::ifstream in(fpath, std::ifstream::in);

        try
        {
            if (!in) {
                throw std::system_error(
                    errno, std::generic_category(),
                    "Unable to open file\'" + fpath + "\'."
                );
            }
        }
        catch(const std::system_error& e)
        {
            std::cerr << "[ERROR]: Reading textual file lines: " << e.what() << "." << '\n';
            return std::nullopt;
        }

        while (std::getline(in, fileLine))
        {
            fileLines.push_back(fileLine);
        }

        in.close();

        return fileLines;
    }

} // end namespace IO
