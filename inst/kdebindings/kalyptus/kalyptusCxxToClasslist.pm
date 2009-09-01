package kalyptusCxxToClasslist;

use File::Path;
use File::Basename;

use Carp;
use Ast;
use kdocAstUtil;
use kdocUtil;
use Iter;
use kalyptusDataDict;

use strict;
no strict "subs";

sub writeDoc
{
    ( my $lib, my $rootnode, my $outputdir, my $opt ) = @_;

    my $debug = $main::debuggen;

    mkpath( $outputdir ) unless -f $outputdir;

    # Write out all parsed classes
    open DAT, ">$outputdir/classlist";
    Iter::LocalCompounds( $rootnode, sub {
        my $className = join( '::', kdocAstUtil::heritage(shift) );
        print DAT "$className\n";
    });
    print DAT "QGlobalSpace\n";
    close DAT;
}

1;
