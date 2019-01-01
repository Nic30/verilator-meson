// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
// DESCRIPTION: Verilator: Marks starts of evaluation of edge dependent code
//
// Code available from: http://www.veripool.org/verilator
//
//*************************************************************************
//
// Copyright 2003-2018 by Wilson Snyder.  This program is free software; you can
// redistribute it and/or modify it under the terms of either the GNU
// Lesser General Public License Version 3 or the Perl Artistic License
// Version 2.0.
//
// Verilator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//*************************************************************************

#include "V3EventMarker.h"
#include "V3EmitCBase.h"
#include <unordered_set>

class EventMarkerVisitor: public AstNVisitor {
private:
	AstTopScope* m_topScopep;	// Current top scope
	AstMTaskBody* m_mtaskBodyp; // Current mtask body
	AstSenTree* m_lastSenp;	// Last sensitivity match, so we can detect duplicates.
	AstCFunc* before_edge_fn; // Function which should be called before processing of sequential events
	AstSenItem* m_lastSeenClock;
	std::unordered_set<AstSenItem*> seen;  // seen clocks

	// METHODS
	VL_DEBUG_FUNC  // Declare debug()
	virtual void visit(AstActive* nodep) override;
	virtual void visit(AstNode* nodep) override;
	virtual void visit(AstExecGraph* nodep) override;
	virtual void visit(AstTopScope* nodep) override;
	virtual void visit(AstSenItem* nodep) override;

	void clearLastSen() {
		m_lastSenp = NULL;
		m_lastSeenClock = nullptr;
	}

public:
	// CONSTUCTORS
	explicit EventMarkerVisitor(AstNetlist* nodep) :
			m_topScopep(nullptr), m_mtaskBodyp(nullptr), m_lastSenp(nullptr), before_edge_fn(nullptr), m_lastSeenClock(nullptr) {
		iterate(nodep);
	}
	virtual ~EventMarkerVisitor() {
	}

};

void EventMarkerVisitor::visit(AstNode* nodep) {
	iterateChildren(nodep);
}

void EventMarkerVisitor::visit(AstExecGraph* nodep) {
	for (m_mtaskBodyp = VN_CAST(nodep->op1p(), MTaskBody); m_mtaskBodyp;
			m_mtaskBodyp = VN_CAST(m_mtaskBodyp->nextp(), MTaskBody)) {
		clearLastSen();
		iterate(m_mtaskBodyp);
	}
	clearLastSen();
}

void EventMarkerVisitor::visit(AstTopScope* nodep) {
	UINFO(4, " TOPSCOPE   "<<nodep<<endl);
	m_topScopep = nodep;
	seen.clear();

	// generate onEvent function and add it to top scope
	before_edge_fn = new AstCFunc(nodep->fileline(),
							"onBeforeEdge", m_topScopep->scopep(), "void");
	before_edge_fn->argTypes(EmitCBaseVisitor::symClassVar() + ", CData& clkSig");
	before_edge_fn->dontCombine(true);
	before_edge_fn->slow(true);
	before_edge_fn->isStatic(false);
	before_edge_fn->isVirtual(true);
	before_edge_fn->symProlog(true);
	m_topScopep->scopep()->addActivep(before_edge_fn);

	// Process the activates
	iterateChildren(nodep);
	// Done, clear so we can detect errors
	UINFO(4, " TOPSCOPEDONE "<<nodep<<endl);
	clearLastSen();
	m_topScopep = NULL;
}

void EventMarkerVisitor::visit(AstSenItem* nodep) {
	auto e = nodep->edgeType();
	if (e==AstEdgeType::ET_POSEDGE || e == AstEdgeType::ET_NEGEDGE) {
		m_lastSeenClock = nodep;
	}
}

void EventMarkerVisitor::visit(AstActive* nodep) {
	if (!m_topScopep || !nodep->stmtsp()) {
		// Not at the top or empty block...
	} else {
		if (m_mtaskBodyp) {
			UINFO(4, "  TR ACTIVE  "<<nodep<<endl);
		} else {
			UINFO(4, "  ACTIVE  " << nodep << endl);
		}
		AstNode* stmtsp = nodep->stmtsp();
		if (nodep->hasClocked()) {
			// Remember the latest sensitivity so we can compare it next time
			if (nodep->hasInitial())
				nodep->v3fatalSrc("Initial block should not have clock sensitivity")
			else if (m_lastSenp && nodep->sensesp()->sameTree(m_lastSenp)) {
				UINFO(4, "    sameSenseTree\n");
			} else {
				clearLastSen();
				m_lastSenp = nodep->sensesp();
				visit(m_lastSenp);
				if (seen.find(m_lastSeenClock) == seen.end()) {
					// if this is a first place where we evaluating event of this signal
					seen.insert(m_lastSeenClock);
					auto clk_ptr = new AstVarRef(nodep->fileline(), m_lastSeenClock->varrefp()->varScopep(), false);
					auto before_edge_call = new AstCCall(
							nodep->fileline(),
							before_edge_fn,
							clk_ptr
					);
					before_edge_call->argTypes("vlSymsp"); // vlSymsp will be before other args

					//before_edge_call->argTypes("nullptr");
					//auto before_edge_call = new AstCStmt(nodep->fileline(), "BEFORE_SEQUENTIAL_UPDATE()\n");
					stmtsp->addPrev(before_edge_call);
				}
				m_lastSeenClock = nullptr;

			}
		} else if (nodep->hasInitial() || nodep->hasSettle()) {
			// Don't need to: clearLastSen();, as we're adding it to different cfunc
			if (m_mtaskBodyp)
				nodep->v3fatalSrc("MTask should not include initial/settle logic.")
		} else {
			// Combo
			clearLastSen();
		}
		VL_DANGLING(nodep);
	}
}

void V3EventMarker::eventMarkerAll(AstNetlist* nodep) {
	EventMarkerVisitor eventMarkerVisitor(nodep);
}
