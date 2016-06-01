/**
 * ura.hh
 *
 *  Created on: Jul 28, 2015
 *      Author: lpzun
 */

#ifndef URA_HH_
#define URA_HH_

#include "../util/utilities.hh"

#include "tse.hh"

namespace sura {

class ura {
public:
    ura();
    ~ura();
    bool unbounded_reachability_analysis(const string& filename,
            const string& s_initl, const string& s_final,
            const bool& is_self_loop);

private:
    thread_state parse_input_tss(const string& str_ts);
    bool reachability_analysis_via_tse(const string& filename,
            const bool& is_self_loop);
};

} /* namespace sura */

#endif /* URA_HH_ */
