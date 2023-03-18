#!/usr/bin/env perl
use strict;
use warnings;
use CGI;
my $cgi = CGI->new();
print $cgi->header;

print <<'END';
<!doctype html>
<html> HTML Goes Here </html>
END