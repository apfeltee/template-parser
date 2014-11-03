
#include "tpl.h"

// --- utility functions ---

std::string& TemplateParser::TrimLeft(std::string& str)
{
    str.erase(
        str.begin(),
        std::find_if(
            str.begin(),
            str.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))
        )
    );
    return str;
}

std::string& TemplateParser::TrimRight(std::string& str)
{
    str.erase(
        std::find_if(
            str.rbegin(),
            str.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))
        ).base(),
        str.end()
    );
    return str;
}

std::string& TemplateParser::Trim(std::string& str)
{
    return TrimLeft(TrimRight(str));
}

bool TemplateParser::ReplaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos;
    if(from.empty())
    {
        return false;
    }
    start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return true;
}


// --- impl ---

TemplateParser::TemplateParser(const std::string& src): m_src(src)
{
    setFilter([&](std::string& str, bool is_code)
    {
        if(is_code)
        {
            Trim(str);
            return true;
        }
        std::string copy = str;
        Trim(copy);
        if(copy.size() < 1)
        {
            return false;
        }
        ReplaceAll(str, "\n", "\\\n");
        return true;
    });
    onBegin([&](std::stringstream&){});
    onEnd([&](std::stringstream&){});
    onCodeBlock([&](std::stringstream&, std::string&){});
    onCodeLine([&](std::stringstream&, std::string&){});
    onData([&](std::stringstream&, std::string&){});
}

void TemplateParser::setFilter(FilterFunc fn)
{
    m_filter = fn;
}

void TemplateParser::onBegin(WrapperFunc fn)
{
    m_onbegin = fn;
}

void TemplateParser::onEnd(WrapperFunc fn)
{
    m_onend = fn;
}

void TemplateParser::onCodeBlock(Callback fn)
{
    m_oncodeblock = fn;
}

void TemplateParser::onCodeLine(Callback fn)
{
    m_oncodeline = fn;
}

void TemplateParser::onData(Callback fn)
{
    m_ondata = fn;
}

std::string TemplateParser::parse()
{
    size_t ci;
    std::stringstream ss;
    ci = 0;
    m_onbegin(ss);
    while(ci < m_src.size())
    {
        std::string chunk;
        auto whenlf = [&]()
        {
            if(m_src[ci] == '\n')
            {
                m_line++;
                m_col = 1;
            }
        };
        if((m_src[ci] == '<') && (m_src[ci+1] == '%'))
        {
            ci += 2;
            while(true)
            {
                whenlf();
                if((m_src[ci] == '%') && (m_src[ci+1] == '>'))
                {
                    ci += 2;
                    break;
                }
                chunk.push_back(m_src[ci]);
                ci++;
                m_col++;
            }
            if(chunk[0] == '=')
            {
                chunk.erase(chunk.begin());
                if(m_filter(chunk, true))
                {
                    m_oncodeline(ss, chunk);
                }
            }
            else if(chunk[0] == '#')
            {
                /* skip comment blocks */
            }
            else
            {
                if(m_filter(chunk, true))
                {
                    m_oncodeblock(ss, chunk);
                }
            }
            chunk.clear();
        }
        else
        {
            while(ci < m_src.size())
            {
                whenlf();
                if((m_src[ci] == '<') && (m_src[ci+1] == '%'))
                {
                    break;
                }
                chunk.push_back(m_src[ci]);
                ci++;
                m_col++;
            }
            if(m_filter(chunk, false))
            {
                m_ondata(ss, chunk);
            }
            chunk.clear();
        }
    }
    m_onend(ss);
    return ss.str();
}

