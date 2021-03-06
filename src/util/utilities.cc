/**
 * @name utilities.hh
 *
 * @date  : Jun 23, 2015
 * @author: Peizun Liu
 */

#include "utilities.hh"

namespace sura {

util::util() {
    // TODO Auto-generated constructor stub

}

util::~util() {
    // TODO Auto-generated destructor stub
}

/**
 *
 * @param s_ts
 * @param delim
 * @return
 */
thread_state util::create_thread_state_from_str(const string& s_ts,
        const char& delim) {
    vector<string> vs_ts = PPRINT::split(s_ts, delim);
    if (vs_ts.size() != 2) {
        throw("The format of thread state is wrong.");
    }
    return thread_state(atol(vs_ts[0].c_str()), atol(vs_ts[1].c_str()));
}

/**
 *
 * @param s_ts
 * @param delim
 * @return
 */
thread_state util::create_thread_state_from_gs_str(const string& s_ts,
        const char& delim) {
    auto vs_ts = PPRINT::split(s_ts, delim);
    if (vs_ts.size() != 2) {
        throw ural_rt_err("The format of global state is wrong.");
    }
    auto vs_locals = PPRINT::split(vs_ts[1], ',');
    refs::TARGET_THR_NUM = vs_locals.size();
    return thread_state(atol(vs_ts[0].c_str()), atol(vs_locals[0].c_str()));
}
/**
 * @brief print all of the transitions in the thread-state transition diagram
 * @param adjacency_list
 * @param out
 */
void util::print_adj_list(const map<thread_state, set<thread_state> >& adj_list,
        ostream& out) {
    out << thread_state::L << " " << thread_state::S << endl;
    for (auto iu = adj_list.begin(); iu != adj_list.end(); ++iu)
        for (auto iv = iu->second.begin(); iv != iu->second.end(); ++iv)
            out << iu->first << " -> " << (*iv) << endl;
}

/**
 * @brief print all of the transitions in the thread-state transition diagram
 * @param adjacency_list
 * @param out
 */
void util::print_adj_list(
        const map<thread_state, deque<thread_state> >& adj_list, ostream& out) {
    out << thread_state::L << " " << thread_state::S << endl;
    for (auto iu = adj_list.begin(); iu != adj_list.end(); ++iu)
        for (auto iv = iu->second.begin(); iv != iu->second.end(); ++iv)
            out << iu->first << " -> " << (*iv) << endl;
}

/**
 * @brief remove the comments of .ttd files
 * @param in
 * @param filename
 * @param comment
 */
void parser::remove_comments(istream& in, const string& filename,
        const string& comment) {
    ofstream out(filename.c_str());
    remove_comments(in, out, comment);
}

/**
 * @brief remove the comments of .ttd files
 * @param in
 * @param out
 * @param comment
 */
void parser::remove_comments(const string& in, string& out,
        const string& comment) {
    std::istringstream instream(in);
    std::ostringstream outstream;
    remove_comments(instream, outstream, comment);
    out = outstream.str();
}

void parser::remove_comments(istream& in, ostream& out, const string& comment) {
    string line;
    while (getline(in, line = "")) {
        const size_t comment_start = line.find(comment);
        out
                << (comment_start == string::npos ?
                        line : line.substr(0, comment_start)) << endl;
    }
}

bool parser::getline(istream& in, string& line, const char& eol) {
    char c = 0;
    while (in.get(c) ? c != eol : false)
        line += c;
    return c != 0;
}

/// end of PARSER
} /* namespace SURA */
