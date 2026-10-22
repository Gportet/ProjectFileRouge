public:
    Font(const FontTag& tag, const Filepath& fileName);
    FT_Face getFont(uint16_t fontSize=14);
    
    [[nodiscard]] std::pair<int, int> getTextSize(const std::string& text, uint16_t fontSize=14);
    
    ~Font();
    
};