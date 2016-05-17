/**
 * @name state.hh
 *
 * @date  : Jun 21, 2015
 * @author: Peizun liu
 */

#ifndef STATE_HH_
#define STATE_HH_

#include "algs.hh"

namespace sura {

/// define local state
typedef unsigned short local_state;
/// define the size of local states
typedef unsigned short size_l;

/// define shared state
typedef unsigned short shared_state;
/// define size of shared states
typedef unsigned short size_s;

/// define the counter of thread state
typedef unsigned short size_p;

/// define the thread state id
typedef unsigned int id_thread_state;

/// class thread state
class thread_state {
public:
    static size_s S; /// the size of shared state
    static size_l L; /// the size of local  state

    inline thread_state();
    inline thread_state(const thread_state& t);
    inline thread_state(const shared_state& share, const local_state& local);
    ~thread_state() {
    }

    ostream& to_stream(ostream& out = cout) const;

    inline local_state get_local() const {
        return local;
    }

    inline shared_state get_share() const {
        return share;
    }

private:
    shared_state share;
    local_state local;
};

/**
 * @brief default constructor
 */
inline thread_state::thread_state() :
        share(0), local(0) {
}

/**
 * @brief constructor with thread state
 * @param t
 */
inline thread_state::thread_state(const thread_state& t) :
        share(t.share), local(t.local) {

}

/**
 * @brief constructor with shared state and local state
 * @param share: shared state
 * @param local: local  state
 */
inline thread_state::thread_state(const shared_state& share,
        const local_state& local) :
        share(share), local(local) {
    __SAFE_ASSERT__(share < S && local < L);
}

/**
 * @brief print thread state
 * @param out
 * @return ostream
 */
inline ostream& thread_state::to_stream(ostream& out) const {
    out << "(" << share << "|" << local << ")";
    return out;
}

/**
 * @brief overloading operator <<: print thread state
 * @param out
 * @param ts
 * @return ostream
 */
inline ostream& operator<<(ostream& out, const thread_state& t) {
    return t.to_stream(out);
}

/**
 * @brief overloading operator ==
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 == t2
 * 		   false: otherwise
 */
inline bool operator==(const thread_state& t1, const thread_state& t2) {
    return t1.get_share() == t2.get_share() && t1.get_local() == t2.get_local();
}

/**
 * @brief overloading operator !=
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 == t2
 * 		   false: otherwise
 */
inline bool operator!=(const thread_state& t1, const thread_state& t2) {
    return !(t1 == t2);
}

/**
 * @brief overloading operator <
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 < t2
 * 		   false: otherwise
 */
inline bool operator<(const thread_state& t1, const thread_state& t2) {
    if (t1.get_share() == t2.get_share())
        return t1.get_local() < t2.get_local();
    else
        return t1.get_share() < t2.get_share();
}

/**
 * @brief overloading operator >
 * @param t1
 * @param t2
 * @return bool
 * 		   true : if t1 > t2
 * 		   false: otherwise
 */
inline bool operator>(const thread_state& t1, const thread_state& t2) {
    return t2 < t1;
}

/// class global state
typedef map<local_state, size_p> Locals;

class global_state {
public:
    inline global_state();
    inline global_state(const thread_state& t);
    inline global_state(const thread_state& t, const size_p& n);
    inline global_state(const shared_state& share, const Locals& locals);
    inline global_state(const shared_state& share, const Locals& locals,
            shared_ptr<const global_state> pi);

    ~global_state() {
    }

    ostream& to_stream(ostream& out = cout, const string& sep = "|") const;

    inline const Locals& get_locals() const {
        return locals;
    }

    inline const shared_ptr<const global_state>& get_pi() const {
        return pi;
    }

    inline shared_state get_share() const {
        return share;
    }

    inline void set_pi(const shared_ptr<const global_state>& pi) {
        this->pi = pi;
    }

private:
    shared_state share;
    Locals locals;
    shared_ptr<const global_state> pi;
};

/**
 * @brief default constructor: initialize
 *        share  = 0
 *        locals = empty map
 */
inline global_state::global_state() :
        share(0), locals(Locals()), pi(nullptr) {
}

/**
 * @brief constructor with a thread state
 * @param t
 */
inline global_state::global_state(const thread_state& t) :
        share(t.get_share()), locals(Locals()), pi(nullptr) {
    locals.emplace(t.get_local(), 1);
}

/**
 * @brief constructor with a thread state and n threads
 * @param t
 * @param n
 */
inline global_state::global_state(const thread_state& t, const size_p& n) :
        share(t.get_share()), locals(Locals()), pi(nullptr) {
    locals.emplace(t.get_local(), n);
}

/**
 * @brief constructor with a shared state and local states
 * @param share : shared state
 * @param locals: local states represented in counter abstraction form
 */
inline global_state::global_state(const shared_state& share,
        const Locals& locals) :
        share(share), locals(locals), pi(nullptr) {
}

/**
 * @brief constructor with a shared state, local states and pi
 * @param share : shared state
 * @param locals: local states represented in counter abstraction form
 * @param pi	: the father of current global state
 */
inline global_state::global_state(const shared_state& share,
        const Locals& locals, shared_ptr<const global_state> pi) :
        share(share), locals(locals), pi(pi) {
}

/**
 * @brief call by <<
 * @param out
 * @param sep
 * @return
 */
inline ostream& global_state::to_stream(ostream& out, const string& sep) const {
    out << "<" << this->share << "|";
    for (auto iloc = this->locals.begin(); iloc != this->locals.end(); ++iloc) {
        out << "(" << iloc->first << "," << iloc->second << ")";
    }
    out << ">";
    return out;
}

/**
 * @brief overloading operator <<
 * @param out
 * @param g
 * @return
 */
inline ostream& operator<<(ostream& out, const global_state& g) {
    return g.to_stream(out);
}

/**
 * @brief overloading operator <
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 < s2
 * 		   false: otherwise
 */
inline bool operator<(const global_state& s1, const global_state& s2) {
    if (s1.get_share() == s2.get_share()) {
        return COMPARE::compare_map(s1.get_locals(), s2.get_locals()) == -1;
    } else {
        return s1.get_share() < s2.get_share();
    }
}

/**
 * @brief overloading operator >
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 > s2
 * 		   false: otherwise
 */
inline bool operator>(const global_state& s1, const global_state& s2) {
    return s2 < s1;
}

/**
 * @brief overloading operator ==
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 == s2
 * 		   false: otherwise
 */
inline bool operator==(const global_state& s1, const global_state& s2) {
    if (s1.get_share() == s2.get_share()) {
        if (s1.get_locals().size() == s2.get_locals().size()) {
            auto is1 = s1.get_locals().begin(), is2 = s2.get_locals().begin();
            while (is1 != s1.get_locals().end()) {
                if ((is1->first != is2->first) || (is1->second != is2->second))
                    return false;
                is1++, is2++;
            }
            return true;
        }
    }
    return false;
}

/**
 * @brief overloading operator !=
 * @param s1
 * @param s2
 * @return bool
 * 		   true : if s1 != s2
 * 		   false: otherwise
 */
inline bool operator!=(const global_state& s1, const global_state& s2) {
    return !(s1 == s2);
}

using id_tran= unsigned int;
using vertex = thread_state;

class Transition {
public:
    static id_tran ID;

    inline Transition(const Transition& t);
    inline Transition(const vertex& src, const vertex& dst);
    inline Transition(const vertex& src, const vertex& dst, const id_tran& id);
    ~Transition() {
    }

    ostream& to_stream(ostream& out = cout) const;

    inline vertex get_src() const {
        return src;
    }

    inline vertex get_dst() const {
        return dst;
    }

    inline id_tran get_id() const {
        return id;
    }

private:
    vertex src; /// source      of transition
    vertex dst; /// destination of transition
    id_tran id;
};

inline Transition::Transition(const Transition& t) :
        src(t.src), dst(t.dst), id(t.id) {

}

inline Transition::Transition(const vertex& src, const vertex& dst) :
        src(src), dst(dst), id(0) {
}

inline Transition::Transition(const vertex& src, const vertex& dst,
        const id_tran& id) :
        src(src), dst(dst), id(id) {

}

inline ostream& Transition::to_stream(ostream& out) const {
    out << src << " ";
    out << "->";
    out << " " << dst;
    out << "\n";
    return out;
}

/**
 * @brief overloading operator <<: print transition
 * @param out
 * @param ts
 * @return ostream
 */
inline ostream& operator<<(ostream& out, const Transition& r) {
    return r.to_stream(out);
}

/**
 * @brief overloading operator ==
 * @param r1
 * @param r2
 * @return bool
 * 		   true : if r1 == r2
 * 		   false: otherwise
 */
inline bool operator==(const Transition& r1, const Transition& r2) {
    return r1.get_src() == r2.get_src() && r1.get_dst() == r2.get_dst();
}

/**
 * @brief overloading operator !=
 * @param r1
 * @param r2
 * @return bool
 * 		   true : if r1 != r2
 * 		   false: otherwise
 */
inline bool operator!=(const Transition& r1, const Transition& r2) {
    return !(r1 == r2);
}

/**
 * @brief overloading operator <
 * @param r1
 * @param r2
 * @return bool
 * 		   true : if r1 < r2
 * 		   false: otherwise
 */
inline bool operator<(const Transition& r1, const Transition& r2) {
    if (r1.get_src() == r2.get_src())
        return r1.get_dst() < r2.get_dst();
    return r1.get_src() < r2.get_src();
}

/**
 * @brief overloading operator >
 * @param r1
 * @param r2
 * @return bool
 * 		   true : if r1 > r2
 * 		   false: otherwise
 */
inline bool operator>(const Transition& r1, const Transition& r2) {
    if (r1.get_src() == r2.get_src())
        return r1.get_dst() > r2.get_dst();
    return r1.get_src() > r2.get_src();
}

} /* namespace SURA */

#endif /* STATE_HH_ */
