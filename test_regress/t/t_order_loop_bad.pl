#!/usr/bin/perl
if (!$::Driver) { use FindBin; exec("$FindBin::Bin/bootstrap.pl", @ARGV, $0); die; }
# DESCRIPTION: Verilator: Verilog Test driver/expect definition
#
# Copyright 2003 by Wilson Snyder. This program is free software; you can
# redistribute it and/or modify it under the terms of either the GNU
# Lesser General Public License Version 3 or the Perl Artistic License
# Version 2.0.

scenarios(simulator => 1);

compile(
    v_flags2 => ["--lint-only"],
    fails => 1,
    verilator_make_gcc => 0,
    make_top_shell => 0,
    make_main => 0,
    expect =>
'%Error: Circular logic when ordering code .*
%Error:      Example path: t/t_order_loop_bad.v:\d+:  ALWAYS
%Error:      Example path: t/t_order_loop_bad.v:\d+:  t.ready
%Error:      Example path: t/t_order_loop_bad.v:\d+:  ACTIVE
.*',
    );

ok(1);
1;
