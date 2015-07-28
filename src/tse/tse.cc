/*
 * tse.cc
 *
 *  Created on: Jul 28, 2015
 *      Author: lpzun
 */

#include "tse.hh"

namespace sura {

tse::tse(const id_tran &size_R) :
		ctx(), n_0(ctx.int_const("n0")), x_affix("x"), x_index(size_R), sum_z(
				ctx.int_val(0)), max_n(0), max_z(0), s_solver(
				(tactic(ctx, "simplify") & tactic(ctx, "qe")
						& tactic(ctx, "smt")).mk_solver()) {

}

tse::~tse() {
}

bool tse::reachability_analysis_via_tse(const vector<inout>& l_in_out,
		const vector<inout>& s_in_out) {

}

/**
 * @brief solicit reachability for Thread State Equation
 * @param l_in_out
 * @param s_in_out
 * @return
 */
result tse::solicit_for_TSE(const vector<inout>& l_in_out,
		const vector<inout>& s_in_out) {

	/// add x_i >= 0
	for (uint idx = 0; idx < x_index; ++idx)
		s_solver.add(
				ctx.int_const((x_affix + std::to_string(idx)).c_str()) >= 0);

	/// add C_L constraints
	const auto& c_L = this->build_CL(l_in_out);
	for (auto iphi = c_L.begin(); iphi != c_L.end(); ++iphi)
		s_solver.add(*iphi);

	/// add C_S constraints
	const auto& c_S = this->build_CS(s_in_out);
	for (auto iphi = c_S.begin(); iphi != c_S.end(); ++iphi)
		s_solver.add(*iphi);

#ifndef NDEBUG
	for (auto iphi = c_L.begin(); iphi != c_L.end(); ++iphi)
		cout << *iphi << "\n";
	for (auto iphi = c_S.begin(); iphi != c_S.end(); ++iphi)
		cout << *iphi << "\n";
#endif
	return this->check_sat_via_smt_solver();
}

/**
 * @brief build local constraints C_L
 * @param l_in_out
 * @return
 */
vec_expr tse::build_CL(const vector<inout>& l_in_out) {
	vec_expr phi(Thread_State::L, ctx.int_val(0));
	phi[Refs::INITL_TS.get_local()] = n_0;

	for (size_t i = 0; i < l_in_out.size(); ++i) {
		for (auto inc = l_in_out[i].first.begin();
				inc != l_in_out[i].first.end(); ++inc)
			phi[i] = phi[i]
					+ ctx.int_const((x_affix + std::to_string(*inc)).c_str());

		for (auto out = l_in_out[i].second.begin();
				out != l_in_out[i].second.end(); ++out)
			phi[i] = phi[i]
					- ctx.int_const((x_affix + std::to_string(*out)).c_str());
	}

	return phi;
}

/**
 * @brief build shared constraints C_S
 * @param s_in_out
 * @return
 */
vec_expr tse::build_CS(const vector<inout>& s_in_out) {
	vec_expr phi(Thread_State::S, ctx.int_val(0));

	if (Refs::INITL_TS.get_share() != Refs::FINAL_TS.get_share()) {
		phi[Refs::INITL_TS.get_share()] = ctx.int_val(1);
		phi[Refs::FINAL_TS.get_share()] = ctx.int_val(-1);
	}

	for (size_t i = 0; i < s_in_out.size(); ++i) {
		for (auto inc = s_in_out[i].first.begin();
				inc != s_in_out[i].first.end(); ++inc)
			phi[i] = phi[i]
					+ ctx.int_const((x_affix + std::to_string(*inc)).c_str());

		for (auto out = s_in_out[i].second.begin();
				out != s_in_out[i].second.end(); ++out)
			phi[i] = phi[i]
					- ctx.int_const((x_affix + std::to_string(*out)).c_str());
	}
	return phi;
}

result tse::check_sat_via_smt_solver() {
	switch (s_solver.check()) {
	case sat:
		this->parse_sat_solution(s_solver.get_model());
		if (check_reach_with_fixed_threads(max_n, max_z))
			return result::reach;
		return result::unknown;
	case unsat:
		return result::unreach;
	case unknown:
		throw ural_rt_err("smt solver returns unknow!");
	}
}

void tse::parse_sat_solution(const model& m) {
	for (size_t i = 0; i < m.size(); i++) {
		func_decl v = m[i];
		assert(v.arity() == 0); /// check if contains only constants
		if (v.name() == n_0.decl().name())
			if (Z3_get_numeral_uint(ctx, m.get_const_interp(v), &max_n))
				break;
	}
	max_z = get_z3_const_uint(m.eval(sum_z));
}

/**
 * @brief retrieve the value from a z3 constant expression
 * @param e: a constant expression
 * @return e's value
 */
uint tse::get_z3_const_uint(const expr& e) {
	if (e.is_const()) {
		uint value = 0;
		Z3_get_numeral_uint(ctx, e, &value);
		return value;
	} else {
		throw ural_rt_err("get_z3_const_uint: input is not a constant");
	}
}

bool tse::solicit_for_CEGAR() {
	while (true) { /// CEGAR loop might never terminate
		/// add incremental constraint
		s_solver.add(n_0 > ctx.int_val(max_n));
		if (max_z > 0 && !sum_z.is_int())
			s_solver.add(sum_z > ctx.int_val(sum_z));

		/// apply  incremental solving
		switch (this->check_sat_via_smt_solver()) {
		case result::reach:
			return true;
		case result::unreach:
			return false;
		case result::unknown:
			break;
		}
	}
	return false;
}

bool tse::check_reach_with_fixed_threads(const uint& n, const uint& z) {
	auto spw = z;
//	cout << "===============================" << spw << endl;
	queue<Global_State, list<Global_State>> W; /// worklist
	W.push(Global_State(Refs::INITL_TS, n)); /// start from the initial state with n threads
	set<Global_State> R; /// reachable global states
	while (!W.empty()) {
		Global_State tau = W.front();
		W.pop();
		const ushort &shared = tau.get_share();
		for (auto il = tau.locals.begin(); il != tau.locals.end(); ++il) {
			Thread_State src(shared, il->first);
			if (src != Refs::FINAL_TS) {
				auto ifind = Refs::original_TTD.find(src);
				if (ifind != Refs::original_TTD.end()) {
					for (auto idst = ifind->second.begin();
							idst != ifind->second.end(); ++idst) {
						auto locals = tau.locals;
						if (this->is_spawn_transition(src, *idst)) { /// if src +> dst true
							if (spw > 0) {
								spw--;
//								cout << "=============" << idst->local << endl;
								locals = this->update_counter(locals,
										idst->local);
							} else { /// if the we already spawn z times, we can't
								continue; /// spawn any more and have to skip src +> dst;
							}
						} else {
							locals = this->update_counter(locals, src.local,
									idst->local);
						}
						Global_State _tau(idst->get_share(), locals);
						if (R.insert(_tau).second) {
							_tau.pi = std::make_shared<Global_State>(tau); /// record _tau's predecessor tau
							W.push(_tau);
						}
					}
				}
			} else { /// if src == final
				cout << "witness path: " << tau << endl; // TODO: print the witness path
				// this->reproduce_witness_path(tau.pi);
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief This procedure updates the counters of local states.
 * @param Z  : the part of local states
 * @param inc: local state whose counter is incremented
 * @return local states after updating counters
 */
map<ushort, ushort> tse::update_counter(const map<ushort, ushort> &Z,
		const ushort &inc) {
	auto _Z = Z;   /// local copy of Z

	auto iinc = _Z.find(inc);
	if (iinc != _Z.end()) {
		iinc->second++;
	} else {
		_Z[inc] = 1;
	}

	return _Z;
}

/**
 * @brief This procedure updates the counters of local states.
 * @param Z  : the part of local states
 * @param dec: local state whose counter is decremented
 * @param inc: local state whose counter is incremented
 * @return local states after updating counters
 */
map<ushort, ushort> tse::update_counter(const map<ushort, ushort> &Z,
		const ushort &dec, const ushort &inc) {
	if (dec == inc) /// if dec == inc
		return Z;

	auto _Z = Z;   /// local copy of Z

	auto idec = _Z.find(dec);
	if (idec != _Z.end()) {
		idec->second--;
		if (idec->second == 0)
			_Z.erase(idec);
	} else {
		throw ural_rt_err("FWS::update_counter: local state misses");
	}

	auto iinc = _Z.find(inc);
	if (iinc != _Z.end()) {
		iinc->second++;
	} else {
		_Z[inc] = 1;
	}

	return _Z;
}

/**
 * @brief determine if (src, dst) correspßonds to a spawn transition
 * @param src
 * @param dst
 * @param spawn_trans
 * @return bool
 * 			true : src +> dst
 * 			false: otherwise
 */
bool tse::is_spawn_transition(const Thread_State& src,
		const Thread_State& dst) {
	auto ifind = Refs::spawntra_TTD.find(src);
	if (ifind == Refs::spawntra_TTD.end()) {
		return false;
	} else {
		auto ifnd = std::find(ifind->second.begin(), ifind->second.end(), dst);
		if (ifnd == ifind->second.end())
			return false;
		else
			return true;
	}
}

} /* namespace sura */
