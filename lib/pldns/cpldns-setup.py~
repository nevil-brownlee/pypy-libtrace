# 1350,  7 Dec 2015 (NZDT)
#
#!/usr/bin/env python

import os
import sys

from setuptools import setup, find_packages

os.chdir(os.path.dirname(sys.argv[0]) or ".")

setup(
    name="cipp",
    version="2.0",
    description="IPprefix module using pypy and cffi",
    #long_description=open("README.rst", "rt").read(),
    url="https://github.com/wolever/python-cffi-example",
    author="Nevil Brownlee",
    author_email="n.brownlee@auckland.ac.nz",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: Implementation :: PyPy",
        "License :: OSI Approved :: BSD License",
    ],
    packages=find_packages(),
    install_requires=["cffi>=1.3.0"],
    setup_requires=["cffi>=1.3.0"],
    cffi_modules=[
        "./build-cipp.py:ffi",
    ],
)
