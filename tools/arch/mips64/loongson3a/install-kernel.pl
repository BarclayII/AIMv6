#!/usr/bin/perl


# Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
#
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.
#

# If you are REALLY interested in Perl grammar, refer to
#     man perl
# for details.

use strict;
use Fcntl qw(SEEK_SET O_RDWR);

sub try_open {
	my $fd = undef;
	my $file = shift;
	sysopen($fd, $file, O_RDWR) or
	    die "Could not open $file: $!\n";
	return $fd;
}

my $usage = "Usage: $0 <device> <kernel-file> <#partition-from-1>\n";

my $fd_dev = undef;
my $fd_kernel = undef;
my $dev = shift or die $usage;
my $kernel = shift or die $usage;
my $partno = shift or die $usage;

#print $dev, ' ', $kernel, ' ', $partno;

$fd_dev = try_open($dev);
$fd_kernel = try_open($kernel);

my $sector_start;
sysseek($fd_dev, 446 + 16 * ($partno - 1) + 8, SEEK_SET);
sysread($fd_dev, $sector_start, 4);
$sector_start = unpack("l", $sector_start);

print "Writing to sector $sector_start...\n";

sysseek($fd_dev, $sector_start * 512, SEEK_SET);

my $kernel_buf;
my $total_bytes = 0;
while (my $bytes = sysread($fd_kernel, $kernel_buf, 512)) {
	syswrite($fd_dev, $kernel_buf, $bytes);
	$total_bytes += $bytes;
}

print "Wrote $total_bytes bytes\n";

close($fd_dev);
close($fd_kernel);
