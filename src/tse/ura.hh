/*
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
	virtual ~ura();
	bool unbounded_reachability_analysis(const string& filename,
			const string& s_initl, const string& s_final);

private:
	Thread_State parse_input_tss(const string& str_ts);
	void parse_input_ttd(const string& filename);

	bool reachability_analysis_via_tse();
};

} /* namespace sura */

#endif /* URA_HH_ */
