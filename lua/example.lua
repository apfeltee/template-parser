#!/usr/bin/env lua

local Template = require("template")

tpl = Template.new([[
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
]])
print(tpl:parse())

