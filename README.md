
a repository containing idiomatic ASP-style template parsers.\\
these parsers use plain, old, manual parsing. no regular expressions involved!

root-impl is the C++-version, in 'c++/', most other are deriatives of that one.

If you plan to use one of these parsers in your project, you're well advised to
modify them to fit your needs; they're intended to showcase the algorithm, but not
to provide a generic library.

License: not sure yet, but I guess public domain. ask me if you have any questions.

More implementations may follow eventually.

---

Supported Syntax:

 + Code blocks:

        <% for thing in stuff do %>
            ...
        <% end %>

 + Code lines (i.e., expression that evaluate to some sort of printable value):

        <%= thing %>

 + Comment blocks (these are skipped):

        <%# nobody will ever find out about my dark secret! %>

Approximate API Pseudocode (if you plan to add your implementation, read this part carefully):

    class TemplateParser
    {
        /* the *only* constructor. */
        function TemplateParser(string input)
        {
            self.input = input
        }

        function setFilter(function fn)
        {
            self.m_filter = fn
        }

        function setOnCodeBlock(function fn)
        {
            self.m_on_codeblock = fn
        }

        function setOnCodeLine(function fn)
        {
            self.m_on_codeline = fn
        }

        function setOnData(function fn)
        {
            self.m_on_data = fn
        }

        function parse() returns string
        {
            /* magic happens here. once done, return the result as string */
        }
    }
