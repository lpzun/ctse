/**
 * @name refs.cc
 *
 * @date: Jun 21, 2015
 * @author: Peizun Liu
 */

#include "refs.hh"

namespace sura {

Refs::Refs() {

}

Refs::~Refs() {

}

bool Refs::OPT_SIMPLIFIED = false;
bool Refs::OPT_NOT_SIMPLE = false;

bool Refs::IS_BWS_TREE = false;
bool Refs::OPT_COMPLETE = false;
bool Refs::OPT_BACKWARD = false;
bool Refs::OPT_SHARED = false;
bool Refs::OPT_PRINT_PATH = false;
bool Refs::OPT_CONSTRAINT = false;

bool Refs::OPT_PRINT_DOT = false;
bool Refs::OPT_PRINT_ADJ = false;
bool Refs::OPT_PRINT_CMD = false;
bool Refs::OPT_PRINT_ALL = false;

ushort Refs::TARGET_THR_NUM = 1; /// default is 1

//string Refs::FILE_NAME_PREFIX = "";
string Refs::S_FINAL_TS = "";

Thread_State Refs::INITL_TS;
Thread_State Refs::FINAL_TS;

adj_list Refs::original_TTD;
adj_list Refs::spawntra_TTD;

bool Refs::is_exists_SPAWN = false;

clock_t Refs::ELAPSED_TIME = clock();

} /* namespace SURA */
