/**============================================================================
 * @name      : CTSE
 * 					C++ version Thread-State Equation
 * 				Unbounded-Thread Program Verification using Thread-State Equations
 *
 * @author    : Peizun Liu
 * @version   : v1.1
 * @copyright : The copy right belong to CAV group in CCIS NU
 * @brief     : Infinite-state reachability problems arising from unbounded-
 *              thread program verification are of great practical importance,
 *              yet algo- rithmically hard. Despite the remarkable success of
 *              explicit-state explo- ration methods to solve such problems,
 *              there is a sense that SMT technology can be beneficial to speed
 *              up the decision making. This vision was pioneered in recent
 *              work by Esparza et al. on SMT-based coverabil- ity analysis of
 *              Petri nets. We present here an approximate coverability method
 *              that operates on thread-transition systems, a model naturally
 *              derived from predicate abstractions of multi-threaded programs.
 *              In addition to successfully proving uncoverability for all our
 *              safe benchmark programs, our approach extends previous work by
 *              the ability to decide the unsafety of many unsafe programs, and
 *              to provide a witness path. We also demonstrate experimentally
 *              that our method beats all leading explicit-state techniques on
 *              safe benchmarks and is competitive on unsafe ones, promising to
 *              be a very accurate and fast coverability analyzer
 *===========================================================================*/

#include <iostream>

#include "util/cmd.hh"
#include "util/refs.hh"

#include "tse/ura.hh"

using namespace sura;
using namespace std;

/**
 * @brief: The main function
 * @param: argc
 * @param: argv
 * @return int
 * 		   exit code
 */
int main(const int argc, const char * const * const argv) {
	try {
		cmd_line cmd;
		try {
			cmd.get_command_line(argc, argv);
		} catch (cmd_line::Help) {
			return 0;
		}

		refs::OPT_PRINT_ADJ = cmd.arg_bool(cmd_line::prob_inst_opts(),
				"--adj-list");

		refs::OPT_CONSTRAINT = cmd.arg_bool(cmd_line::exp_mode_opts(),
				"--constraint");
		refs::OPT_PRINT_ALL = cmd.arg_bool(cmd_line::other_opts(), "--all");

		const bool& is_self_loop = cmd.arg_bool(cmd_line::exp_mode_opts(),
				"--self-loop");

		if (cmd.arg_bool(cmd_line::other_opts(), "--cmd-line")
				|| refs::OPT_PRINT_ALL) {
		}

		const string& filename = cmd.arg_value(cmd_line::prob_inst_opts(),
				"--input-file");
		const string& initl_ts = cmd.arg_value(cmd_line::prob_inst_opts(),
				"--initial");
		const string& final_ts = cmd.arg_value(cmd_line::prob_inst_opts(),
				"--target");

		ura u;
		const bool& is_reachable = u.unbounded_reachability_analysis(filename,
				initl_ts, final_ts, is_self_loop);
		cout << "======================================================\n";
		cout << " (" << refs::S_FINAL_TS << ")";
		if (is_reachable)
			cout << " is reachable: verification failed!\n";
		else
			cout << " is unreachable: verification successful!\n";
		cout << "======================================================"
				<< endl;
		return 0;
	} catch (const sura_exception & e) {
		e.what();
	} catch (const z3::exception &e) {
		std::cerr << e.msg() << endl;
	} catch (const std::exception& e) {
		std::cerr << e.what() << endl;
	} catch (...) {
		std::cerr << sura_exception("main: unknown exception occurred").what()
				<< endl;
	}
}
