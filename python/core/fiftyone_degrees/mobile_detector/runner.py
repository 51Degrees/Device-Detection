# -*- coding: utf-8 -*-

'''
:copyright: (c) 2015 by 51Degrees.com, see README.md for more details.
:license: MPL2, see LICENSE.txt for more details.
'''

from __future__ import absolute_import
import os
import sys
import subprocess


def settings(args, help):
    import inspect
    from fiftyone_degrees.mobile_detector.conf import default
    sys.stdout.write(inspect.getsource(default))


def match(args, help):
    if len(args) == 1:
        from fiftyone_degrees import mobile_detector
        device = mobile_detector.match(args[0])
        for name, value in device.properties.iteritems():
            sys.stdout.write('%s: %s\n' % (name, unicode(value),))
    else:
        sys.stderr.write(help)
        sys.exit(1)


def update_premium_pattern_wrapper(args, help):
    import tempfile
    import urllib2
    import gzip
    import shutil
    from fiftyone_degrees.mobile_detector.conf import settings
    sys.stdout.write('Starting Update \n')

    if settings.LICENSE:
        # Build source URL.
        url = 'https://distributor.51degrees.com/api/v2/download?LicenseKeys=%s&Type=BinaryV32&Download=True' % (
            settings.LICENSE
        )

        with tempfile.NamedTemporaryFile(
                suffix='.dat.gz',
                prefix='51d_temp',
                delete=False) as fh:
            delete = True
            try:
                # Fetch URL (no verification of the server's certificate here).
                uh = urllib2.urlopen(url, timeout=120)

                # Check server response.
                if uh.headers['Content-Disposition'] is not None:
                    # Download the package.
                    file_size = int(uh.headers['Content-Length'])
                    sys.stdout.write('=> Downloading %s bytes... ' % file_size)
                    downloaded = 0
                    while True:
                        buffer = uh.read(8192)
                        if buffer:
                            downloaded += len(buffer)
                            fh.write(buffer)
                            status = r'%3.2f%%' % (downloaded * 100.0 / file_size)
                            status = status + chr(8) * (len(status) + 1)
                            print status,
                        else:
                            break
                    #Done with temporary file. Close it.
                    if not fh.closed:
                        fh.close()

                    #Open zipped file.
                    f_name = fh.name
                    zipped_file = gzip.open(f_name, "rb")
                    #Open temporary file to store unzipped content.
                    unzipped_file = open("unzipped_temp.dat", "wb")
                    #Unarchive content to temporary file.
                    unzipped_file.write(zipped_file.read())
                    #Close and remove compressed file.
                    zipped_file.close()
                    os.remove(f_name)
                    #Copy unzipped file to the file used for detection.
                    path = settings.V3_WRAPPER_DATABASE
                    shutil.copy2("unzipped_temp.dat", path)

                    #clean-up
                    if not zipped_file.closed:
                        zipped_file.close()
                    if not unzipped_file.closed:
                        unzipped_file.close()

                    sys.stdout.write("\n Update was successfull \n")

                    #End of try to update package.
                else:
                    sys.stderr.write('Failed to download the package: is your license key expired?\n')
            except Exception as e:
                sys.stderr.write('Failed to download the package: %s.\n' % unicode(e))
            finally:
                try:
                    os.remove(fh)
                except:
                    pass
    else:
        sys.stderr.write('Failed to download the package: you need a license key. Please, check you settings.\n')


def main():
    # Build help message.
    help = '''Usage:

  %(cmd)s settings:
      Dumps sample settings file.

  %(cmd)s match <user agent>
      Fetches device properties based on the input user agent string.

  %(cmd)s update-premium-pattern-wrapper
      Downloads and installs latest premium pattern wrapper package available
      at 51Degrees.com website (a valid license key is required).

''' % {
        'cmd': os.path.basename(sys.argv[0])
    }

    # Check base arguments.
    if len(sys.argv) > 1:
        command = sys.argv[1].replace('-', '_')
        if command in ('settings', 'match', 'update_premium_pattern_wrapper'):
            getattr(sys.modules[__name__], command)(sys.argv[2:], help)
        else:
            sys.stderr.write(help)
            sys.exit(1)
    else:
        sys.stderr.write(help)
        sys.exit(1)


if __name__ == '__main__':
    main()
