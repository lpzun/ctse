/**
 * tse.cc
 *
 *  Created on: Jul 28, 2015
 *      Author: lpzun
 */

#include "tse.hh"

namespace sura {

tse::tse(const id_tran &size_R, const deque<id_tran>& spawns) :
        ctx(), n_0(ctx.int_const("n0")), x_affix("x"), x_index(size_R), sum_z(
                ctx.int_val(0)), max_n(0), max_z(0), s_solver(
                (tactic(ctx, "simplify") & tactic(ctx, "solve-eqs")
                        & tactic(ctx, "smt")).mk_solver()) {
    /// set up the expression of summarizing all spawn variables
    for (auto iv = spawns.begin(); iv != spawns.end(); ++iv) {
        const id_tran& id = *iv;
        sum_z = sum_z + ctx.int_const((x_affix + std::to_string(id)).c_str());
    }
}

tse::~tse() {
}

/**
 * @brief determine the reachability of FINAL_TS via TSE
 * @param l_in_out
 * @param s_in_out
 * @return bool
 * 		true : if reachable
 * 		false: otherwise
 */
bool tse::reachability_analysis_via_tse(const vector<inout>& l_in_out,
        const vector<inout>& s_in_out) {
    switch (this->solicit_for_TSE(l_in_out, s_in_out)) {
    case result::reach:
        return true;
    case result::unreach:
        return false;
    default: /// unknown
        return this->solicit_for_CEGAR();
    }
}

/**
 * @brief solicit reachability for Thread State Equation
 * @param l_in_out
 * @param s_in_out
 * @return
 */
result tse::solicit_for_TSE(const vector<inout>& l_in_out,
        const vector<inout>& s_in_out) {
    /// add n_0 >= 1
    s_solver.add(n_0 >= 1);

    /// add x_i >= 0
    for (uint idx = 0; idx < x_index; ++idx)
        s_solver.add(
                ctx.int_const((x_affix + std::to_string(idx)).c_str()) >= 0);

    /// add C_L constraints
    const auto& c_L = this->build_CL(l_in_out);
    for (size_t i = 0; i != c_L.size(); ++i)
        s_solver.add(c_L[i]);

    /// add C_S constraints
    const auto& c_S = this->build_CS(s_in_out);
    for (size_t i = 0; i != c_S.size(); ++i)
        s_solver.add(c_S[i]);

#ifndef NDEBUG
    for (auto iphi = c_L.begin(); iphi != c_L.end(); ++iphi)
    cout << *iphi << "\n";
    for (auto iphi = c_S.begin(); iphi != c_S.end(); ++iphi)
    cout << *iphi << "\n";
#endif

    if (refs::OPT_CONSTRAINT) { /// output constraints
        cout << "(declare-fun " << n_0 << " () Int)" << "\n";
        for (uint idx = 0; idx < x_index; ++idx)
            cout << "(declare-fun " << x_affix << idx << " () Int)" << "\n";
        cout << "(assert \n";
        cout << s_solver << "\n";
        cout << ")\n";
        cout << "(check-sat)" << endl;
    }

    return this->check_sat_via_smt_solver();
}

/**
 * @brief build local constraints C_L
 * @param l_in_out
 * @return
 */
vec_expr tse::build_CL(const vector<inout>& l_in_out) {
    vec_expr phi(thread_state::L, ctx.int_val(0));
    phi[refs::INITL_TS.get_local()] = n_0;

    for (size_t i = 0; i < l_in_out.size(); ++i) {
        expr lhs(ctx.int_val(0)); // left-hand		side
        expr rhs(ctx.int_val(0)); // right-hand 	side
        if (i == refs::FINAL_TS.get_local())
            rhs = ctx.int_val(refs::TARGET_THR_NUM);

        for (auto inc = l_in_out[i].first.begin();
                inc != l_in_out[i].first.end(); ++inc)
            lhs = lhs + ctx.int_const((x_affix + std::to_string(*inc)).c_str());

        for (auto out = l_in_out[i].second.begin();
                out != l_in_out[i].second.end(); ++out)
            rhs = rhs + ctx.int_const((x_affix + std::to_string(*out)).c_str());

        phi[i] = (phi[i] + lhs) >= rhs;
    }
    DBG_LOC();
    return phi;
}

/**
 * @brief build shared constraints C_S
 * @param s_in_out
 * @return
 */
vec_expr tse::build_CS(const vector<inout>& s_in_out) {
    vec_expr phi(thread_state::S, ctx.int_val(0));

    if (refs::INITL_TS.get_share() != refs::FINAL_TS.get_share()) {
        phi[refs::INITL_TS.get_share()] = ctx.int_val(1);
        phi[refs::FINAL_TS.get_share()] = ctx.int_val(-1);
    }

    for (size_t i = 0; i < s_in_out.size(); ++i) {
        expr lhs(ctx.int_val(0)); // left-hand 	side
        expr rhs(ctx.int_val(0)); // right-hand 	side

        for (auto inc = s_in_out[i].first.begin();
                inc != s_in_out[i].first.end(); ++inc)
            lhs = lhs + ctx.int_const((x_affix + std::to_string(*inc)).c_str());

        for (auto out = s_in_out[i].second.begin();
                out != s_in_out[i].second.end(); ++out)
            rhs = rhs + ctx.int_const((x_affix + std::to_string(*out)).c_str());

        phi[i] = (phi[i] + lhs) == rhs;
    }
    DBG_LOC();
    return phi;
}

/**
 * @brief parse the sat solution and update max_n & max_z
 * @return void
 */
result tse::check_sat_via_smt_solver() {
    DBG_LOC();
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

/**
 * @brief parse the sat solution and see if max_n or max_z is updated
 * @param m: the solution returned from z3 SMT solver
 */
void tse::parse_sat_solution(const model& m) {
#ifndef NDEBUG
    cout << __func__ << "\n";
    cout << m << endl;
#endif
    for (size_t i = 0; i < m.size(); i++) {
        func_decl v = m[i];
        assert(v.arity() == 0); /// check if contains only constants
        if (v.name() == n_0.decl().name())
            if (Z3_get_numeral_uint(ctx, m.get_const_interp(v), &max_n))
                break;
    }

    if (refs::is_exists_SPAWN) {
        const auto z = get_z3_const_uint(m.eval(sum_z));
        if (max_z < z)
            max_z = z;
        else
            max_z += 2;
    }
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

/**
 * @brief solicit for CEGAR to refine pathwise encoding
 * @return bool
 * 		true : if we find a witness path,
 * 		false: otherwise
 */
bool tse::solicit_for_CEGAR() {
    while (true) { /// CEGAR loop might never terminate
        /// add incremental constraint
        s_solver.add(n_0 > ctx.int_val(max_n));
        if (max_z > 0 && !sum_z.is_int())
            s_solver.add(sum_z > ctx.int_val(max_z));

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

/**
 * @brief  This procedure checks the reachability of final thread state with
 * 		fixed number of threads, i.e., if there exists a path tau_0 ->* tau_F
 * 		s.t. tau_F covers final.
 * @param n: # of initial   threads
 * @param z: # of spawn transitions
 * @return bool
 * 		true : if there is a witness path
 * 		false: otherwise
 */
bool tse::check_reach_with_fixed_threads(const uint& n, const uint& z) {
    /// if there is no spwan transitions, so call standard_FWS
    if (!refs::is_exists_SPAWN)
        return this->standard_FWS(n, z);

    /// enumeratively calling standard_FWS over (1, z) ... (n, ..., z)
    for (unsigned in = 1; in < n; ++in) {
        if (this->standard_FWS(in, z))
            return true;
    }

    /// enumeratively calling standard_FWS over (n, 1) ... (n, ..., z)
//	for (unsigned iz = 1; iz < z; ++iz) {
//		if (this->standard_FWS(n, iz))
//			return true;
//	}
    return false;
}

/**
 * @brief  standard bounded forward search
 * 	    This procedure checks the reachability of final thread state with
 * 		fixed number of threads, i.e., if there exists a path tau_0 ->* tau_F
 * 		s.t. tau_F covers final.
 * @param n: # of initial   threads
 * @param z: # of spawn transitions
 * @return bool
 * 		true : if there is a witness path
 * 		false: otherwise
 */
bool tse::standard_FWS(const uint& n, const uint& z) {
    auto spw = z;
//	cout << "fws: " << n << "               " << z << endl;
    queue<global_state, list<global_state>> W; /// worklist
    W.push(global_state(refs::INITL_TS, n)); /// start from the initial state with n threads
    set<global_state> R; /// reachable global states
    while (!W.empty()) {
        global_state tau = W.front();
        W.pop();
        const ushort &shared = tau.get_share();
        for (auto il = tau.get_locals().begin(); il != tau.get_locals().end();
                ++il) {
            thread_state src(shared, il->first);
            if (src != refs::FINAL_TS) {
                auto ifind = refs::original_TTD.find(src);
                if (ifind != refs::original_TTD.end()) {
                    for (auto idst = ifind->second.begin();
                            idst != ifind->second.end(); ++idst) {
                        auto locals = tau.get_locals();
                        if (refs::is_exists_SPAWN
                                && this->is_spawn_transition(src, *idst)) { /// if src +> dst true
                            if (spw > 0) {
                                spw--;
                                locals = this->update_counter(locals,
                                        idst->get_local());
                            } else { /// if the we already spawn z times, we can't
                                continue; /// spawn any more and have to skip src +> dst;
                            }
                        } else {
                            locals = this->update_counter(locals,
                                    src.get_local(), idst->get_local());
                        }
                        global_state _tau(idst->get_share(), locals);
                        if (R.insert(_tau).second) {
                            /// record _tau's predecessor tau: for witness
                            //_tau.pi = std::make_shared<Global_State>(tau);
                            W.push(_tau);
                        }
                    }
                }
            } else { /// if src == final
                cout << src << endl;
                cout << "covering configuration: " << tau << endl;
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
bool tse::is_spawn_transition(const thread_state& src,
        const thread_state& dst) {
    auto ifind = refs::spawntra_TTD.find(src);
    if (ifind == refs::spawntra_TTD.end()) {
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
