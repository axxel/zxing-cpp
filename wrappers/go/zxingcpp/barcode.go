// Copyright 2026 Axel Waggershauser
// SPDX-License-Identifier: Apache-2.0

package zxingcpp

/*
#include <stdlib.h>
#include "zxingcgo.h"
*/
import "C"

import (
	"unsafe"
)

type Barcode struct {
	ptr *C.ZXing_Barcode
}

func (b *Barcode) Close() error {
	if b == nil || b.ptr == nil {
		return nil
	}
	C.ZXing_Barcode_delete(b.ptr)
	b.ptr = nil
	return nil
}

func (b *Barcode) IsValid() bool            { return b != nil && bool(C.ZXing_Barcode_isValid(b.ptr)) }
func (b *Barcode) Format() BarcodeFormat    { return BarcodeFormat(C.ZXing_Barcode_format(b.ptr)) }
func (b *Barcode) Symbology() BarcodeFormat { return BarcodeFormat(C.ZXing_Barcode_symbology(b.ptr)) }
func (b *Barcode) ContentType() ContentType { return ContentType(C.ZXing_Barcode_contentType(b.ptr)) }
func (b *Barcode) Text() string             { return zxString2Go(C.ZXing_Barcode_text(b.ptr)) }

func (b *Barcode) Bytes() []byte {
	return zxBytes2Go(func(n *C.int) *C.uint8_t { return C.ZXing_Barcode_bytes(b.ptr, n) })
}
func (b *Barcode) BytesECI() []byte {
	return zxBytes2Go(func(n *C.int) *C.uint8_t { return C.ZXing_Barcode_bytesECI(b.ptr, n) })
}

func (b *Barcode) SymbologyIdentifier() string {
	return zxString2Go(C.ZXing_Barcode_symbologyIdentifier(b.ptr))
}

func (b *Barcode) Position() Position {
	p := C.ZXing_Barcode_position(b.ptr)
	return Position{
		TopLeft:     PointI{X: p.topLeft.x, Y: p.topLeft.y},
		TopRight:    PointI{X: p.topRight.x, Y: p.topRight.y},
		BottomRight: PointI{X: p.bottomRight.x, Y: p.bottomRight.y},
		BottomLeft:  PointI{X: p.bottomLeft.x, Y: p.bottomLeft.y},
	}
}

func (b *Barcode) Orientation() int   { return int(C.ZXing_Barcode_orientation(b.ptr)) }
func (b *Barcode) HasECI() bool       { return bool(C.ZXing_Barcode_hasECI(b.ptr)) }
func (b *Barcode) IsInverted() bool   { return bool(C.ZXing_Barcode_isInverted(b.ptr)) }
func (b *Barcode) IsMirrored() bool   { return bool(C.ZXing_Barcode_isMirrored(b.ptr)) }
func (b *Barcode) LineCount() int     { return int(C.ZXing_Barcode_lineCount(b.ptr)) }
func (b *Barcode) SequenceIndex() int { return int(C.ZXing_Barcode_sequenceIndex(b.ptr)) }
func (b *Barcode) SequenceSize() int  { return int(C.ZXing_Barcode_sequenceSize(b.ptr)) }
func (b *Barcode) SequenceID() string { return zxString2Go(C.ZXing_Barcode_sequenceId(b.ptr)) }

func (b *Barcode) Error() BarcodeError {
	return BarcodeError{Type: ErrorType(C.ZXing_Barcode_errorType(b.ptr)), Msg: zxString2Go(C.ZXing_Barcode_errorMsg(b.ptr))}
}

// Extra returns additional format-specific metadata.
//
// - If called with no arguments, it returns the full JSON metadata object.
// - If called with one key, it returns the value for that key.
func (b *Barcode) Extra(key ...string) string {
	var ckey *C.char
	if len(key) > 0 {
		ckey = C.CString(key[0])
		defer C.free(unsafe.Pointer(ckey))
	}
	return zxString2Go(C.ZXing_Barcode_extra(b.ptr, ckey))
}
