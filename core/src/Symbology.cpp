/*
 * Copyright 2026 Axel Waggershauser
 */
// SPDX-License-Identifier: Apache-2.0

#include "Symbology.h"

#include "Version.h"
#include "ZXAlgorithms.h"

#include <algorithm>
#include <format>

namespace ZXing {

// =================================== Symbology =================================

static_assert(ZX_SYMBOLOGY_ID("\0\0\0") == 0, "SymbologyID encoding error");

#define DEFINE_SYM(NAME, ID, FLAGS, ZINT, ENABLED, HRI) const Symbology Symbology::NAME{SymbologyID(ZX_SYMBOLOGY_ID(ID))};
ZX_SYMBOLOGY_LIST(DEFINE_SYM)
#undef DECLARE_SYM

Symbology::Symbology(std::string_view str)
{
    for (auto s : Symbologies::all()) {
        if (str == s.idStr() || IsEqualIgnoreCaseAnd(str, s.variant(), " -_/")) {
            id_ = s.id_;
            return;
        }
    }
}

std::string_view Symbology::name(ID id)
{
	switch (id) {
#define DECLARE_SYM_NAME_CASE(NAME, ID, FLAGS, ZINT, ENABLED, HRI) \
	case SymbologyID(ZX_SYMBOLOGY_ID(ID)): return HRI;
		ZX_SYMBOLOGY_LIST(DECLARE_SYM_NAME_CASE)
#undef DECLARE_SYM_NAME_CASE
	default: return "Unknown";
	};
}

Symbology::ID Symbology::groupId() const
{
	return id_ != SymbologyID::None ? ID((uint32_t(id_) & ZX_SYMBOLOGY_ID("\x00\xFF\x00")) | ZX_SYMBOLOGY_ID("]\x00 "))
									: SymbologyID::None;
}

bool Symbology::is(Feature f) const noexcept
{
#ifdef ZXING_USE_ZINT
#define USING_ZINT 1
#else
#define USING_ZINT 0
#endif

    switch (id_) {
#define DECLARE_SYM_NAME_CASE(NAME, ID, FLAGS, ZINT, ENABLED, HRI) \
	case SymbologyID(ZX_SYMBOLOGY_ID(ID)): \
		return ENABLED && (f & Readable) && FLAGS[1] == 'r' \
			   || ENABLED && (f & Creatable) && (USING_ZINT ? ZINT != 0 : FLAGS[2] == 'w') \
			   || (f & Linear) && FLAGS[0] == 'l' || (f & Stacked) && FLAGS[0] == 's' || (f & Matrix) && FLAGS[0] == 'm' \
			   || (f & GS1) && FLAGS[2] == 'g';
		ZX_SYMBOLOGY_LIST(DECLARE_SYM_NAME_CASE)
#undef DECLARE_SYM_NAME_CASE
	default: return false;
	};
}

// ==================================== Symbologies =================================

void Symbologies::normalize()
{
	std::sort(syms_.begin(), syms_.end());
	syms_.erase(std::unique(syms_.begin(), syms_.end()), syms_.end());
}

Symbologies::Symbologies(std::string_view str)
{
	while (str.size() >= 3 && str[0] == ']') {
		syms_.push_back(SymbologyID(ZX_SYMBOLOGY_ID(str)));
        str.remove_prefix(3);
	}

    ForEachToken(TrimWS(str, " []"), ",|", [this](std::string_view token) {
		if (!token.empty()) {
			auto s = Symbology(token);
			if (s == Symbology::None)
				throw std::invalid_argument("This is not a valid barcode symbology: '" + std::string(token) + "'");
			syms_.push_back(s);
		}
	});

    normalize();
}

const Symbologies& Symbologies::all(Symbology::Feature feature)
{
    auto addIf = [](std::vector<Symbology>& res, const char* id, bool pred) {
        if (pred)
            res.push_back(SymbologyID(ZX_SYMBOLOGY_ID(id)));
    };

	static const auto creatable = [addIf] {
		std::vector<Symbology> res;
#ifdef ZXING_WRITERS
		res.reserve(100);
#ifdef ZXING_USE_ZINT
#define DECLARE_SYM_ARRAY(NAME, ID, FLAGS, ZINT, ENABLED, HRI) addIf(res, ID, ENABLED && ZINT != 0);
#else
#define DECLARE_SYM_ARRAY(NAME, ID, FLAGS, ZINT, ENABLED, HRI) addIf(res, ID, ENABLED && FLAGS[2] == 'w');
#endif // ZXING_USE_ZINT
		ZX_SYMBOLOGY_LIST(DECLARE_SYM_ARRAY)
#undef DECLARE_SYM_ARRAY
#endif // ZXING_WRITERS
		return Symbologies(std::move(res));
	}();

	static const auto readable = [addIf] {
		std::vector<Symbology> res;
#ifdef ZXING_READERS
		res.reserve(100);
#define DECLARE_SYM_ARRAY(NAME, ID, FLAGS, ZINT, ENABLED, HRI) addIf(res, ID, ENABLED && FLAGS[1] == 'r');
		ZX_SYMBOLOGY_LIST(DECLARE_SYM_ARRAY)
#undef DECLARE_SYM_ARRAY
#endif // ZXING_READERS
		return Symbologies(std::move(res));
	}();

	static const auto linear = [addIf] {
		std::vector<Symbology> res;
        res.reserve(50);
#define DECLARE_SYM_ARRAY(NAME, ID, FLAGS, ZINT, ENABLED, HRI) addIf(res, ID, FLAGS[0] == 'l');
		ZX_SYMBOLOGY_LIST(DECLARE_SYM_ARRAY)
#undef DECLARE_SYM_ARRAY
		return Symbologies(std::move(res));
	}();

	static const auto either = Symbologies(creatable) | readable;
    static const auto none = Symbologies();

	switch (feature) {
	case Symbology::Any: return either;
	case Symbology::Readable: return readable;
	case Symbology::Creatable: return creatable;
	case Symbology::Linear: return linear;
	default: break; // TODO...
	}

	return none;
}

bool Symbologies::contains(Symbology s) const noexcept
{
	return std::find(begin(), end(), s) != end();
}

Symbologies&& Symbologies::operator|(Symbology s) &&
{
	syms_.push_back(s);
	normalize();
	return std::move(*this);
}

Symbologies&& Symbologies::operator|(const Symbologies& other) &&
{
	syms_.insert(std::end(syms_), other.begin(), other.end());
	normalize();
	return std::move(*this);
}

Symbologies Symbologies::operator&(const Symbologies& other)
{
	std::vector<Symbology> res;
	std::set_intersection(begin(), end(), other.begin(), other.end(), std::back_inserter(res));
	return res;
}

std::string ToString(const Symbologies& syms)
{
	if (syms.empty())
		return {};
	std::string res;
	for (auto s : syms)
		res += std::string(s.variant()) + "|";
	return res.substr(0, res.size() - 1);
}

} // namespace ZXing
