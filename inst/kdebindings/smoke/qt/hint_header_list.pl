
# CVS admin : run this script to compare the set of Qt headers to
#             the actual content of header_list

$l = "header_list";
$q = $ENV{'QTDIR'} || die "QTDIR not set\n";
@h = <$q/include/*.h>;
grep s|.*/(.*\.h)|$1|, @h;
open(IN, $l) or die "$!: $l\n";
@old = <IN>;
chomp @old;
map{ $h{$_}++ } @h;
map{ $old{$_}++ } @old;
map{ $diff{$_}++ unless $old{$_} } keys %h; 
map{ delete $diff{$_} if /^qconfig.*/ or !/^q/ } keys %diff;
print "Check if any of the following headers should be added to $l:\n\n";
print join "\n", keys %diff,  "\n"






