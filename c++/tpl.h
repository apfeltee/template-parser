
#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

class TemplateParser
{
    public:
        using Callback = std::function<void(std::stringstream&, std::string&)>;
        using FilterFunc = std::function<bool(std::string&, bool)>;
        using WrapperFunc = std::function<void(std::stringstream&)>;

    private:
        size_t m_line = 1;
        size_t m_col  = 1;
        std::string m_src;
        FilterFunc m_filter;
        WrapperFunc m_onbegin;
        WrapperFunc m_onend;
        Callback m_oncodeblock;
        Callback m_oncodeline;
        Callback m_ondata;

    public: // utility functions
        static std::string& TrimLeft(std::string& str);

        static std::string& TrimRight(std::string& str);

        static std::string& Trim(std::string& str);

        static bool ReplaceAll(std::string& str, const std::string& from, const std::string& to);

    public:
        TemplateParser(const std::string& src);

        void setFilter(FilterFunc fn);

        void onBegin(WrapperFunc fn);

        void onEnd(WrapperFunc fn);

        void onCodeBlock(Callback fn);

        void onCodeLine(Callback fn);

        void onData(Callback fn);

        std::string parse();
};
