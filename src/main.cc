/**===========================================================================================
 * @note        I am rewriting the URSULA on this project. I am trying to use more generic
 * 				programming techniques. This may spend some time. Not in a rush.
 *
 * @name       	: URSULA
 * 					Unbounded-thread Reachability via Symbolic execUtion and Loop Acceleration
 * @author     	: Peizun Liu
 * @version    	: v0.2
 * @copyright  	: The copy right belong to CAV group in CCIS NU
 * @brief		: We present a new approach to reachability analysis for communicating
 * 				finite-state threads that formulates the analysis as a logical decision
 * 				problem. In addition to the unbounded number of running threads, the
 * 				difficulty in making such approaches sound has traditionally been the
 * 				existence of unbounded loops in the program executed by a thread.
 * 				We show in this paper how loop iterations can be summarized into
 * 				Presburger arithmetic constraints. The constraints are defined over a
 * 				single-threaded abstract program and are satisfiable exactly if the original
 * 				unbounded-thread reachability problem has a solution. An advantage of
 * 				our approach is that the purely static analysis involved in building the
 * 				Presburger formula can often reveal its (un-)satisfiability, practically in
 * 				zero time, before any kind of arithmetic has been performed.
 * 				We demonstrate the power of this technique for checking safety properties
 * 				of unbounded-thread programs and infinite-state transition systems.
 *==========================================================================================*/

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
		Cmd_Line cmd = CMD::create_argument_list();
		try {
			cmd.get_command_line(argc, argv);
		} catch (Cmd_Line::Help) {
			return 0;
		}

		Refs::OPT_PRINT_DOT = cmd.arg_bool(PROB_INST_OPTS, "--ettd2dot");
		Refs::OPT_PRINT_ADJ = cmd.arg_bool(PROB_INST_OPTS, "--adj-list");

		Refs::OPT_PRINT_PATH = cmd.arg_bool(EXP_MODE_OPTS, "--path");
		Refs::IS_BWS_TREE = cmd.arg_bool(EXP_MODE_OPTS, "--bwstree");
		Refs::OPT_COMPLETE = cmd.arg_bool(EXP_MODE_OPTS, "--complete");
		Refs::OPT_BACKWARD = cmd.arg_bool(EXP_MODE_OPTS, "--backward");
		Refs::OPT_SHARED = cmd.arg_bool(EXP_MODE_OPTS, "--shared");

		Refs::OPT_NOT_SIMPLE = cmd.arg_bool(OTHER_OPTS, "--nosimpl");
		Refs::OPT_PRINT_ALL = cmd.arg_bool(OTHER_OPTS, "--all");
		Refs::OPT_CONSTRAINT = cmd.arg_bool(OTHER_OPTS, "--cstr");

		if (cmd.arg_bool(OTHER_OPTS, "--cmd-line") || Refs::OPT_PRINT_ALL) {
		}

		const string& filename = cmd.arg_value(PROB_INST_OPTS, "--input-file");
		const string& initl_ts = cmd.arg_value(PROB_INST_OPTS, "--initial");
		const string& final_ts = cmd.arg_value(PROB_INST_OPTS, "--target");

		ura u;
		const bool& is_reachable = u.unbounded_reachability_analysis(filename,
				initl_ts, final_ts);
		cout << "======================================================\n";
		cout << " " << Refs::FINAL_TS;
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
