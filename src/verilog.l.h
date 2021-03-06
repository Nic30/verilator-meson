#pragma once

#include "V3Number.h"
#include "V3ParseImp.h"  // Defines YYTYPE; before including bison header
#include "V3ParseBison.h"  // Generated by bison

#include <cstdlib>

extern void yyerror(const char*);
extern void yyerrorf(const char* format, ...);

#define STATE_VERILOG_RECENT  S17  // State name for most recent Verilog Version

#define PARSEP V3ParseImp::parsep()
#define SYMP PARSEP->symp()

#define YY_INPUT(buf,result,max_size) \
    result = PARSEP->flexPpInputToLex(buf,max_size);

//======================================================================

#define NEXTLINE() {PARSEP->linenoInc();}
#define LINECHECKS(textp,len)  { const char* cp=textp; for (int n=len; n; --n) if (cp[n]=='\n') NEXTLINE(); }
#define LINECHECK()  LINECHECKS(yytext,yyleng)
#define CRELINE() (PARSEP->copyOrSameFileLine())

#define FL { yylval.fl = CRELINE(); }

#define RETURN_BBOX_SYS_OR_MSG(msg,yytext) {	\
	if (!v3Global.opt.bboxSys()) yyerrorf(msg,yytext); \
	return yaD_IGNORE; }



// See V3Read.cpp
//void V3ParseImp::statePop() { yy_pop_state(); }
