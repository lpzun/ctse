/**
 * @name refs.hh
 *
 * @date: Jun 21, 2015
 * @author: Peizun Liu
 */

#ifndef REFS_HH_
#define REFS_HH_

#include "state.hh"

namespace sura {

using adj_list = map<thread_state, deque<thread_state>>;

class refs {
public:
	refs();
	~refs();

	static bool OPT_PRINT_ALL;
	static bool OPT_PRINT_DOT;
	static bool OPT_SIMPLIFIED;
	static bool OPT_PRINT_CMD;
	static bool OPT_NOT_SIMPLE;
	static bool IS_BWS_TREE;
	static bool OPT_PRINT_PATH;
	static bool OPT_CONSTRAINT;
	static bool OPT_PRINT_ADJ;
	static bool OPT_COMPLETE;
	static bool OPT_BACKWARD;
	static bool OPT_SHARED;

	static ushort TARGET_THR_NUM;

	static thread_state INITL_TS;
	static thread_state FINAL_TS;

	static string S_FINAL_TS;

	static adj_list original_TTD;
	static adj_list spawntra_TTD;

	static bool is_exists_SPAWN;

	/// global variable for elapsed time
	static clock_t ELAPSED_TIME;
};
} /* namespace SURA */

#endif /* REFS_HH_ */
