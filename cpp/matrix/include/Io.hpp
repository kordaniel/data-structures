#ifndef IO_HPP
#define IO_HPP

#include <string_view>
#include <string>
#include <vector>
#include <optional>
#include <filesystem>


namespace IO
{
    namespace fs = std::filesystem;

    struct FilePath
    {
        std::string Path;
        std::string Filename;
    };

    FilePath PathToNormalizedFilePath(const std::filesystem::path& path);

    std::optional<std::vector<FilePath>> FilesInDirectory(std::string_view path);

    std::optional<std::vector<std::string>> ReadLinesTextFile(const std::string& fpath);
} // end namespace IO

#endif // IO_HPP
