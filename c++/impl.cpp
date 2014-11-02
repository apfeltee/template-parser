
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

    private:
        size_t m_line = 1;
        size_t m_col  = 1;
        std::string m_src;
        FilterFunc m_filter;
        Callback m_oncodeblock;
        Callback m_oncodeline;
        Callback m_ondata;

    public: // utility functions
        static inline std::string& TrimLeft(std::string& str)
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

        static inline std::string& TrimRight(std::string& str)
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

        static inline std::string& Trim(std::string& str)
        {
            return TrimLeft(TrimRight(str));
        }

    public:
        TemplateParser(const std::string& src): m_src(src)
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
                return true;
            });
            onCodeBlock([&](std::stringstream& ss, std::string& str)
            {
                ss << str << ";" << std::endl;
            });
            onCodeLine([&](std::stringstream& ss, std::string& str)
            {
                ss << "print(" << str << ");" << std::endl;
            });
            onData([&](std::stringstream& ss, std::string& str)
            {
                ss << "print([[" << str << "]]);" << std::endl;
            });
        }

        void setFilter(FilterFunc fn)
        {
            m_filter = fn;
        }

        void onCodeBlock(Callback fn)
        {
            m_oncodeblock = fn;
        }

        void onCodeLine(Callback fn)
        {
            m_oncodeline = fn;
        }

        void onData(Callback fn)
        {
            m_ondata = fn;
        }

        std::string parse()
        {
            size_t ci;
            std::stringstream ss;
            ci = 0;
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
            return ss.str();
        }
};

auto example = R"(
Hello, <%= CurrentUser.getUsername() %>!
<%# another comment
over
several
lines%>
<h2>Current Entries:</h2>
<ul>
    <% for itm in Storage.getEntries() do %>
        <%# this is also a comment %>
        <% if itm.isOwnedBy(<CurrentUser>) then %>
            <li>
                <%= itm.name() %>
            </li>
        <% end %>
    <% end %>
</ul>    
)";

int main(int argc, char* argv[])
{
    TemplateParser tpl(example);
    std::cout << tpl.parse() << std::endl;
}
