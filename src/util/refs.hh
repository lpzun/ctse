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

using adj_list = map<Thread_State, deque<Thread_State>>;

class Refs {
public:
	Refs();
	~Refs();

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

	static Thread_State INITL_TS;
	static Thread_State FINAL_TS;

	static adj_list original_TTD;
	static adj_list spawntra_TTD;

	/// global variable for elapsed time
	static clock_t ELAPSED_TIME;
};
} /* namespace SURA */

#endif /* REFS_HH_ */
