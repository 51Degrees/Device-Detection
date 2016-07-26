package FiftyOneDegrees::MakeMakerConfig;

use 5.010001;

use strict;
use warnings;

use FindBin;
use File::Spec;

unless ( eval { require ExtUtils::CChecker } ) {
    require Carp;
    Carp::confess 'ExtUtils::CChecker is required to ensure that all necessary libraries are installed';
}

my %makemaker_defaults;
{
    my $required = sub {
        require Carp;
        Carp::confess sprintf '%s is required', $_[1];
    };

    %makemaker_defaults = (
        name            => $required,
        version         => $required,
        object          => sub {
            my ( $self, $type ) = @_;

            my $sourcedir = $self->sourcedir;
            my $type_sourcedir = $self->type_sourcedir;

            join ' ', (map {
                File::Spec->catfile($type_sourcedir, $_);
            } qw(
                51Degrees.o
                Match.o
                Provider.o
                Profiles.o
                51Degrees_perl.o
            )), File::Spec->catfile($sourcedir, 'threading.o'),
                File::Spec->catfile($sourcedir, qw(cityhash city.o));
        },
        inc             => sub {
            my $self = shift;
            join ' ', map { "-I$_" } $self->sourcedir, $self->type_sourcedir;
        },
        libs            => sub {
            my $self = shift;

            my @libs = qw(-lm);

            my $cc = ExtUtils::CChecker->new(quiet => 1);

            # Silence (anticipated) compilation errors.
            {
                open my $olderr, '>&', \*STDERR or warn "couldn't preserve STDERR: $!";
                close STDERR;

                $cc->find_libs_for(
                    libs    => ['', 'rt'],
                    diag    => 'no clock_gettime()',
                    source  => <<'CCODE',
#include <time.h>

int main(int argc, char *argv[]) {
    struct timespec timeout;
    return clock_gettime(CLOCK_REALTIME, &timeout);
}
CCODE
                );

                open STDERR, '>&', $olderr or warn "failed to restore STDERR: $!";
            }

            push @libs, @{$cc->extra_linker_flags // []};

            return join ' ', @libs;
        },
        ldfrom          => sub { '51Degrees.o Match.o Provider.o Profiles.o 51Degrees_perl.o city.o threading.o' },
        cc				=> sub { 'g++' },
        ld				=> sub { 'g++' },
        define 			=> sub { '-DHTTP_HEADERS_PREFIXED' },
        optimize		=> sub { $ENV{'51DEGREES_DEBUG_COMPILE'} // 0 == 1 ? '-g' : '-O2' },
        build_requires  => sub { +{
            'ExtUtils::MakeMaker' => '0',
            'ExtUtils::CChecker'  => '0',
        } },
    );

    if ( $] >= 5.005 ) {
        $makemaker_defaults{abstract} = sub {
            sprintf 'A wrapper for 51Degrees Device Detection %s library for detecting devices from their User-Agent.',
                ucfirst shift->type;
        };
        $makemaker_defaults{author} = sub { ['support@51degrees.com'] };
    }

    my %defaults = (
        type            => $required,
        sourcedir       => sub {
            File::Spec->catdir($FindBin::Bin, qw(.. .. src));
        },
        type_sourcedir  => sub {
            my ( $self ) = @_;
            File::Spec->catdir($self->sourcedir, $self->type);
        },
        source          => sub {
            File::Spec->catfile(shift->type_sourcedir, qw(51Degrees_perl.cxx));
        },
        interface       => sub {
            File::Spec->catfile(shift->type_sourcedir, qw(51Degrees.i));
        },
        outdir          => sub {
            File::Spec->catdir($FindBin::Bin, qw(lib FiftyOneDegrees));
        },
        %makemaker_defaults,
    );

    while ( my ( $attr, $code ) = each %defaults ) {
        my $accessor = sub {
            my $self = shift;

            if ( @_ ) {
                $self->{$attr} = $_[0];
            }
            elsif ( !exists $self->{$attr} ) {
                $self->{$attr} = $self->$code( $attr );
            }

            my $returned = $self->{$attr};
        };

        {
            no strict 'refs';
            no warnings qw(once redefine);
            *$attr = $accessor;
        }
    }
}

sub new {
    my ( $class, %params ) = @_;
    bless \%params, $class;
}

sub swigbuild {
    my $self = shift;

    system(
        "swig",
        "-perl",
        "-c++",
        "-module", $self->name,
        "-outdir", $self->outdir,
        "-o", $self->source,
        $self->interface,
    );

    my $err = $! || 'unknown error';

    my $status = $? >> 8;

    die "Error running swig: $err" unless (my $success = $status == 0);

    return $success;
}

sub settings {
    my ( $self, %settings ) = @_;

    (exists $settings{uc $_} or $settings{uc $_} = $self->$_) for keys %makemaker_defaults;

    return \%settings;
}

sub write_makefile {
    my $self = shift;

    # See lib/ExtUtils/MakeMaker.pm for details of how to influence
    # the contents of the Makefile that is written.
    require ExtUtils::MakeMaker;
    ExtUtils::MakeMaker::WriteMakefile(%{$self->settings(@_)});
}

sub prepare {
    my $self = shift;
    $self->swigbuild(@_);
    $self->write_makefile(@_);
}

1;
