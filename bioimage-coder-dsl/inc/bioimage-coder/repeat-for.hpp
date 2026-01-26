#pragma once

/** Domain specific language (DSL) for 96-eyes instrument control. */
namespace bioimage_coder{

/** A generic integer range.
 *
 * @tparam Symbol The letter "i" in Python code `for i in range(1, 3, step):...`
 * @tparam Integer Either signed or unsigned integer type.
 */
template <char Symbol, typename Integer>
struct Range {
    static constexpr char symbol{Symbol};
    Integer begin;
    Integer end;
    Integer step{1};
};

/** Intermediate representation (IR) for the repeated steps. */
template <char Symbol, typename Integer, class Callable>
struct repeat_for_t {
    Range<Symbol, Integer> range;
    Callable steps;
};

/** A hint for the 96-eyes instrument to repeat the steps by N times.
 *
 * @tparam Symbol The letter "i" in Python code `for i in range(1, 3, step):...`
 * @tparam Integer Either signed or unsigned integer type.
 * @tparam Callable the repeatable instrument control steps, pameterized by an integer variable.
 */
template <char Symbol, typename Integer, class Callable>
constexpr repeat_for_t<Symbol, Integer, Callable>
repeat_for(Range<Symbol, Integer>&& r, Callable&& steps) {
    return {r, steps};
}

template <typename T>
struct is_repeat_for_t : std::false_type {};

template <char Symbol, typename... Args>
struct is_repeat_for_t<repeat_for_t<Symbol, Args...>> : std::true_type {};

template <typename T>
constexpr bool is_repeat_for_v = is_repeat_for_t<std::decay_t<T>>::value;

}  // namespace bioimage_coder_dsl