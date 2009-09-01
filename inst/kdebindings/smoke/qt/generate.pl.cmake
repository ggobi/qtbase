#!/usr/bin/perl -w

## Run this first, to generate the x_*.cpp files from the Qt headers
## using kalyptus

my $kalyptusdir = "@CMAKE_CURRENT_SOURCE_DIR@/../../kalyptus";

use File::Basename;
use File::Copy qw|cp|;
use File::Compare;
use File::Find;
use Cwd;

my $here = getcwd;
my $outdir = $here . "/generate.pl.tmpdir";
my $finaloutdir = $here;
my $defines = "qtdefines";
my $headerlist = "@CMAKE_CURRENT_SOURCE_DIR@/header_list";
my $definespath = "$here/../$defines";
my $headerlistpath = "$headerlist";
my $qscintilla_headerlist = "";
my $qscintilla_headerlistpath = "";

if("@QSCINTILLA_FOUND@" eq "YES")
{
 $qscintilla_headerlist = "@CMAKE_CURRENT_SOURCE_DIR@/qscintilla2_header_list";
 $qscintilla_headerlistpath = "$here/$qscintilla_headerlist";
}

if("@Qwt5_Qt4_FOUND@" eq "TRUE")
{
 $qwt_headerlist = "@CMAKE_CURRENT_SOURCE_DIR@/qwt_header_list";
 $qwt_headerlistpath = "$here/$qwt_headerlist";
}

## If srcdir != builddir, use headerlist from src
$headerlistpath = $headerlist if ($headerlist =~ /^\//);
if("@QSCINTILLA_FOUND@" eq "YES")
{
 $qscintilla_headerlistpath = $qscintilla_headerlist if ($qscintilla_headerlist =~ /^\//);
}

if("@Qwt5_Qt4_FOUND@" eq "TRUE")
{
 $qwt_headerlistpath = $qwt_headerlist if ($qwt_headerlist =~ /^\//);
}
## Note: outdir and finaloutdir should NOT be the same dir!

# Delete all x_*.cpp files under outdir (or create outdir if nonexistent)
if (-d $outdir) { system "rm -f $outdir/x_*.cpp"; } else { mkdir $outdir; }

mkdir $finaloutdir unless (-d $finaloutdir);

#  Load the QT_NO_* macros found in "qtdefines". They'll be passed to kalyptus
my $macros="";
if ( -e $definespath ){
    print "Found '$defines'. Reading preprocessor symbols from there...\n";
    $macros = " --defines=$definespath ";
}

mkdir $kalyptusdir, 0777;
# Need to cd to kalyptus's directory so that perl finds Ast.pm etc.
chdir "$kalyptusdir" or die "Couldn't go to $kalyptusdir (edit script to change dir)\n";

# Find out which header files we need to parse
# We don't want all of them - e.g. not template-based stuff
my %excludes = (
#    'qaccessible.h' => 1,  # Accessibility support is not compiled by defaut
#    'qassistantclient.h' => 1, # Not part of Qt (introduced in Qt-3.1)
#    'qmotif.h' => 1,       # 
#    'qmotifwidget.h' => 1, # Motif extension (introduced in Qt-3.1)
#    'qmotifdialog.h' => 1, #
#    'qxt.h' => 1, # Xt
#    'qxtwidget.h' => 1, # Xt
#    'qdns.h' => 1, # internal
#    'qgl.h' => 1, # OpenGL
#    'qglcolormap.h' => 1, # OpenGL
#    'qnp.h' => 1, # NSPlugin
 #   'qttableview.h' => 1,  # Not in Qt anymore...
#    'qtmultilineedit.h' => 1,  # Not in Qt anymore...
#    'qwidgetfactory.h' => 1,  # Just an interface
#    'qsharedmemory.h' => 1, # "not part of the Qt API" they say
#    'qwindowsstyle.h' => 1, # Qt windowsstyle, plugin
#    'qmotifstyle.h' => 1,
#    'qcompactstyle.h' => 1,
#    'qinterlacestyle.h' => 1,
#    'qmotifplusstyle.h' => 1,
#    'qsgistyle.h' => 1,
#    'qplatinumstyle.h' => 1,
#    'qcdestyle.h' => 1,
#	 'qworkspace.h' => 1,
#    'qwindowsxpstyle.h' => 1 # play on the safe side 
);

# Some systems have a QTDIR = KDEDIR = PREFIX
# We need a complete list

my %includes;
open(HEADERS, $headerlistpath) or die "Couldn't open $headerlistpath: $!\n";
map { chomp ; $includes{$_} = 1 } <HEADERS>;
close HEADERS;

if("@QSCINTILLA_FOUND@" eq "YES")
{
 open(HEADERS, $qscintilla_headerlistpath) or die "Couldn't open $qscintilla_headerlistpath: $!\n";
 map { chomp ; $includes{$_} = 1 } <HEADERS>;
 close HEADERS;
}

my %qwt_includes;
if("@Qwt5_Qt4_FOUND@" eq "TRUE")
{
 open(HEADERS, $qwt_headerlistpath) or die "Couldn't open $qwt_headerlistpath: $!\n";
 map { chomp ; $qwt_includes{$_} = 1 } <HEADERS>;
 close HEADERS;
}
 
# Can we compile the OpenGl module ?
if("@QT_OPENGL_FOUND@" eq "YES")
{
    open(DEFS, $definespath);
    my @defs = <DEFS>;
    close DEFS;
    if(!grep(/QT_NO_OPENGL/, @defs))
    {
      $excludes{'qgl.h'} = undef;
      $excludes{'qglcolormap.h'} = undef;
    }
    else
    {
      print STDERR "Qt was not compiled with OpenGL support...\n Skipping QGL Classes.\n";
    }
}

# List Qt headers, and exclude the ones listed above
my @headers = ();

@qtinc= (@qt_incs@);

find(
    {   wanted => sub {
	    (-e || -l and !-d) and do {
	        $f = $_;
                if( !defined $excludes{$f} # Not excluded
                     && $includes{$f}        # Known header
                     && /\.h$/)     # Not a backup file etc. Only headers.
                {
                    my $header = $File::Find::name;
                    if ( $header !~ /src/ ) {
                        open(FILE, $header);
                        my @header_lines = <FILE>;
                        if (@header_lines == 1) {
                            $line = $header_lines[0];
                            if ($line =~ /^#include "(.*)"/) {
                                push ( @headers, $File::Find::dir . substr($1, 2) );
                            } else {
                                push ( @headers, $header );
                            }
                        } else {
                            push ( @headers, $header );
                        }
                    }
                }
	    	undef $includes{$f}   
	     };
	},
	follow_fast => 1,
	follow_skip => 2,
	no_chdir => 0
    }, @qtinc
 );

$qwtinc = '@Qwt5_INCLUDE_DIR@';

find(
    {   wanted => sub {
	    (-e || -l and !-d) and do {
	        $f = substr($_, 1 + length $qwtinc);
		push ( @headers, $_ )
                if( !defined $excludes{$f} # Not excluded
                     && $qwt_includes{$f}        # Known header
                     && /\.h$/);     # Not a backup file etc. Only headers.
	    	undef $qwt_includes{$f}   
	     };
	},
	follow_fast => 1,
	follow_skip => 2,
	no_chdir => 1
    }, $qwtinc
 );
 
# Launch kalyptus
chdir "../smoke/qt";
system "perl -I@kdebindings_SOURCE_DIR@/kalyptus @kdebindings_SOURCE_DIR@/kalyptus/kalyptus @ARGV --qt4 --globspace -fsmoke --name=qt $macros --no-cache --outputdir=$outdir @headers";
my $exit = $? >> 8;
exit $exit if ($exit);
chdir "$kalyptusdir";

# Generate diff for smokedata.cpp
unless ( -e "$finaloutdir/smokedata.cpp" ) {
    open( TOUCH, ">$finaloutdir/smokedata.cpp");
    close TOUCH;
}
system "diff -u $finaloutdir/smokedata.cpp $outdir/smokedata.cpp > $outdir/smokedata.cpp.diff";

# Copy changed or new files to finaloutdir
opendir (OUT, $outdir) or die "Couldn't opendir $outdir";
foreach $filename (readdir(OUT)) {
    next if ( -d "$outdir/$filename" ); # only files, not dirs
    my $docopy = 1;
    if ( -f "$finaloutdir/$filename" ) {
        $docopy = compare("$outdir/$filename", "$finaloutdir/$filename"); # 1 if files are differents
    }
    if ($docopy) {
	#print STDERR "Updating $filename...\n";
	cp("$outdir/$filename", "$finaloutdir/$filename");
    }
}
closedir OUT;

# Check for deleted files and warn
my $deleted = 0;
opendir(FINALOUT, $finaloutdir) or die "Couldn't opendir $finaloutdir";
foreach $filename (readdir(FINALOUT)) {
    next if ( -d "$finaloutdir/$filename" ); # only files, not dirs
    if ( $filename =~ /.cpp$/ && ! ($filename =~ /_la_closure.cpp/) && ! -f "$outdir/$filename" ) {
      print STDERR "Removing obsolete file $filename\n";
      unlink "$finaloutdir/$filename";
      $deleted = 1;
    }
}
closedir FINALOUT;

# Delete outdir
system "rm -rf $outdir";

