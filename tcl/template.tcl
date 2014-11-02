#!/usr/bin/env tclsh

proc on_codeblock {str} {
  return "codeblock \[ $str \] ;\n"
}

proc on_codeline {str} {
  return "codeline \[ $str \];\n"
}

proc on_data {str} {
  return "data { $str }\n"
}

proc parse {str} {
  set buf ""
  set ci 0
  set strlen [string length $str]
  while {$ci < $strlen} {
    puts "str\[ci+0\]=\"[string index $str $ci]\""
    puts "str\[ci+1\]=\"[string index $str [expr $ci + 1]]\""
    if {([string index $str $ci] == "<") && ([string index $str [expr $ci + 1]] == "%")} {
      set chunk ""
      set ci [expr $ci + 2]
      while {true} {
        if {([string index $str $ci] == "%") && ([string index $str [expr $ci + 1]] == ">")} {
          set ci [expr $ci + 2]
          break
        }
        append chunk [string index $str $ci]
        set ci [expr $ci + 1]
      }
      if {[string index $chunk 0] == "="} {
        append buf [on_codeline [string range $chunk 1 end]]
      } else {
        append buf $chunk
      }
      set chunk ""
    } else {
      set chunk ""
      while {$ci < $strlen} {
        if {([string index $str $ci] == "<") && ([string index $str [expr $ci + 1]] == "%")} {
          break;
        }
        append chunk [string index $str $ci]
        set ci [expr $ci + 1]
      }
      append buf [on_data $chunk]
      set chunk ""
    }
    set ci [expr $ci + 1]
  }
  return $buf
}

set tpl {
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
}

puts [parse $tpl]


