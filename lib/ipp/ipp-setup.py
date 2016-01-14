# 1350,  7 Dec 2015 (NZDT)
#
# ipp-setup.py:  Build and install the ipp extension

from distutils.core import setup, Extension

setup(
    name='ipp',
    version='2.0',
    description='IPprefix, a library for IP addresses and widths',
    author='Nevil Brownlee',
    author_email='n.brownlee@auckland.ac.nz',
    url='https://www.cs.auckland.ac.nz/~nevil/python-libtrace/',

    py_modules = ['ipp']  # Just the ipp.py module
    )
