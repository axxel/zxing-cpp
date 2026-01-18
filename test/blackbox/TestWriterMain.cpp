/*
* Copyright 2016 Nu-book Inc.
* Copyright 2019 Axel Waggershauser
*/
// SPDX-License-Identifier: Apache-2.0

#include "CreateBarcode.h"
#include "WriteBarcode.h"
#include "Version.h"

#include <vector>

using namespace ZXing;
using namespace std::literals;

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void savePng(ImageView iv, Symbology symbology)
{
	stbi_write_png((std::string(symbology.variant()) + ".png"s).c_str(), iv.width(), iv.height(), iv.pixStride(), iv.data(), iv.rowStride());
}

int main()
{
	std::string text = "http://www.google.com/";
	for (auto symbology : {
#ifdef ZXING_ENABLE_AZTEC
		Symbology::Aztec,
#endif
#ifdef ZXING_ENABLE_DATAMATRIX
		Symbology::DataMatrix,
#endif
#ifdef ZXING_ENABLE_PDF417
		Symbology::PDF417,
#endif
#ifdef ZXING_ENABLE_QRCODE
		Symbology::QRCode,
#endif
	})
	{
		savePng(CreateBarcodeFromText(text, symbology).symbol(), symbology);
	}

#ifdef ZXING_ENABLE_1D
	text = "012345678901234567890123456789";
	using SymSpecs = std::vector<std::pair<Symbology, size_t>>;
	for (const auto& [symbology, length] : SymSpecs({
//		{Symbology::Codabar, 0},
		{Symbology::Code39, 0},
		{Symbology::Code93, 0},
		{Symbology::Code128, 0},
		{Symbology::EAN8, 7},
		{Symbology::EAN13, 12},
		{Symbology::ITF, 0},
		{Symbology::UPCA, 11},
		{Symbology::UPCE, 7}
	}))
	{
		auto input = length > 0 ? text.substr(0, length) : text;
		savePng(WriteBarcodeToImage(CreateBarcodeFromText(input, symbology)), symbology);
	}
#endif
}
