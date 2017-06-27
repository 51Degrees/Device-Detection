#!/usr/bin/env perl
use FiftyOneDegrees::TrieV3;
use feature qw/say/;
use Time::HiRes qw/ time /;
use Memory::Usage;

my $litefilename = "../../data/51Degrees-LiteV3.2.trie";
my $premiumfilename = "../../data/51Degrees-PremiumV3.2.trie";
my $enterprisefilename = "../../data/51Degrees-EnterpriseV3.2.trie";
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

sub runtests{
	my $dataFile = @_[0];
	if(-e $dataFile){
		my $mu = Memory::Usage->new();
		$mu->record('start');
		my $start = time;

		my $provider = new FiftyOneDegrees::TrieV3::Provider(
			$dataFile,
			$propertyList);
		my $end = time;

		$mu->record('end');

		my $startmem = @{@{ $mu->state }[0]}[3];
		my $endmem =  @{@{ $mu->state }[1]}[3];
		my $memused = ($endmem - $startmem) / 1000;

		print "   Initialization time: " . ( $end - $start)*1000 . " ms\n";
		print "   Detections speed: " . speedtest($provider, $inputfile) . " ms\n";
		print "   Memory usage: " . $memused . " Mb\n";
	}
	else{
		print "   ".$dataFile." could not be found.\n";
	}
}

print "Lite Tests:\n";
runtests($litefilename);

print "Premium Tests:\n";
runtests($premiumfilename);

print "Enterprise Tests:\n";
runtests($enterprisefilename);
