#include <map>
#include <utility>
#include <iterator>

template<typename K, typename V>
class interval_map {
	friend void IntervalMapTest();
	V m_valBegin;
	std::map<K, V> m_map;

public:
	// constructor associates whole range of K with val
	template<typename V_forward>
	interval_map(V_forward&& val)
	: m_valBegin(std::forward<V_forward>(val))
	{}

	// Assign value val to interval [keyBegin, keyEnd).
	template<typename V_forward>
	void assign(const K& keyBegin, const K& keyEnd, V_forward&& val)
		requires (std::is_same<std::remove_cvref_t<V_forward>, V>::value)
	{
		if (!(keyBegin < keyEnd)) {
			return; // Empty interval, do nothing.
		}

		// 1. Erase elements within [keyBegin, keyEnd)
		auto it = m_map.lower_bound(keyBegin);
		auto endIt = m_map.lower_bound(keyEnd);

		// Check if we need to insert/update keyBegin
		if (it == m_map.begin() && val == m_valBegin) {
			// `val` is already the starting value before m_map's range.
		} else if (it == m_map.end() || it->first != keyBegin || it->second != val) {
			it = m_map.emplace_hint(it, keyBegin, std::forward<V_forward>(val));
		}

		// Erase all elements within the range
		m_map.erase(std::next(it), endIt);

		// 2. Adjust the end point to maintain canonicity
		if (endIt != m_map.end() && endIt->second == val) {
			m_map.erase(endIt);
		} else if (keyEnd != m_map.end()) {
			m_map[keyEnd]
