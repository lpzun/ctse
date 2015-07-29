/*
 * ura.cc
 *
 *  Created on: Jul 28, 2015
 *      Author: lpzun
 */

#include "ura.hh"

namespace sura {

ura::ura() {
	// TODO Auto-generated constructor stub

}

ura::~ura() {
	// TODO Auto-generated destructor stub
}

/**
 * @brief
 * @param filename
 * @param s_initl
 * @param s_final
 * @return
 */
bool ura::unbounded_reachability_analysis(const string& filename,
		const string& s_initl, const string& s_final) {
	Refs::INITL_TS = this->parse_input_tss(s_initl);
	Refs::FINAL_TS = this->parse_input_tss(s_final);
	return this->reachability_analysis_via_tse(filename);
}

/**
 * @brief parse the input initial and final thread state
 * @param str_ts: the thread state represented by string
 */
Thread_State ura::parse_input_tss(const string& str_ts) {
	Thread_State ts;
	if (str_ts.find('|') != std::string::npos) {
		ts = Util::create_thread_state_from_gs_str(str_ts);
	} else { /// str_ts is store in a file
		ifstream in(str_ts.c_str());
		if (in.good()) {
			string s_ts;
			std::getline(in, s_ts);
			ts = Util::create_thread_state_from_gs_str(s_ts);
			in.close();
		} else {
			throw ural_rt_err("read_in_thread_state: unknown input file");
		}
	}
	return ts;
}

/**
 * @brief parse the input TTD
 * @param filename: the name of input .ttd file
 */
bool ura::reachability_analysis_via_tse(const string& filename) {
	if (filename == "X") { // make random structure
		throw ural_rt_err("Please assign the input file!");
	} else {
		/// original input file before removing comments
		ifstream org_in(filename.c_str());
		if (!org_in.good())
			throw ural_rt_err("Input file does not find!");
		Parser::remove_comments(org_in, "/tmp/tmp.ttd.no_comment", "#");
		org_in.close();

		/// new input file after removing comments
		ifstream new_in("/tmp/tmp.ttd.no_comment");

		new_in >> Thread_State::S >> Thread_State::L;

		/// store all incoming & outgoing transitions from same local state
		/// key is the shared state,
		///        first  value is the set of incoming transition id
		///        second value is the set of outgoing transition id
		vector<inout> l_in_out(Thread_State::L);

		/// store all incoming & outgoing transitions from same shared state
		/// key is the shared state,
		///        first  value is the set of incoming transition id
		///        second value is the set of outgoing transition id
		vector<inout> s_in_out(Thread_State::S);

		id_tran transition_ID = 0;  /// define unique transition ID
		deque<id_tran> spawn_vars;

		Shared_State s1, s2;              /// shared states
		Local_State l1, l2;               /// local  states
		string sep;                       /// separator
		while (new_in >> s1 >> l1 >> sep >> s2 >> l2) {
			DBG_STD(
					cout << s1 << " " << l1 << " -> " << s2 << " " << l2 << " "
							<< transition_ID << "\n")
			if (l1 == l2) /// remove self loops and vertical transitions
				continue;

			if (sep == "->" || sep == "+>") {
				const Thread_State src_TS(s1, l1);
				const Thread_State dst_TS(s2, l2);

				l_in_out[l1].second.emplace_back(transition_ID);
				l_in_out[l2].first.emplace_back(transition_ID);

				if (s1 != s2) {
					s_in_out[s1].second.emplace_back(transition_ID);
					s_in_out[s2].first.emplace_back(transition_ID);
				}

				if (sep == "+>") {
					Refs::spawntra_TTD[src_TS].emplace_back(dst_TS);
					spawn_vars.emplace_back(transition_ID);
				}
				Refs::original_TTD[src_TS].emplace_back(dst_TS);

				transition_ID++; /// increment unique transition ID
			} else {
				throw ural_rt_err("illegal transition");
			}
		}
		new_in.close();

#ifndef NDEBUG
		cout << __func__ << "\n";
		cout << "Initial Thread State " << Refs::INITL_TS << "\t";
		cout << "Final Thread State " << Refs::FINAL_TS << "\n";

		for (size_t is = 0; is < l_in_out.size(); ++is) {
			cout << "local state: " << is << " ";
			for (auto iv = l_in_out[is].first.begin();
					iv != l_in_out[is].first.end(); ++iv)
				cout << "x" << *iv
						<< (std::next(iv) != l_in_out[is].first.end() ?
								" + " : "");
			for (auto iv = l_in_out[is].second.begin();
					iv != l_in_out[is].second.end(); ++iv)
				cout << " - " << "x" << *iv;
			cout << "\n";
		}

		for (size_t is = 0; is < s_in_out.size(); ++is) {
			cout << "shared state: " << is << " ";
			for (auto iv = s_in_out[is].first.begin();
					iv != s_in_out[is].first.end(); ++iv)
				cout << "x" << *iv
						<< (std::next(iv) != s_in_out[is].first.end() ?
								" + " : "");
			for (auto iv = s_in_out[is].second.begin();
					iv != s_in_out[is].second.end(); ++iv)
				cout << " - " << "x" << *iv;
			cout << "\n";
		}
#endif

		if (Refs::OPT_PRINT_ADJ || Refs::OPT_PRINT_ALL) {
			cout << "The original TTD:" << endl;
			Util::print_adj_list(Refs::original_TTD);
			cout << endl;
		}

		tse t(transition_ID, spawn_vars);
		return t.reachability_analysis_via_tse(l_in_out, s_in_out);
	}
}

} /* namespace sura */
