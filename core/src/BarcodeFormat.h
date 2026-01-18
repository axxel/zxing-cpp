/*
* Copyright 2016 Nu-book Inc.
* Copyright 2016 ZXing authors
*/
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flags.h"
#include "Symbology.h"

#include <string>
#include <string_view>

namespace ZXing {

/**
* Enumerates barcode formats known to this package.
*/
enum class BarcodeFormat
{
	// The values are an implementation detail. The c++ use-case (ZXing::Flags) could have been designed such that it
	// would not have been necessary to explicitly set the values to single bit constants. This has been done to ease
	// the interoperability with C-like interfaces, the python and the Qt wrapper.
	None            = 0,         ///< Used as a return value if no valid barcode has been detected
	Aztec           = (1 << 0),  ///< Aztec
	Codabar         = (1 << 1),  ///< Codabar
	Code39          = (1 << 2),  ///< Code39
	Code93          = (1 << 3),  ///< Code93
	Code128         = (1 << 4),  ///< Code128
	DataBar         = (1 << 5),  ///< GS1 DataBar, formerly known as RSS 14
	DataBarExpanded = (1 << 6),  ///< GS1 DataBar Expanded, formerly known as RSS EXPANDED
	DataMatrix      = (1 << 7),  ///< DataMatrix
	EAN8            = (1 << 8),  ///< EAN-8
	EAN13           = (1 << 9),  ///< EAN-13
	ITF             = (1 << 10), ///< ITF (Interleaved Two of Five)
	MaxiCode        = (1 << 11), ///< MaxiCode
	PDF417          = (1 << 12), ///< PDF417
	QRCode          = (1 << 13), ///< QR Code
	UPCA            = (1 << 14), ///< UPC-A
	UPCE            = (1 << 15), ///< UPC-E
	MicroQRCode     = (1 << 16), ///< Micro QR Code
	RMQRCode        = (1 << 17), ///< Rectangular Micro QR Code
	DXFilmEdge      = (1 << 18), ///< DX Film Edge Barcode
	DataBarLimited  = (1 << 19), ///< GS1 DataBar Limited

	LinearCodes = Codabar | Code39 | Code93 | Code128 | EAN8 | EAN13 | ITF | DataBar | DataBarExpanded | DataBarLimited
				  | DXFilmEdge | UPCA | UPCE,
	MatrixCodes = Aztec | DataMatrix | MaxiCode | PDF417 | QRCode | MicroQRCode | RMQRCode,
	Any         = LinearCodes | MatrixCodes,

	_max = DataBarLimited, ///> implementation detail, don't use
};

ZX_DECLARE_FLAGS(BarcodeFormats, BarcodeFormat)

std::string ToString(BarcodeFormat format);
std::string ToString(BarcodeFormats formats);

/**
 * @brief Parse a string into a BarcodeFormat. '-' and '_' are optional.
 * @return None if str can not be parsed as a valid enum value
 */
BarcodeFormat BarcodeFormatFromString(std::string_view str);

/**
 * @brief Parse a string into a set of BarcodeFormats.
 * Separators can be (any combination of) '|', ',' or ' '.
 * Underscores are optional and input can be lower case.
 * e.g. "EAN-8 qrcode, Itf" would be parsed into [EAN8, QRCode, ITF].
 * @throws std::invalid_parameter if the string can not be fully parsed.
 */
BarcodeFormats BarcodeFormatsFromString(std::string_view str);

inline Symbology BarcodeFormatToSymbology(BarcodeFormat format)
{
	switch (format) {
	case BarcodeFormat::Aztec: return Symbology::Aztec;
	case BarcodeFormat::Codabar: return Symbology::Codabar;
	case BarcodeFormat::Code128 : return Symbology::Code128;
	case BarcodeFormat::Code39 : return Symbology::Code39;
	case BarcodeFormat::Code93 : return Symbology::Code93;
	case BarcodeFormat::DataBar : return Symbology::DataBar;
	case BarcodeFormat::DataBarExpanded : return Symbology::DataBarExp;
	case BarcodeFormat::DataBarLimited : return Symbology::DataBarLtd;
	case BarcodeFormat::DataMatrix: return Symbology::DataMatrix;
	case BarcodeFormat::DXFilmEdge: return Symbology::DXFilmEdge;
	case BarcodeFormat::EAN13: return Symbology::EAN13;
	case BarcodeFormat::EAN8: return Symbology::EAN8;
	case BarcodeFormat::ITF: return Symbology::ITF;
	case BarcodeFormat::MaxiCode: return Symbology::MaxiCode;
	case BarcodeFormat::MicroQRCode: return Symbology::MicroQRCode;
	case BarcodeFormat::PDF417: return Symbology::PDF417;
	case BarcodeFormat::QRCode: return Symbology::QRCode;
	case BarcodeFormat::RMQRCode: return Symbology::RMQRCode;
	case BarcodeFormat::UPCA: return Symbology::UPCA;
	case BarcodeFormat::UPCE: return Symbology::UPCE;
	default: return Symbology::None;
	}
}

inline BarcodeFormat BarcodeFormatFromSymbology(Symbology symbology)
{
	switch (symbology) {
	case SymbologyID::Aztec: return BarcodeFormat::Aztec;
	case SymbologyID::Codabar: return BarcodeFormat::Codabar;
	case SymbologyID::Code128: return BarcodeFormat::Code128;
	case SymbologyID::Code39: return BarcodeFormat::Code39;
	case SymbologyID::Code93: return BarcodeFormat::Code93;
	case SymbologyID::DataBar: return BarcodeFormat::DataBar;
	case SymbologyID::DataBarExp: return BarcodeFormat::DataBarExpanded;
	case SymbologyID::DataBarLtd: return BarcodeFormat::DataBarLimited;
	case SymbologyID::DataMatrix: return BarcodeFormat::DataMatrix;
	case SymbologyID::DXFilmEdge: return BarcodeFormat::DXFilmEdge;
	case SymbologyID::EAN13: return BarcodeFormat::EAN13;
	case SymbologyID::EAN8: return BarcodeFormat::EAN8;
	case SymbologyID::ITF: return BarcodeFormat::ITF;
	case SymbologyID::MaxiCode: return BarcodeFormat::MaxiCode;
	case SymbologyID::MicroQRCode: return BarcodeFormat::MicroQRCode;
	case SymbologyID::PDF417: return BarcodeFormat::PDF417;
	case SymbologyID::QRCode: return BarcodeFormat::QRCode;
	case SymbologyID::RMQRCode: return BarcodeFormat::RMQRCode;
	case SymbologyID::UPCA: return BarcodeFormat::UPCA;
	case SymbologyID::UPCE: return BarcodeFormat::UPCE;
	default: return BarcodeFormat::None;
	}
}

} // ZXing
