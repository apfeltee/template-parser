
#include "tpl.h"

#define STR(...) #__VA_ARGS__

enum ConstVars
{
    bufsize = (1024 * 10),
    chunksize = (1024 * 4),
};

const char* example = STR
(
    Hello, <%= CurrentUser.getUsername() %>!

    <h2>Current Entries:</h2>
    <ul>
        <% for itm in Storage.getEntries() do %>
            <li>
                <%= itm.name() %>
            </li>
        <% end %>
    </ul>
);

int main()
{
    char buffer[bufsize + 1];
    char chunk[chunksize + 1];
    TemplateParser tpl;
    tplparser_init(&tpl, example, buffer, bufsize, chunk, chunksize);
    tplparser_parse(&tpl);
    printf("result:\n%s\n", tpl.result);
    tplparser_fini(&tpl);
}

