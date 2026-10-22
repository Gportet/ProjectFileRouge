#include "Engine/utils/Filepath.h"


Filepath::Filepath(const std::string& path) : _path(path)
{
    
}

Filepath::Filepath(const std::string& path, const std::string& name)
{
    _path = std::filesystem::path(path) / name;
}

bool Filepath::contains(const Filepath& tag) const
{
    if (_path.string().find(tag.str()) != std::string::npos)
    {
        return true;
    }
    return false;
}

std::string Filepath::parentPath() const
{
    return _path.parent_path().string();
}

std::string Filepath::fileName() const
{
    return _path.filename().string();
}
