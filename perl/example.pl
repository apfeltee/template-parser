
use Template;

my $tpl = new Template('Hello, <%=$config->{username}%>!');

print("result:\n" . $tpl->parse() . "\n");
