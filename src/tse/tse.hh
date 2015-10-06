/*
 * tse.hh
 *
 *  Created on: Jul 28, 2015
 *      Author: lpzun
 */

#ifndef TSE_HH_
#define TSE_HH_

#include "z3++.h"

#include "../util/heads.hh"
#include "../util/refs.hh"

using namespace z3;

namespace sura {
/// Aliasing the pair of incoming & outgoing transitions
using inout = pair<deque<id_tran>, deque<id_tran>>;

/// Aliasing vector<expr> as vec_expr
using vec_expr = vector<expr>;

enum class result {
	reach = 0, unreach = 1, unknown = 2
};

class tse {
public:
	tse(const id_tran &size_R, const deque<id_tran>& spawns);
	virtual ~tse();
	bool reachability_analysis_via_tse(const vector<inout>& l_in_out,
			const vector<inout>& s_in_out);
private:
	/// All expressions, func_decl, etc., appearing in the class must be
	/// defined in same context; otherwise, segmentation fault happens
	context ctx;

	expr n_0; /// counter variable for initial local state

	string x_affix; /// prefix for marking equation variables
	uint x_index;   /// index for marking equation variables

	expr sum_z; /// expression of summarizing fired spawns
	uint max_n;  /// maximal number of initial   threads
	uint max_z;  /// maximal number of spawn transitions

	solver s_solver; /// define a global solver

	result solicit_for_TSE(const vector<inout>& l_in_out,
			const vector<inout>& s_in_out);
	vec_expr build_CL(const vector<inout>& l_in_out);
	vec_expr build_CS(const vector<inout>& s_in_out);

	result check_sat_via_smt_solver();
	void parse_sat_solution(const model& m);
	uint get_z3_const_uint(const expr& e);

	bool solicit_for_CEGAR();
	bool check_reach_with_fixed_threads(const uint& n, const uint& z);
	bool standard_FWS(const uint& n, const uint& z);
	map<ushort, ushort> update_counter(const map<ushort, ushort> &Z,
			const ushort &inc);
	map<ushort, ushort> update_counter(const map<ushort, ushort> &Z,
			const ushort &dec, const ushort &inc);
	bool is_spawn_transition(const Thread_State& src, const Thread_State& dst);
};

} /* namespace sura */

#endif /* TSE_HH_ */
