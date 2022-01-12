module;

#include <iterator>

export module Experiments;
import RozeFoundUtils;

namespace ns {

	template<typename container> struct iterator {

		using difference_type = std::ptrdiff_t;

		using value_type = container::value_type;
		using pointer = value_type*;
		using reference = value_type&;

		using iterator_tag = std::random_access_iterator_tag;
		using iterator_concept = std::contiguous_iterator_tag;

		iterator() = default;
		explicit constexpr iterator(pointer Ptr) : m_Ptr(Ptr) {}

		constexpr auto& operator++() noexcept { ++m_Ptr; return *this; }
		constexpr auto operator++(int) noexcept { auto Tmp = *this; ++* this; return Tmp; }

		constexpr auto& operator--() noexcept { --m_Ptr; return *this; }
		constexpr auto operator--(int) noexcept { auto Tmp = *this; --* this; return Tmp; }

		constexpr auto& operator+=(const ptrdiff_t offset) noexcept { m_Ptr += offset; return *this; }
		constexpr auto& operator-=(const ptrdiff_t offset) noexcept { m_Ptr -= offset; return *this; }

		friend constexpr auto operator+(const ptrdiff_t offset, iterator It) noexcept { It += offset; return It; }
		[[nodiscard]] constexpr auto operator+(const ptrdiff_t offset) const noexcept { auto Tmp = *this; return Tmp += offset; }
		friend constexpr auto operator-(const ptrdiff_t offset, iterator It) noexcept { It -= offset; return It; }
		[[nodiscard]] constexpr auto operator-(const ptrdiff_t offset) const noexcept { auto Tmp = *this; return Tmp -= offset; }

		friend constexpr ptrdiff_t operator-(iterator lhs, iterator rhs) noexcept { return lhs.m_Ptr - rhs.m_Ptr; }

		constexpr auto operator<=>(iterator const&) const = default;

		[[nodiscard]] constexpr reference operator*() const noexcept { return *operator->(); }
		[[nodiscard]] constexpr reference operator[](const ptrdiff_t offset) const noexcept { return *(*this + offset); };
		[[nodiscard]] constexpr pointer operator->() const noexcept { return m_Ptr; }

	private:

		pointer m_Ptr;
	};
}

export template<typename T = int> class array {

public:

	using value_type = typename T;
	using iterator = ns::iterator<array<T>>;
	using reverse_iterator = std::reverse_iterator<iterator>;

public:

	// Constructors

	array(size_t size) : m_Data(new T[size]), m_Size(size) {}
	array(std::initializer_list<T> list) : m_Size(list.size()), m_Data(new T[m_Size]) {
		std::copy(list.begin(), list.end(), m_Data);
	}
	~array() { delete[] m_Data; }

	// Methods

	const size_t size() const { return m_Size; }

	T* data() { return m_Data; }
	const T* data() const { return m_Data; }

	T& front() { return m_Data[0]; }
	const T& front() const { return m_Data[0]; }
	T& back() { return m_Data[m_Size - 1]; }
	const T& back() const { return m_Data[m_Size - 1]; }

	constexpr iterator begin() { return iterator(m_Data); }
	constexpr iterator end() { return iterator(m_Data + m_Size); }

	constexpr reverse_iterator rbegin() { return reverse_iterator(begin()); }
	constexpr reverse_iterator rend() { return reverse_iterator(end()); }

	// Operator overloads

	T& operator[](size_t offset) { return m_Data[offset]; }
	const T& operator[](size_t offset) const { return m_Data[offset]; }

	//operator std::string() const { return (*this | ext::join); };

private:

	size_t m_Size;
	T* m_Data;

};

template<typename T = size_t> class Range {

	struct iterator {
		iterator(T Data) : m_Data(Data) {}
		T operator*() const { return m_Data; }
		iterator& operator++() { m_Data++; return *this; }
		bool operator==(const iterator& other) const { return m_Data == other.m_Data; }
	private:
		T m_Data;
	};

public:

	Range(T max) : m_Max(max) {}

	iterator begin() { return iterator(m_Data); }
	iterator end() { return iterator(m_Data + m_Max); }

private:

	T m_Data = 0;
	T m_Max;
};