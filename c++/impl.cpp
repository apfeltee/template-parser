
#include <iostream>
#include <sstream>
#include <string>
#include <functional>

class TemplateParser
{
    public:
        using Callback = std::function<void(std::stringstream&, const std::string&)>;

    private:
        size_t m_line = 1;
        size_t m_col  = 1;
        std::string m_src;
        Callback m_oncodeblock;
        Callback m_oncodeline;
        Callback m_ondata;

    public:
        TemplateParser(const std::string& src): m_src(src)
        {
            onCodeBlock([&](std::stringstream& ss, const std::string& str)
            {
                ss << str << ";" << std::endl;
            });
            onCodeLine([&](std::stringstream& ss, const std::string& str)
            {
                ss << "print(" << str << ");" << std::endl;
            });
            onData([&](std::stringstream& ss, const std::string& str)
            {
                ss << "print([[" << str << "]]);" << std::endl;
            });
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
                        m_oncodeline(ss, chunk);
                    }
                    else
                    {
                        m_oncodeblock(ss, chunk);
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
                    m_ondata(ss, chunk);
                    chunk.clear();
                }
            }
            return ss.str();
        }
};

auto example = R"(
    Hello, <%= CurrentUser.getUsername() %>!

    <h2>Current Entries:</h2>
    <ul>
        <% for itm in Storage.getEntries() do %>
            <li>
                <%= itm.name() %>
            </li>
        <% end %>
    </ul>
    
)";

int main(int argc, char* argv[])
{
    TemplateParser tpl(example);
    std::cout << tpl.parse() << std::endl;
}
