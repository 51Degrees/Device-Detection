sudo killall varnishd
sudo /usr/local/sbin/varnishd -a localhost:99 -f example.vcl
echo "Started varnish on port 99 with config 'example.vcl'."
