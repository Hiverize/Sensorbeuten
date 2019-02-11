#!/bin/bash

rm -r stage2/99-hiverize/files/*
mkdir -p stage2/99-hiverize/files/
rsync -r ../rpi-system/ stage2/99-hiverize/files
tar -czf stage2/99-hiverize/files/hiverize.tar.gz ../hiverize/
