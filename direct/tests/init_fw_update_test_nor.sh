#!/bin/bash
# Run this to initialise the file system for the test runs.
rm seed-nor-*
rm emfile-nor*
./yaffs_test  -u -i M18-1
