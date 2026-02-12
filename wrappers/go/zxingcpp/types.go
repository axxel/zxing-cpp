// Copyright 2026 Axel Waggershauser
// SPDX-License-Identifier: Apache-2.0

package zxingcpp

/*
#include "zxingcgo.h"
*/
import "C"

// ImageFormat describes how pixel bytes are interpreted.
type ImageFormat = C.ZXing_ImageFormat

const (
	ImageFormatNone ImageFormat = C.ZXing_ImageFormat_None
	ImageFormatLum  ImageFormat = C.ZXing_ImageFormat_Lum
	ImageFormatLumA ImageFormat = C.ZXing_ImageFormat_LumA
	ImageFormatRGB  ImageFormat = C.ZXing_ImageFormat_RGB
	ImageFormatBGR  ImageFormat = C.ZXing_ImageFormat_BGR
	ImageFormatRGBA ImageFormat = C.ZXing_ImageFormat_RGBA
	ImageFormatARGB ImageFormat = C.ZXing_ImageFormat_ARGB
	ImageFormatBGRA ImageFormat = C.ZXing_ImageFormat_BGRA
	ImageFormatABGR ImageFormat = C.ZXing_ImageFormat_ABGR
)

// BarcodeFormat identifies a barcode format/symbology or a filter/group.
// Concrete values are generated in barcodeformat.go.
type BarcodeFormat C.ZXing_BarcodeFormat

type Binarizer C.ZXing_Binarizer

const (
	BinarizerLocalAverage    Binarizer = C.ZXing_Binarizer_LocalAverage
	BinarizerGlobalHistogram Binarizer = C.ZXing_Binarizer_GlobalHistogram
	BinarizerFixedThreshold  Binarizer = C.ZXing_Binarizer_FixedThreshold
	BinarizerBoolCast        Binarizer = C.ZXing_Binarizer_BoolCast
)

type EanAddOnSymbol C.ZXing_EanAddOnSymbol

const (
	EanAddOnSymbolIgnore  EanAddOnSymbol = C.ZXing_EanAddOnSymbol_Ignore
	EanAddOnSymbolRead    EanAddOnSymbol = C.ZXing_EanAddOnSymbol_Read
	EanAddOnSymbolRequire EanAddOnSymbol = C.ZXing_EanAddOnSymbol_Require
)

type TextMode C.ZXing_TextMode

const (
	TextModePlain   TextMode = C.ZXing_TextMode_Plain
	TextModeECI     TextMode = C.ZXing_TextMode_ECI
	TextModeHRI     TextMode = C.ZXing_TextMode_HRI
	TextModeEscaped TextMode = C.ZXing_TextMode_Escaped
	TextModeHex     TextMode = C.ZXing_TextMode_Hex
	TextModeHexECI  TextMode = C.ZXing_TextMode_HexECI
)

type ContentType C.ZXing_ContentType

const (
	ContentTypeText       ContentType = C.ZXing_ContentType_Text
	ContentTypeBinary     ContentType = C.ZXing_ContentType_Binary
	ContentTypeMixed      ContentType = C.ZXing_ContentType_Mixed
	ContentTypeGS1        ContentType = C.ZXing_ContentType_GS1
	ContentTypeISO15434   ContentType = C.ZXing_ContentType_ISO15434
	ContentTypeUnknownECI ContentType = C.ZXing_ContentType_UnknownECI
)

type ErrorType C.ZXing_ErrorType

const (
	ErrorTypeNone        ErrorType = C.ZXing_ErrorType_None
	ErrorTypeFormat      ErrorType = C.ZXing_ErrorType_Format
	ErrorTypeChecksum    ErrorType = C.ZXing_ErrorType_Checksum
	ErrorTypeUnsupported ErrorType = C.ZXing_ErrorType_Unsupported
)

type PointI struct {
	X C.int
	Y C.int
}

type Position struct {
	TopLeft     PointI
	TopRight    PointI
	BottomRight PointI
	BottomLeft  PointI
}

type BarcodeError struct {
	Type ErrorType
	Msg  string
}

func (e BarcodeError) Error() string {
	return e.Msg
}

func (ct ContentType) String() string {
	return zxString2Go(C.ZXing_ContentTypeToString(C.ZXing_ContentType(ct)))
}

func (p Position) String() string {
	cp := C.ZXing_Position{
		topLeft:     C.ZXing_PointI{x: p.TopLeft.X, y: p.TopLeft.Y},
		topRight:    C.ZXing_PointI{x: p.TopRight.X, y: p.TopRight.Y},
		bottomRight: C.ZXing_PointI{x: p.BottomRight.X, y: p.BottomRight.Y},
		bottomLeft:  C.ZXing_PointI{x: p.BottomLeft.X, y: p.BottomLeft.Y},
	}
	return zxString2Go(C.ZXing_PositionToString(cp))
}
