#ifndef YARMA_HPP
#define YARMA_HPP

#include <utility>
#include <array>
#include <variant>
#include <functional>
#include <type_traits>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

namespace yarma {

    template<typename T>
    struct split_member;

    template<typename T, typename C>
    struct split_member<T C::*> {
        using type = T;
        using parent = C;
    };

    template<typename T, T mptr>
    struct member {
        
        using type = typename split_member<T>::type;
        using of_class = typename split_member<T>::parent;
        static constexpr T ptr = mptr;

        template<typename... Args>
        static decltype(auto) get(Args&&... args) {
            return std::invoke(mptr, static_cast<Args&&>(args)...);
        }
    };

    template<typename T, typename M, typename V>
    decltype(auto) visit_member(T && obj, M && m, V && v) {
        return std::visit(
            [&obj, &v](auto && mm){return v(mm.get(obj));},
            static_cast<M&&>(m)); 
    }

    template<typename... members>
    static auto const & get_members() {
        using type = std::array<std::variant<members...>, sizeof...(members)>;
        static type const results{members{}...}; 
        return results;
    }
} // namespace yarma

#define YARMA_REFLECT(seq)                                              \
    decltype(auto) members() const {                                    \
        using yarma_reflectable_type_ =                                 \
            std::remove_const_t<std::remove_pointer_t<decltype(this)>>; \
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), YARMA_NAMES, seq)       \
        return ::yarma::get_members<BOOST_PP_REPEAT(                    \
            BOOST_PP_SEQ_SIZE(seq), YARMA_MEMBERS, seq)>();             \
    }                                                                   \
/**/

#define YARMA_NAMES(z, i, seq)                                          \
    struct BOOST_PP_CAT(yarma_reflectable_member_, i) : yarma::member<  \
        decltype(&yarma_reflectable_type_::BOOST_PP_SEQ_ELEM(i,seq)),   \
        &yarma_reflectable_type_::BOOST_PP_SEQ_ELEM(i,seq)> {           \
        static constexpr auto & name() {                                \
            return BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(i,seq));        \
        }                                                               \
    };                                                                  \
/**/

#define YARMA_MEMBERS(z, i, seq) BOOST_PP_COMMA_IF(i) \
    BOOST_PP_CAT(yarma_reflectable_member_, i)        \
/**/

#endif // #ifndef YARMA_HPP