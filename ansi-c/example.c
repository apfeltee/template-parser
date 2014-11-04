
#include "tpl.h"

#define STR(...) #__VA_ARGS__

enum ConstVars
{
    bufsize = (1024 * 10),
    chunksize = (1024 * 4),
};

const char* example = STR
(
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
);

int main()
{
    {
        TemplateParser tpl;
        const char* str = "hello, <%=get_username()%>!";
        tplparser_init(&tpl, str, strlen(str), 1024 * 10, 1024 * 4);
        tplparser_parse(&tpl);
        // 'tpl.result' now contains the generated code
        puts(tpl.result);
        tplparser_fini(&tpl);
    }
    puts("-----");
    {
        TemplateParser tpl;
        tplparser_init(&tpl, example, strlen(example), bufsize, chunksize);
        tplparser_parse(&tpl);
        //printf("result:\n%s\n", tpl.result);
        puts("result:"); puts(tpl.result); puts("");
        tplparser_fini(&tpl);
    }
}

