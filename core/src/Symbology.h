/*
 * Copyright 2026 Axel Waggershauser
 */
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Version.h"

#include <stdint.h>

// FLAGS[0]: l == linear, s == stacked, m == matrix
// FLAGS[1]: r == readable
// FLAGS[2]: w == writable (MultiformatWriter))
// FLAGS[3]: g == GS1

// clang-format off
//    NAME,              ID,   FLAGS, ZINT, ENABLED,                 HRI
#define ZX_SYMBOLOGY_LIST(X) \
	X(None,          "\0\0\0", "    ",   0, 1,                       "None") \
	X(Codabar,          "]F ", "lrw ",  18, ZXING_ENABLE_1D,         "Codabar") \
	X(Code39,           "]A ", "lrw ",   8, ZXING_ENABLE_1D,         "Code39") \
	X(PZN,              "]Ap", "lr  ",   8, ZXING_ENABLE_1D,         "Pharmazentralnummer") \
	X(Code93,           "]G ", "lrw ",  25, ZXING_ENABLE_1D,         "Code93") \
	X(Code128,          "]C ", "lrwg",  20, ZXING_ENABLE_1D,         "Code128") \
	X(ITF,              "]I ", "lrw ",   3, ZXING_ENABLE_1D,         "ITF") \
	X(DataBar,          "]e ", "lr  ",  29, ZXING_ENABLE_1D,         "DataBar") \
	X(DataBarOmD,       "]eo", "lr  ",  29, ZXING_ENABLE_1D,         "DataBar Omnidirectional") \
	X(DataBarLtd,       "]el", "lr  ",  30, ZXING_ENABLE_1D,         "DataBar Limited") \
	X(DataBarExp,       "]ee", "lr g",  31, ZXING_ENABLE_1D,         "DataBar Expanded") \
	X(EANUPC,           "]E ", "lr  ",  15, ZXING_ENABLE_1D,         "EAN/UPC") \
	X(EAN13,            "]E1", "lrw ",  15, ZXING_ENABLE_1D,         "EAN-13") \
	X(EAN8,             "]E8", "lrw ",  10, ZXING_ENABLE_1D,         "EAN-8") \
	X(UPCA,             "]Ea", "lrw ",  34, ZXING_ENABLE_1D,         "UPC-A") \
	X(UPCE,             "]Ee", "lrw ",  37, ZXING_ENABLE_1D,         "UPC-E") \
	X(DXFilmEdge,       "]Xx", "lr  ", 147, ZXING_ENABLE_1D,         "DX Film Edge") \
	X(PDF417,           "]L ", "srw ",  55, ZXING_ENABLE_PDF417,     "PDF417") \
	X(CompactPDF417,    "]Lc", "sr  ",  56, ZXING_ENABLE_PDF417,     "Compact PDF417") \
	X(MicroPDF417,      "]Lm", "s   ",  84, ZXING_ENABLE_PDF417,     "MicroPDF417") \
	X(Aztec,            "]z ", "mrwg",  92, ZXING_ENABLE_AZTEC,      "Aztec") \
	X(AztecRune,        "]zC", "mr  ", 128, ZXING_ENABLE_AZTEC,      "Aztec Rune") \
	X(QRCode,           "]Q ", "mrwg",  58, ZXING_ENABLE_QRCODE,     "QR Code") \
	X(MicroQRCode,      "]Qm", "mr  ",  97, ZXING_ENABLE_QRCODE,     "Micro QR Code") \
	X(RMQRCode,         "]Qr", "mr g", 145, ZXING_ENABLE_QRCODE,     "rMQR Code") \
	X(DataMatrix,       "]d ", "mrwg",  71, ZXING_ENABLE_DATAMATRIX, "Data Matrix") \
	X(MaxiCode,         "]U ", "mr  ",  57, ZXING_ENABLE_MAXICODE,   "MaxiCode") \
	/* Add new symbologies here */
// clang-format on

#define ZX_SYMBOLOGY_ID(ID) (((uint8_t)(ID[0]) << 0) | ((uint8_t)(ID[1]) << 8) | ((uint8_t)(ID[2]) << 16))

#ifdef __cplusplus

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ZXing {

enum class SymbologyID : unsigned int
{
#define DECLARE_SYM(NAME, ID, FLAGS, ZINT, ENABLED, HRI) NAME = ZX_SYMBOLOGY_ID(ID),
	ZX_SYMBOLOGY_LIST(DECLARE_SYM)
#undef DECLARE_SYM
};

class Symbology
{
	using ID = SymbologyID;
	ID id_ = SymbologyID::None;

	// friend class Symbologies;

	static std::string_view name(ID id);

public:
	enum Feature { Any = 0, Readable = 1, Creatable = 2, Linear = 4, Stacked = 8, Matrix = 16, GS1 = 32 };

	Symbology() = default;
	Symbology(std::string_view str);

	constexpr Symbology(ID id) : id_(id) {}

	ID id() const { return id_; }
	ID groupId() const;
	std::string_view idStr() const { return reinterpret_cast<const char*>(&id_); }
	std::string_view variant() const { return name(id_); } // TODO: call this name() and rename name() to group()?
	std::string_view name() const { return name(groupId()); }
	bool is(Feature f) const noexcept;

	template <typename... Args>
	bool is(SymbologyID sid, Args... args) const noexcept
	{
		return (id_ == sid) || ((id_ == args) || ...);
	}

	// Allow switch and comparisons.
	constexpr operator ID() const { return id_; }
	constexpr bool operator<(const Symbology& other) const
	{
		return idStr()[1] < other.idStr()[1] || (idStr()[1] == other.idStr()[1] && idStr()[2] < other.idStr()[2]);
	}

#define DECLARE_SYM(NAME, ID, FLAGS, ZINT, ENABLED, HRI) static const Symbology NAME;
	ZX_SYMBOLOGY_LIST(DECLARE_SYM)
#undef DECLARE_SYM
};

class Symbologies
{
	std::vector<Symbology> syms_;

	void normalize();

public:
	Symbologies() = default;
	Symbologies(Symbology s) : syms_{s} {}

	Symbologies(const Symbologies&) = default;
	Symbologies(Symbologies&&) = default;
	Symbologies& operator=(const Symbologies&) = default;
	Symbologies& operator=(Symbologies&&) = default;
	bool operator==(const Symbologies& o) const noexcept { return syms_ == o.syms_; }
	bool operator!=(const Symbologies& o) const noexcept { return !(*this == o); }
	// ~Symbologies() = default;

	Symbologies(std::vector<Symbology>&& syms) : syms_(std::move(syms)) { normalize(); }

	explicit Symbologies(std::string_view ids);

	static const Symbologies& all(Symbology::Feature op = Symbology::Any);

	auto begin() const noexcept { return syms_.cbegin(); }
	auto end() const noexcept { return syms_.cend(); }

	bool empty() const noexcept { return syms_.empty(); }
	int size() const noexcept { return syms_.size(); }

	bool contains(Symbology s) const noexcept;

	// inline void clear() noexcept { syms_.clear(); }

	Symbologies&& operator|(Symbology s) &&;
	Symbologies&& operator|(const Symbologies& other) &&;
	Symbologies operator&(const Symbologies& other);
};

inline Symbologies operator|(Symbology s1, Symbology s2)
{
	return Symbologies(std::vector{ s1, s2 });
}

std::string ToString(const Symbologies& syms);

} // namespace ZXing

#endif // __cplusplus
