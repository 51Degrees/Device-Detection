#!/usr/bin/env perl
use FiftyOneDegrees::PatternV3;
use feature qw/say/;
use Time::HiRes qw/ time /;
use Memory::Usage;

my $litefilename = "../../data/51Degrees-LiteV3.2.dat";
my $premiumfilename = "../../data/51Degrees-PremiumV3.2.dat";
my $enterprisefilename = "../../data/51Degrees-EnterpriseV3.2.dat";
my $inputFile = "../../data/20000 User Agents.csv";
my $propertyList = "IsMobile";
my $cacheSize = 10000;
my $poolSize = 20;

sub speedtest{
	my $provider = @_[0];
	my $file_in = @_[1];
	open my $file_in, $inputFile;
	my $speedtest_start = time;
	while( my $userAgent = <$file_in>) {
		chomp $userAgent;
		my $match = $provider->getMatch($userAgent);
		last if $. == 20000;
	}
	my $speedtest_end = time;
	return sprintf("%.3f", ( $speedtest_end - $speedtest_start ) / 20);
	close $file_in;
}
my $mu = Memory::Usage->new();
$mu->record('start');
my $start = time;

my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$litefilename,
	$propertyList,
	$cacheSize,
	$poolSize);
my $end = time;

$mu->record('end');

my $startmem = @{@{ $mu->state }[0]}[3];
my $endmem =  @{@{ $mu->state }[1]}[3];
my $memused = ($endmem - $startmem) / 1000;

print "Lite initialization time: " . ( $end - $start)*1000 . " ms\n";
print "Lite detections speed: " . speedtest($provider, $inputfile) . " ms\n";
print "Lite memory usage: " . $memused . " Mb\n";


my $start = time;
$mu->record('start');
my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$premiumfilename,
	$propertyList,
	$cacheSize,
	$poolSize);
my $end = time;

$mu->record('end');

$startmem = @{@{ $mu->state }[2]}[3];
$endmem =  @{@{ $mu->state }[3]}[3];
$memused = ($endmem - $startmem) / 1000;

print "Premium initialization time: " . ( $end - $start)*1000 . " ms\n";
print "Premium detections speed: " . speedtest($provider, $inputfile) . " ms\n";
print "Premium memory usage: " . $memused . " Mb\n";

my $start = time;
$mu->record('start');
my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$enterprisefilename,
	$propertyList,
	$cacheSize,
	$poolSize);
my $end = time;

$mu->record('end');

$startmem = @{@{ $mu->state }[4]}[3];
$endmem =  @{@{ $mu->state }[5]}[3];
$memused = ($endmem - $startmem) / 1000;

print "Enterprise initialization time: " . ( $end - $start)*1000 . " ms\n";
print "Enterprise detections speed: " . speedtest($provider, $inputfile) . " ms\n";
print "Enterprise memory usage: " . $memused . " Mb\n";

