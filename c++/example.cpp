
#include "tpl.h"

auto example = R"(
<% header("Content-Type: text/html"); %>
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
    tpl.onBegin([&](std::stringstream& ss)
    {
        ss
            << "#include <iostream>\n"
            << "int main()\n{\n"
        ;
    });
    tpl.onEnd([&](std::stringstream& ss)
    {
        ss << "\n};\n";
    });
    tpl.onCodeBlock([&](std::stringstream& ss, std::string& str)
    {
        ss << str << ";" << std::endl;
    });
    tpl.onCodeLine([&](std::stringstream& ss, std::string& str)
    {
        ss << "std::cout << (" << str << ") << std::endl;" << std::endl;
    });
    tpl.onData([&](std::stringstream& ss, std::string& str)
    {
        ss << "puts(\"" << str << "\");" << std::endl;
    });
    std::cout << tpl.parse() << std::endl;
}

