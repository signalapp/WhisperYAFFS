#!/bin/bash
# Run this to initialise the file system for the test runs.
   rm seed-nand-*
   rm emfile-2k-0*
   ./yaffs_test  -u -i yaffs2

