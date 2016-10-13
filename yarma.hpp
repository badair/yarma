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

#include <callable_traits/parent_class_of.hpp>
#include <callable_traits/return_type.hpp>
#include <callable_traits/args.hpp>

namespace yarma {

    template<typename T, T mptr>
    struct member {
        
        using return_type = typename callable_traits::return_type_t<T>;
        using parameter_types = callable_traits::args_t<T>;
        using parent_class = callable_traits::parent_class_of_t<T>;
        using type = T;

        static constexpr T ptr = mptr;

        template<typename... Args>
        static decltype(auto) get(Args&&... args) {
            return std::invoke(mptr, static_cast<Args&&>(args)...);
        }
    };

    template<typename... members>
    struct member_var;
} // namespace yarma

namespace std {

    template<typename... members>
    struct variant_size< ::yarma::member_var<members...>>
      : public variant_size<std::variant<members...>> {};
    
    template<std::size_t I, typename... members>
    struct variant_alternative<I, ::yarma::member_var<members...>>
      : public variant_alternative<I, std::variant<members...>> {};
} // namespace std

namespace yarma {
    
    template<typename... members>
    struct member_var 
      : public std::variant<members...> {
        
        using std::variant<members...>::variant;
        using std::variant<members...>::operator=;
          
        char const * name() const {
            return std::visit( [](auto & mem){
                return mem.name();
            }, *this);
        }

        bool is_data() const {
            return std::visit( [](auto mem){
                return !std::is_member_function_pointer_v<typename decltype(mem)::type>;
            }, *this);
        }

        int arity() const {
            return std::visit( [](auto mem){
                return std::is_member_function_pointer_v<typename decltype(mem)::type>?
                    // minus one to exclude the `this` object
                    static_cast<int>(std::tuple_size_v<typename decltype(mem)::parameter_types> - 1)
                    : -1;
            }, *this);
        }

        template<typename ret = std::common_type<typename members::return_type...>,
          typename... Args>
        typename ret::type get(Args&&... args) const {
            return std::visit( [&](auto mem){
                return std::invoke(mem.ptr, static_cast<Args&&>(args)...);
            }, *this);
        }
    };
    
    template<typename... members>
    static auto const & get_members() {
        using type = std::array<member_var<members...>, sizeof...(members)>;
        static type const results{members{}...}; 
        return results;
    }
} // namespace yarma

#define YARMA_REFLECT(seq)                                              \
    decltype(auto) members() const {                                    \
        using T = std::remove_const_t<                                  \
            std::remove_pointer_t<decltype(this)>>;                     \
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), YARMA_NAMES, seq)       \
        return ::yarma::get_members<BOOST_PP_REPEAT(                    \
            BOOST_PP_SEQ_SIZE(seq), YARMA_MEMBERS, seq)>();             \
    }                                                                   \
/**/

#define YARMA_NAMES(z, i, seq)                                          \
    struct BOOST_PP_CAT(yarma_, i) : yarma::member<                     \
        decltype(&T::BOOST_PP_SEQ_ELEM(i,seq)),                         \
        &T::BOOST_PP_SEQ_ELEM(i,seq)> {                                 \
        static constexpr const char * name() {                          \
            return BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(i,seq));        \
        }                                                               \
    };                                                                  \
/**/

#define YARMA_MEMBERS(z, i, seq) BOOST_PP_COMMA_IF(i) BOOST_PP_CAT(yarma_, i)

#endif // #ifndef YARMA_HPP
