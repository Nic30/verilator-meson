

build_system = build_machine.system()
# On Windows, if flex/bison aren't found, we use a subproject to get them
flex = find_program('flex', required : build_system != 'windows')
bison = find_program('bison', required : build_system != 'windows')
yacc = find_program('yacc', required : build_system != 'windows')
if not flex.found() or not bison.found() or not yacc.found()
  subproject('win-flex-bison-binaries')
  yacc = bison + ' -y ' 
endif

astgen = find_program('astgen', required : true)
config_rev = find_program('config_rev.pl', required : true)
bisonpre = find_program('bisonpre', required : true)
vlcovgen = find_program('vlcovgen', required : true)

LFLAGS = ['-d']

base_srcs = [
	'Verilator.cpp',
	'V3Active.cpp',
	'V3ActiveTop.cpp',
	'V3Assert.cpp',
	'V3AssertPre.cpp',
	'V3Ast.cpp',
	'V3AstNodes.cpp',
	'V3Begin.cpp',
	'V3Branch.cpp',
	'V3Broken.cpp',
	'V3CCtors.cpp',
	'V3Case.cpp',
	'V3Cast.cpp',
	'V3Cdc.cpp',
	'V3Changed.cpp',
	'V3Clean.cpp',
	'V3ClkGater.cpp',
	'V3Clock.cpp',
	'V3Combine.cpp',
	'V3Config.cpp',
	'V3Coverage.cpp',
	'V3CoverageJoin.cpp',
	'V3Dead.cpp',
	'V3Delayed.cpp',
	'V3Depth.cpp',
	'V3DepthBlock.cpp',
	'V3Descope.cpp',
	'V3EmitC.cpp',
	'V3EmitCInlines.cpp',
	'V3EmitCSyms.cpp',
	'V3EmitMk.cpp',
	'V3EmitV.cpp',
	'V3EmitXml.cpp',
	'V3Error.cpp',
	'V3EventMarker.cpp',
	'V3Expand.cpp',
	'V3File.cpp',
	'V3FileLine.cpp',
	'V3Gate.cpp',
	'V3GenClk.cpp',
	'V3Graph.cpp',
	'V3GraphAlg.cpp',
	'V3GraphAcyc.cpp',
	'V3GraphDfa.cpp',
	'V3GraphPathChecker.cpp',
	'V3GraphTest.cpp',
	'V3Hashed.cpp',
	'V3Inline.cpp',
	'V3Inst.cpp',
	'V3InstrCount.cpp',
	'V3Life.cpp',
	'V3LifePost.cpp',
	'V3LinkCells.cpp',
	'V3LinkDot.cpp',
	'V3LinkJump.cpp',
	'V3LinkLValue.cpp',
	'V3LinkLevel.cpp',
	'V3LinkParse.cpp',
	'V3LinkResolve.cpp',
	'V3Localize.cpp',
	'V3Name.cpp',
	'V3Number.cpp',
	'V3Options.cpp',
	'V3Order.cpp',
	'V3Os.cpp',
	'V3Param.cpp',
	'V3Partition.cpp',
	'V3PreShell.cpp',
	'V3Premit.cpp',
	'V3Reloop.cpp',
	'V3Scope.cpp',
	'V3Scoreboard.cpp',
	'V3Slice.cpp',
	'V3Split.cpp',
	'V3SplitAs.cpp',
	'V3Stats.cpp',
	'V3StatsReport.cpp',
	'V3String.cpp',
	'V3Subst.cpp',
	'V3Table.cpp',
	'V3Task.cpp',
	'V3Trace.cpp',
	'V3TraceDecl.cpp',
	'V3Tristate.cpp',
	'V3TSP.cpp',
	'V3Undriven.cpp',
	'V3Unknown.cpp',
	'V3Unroll.cpp',
	'V3Width.cpp',
	'V3WidthSel.cpp',
]

# Non-concatable
nc_srcs = [
	'V3ParseImp.cpp',
	'V3ParseGrammar.cpp',
	'V3ParseLex.cpp',
	'V3PreProc.cpp',
]

# verilator_coverage
vlcov_srcs = [
	'VlcMain.cpp'
]

workdir = meson.current_source_dir() 
gen_src_cpp = custom_target('generated_cpp',
    output : 'V3Const__gen.cpp',
    command : [astgen, '-I' + workdir, '-o=@OUTDIR@', 'V3Const.cpp']
)
gen_rev_h = custom_target('config_rev.h',
	output :  'config_rev.h',
	command : [config_rev, meson.source_root(), '@OUTDIR@'],
	capture : true,
)
gen_src_h = custom_target('generated_h',
    output : 'V3Ast__gen_classes.h',
    command : [astgen, '-I' + workdir, '-o=@OUTDIR@', '--classes']
)

# If you get errors from verilog.y below, try upgrading bison to version 1.875 or newer.
V3ParseBison = custom_target('V3ParseBison.c',
	input : 'verilog.y',
	output : 'V3ParseBison.c',
	# todo platform independent yacc
	command : [bisonpre, '--yacc', yacc, '-d', '-v', '-o', '@OUTPUT@', '@INPUT@'])	

V3Lexer = custom_target('V3Lexer.yy.cpp',
	depends : [V3ParseBison],
	input : ['verilog.l'],
	output : 'V3Lexer.yy.cpp',
	command : [flex, '-P', 'V3LexerBase'] + LFLAGS + ['-o@OUTPUT@', '@INPUT@']
)

V3PreLex = custom_target('V3PreLex.yy.cpp',
	input : 'V3PreLex.l',
	output : 'V3PreLex.yy.cpp',
	command : [flex, '-P', 'V3PreLex'] + LFLAGS + ['-o@OUTPUT@', '@INPUT@']
)


incdir = include_directories('../include')
executable('verilator_bin',
	sources : [gen_src_cpp, gen_src_h, gen_rev_h,
			   V3Lexer, V3PreLex] + base_srcs + nc_srcs,
	include_directories : incdir,
	install : true)
