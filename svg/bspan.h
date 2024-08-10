#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iterator>	// for std::data(), std::size()

#include "bithacks.h"
#include "charset.h"
//#include "maths.h"

namespace waavs {


	//
	// ByteSpan
	// 
	// A core type for representing a contiguous sequence of bytes.
	// As of C++ 20, there is std::span<>, and that would be a good 
	// choice, but it is not yet widely supported, and forces a jump
	// to C++20 besides.
	// 
	// The ByteSpan is used in everything from networking
	// to graphics bitmaps to audio buffers.
	// Having a universal representation of a chunk of data
	// allows for easy interoperability between different
	// subsystems.  
	// 
	// The ByteSpan, is just like 'span' and 'view' objects
	// it does not "own" the memory, it just points at it.
	// It is used as a stand-in for various data representations
	// as well as acting like a 'cursor', when trying to traverse
	// a sequence of bytes.

	struct ByteSpan
	{
		const unsigned char* fStart{ nullptr };
		const unsigned char* fEnd{ nullptr };

		// Constructors
		ByteSpan() : fStart(nullptr), fEnd(nullptr) {}
		ByteSpan(const unsigned char* start, const unsigned char* end) : fStart(start), fEnd(end) {}
		ByteSpan(const char* cstr) : fStart((const unsigned char*)cstr), fEnd((const unsigned char*)cstr + strlen(cstr)) {}
		explicit ByteSpan(const void* data, size_t sz) :fStart((const unsigned char*)data), fEnd((const unsigned char*)data + sz) {}



		// Type conversions
		explicit constexpr operator bool() const { return (fEnd - fStart) > 0; };


		// Array access
		unsigned char& operator[](size_t i) { return ((unsigned char*)fStart)[i]; }
		const unsigned char& operator[](size_t i) const { return ((unsigned char*)fStart)[i]; }

		// get current value from fStart, like a 'peek' operation
		unsigned char& operator*() { static unsigned char zero = 0;  if (fStart < fEnd) return *(unsigned char*)fStart; return  zero; }
		const uint8_t& operator*() const { static unsigned char zero = 0;  if (fStart < fEnd) return *(unsigned char*)fStart; return  zero; }


		//
		// operators for comparison
		// operator==;
		// operator!=;
		// operator<=;
		// operator>=;
		bool operator==(const ByteSpan& b) const noexcept
		{
			if (size() != b.size())
				return false;
			return memcmp(fStart, b.fStart, size()) == 0;
		}

		bool operator==(const char* b) const noexcept
		{
			size_t len = strlen(b);
			if (size() != len)
				return false;
			return memcmp(fStart, b, len) == 0;
		}

		bool operator!=(const ByteSpan& b) const noexcept
		{
			if (size() != b.size())
				return true;
			return memcmp(fStart, b.fStart, size()) != 0;
		}

		bool operator<(const ByteSpan& b) const noexcept
		{
			size_t maxBytes = size() < b.size() ? size() : b.size();
			return memcmp(fStart, b.fStart, maxBytes) < 0;
		}

		bool operator>(const ByteSpan& b) const noexcept
		{
			size_t maxBytes = size() < b.size() ? size() : b.size();
			return memcmp(fStart, b.fStart, maxBytes) > 0;
		}

		bool operator<=(const ByteSpan& b) const noexcept
		{
			size_t maxBytes = size() < b.size() ? size() : b.size();
			return memcmp(fStart, b.fStart, maxBytes) <= 0;
		}

		bool operator>=(const ByteSpan& b) const noexcept
		{
			size_t maxBytes = size() < b.size() ? size() : b.size();
			return memcmp(fStart, b.fStart, maxBytes) >= 0;
		}

		
		ByteSpan& operator+= (size_t n) {
			if (n > size())
				n = size();
			fStart += n;

			return *this;
		}


		ByteSpan& operator++() { return operator+=(1); }			// prefix notation ++y
		ByteSpan& operator++(int i) { return operator+=(1); }       // postfix notation y++



		// setting up for a range-based for loop
		const unsigned char* data() const noexcept { return (unsigned char*)fStart; }
		const unsigned char* begin() const noexcept { return fStart; }
		const unsigned char* end() const noexcept { return fEnd; }
		size_t size()  const noexcept { ptrdiff_t sz = fEnd - fStart; if (sz < 0) return 0; return sz;}
		const bool empty() const noexcept { return fStart == fEnd; }

		void setAll(unsigned char c) noexcept { memset((uint8_t*)fStart, c, size()); }
		
		// subSpan()
		// Create a bytespan that is a subspan of the current span
		// If the requested position plus size is greater than the amount
		// of span remaining at that position, the size will be truncated 
		// to the amount remaining from the requested position.
		ByteSpan subSpan(const size_t startAt, const size_t sz) const noexcept
		{
			const uint8_t* start = fStart;
			const uint8_t* end = fEnd;
			if (startAt < size())
			{
				start += startAt;
				if (start + sz < end)
					end = start + sz;
				else
					end = fEnd;
			}
			else
			{
				start = end;
			}
			return { start, end };
		}

		ByteSpan take(size_t n) const noexcept
		{
			return subSpan(0, n);
		}

		// Some convenient routines

		bool startsWith(const ByteSpan& b) const noexcept
		{
			return (subSpan((size_t)0, b.size()) == b);
		}
		
		bool endsWith(const ByteSpan& b) const noexcept
		{
			return (subSpan(size() - b.size(), b.size()) == b);
		}
	};


	static INLINE size_t copy(ByteSpan& a, const ByteSpan& b) noexcept;
	static INLINE size_t copy_to_cstr(char* str, size_t len, const ByteSpan& a) noexcept;
	static INLINE int compare(const ByteSpan& a, const ByteSpan& b) noexcept;
	static INLINE int comparen(const ByteSpan& a, const ByteSpan& b, int n) noexcept;
	static INLINE int comparen_cstr(const ByteSpan& a, const char* b, int n) noexcept;
	static INLINE bool chunk_is_equal_cstr(const ByteSpan& a, const char* s) noexcept;

	// Some utility functions for common operations

	static INLINE void chunk_truncate(ByteSpan& dc) noexcept;
	static INLINE ByteSpan& chunk_skip(ByteSpan& dc, size_t n) noexcept;
	static INLINE ByteSpan& chunk_skip_to_end(ByteSpan& dc) noexcept;

	





	// ByteSpan routines
	static INLINE ByteSpan chunk_from_cstr(const char* data) noexcept { return ByteSpan{ (uint8_t*)data, (uint8_t*)data + strlen(data) }; }


	//static inline size_t chunk_size(const ByteSpan& a) noexcept { return a.size(); }
	static INLINE bool chunk_empty(const ByteSpan& dc)  noexcept { return dc.fEnd == dc.fStart; }
	static INLINE size_t copy(ByteSpan& a, const ByteSpan& b) noexcept
	{
		size_t maxBytes = a.size() < b.size() ? a.size() : b.size();
		memcpy((uint8_t*)a.fStart, b.fStart, maxBytes);
		return maxBytes;
	}

	static inline int compare(const ByteSpan& a, const ByteSpan& b) noexcept
	{
		size_t maxBytes = a.size() < b.size() ? a.size() : b.size();
		return memcmp(a.fStart, b.fStart, maxBytes);
	}

	static inline int comparen(const ByteSpan& a, const ByteSpan& b, int n) noexcept
	{
		size_t maxBytes = a.size() < b.size() ? a.size() : b.size();
		if (maxBytes > n)
			maxBytes = n;
		return memcmp(a.fStart, b.fStart, maxBytes);
	}

	static inline int comparen_cstr(const ByteSpan& a, const char* b, int n) noexcept
	{
		size_t maxBytes = a.size() < n ? a.size() : n;
		return memcmp(a.fStart, b, maxBytes);
	}



	static inline bool chunk_is_equal_cstr(const ByteSpan& a, const char* cstr) noexcept
	{
		size_t len = strlen(cstr);
		if (a.size() != len)
			return false;
		return memcmp(a.fStart, cstr, len) == 0;
	}


	static inline void chunk_truncate(ByteSpan& dc) noexcept
	{
		dc.fEnd = dc.fStart;
	}

	static inline ByteSpan& chunk_skip(ByteSpan& dc, size_t n) noexcept
	{
		if (n > dc.size())
			n = dc.size();
		dc.fStart += n;

		return dc;
	}

	static inline ByteSpan& chunk_skip_to_end(ByteSpan& dc) noexcept { dc.fStart = dc.fEnd; }


	

	



}



// Implementation of hash function for ByteSpan
// so it can be used in 'map' collections
/*
namespace std {
	template<>
	struct hash<waavs::ByteSpan> {
		size_t operator()(const waavs::ByteSpan& span) const 
		{
			return waavs::fnv1a_32(span.data(), span.size());
		}
	};
}
*/

namespace waavs {
	struct ByteSpanHash {
		size_t operator()(const ByteSpan& span) const noexcept {
			return waavs::fnv1a_32(span.data(), span.size());
		}
	};

}

// Functions that are implemented here
namespace waavs {

	//static void writeChunkToFile(const ByteSpan& chunk, const char* filename) noexcept;
	static void writeChunk(const ByteSpan& chunk) noexcept;
	static void writeChunkBordered(const ByteSpan& chunk) noexcept;
	static void printChunk(const ByteSpan& chunk) noexcept;
}

namespace waavs
{
	static inline uint64_t chunk_to_u64(ByteSpan& s) noexcept;
	static inline int64_t chunk_to_i64(ByteSpan& s) noexcept;

	// simple type parsing
	//static inline int64_t toInteger(const ByteSpan& inChunk) noexcept;
	//static inline double toNumber(const ByteSpan& inChunk) noexcept;
	//static inline double toDouble(const ByteSpan& inChunk) noexcept;
	static inline std::string toString(const ByteSpan& inChunk) noexcept;
	//static inline int toBoolInt(const ByteSpan& inChunk) noexcept;

	// Number Conversions
	static inline double chunk_to_double(const ByteSpan& inChunk) noexcept;
}

namespace waavs
{
	static inline size_t copy_to_cstr(char* str, size_t len, const ByteSpan& a) noexcept;
	static inline ByteSpan chunk_ltrim(const ByteSpan& a, const charset& skippable) noexcept;
	static inline ByteSpan chunk_rtrim(const ByteSpan& a, const charset& skippable) noexcept;
	static inline ByteSpan chunk_trim(const ByteSpan& a, const charset& skippable) noexcept;
	static inline ByteSpan chunk_skip_wsp(const ByteSpan& a) noexcept;


	static inline bool chunk_starts_with_char(const ByteSpan& a, const uint8_t b) noexcept;
	static inline bool chunk_starts_with_cstr(const ByteSpan& a, const char* b) noexcept;


	static INLINE bool chunk_ends_with_char(const ByteSpan& a, const uint8_t b) noexcept;
	static INLINE bool chunk_ends_with_cstr(const ByteSpan& a, const char* b) noexcept;

	static inline ByteSpan chunk_token(ByteSpan& a, const charset& delims) noexcept;
	static inline ByteSpan chunk_find_char(const ByteSpan& a, char c) noexcept;



}






namespace waavs
{
	static inline size_t copy_to_cstr(char* str, size_t len, const ByteSpan& a) noexcept
	{
		size_t maxBytes = a.size() < len ? a.size() : len;
		memcpy(str, a.fStart, maxBytes);
		str[maxBytes] = 0;

		return maxBytes;
	}

	// Trim the left side of skippable characters
	static inline ByteSpan chunk_ltrim(const ByteSpan& a, const charset& skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < a.fEnd && skippable(*start))
			++start;
		return { start, a.fEnd };
	}

	// trim the right side of skippable characters
	static inline ByteSpan chunk_rtrim(const ByteSpan& a, const charset& skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*(end - 1)))
			--end;

		return { start, end };
	}

	// trim the left and right side of skippable characters
	static inline ByteSpan chunk_trim(const ByteSpan& a, const charset& skippable) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && skippable(*start))
			++start;
		while (start < end && skippable(*(end - 1)))
			--end;
		return { start, end };
	}

	static inline ByteSpan chunk_skip_wsp(const ByteSpan& a) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end&& chrWspChars(*start))
			++start;
		return { start, end };
	}

	static INLINE ByteSpan chunk_skip_until_char(const ByteSpan& inChunk, const uint8_t achar) noexcept
	{
		const uint8_t* start = inChunk.fStart;
		const uint8_t* end = inChunk.fEnd;
		while (start < end && *start != achar)
			++start;
		
		return { start, end };
	}

	static INLINE bool chunk_starts_with(const ByteSpan& a, const ByteSpan& b) noexcept
	{
		return a.startsWith(b);
	}

	static INLINE bool chunk_starts_with_char(const ByteSpan& a, const uint8_t b) noexcept
	{
		return a.size() > 0 && a.fStart[0] == b;
	}

	static INLINE bool chunk_starts_with_cstr(const ByteSpan& a, const char* b) noexcept
	{
		return a.startsWith(ByteSpan(b));
	}

	static INLINE bool chunk_ends_with(const ByteSpan& a, const ByteSpan& b) noexcept
	{
		return a.endsWith(b);
	}

	static INLINE bool chunk_ends_with_char(const ByteSpan& a, const uint8_t b) noexcept
	{
		return ((a.size() > 0) && (a.fEnd[-1] == b));
	}

	static INLINE bool chunk_ends_with_cstr(const ByteSpan& a, const char* b) noexcept
	{
		return a.endsWith(chunk_from_cstr(b));
	}

	// Given an input chunk
	// spit it into two chunks, 
	// Returns - the first chunk before delimeters
	// a - adjusted to reflect the rest of the input after delims
	// If delimeter NOT found
	// returns the entire input chunk
	// and 'a' is set to an empty chunk
	static INLINE ByteSpan chunk_token_char(ByteSpan& a, const char delim) noexcept
	{
		if (!a) {
			a = {};
			return {};
		}

		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		const uint8_t* tokenEnd = start;
		while (tokenEnd < end && *tokenEnd != delim)
			++tokenEnd;

		if (*tokenEnd == delim)
		{
			a.fStart = tokenEnd + 1;
		}
		else {
			a.fStart = tokenEnd;
		}

		return { start, tokenEnd };
	}
	
	static INLINE ByteSpan chunk_token(ByteSpan& a, const charset& delims) noexcept
	{
		if (!a) {
			a = {};
			return {};
		}

		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		const uint8_t* tokenEnd = start;
		while (tokenEnd < end && !delims(*tokenEnd))
			++tokenEnd;

		if (delims(*tokenEnd))
		{
			a.fStart = tokenEnd + 1;
		}
		else {
			a.fStart = tokenEnd;
		}

		return { start, tokenEnd };
	}

	// name alias
	static INLINE ByteSpan nextToken(ByteSpan& a, const charset&& delims) noexcept
	{
		return chunk_token(a, delims);
	}

	// Given an input chunk
	// find the first instance of a specified character
	// return the chunk preceding the found character
	// or or the whole chunk of the character is not found
	static inline ByteSpan chunk_find_char(const ByteSpan& a, char c) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		while (start < end && *start != c)
			++start;

		return { start, end };
	}

	static INLINE ByteSpan chunk_find_cstr(const ByteSpan& a, const char* c) noexcept
	{
		const uint8_t* start = a.fStart;
		const uint8_t* end = a.fEnd;
		const uint8_t* cstart = (const uint8_t*)c;
		const uint8_t* cend = cstart + strlen(c);
		ByteSpan cChunk(cstart, cend);
		
		while (start < end)
		{
			if (*start == *cstart)
			{
				
				if (chunk_starts_with({ start, end }, cChunk))
				{
					break;
					//return { start, end };
				}
			}
			
			++start;
		}
		
		return { start, end };
	}

	static inline ByteSpan chunk_read_bracketed(ByteSpan& src, const uint8_t lbracket, const uint8_t rbracket) noexcept
	{
		uint8_t* beginattrValue = nullptr;
		uint8_t* endattrValue = nullptr;
		uint8_t quote{};

		// Skip white space before the quoted bytes
		src = chunk_ltrim(src, chrWspChars);

		if (!src || *src != lbracket)
			return {};


		// advance past the lbracket, then look for the matching close quote
		src++;
		beginattrValue = (uint8_t*)src.fStart;

		// Skip until end of the value.
		while (src && *src != rbracket)
			src++;

		if (src)
		{
			endattrValue = (uint8_t*)src.fStart;
			src++;
		}

		// Store only well formed quotes
		return { beginattrValue, endattrValue };
	}

	// Take a chunk containing a series of digits and turn
	// it into a 64-bit unsigned integer
	// Stop processing when the first non-digit is seen, 
	// or the end of the chunk
	// This routine alters the input chunk to reflect the remaining
	// characters after the number
	static inline uint64_t chunk_to_u64(ByteSpan& s) noexcept
	{
		static const std::bitset<256> decDigits = chrDecDigits.bits;
		
		uint64_t v = 0;

		while (s && decDigits[*s])
		{
			v = v * 10 + (uint64_t)(*s - '0');
			s++;
		}

		return v;
	}

	static inline int64_t chunk_to_i64(ByteSpan& s) noexcept
	{
		int64_t v = 0;

		bool negative = false;
		if (s && *s == '-')
		{
			negative = true;
			s++;
		}

		while (s && chrDecDigits(*s))
		{
			v = v * 10 + (int64_t)(*s - '0');
			s++;
		}

		if (negative)
			v = -v;

		return v;
	}



	//
	// chunk_to_double()
	// 
	// parse floating point number
	// includes sign, exponent, and decimal point
	// The input chunk is altered, with the fStart pointer moved to the end of the number
	// Note:  If we want to include "charconv", the we can use the std::from_chars
	// This should be a fast implementation, far batter than something like atof, or sscanf
	// But, the routine here should be universally fast, when charconv is not available on the 
	// target platform.
	//
			// Just put this from_chars implementation here in case
		// we ever want to use it instead
		//double outNumber = 0;
		//auto res = std::from_chars((const char*)s.fStart, (const char*)s.fEnd, outNumber);
		//if (res.ec == std::errc::invalid_argument)
		//{
		//	printf("chunk_to_double: INVALID ARGUMENT: ");
		//	printChunk(s);
		//}
		//return outNumber;
	
	static inline double chunk_to_double(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;

		double sign = 1.0;
		double res = 0.0;
		long long intPart = 0;
		uint64_t fracPart = 0;
		bool hasIntPart = false;
		bool hasFracPart = false;

		// Parse optional sign
		if (*s == '+') {
			s++;
		}
		else if (*s == '-') {
			sign = -1;
			s++;
		}

		// Parse integer part
		if (chrDecDigits[*s]) {

			intPart = chunk_to_u64(s);

			res = (double)intPart;
			hasIntPart = true;
		}

		// Parse fractional part.
		if (*s == '.') {
			s++; // Skip '.'
			auto sentinel = s.fStart;

			if (chrDecDigits(*s)) {
				fracPart = chunk_to_u64(s);
				auto ending = s.fStart;

				ptrdiff_t diff = ending - sentinel;
				res = res + ((double)fracPart) / (double)powd((double)10, double(diff));
				hasFracPart = true;
			}
		}

		// A valid number should have integer or fractional part.
		if (!hasIntPart && !hasFracPart)
			return 0.0;


		// Parse optional exponent
		if (*s == 'e' || *s == 'E') {
			long long expPart = 0;
			s++; // skip 'E'

			double expSign = 1.0;
			if (*s == '+') {
				s++;
			}
			else if (*s == '-') {
				expSign = -1.0;
				s++;
			}

			if (chrDecDigits[*s]) {
				expPart = chunk_to_u64(s);
				res = res * powd(10, double(expSign * double(expPart)));
			}
		}

		return res * sign;
		
	}
	
}


namespace waavs {
	/*
	static void writeChunkToFile(const ByteSpan& chunk, const char* filename) noexcept
	{
		FILE* f{};
		errno_t err = fopen_s(&f, filename, "wb");
		if ((err != 0) || (f == nullptr))
			return;

		fwrite(chunk.data(), 1, chunk.size(), f);
		fclose(f);
	}
	*/

	static void writeChunk(const ByteSpan& chunk) noexcept
	{
		ByteSpan s = chunk;

		while (s && *s) {
			printf("%c", *s);
			s++;
		}
	}

	static void writeChunkBordered(const ByteSpan& chunk) noexcept
	{
		ByteSpan s = chunk;

		printf("||");
		while (s && *s) {
			printf("%c", *s);
			s++;
		}
		printf("||");
	}

	static void printChunk(const ByteSpan& chunk) noexcept
	{
		if (chunk)
		{
			writeChunk(chunk);
			printf("\n");
		}
		else
			printf("BLANK==CHUNK\n");

	}
}

namespace waavs {
	
	static inline int64_t toInteger(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;
		return chunk_to_i64(s);
	}

	// toNumber
	// a floating point number
	static inline double toNumber(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;
		return chunk_to_double(s);
	}

	static inline double toDouble(const ByteSpan& s) noexcept
	{
		return chunk_to_double(s);

	}
	
		
	/*
	// return 1 if the chunk is "true" or "1" or "t" or "T" or "y" or "Y" or "yes" or "Yes" or "YES"
	// return 0 if the chunk is "false" or "0" or "f" or "F" or "n" or "N" or "no" or "No" or "NO"
	// return 0 otherwise
	static inline int toBoolInt(const ByteSpan& inChunk) noexcept
	{
		ByteSpan s = inChunk;

		if (s == "true" || s == "1" || s == "t" || s == "T" || s == "y" || s == "Y" || s == "yes" || s == "Yes" || s == "YES")
			return 1;
		else if (s == "false" || s == "0" || s == "f" || s == "F" || s == "n" || s == "N" || s == "no" || s == "No" || s == "NO")
			return 0;
		else
			return 0;
	}
	*/
	static inline std::string toString(const ByteSpan& inChunk) noexcept
	{
		if (!inChunk)
			return std::string();
		
		return std::string(inChunk.fStart, inChunk.fEnd);
	}

}

namespace waavs {
	//
	// MemBuff
	// 
	// This is a very simple data structure that allocates a chunk of memory
	// When the destructor is called, the memory is freed.
	// This could easily be handled by something like a unique_ptr, but I don't
	// want to force the usage of std library when it's not really needed.
	// besides, it's so easy and convenient and small.
	// Note:  This could be a sub-class of ByteSpan, but the semantics are different
	// With a ByteSpan, you can alter the start/end pointers, but with a memBuff, you can't.
	// so, it is much easier to return a ByteSpan, and let that be manipulated instead.
	// 
	
	struct MemBuff final 
	{
		uint8_t* fData{};
		ptrdiff_t fSize{};

		MemBuff() {}
		
		MemBuff(size_t sz)
		{
			initSize(sz);
		}

		~MemBuff()
		{
			if (fData != nullptr)
				delete[] fData;
		}

		uint8_t* data() const { return fData; }
		size_t size() const { return fSize; }

		// initSize
		// Initialize the memory buffer with a given size
		bool initSize(const size_t sz)
		{
			fData = new uint8_t[sz];
			fSize = sz;

			return true;
		}
		
		// initFromSpan
		// copy the data from the input span into the memory buffer
		//
		bool initFromSpan(const ByteSpan& srcSpan)
		{
			if (fData != nullptr)
				delete[] fData;
			
			fSize = srcSpan.size();
			fData = new uint8_t[fSize];

			memcpy(fData, srcSpan.fStart, fSize);
			
			return true;
		}
		

		// create a ByteSpan from the memory buffer
		// The lifetime of the ByteSpan that is returned it not governed
		// by the MemBuff object.  This is something the caller must manage.
		ByteSpan span() const { return ByteSpan(fData, fData + fSize); }

	};
}


