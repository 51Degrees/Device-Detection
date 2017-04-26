sudo killall varnishd

if [ ! -e "/etc/varnish/51Degrees.dat" ]; then
	sudo mkdir /etc/varnish -p
	sudo cp data/51Degrees-LiteV3.2.dat /etc/varnish/51Degrees.dat -v
fi
sudo /usr/local/sbin/varnishd -a localhost:99 -f example.vcl
echo "Started varnish on port 99 with config 'example.vcl'."
