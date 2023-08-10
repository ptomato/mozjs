# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.

from setuptools import setup

PACKAGE_VERSION = "0.1"

setup(
    name="mozserve",
    version=PACKAGE_VERSION,
    description="Python test server launcher intended for use with Mozilla testing",
    long_description="see https://firefox-source-docs.mozilla.org/mozbase/index.html",
    license="MPL",
    packages=["mozserve"],
    zip_safe=False,
)
