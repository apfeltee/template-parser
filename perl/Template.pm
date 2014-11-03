
package Template;
use warnings;
use strict;
sub new
{
    my $class = shift;
    my $input = shift;
    my $self =
    {
        input => $input,
        line => 1,
        column => 1,
        onCodeBlock => sub
        {
            my $str = shift;
            print("onCodeBlock.str = $str\n");
            return sprintf("%s;\n", $str);
        },
        onCodeLine => sub
        {
            my $str = shift;
            print("onCodeLine.str = $str\n");
            return sprintf("print(%s);\n", $str);
        },
        onData => sub
        {
            my $str = shift;
            print("onData.str = $str\n");
            return sprintf("print(qq{%s});\n", $str);
        },
    };
    bless $self, $class;
    return $self;
}

sub setOnCodeBlock
{
    my $self = shift;
    return ($self->{onCodeBlock} = shift);
}

sub setOnCodeLine
{
    my $self = shift;
    return ($self->{onCodeLine} = shift);
}

sub setOnData
{
    my $self = shift;
    return ($self->{onData} = shift);
}

sub parse
{
    my $ci = 0;
    my $ss = "";
    my $self = shift;
    my $src = $self->{input};
    my $srclen = length($src);
    while($ci < $srclen)
    {
        my $chunk = "";
        if((substr($src, $ci, $ci) eq '<') and (substr($src, $ci+1, $ci+1) eq '%'))
        {
            $ci += 2;
            while(1)
            {
                if((substr($src, $ci, $ci) eq '%') and (substr($src, $ci+1, $ci+1) eq '>'))
                {
                    $ci += 2;
                    last;
                }
                $chunk .= substr($src, $ci, $ci);
                $ci++;
                $self->{column}++;
            }
            if(substr($chunk, 0, 0) eq '=')
            {
                $chunk = substr($chunk, 1, length($chunk));
                $ss .= $self->{onCodeLine}->($chunk);
            }
            elsif(substr($chunk, 0, 0) eq '#')
            {
                # skip comment blocks
            }
            else
            {
                $ss .= $self->{onCodeBlock}->($chunk);
            }
            $chunk = "";
        }
        else
        {
            while($ci < $srclen)
            {
                if((substr($src, $ci, $ci) eq '<') and (substr($src, $ci+1, $ci+1) eq '%'))
                {
                    last;
                }
                $chunk .= substr($src, $ci, $ci);
                $ci++;
                $self->{column}++;
            }
            $ss .= $self->{onData}->($chunk);
            $chunk = "";
        }
    }
    return $ss;
}

1;