if [ -d "varnish-cache" ]; then
	echo "varnish-cache 4.0 already downloaded."
else
	git clone -b 4.0 https://github.com/varnishcache/varnish-cache.git
fi
./autogen.sh
./configure VARNISHSRC=./varnish-cache
make
sudo make install
