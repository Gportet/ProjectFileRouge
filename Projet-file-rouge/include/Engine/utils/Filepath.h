#pragma once

#include <filesystem>
#include <string>

class Filepath final
{
private:
    std::filesystem::path _path;
    static std::filesystem::path rootPath;
public:
    explicit Filepath(const std::string& name = "");
    Filepath(const std::string& path, const std::string& name); 
    
    [[nodiscard]] std::string str() const { return _path.string(); };
    
    [[nodiscard]] bool empty() const { return _path.empty(); };
    
    bool operator==(const Filepath &name) const { return _path == name._path; };
    bool operator!=(const Filepath &name) const { return _path != name._path; };
    bool operator<(const Filepath &name) const { return _path < name._path; };
    
    [[nodiscard]] bool contains(const Filepath& tag) const;
    
    [[nodiscard]] std::string parentPath() const;
    [[nodiscard]] std::string fileName() const;
    
};
