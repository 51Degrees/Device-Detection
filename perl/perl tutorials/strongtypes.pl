#!/usr/bin/env perl
use FiftyOneDegrees::PatternV3;
use feature qw/say/;

my $filename;
my $propertyList;
my $userAgentString;

my $filename = "../51Degrees-EnterpriseV3_2.dat";
my $propertyList = "IsMobile,SupportedBearers";

my $provider = new FiftyOneDegrees::PatternV3::Provider(
	$filename,
	$propertyList);

$userAgentString = "Mozilla/5.0 (Linux; Android 4.4.2; en-us; SAMSUNG " .
	"SCH-I545 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) " .
	"Version/1.5 Chrome/28.0.1500.94 Mobile Safari/537.36";

my $match = $provider->getMatch($userAgentString);

say "Supported Bearers as String:";
my $supportedBearers = $match->getValue("SupportedBearers");
say $supportedBearers;

say "Supported Bearers as Array:";
my $supportedBearersArray = $match->getValues("SupportedBearers");
foreach $value (@$supportedBearersArray) {
	say $value;
}
