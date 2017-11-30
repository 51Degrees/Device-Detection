#!/usr/bin/perl

# (C) Sergey Kandaurov
# (C) Maxim Dounin
# (C) Nginx, Inc.

# Tests for 51Degrees Hash Trie module.

###############################################################################

use warnings;
use strict;

use Test::More;

BEGIN { use FindBin; chdir($FindBin::Bin); }

use lib 'nginx-tests/lib';
use Test::Nginx;
use URI::Escape;

###############################################################################

select STDERR; $| = 1;
select STDOUT; $| = 1;

my $t = Test::Nginx->new()->has(qw/http/)->plan(17)
	->write_file_expand('nginx.conf', <<'EOF');

daemon off;

%%TEST_GLOBALS%%
events {
}

http {
    %%TEST_GLOBALS_HTTP%%

	51D_valueSeparator ^sep^;
	
	
    server {
        listen       127.0.0.1:8080;
        server_name  localhost;
		

		51D_match_single x-server-ismobile-single IsMobile;
		51D_match_all x-server-ismobile-all IsMobile;
		
        location /single {
			51D_match_single x-ismobile IsMobile;
			add_header x-ismobile $http_x_ismobile;
        }

        location /all {
			51D_match_all x-ismobile IsMobile;
			51D_match_all x-browsername BrowserName;
			add_header x-ismobile $http_x_ismobile;
			add_header x-browsername $http_x_browsername;
        }
		
		location /metrics {
			51D_match_single x-id Id;
			51D_match_single x-deviceid DeviceId;
			51D_match_single x-rank Rank;
			51D_match_single x-method Method;
			51D_match_single x-difference Difference;
			add_header x-metrics-trie $http_x_id;
			add_header x-metrics-pattern $http_x_deviceid,$http_x_rank,$http_x_method,$http_x_difference;

		}
		
		location /more-properties {
			51D_match_single x-more-properties Ismobile,BrowserName;
			add_header x-more-properties $http_x_more_properties;
		}
		
		location /non-property {
			51D_match_single x-non-property thisisnonarealpropertyname;
			add_header x-non-property $http_x_non_property;
		}
		
		location /redirect {
			51D_match_single x-ismobile IsMobile;
			if ($http_x_ismobile ~ "True") {
				return 301 https://mobilesite;
			}
		}
		
		location /mobile {
			add_header x-test mobile;
		}
		location /desktop {
			add_header x-test desktop;
		}
		
		location /locations {
			51D_match_single x-ismobile-single IsMobile;
			51D_match_all x-ismobile-all IsMobile;
			add_header x-location-ismobile $http_x_ismobile_single$http_x_ismobile_all;
			add_header x-server-ismobile $http_x_server_ismobile_single$http_x_server_ismobile_all;
		}
		
		location /variable {
			51D_match_single x-ismobile-from-var IsMobile $arg_ua;
			add_header x-ismobile-from-var $http_x_ismobile_from_var;
			add_header x-ua $arg_ua;
		}
	}
}

EOF

$t->write_file('single', '');
$t->write_file('all', '');
$t->write_file('locations', '');
$t->write_file('metrics', '');
$t->write_file('more-properties', '');
$t->write_file('non-property', '');
$t->write_file('redirect', '');
$t->write_file('variable', '');

$t->run();

sub get_with_ua {
	my ($uri, $ua) = @_;
	return http(<<EOF);
HEAD $uri HTTP/1.1
Host: localhost
Connection: close
User-Agent: $ua

EOF
}

sub get_with_opera_header {
	my ($uri, $ua) = @_;
	return http(<<EOF);
HEAD $uri HTTP/1.1
Host: localhost
Connection: close
X-OperaMini-Phone-UA: $ua

EOF
}


###############################################################################
# Constants.
###############################################################################

my $mobileUserAgent = 'Mozilla/5.0 (iPhone; CPU iPhone OS 7_1 like Mac OS X) AppleWebKit/537.51.2 (KHTML, like Gecko) Version/7.0 Mobile/11D167 Safari/9537.53';
my $desktopUserAgent = 'Mozilla/5.0 (Windows NT 6.3; WOW64; rv:41.0) Gecko/20100101 Firefox/41.0';

###############################################################################
# Test matching scenarios.
###############################################################################

my $r;

# Single User-Agent.
$r = get_with_ua('/single', $desktopUserAgent);
like($r, qr/x-ismobile: False/, 'Desktop match (single User-Agent)');
$r = get_with_ua('/single', $mobileUserAgent);
like($r, qr/x-ismobile: True/, 'Mobile match (single User-Agent)');

# Multiple HTTP headers.
$r = get_with_ua('/all', $desktopUserAgent);
like($r, qr/x-ismobile: False/, 'Desktop match (all HTTP headers)');
$r = get_with_ua('/all', $mobileUserAgent);
like($r, qr/x-ismobile: True/, 'Mobile match (all HTTP headers)');

# No User-Agent.
$r = http_get('/single');
like($r, qr/x-ismobile: (True|False)/, 'Handle missing User-Agent');

###############################################################################
# Test properties.
###############################################################################

# Metrics
$r = get_with_ua('/metrics', $mobileUserAgent);
like($r, qr/(x-metrics-trie: \d+-\d+-\d+-\d+|x-metrics-pattern: \d+-\d+-\d+-\d+,\d+,(Exact|Closest|Numeric|None),\d+)/, 'Return metrics');

# Multiple properties.
$r = http_get('/more-properties');
like($r, qr/x-more-properties: \w+\^sep\^\w+/, 'Concatinate multiple property values');

# Non-existent property.
$r = http_get('/non-property');
like($r, qr/x-non-property: NA/, 'Handles non-existent property');

###############################################################################
# test the reload functionality.
###############################################################################

# Reload Nginx.
$t->reload();

###############################################################################
# Test config blocks.
###############################################################################

# Match desktop in server and location blocks.
$r = get_with_ua('/locations', $desktopUserAgent);
like($r, qr/x-server-ismobile: FalseFalse/, 'Desktop match in server block');
like($r, qr/x-location-ismobile: FalseFalse/, 'Desktop match in location block');

# Match mobile in server and location blocks.
$r = get_with_ua('/locations', $mobileUserAgent);
like($r, qr/x-server-ismobile: TrueTrue/, 'Mobile match in server block');
like($r, qr/x-location-ismobile: TrueTrue/, 'Mobile match in location block');

###############################################################################
# Test multiple HTTP header functionality.
###############################################################################

# Match mobile User-Agent in Opera header.
$r = get_with_opera_header('/all', $mobileUserAgent);
like($r, qr/x-ismobile: True/, 'Opera mobile header');

# Match desktop User-Agent in Opera header.
$r = get_with_opera_header('/all', $desktopUserAgent);
like($r, qr/x-ismobile: False/, 'Opera desktop header');

###############################################################################
# Test extended matching.
###############################################################################

# Match with a paramter passed from the config.
$r = get_with_ua('/variable?ua='.uri_escape($mobileUserAgent), $desktopUserAgent);
like($r, qr/x-ismobile-from-var: True/, 'Match from parameter');


###############################################################################
# Test use cases.
###############################################################################

# Redirect mobile using the IsMobile property.
$r = get_with_ua('/redirect', $mobileUserAgent);
like($r, qr/301 Moved Permanently/, 'Redirected to mobile');

# Redirect desktop using the IsMobile property.
$r = get_with_ua('/redirect', $desktopUserAgent);
unlike($r, qr/301 Moved Permanently/, 'Didn\'t redirect for desktop');

###############################################################################
