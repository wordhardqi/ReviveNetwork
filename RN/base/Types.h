//
// Created by rmqi on 28/6/18.
//

#ifndef RN_TYPES_H
#define RN_TYPES_H

#include <stdint.h>


#ifdef RN_STD_STRING
#include <string>
#else

#include <ext/vstring.h>
#include <ext/vstring_fwd.h>

#endif

#ifndef NDEBUG

#include <assert.h>

#endif

#include <RN/base/noncopyable.h>

namespace RN {
#ifdef RN_STD_STRING
    using std::string;
#else
    typedef __gnu_cxx::__sso_string string;
#endif

    template<typename To, typename From>
    inline To implicit_cast(From const &f) {
        return f;
    }

    template<typename To, typename From>
    inline To down_cast(From *f) {
        //Check at compilte time.
        if (false) {
            implicit_cast<From *, To>(0);
        }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
        assert(f == NULL || dynamic_cast<To>(f) != NULL);
#endif
        return static_cast<To>(f);
    }
}
#endif //RN_TYPES_H
