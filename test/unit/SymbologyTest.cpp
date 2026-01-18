/*
 * Copyright 2020 Axel Waggershauser
 */
// SPDX-License-Identifier: Apache-2.0

#include "Symbology.h"

#include "Version.h"

#include "gtest/gtest.h"
#include <stdexcept>
#include <vector>

using namespace ZXing;

TEST(SymbologyTest, Symbology)
{
	EXPECT_EQ(Symbology::None.id(), SymbologyID::None);
	EXPECT_EQ(Symbology::None, SymbologyID::None);
	EXPECT_EQ(Symbology::None, SymbologyID(0));
	EXPECT_EQ(Symbology::None.name(), "None");

#if ZXING_ENABLE_1D
	EXPECT_EQ(Symbology::DataBarLtd.name(), "DataBar");
	EXPECT_EQ(Symbology::DataBarLtd.variant(), "DataBar Limited");
	EXPECT_EQ(ToString(Symbology::EAN13 | Symbology::DataBarLtd), "EAN-13|DataBar Limited");

	EXPECT_EQ(Symbology::EAN8, Symbology("EAN_8"));
	EXPECT_EQ(Symbology::EAN8, Symbology("EAN-8"));
	EXPECT_EQ(Symbology::EAN8, Symbology("EAN8"));
	EXPECT_EQ(Symbology::EAN8, Symbology("ean8"));
	EXPECT_EQ(Symbology::None, Symbology("invalid-string"));

	EXPECT_TRUE(Symbologies("").empty());

	auto syms = Symbology::EAN8 | Symbology::ITF;
	EXPECT_EQ(syms, Symbologies("EAN-8,ITF"));
	EXPECT_EQ(syms, Symbologies("EAN-8, ITF"));
	EXPECT_EQ(syms, Symbologies("ean8|itf"));
	EXPECT_EQ(syms, Symbologies("[EAN-8,, ITF]"));

	auto s1 = std::vector<Symbology>(syms.begin(), syms.end());
	auto s2 = std::vector<Symbology>{Symbology::EAN8, Symbology::ITF};
	EXPECT_EQ(s1, s2);
#endif

	EXPECT_THROW(Symbologies("ITF, invalid-string"), std::invalid_argument);
}
