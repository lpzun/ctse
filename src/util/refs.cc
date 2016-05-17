/**
 * @name refs.cc
 *
 * @date: Jun 21, 2015
 * @author: Peizun Liu
 */

#include "refs.hh"

namespace sura {

refs::refs() {

}

refs::~refs() {

}

bool refs::OPT_SIMPLIFIED = false;
bool refs::OPT_NOT_SIMPLE = false;

bool refs::IS_BWS_TREE = false;
bool refs::OPT_COMPLETE = false;
bool refs::OPT_BACKWARD = false;
bool refs::OPT_SHARED = false;
bool refs::OPT_PRINT_PATH = false;
bool refs::OPT_CONSTRAINT = false;

bool refs::OPT_PRINT_DOT = false;
bool refs::OPT_PRINT_ADJ = false;
bool refs::OPT_PRINT_CMD = false;
bool refs::OPT_PRINT_ALL = false;

ushort refs::TARGET_THR_NUM = 1; /// default is 1

//string Refs::FILE_NAME_PREFIX = "";
string refs::S_FINAL_TS = "";

thread_state refs::INITL_TS;
thread_state refs::FINAL_TS;

adj_list refs::original_TTD;
adj_list refs::spawntra_TTD;

bool refs::is_exists_SPAWN = false;

clock_t refs::ELAPSED_TIME = clock();

} /* namespace SURA */
