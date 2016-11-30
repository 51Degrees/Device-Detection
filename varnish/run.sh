sudo killall varnishd
sudo /usr/local/sbin/varnishd -a localhost:99 -f varnish.vcl
echo "Started varnish on port 99 with config 'varnish.vcl'."
