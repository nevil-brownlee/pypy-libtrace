# plt-setup.py:  Build and install the plt module

from distutils.core import setup, Extension

setup(
    name='plt',
    version='2.0',
    description="Python libtrace, an OO interface to WAND's libtrace",
    author='Nevil Brownlee',
    author_email='n.brownlee@auckland.ac.nz',
    url='https://www.cs.auckland.ac.nz/~nevil/python-libtrace/',

    py_modules = ['plt']
    )
