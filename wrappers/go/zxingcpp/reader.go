// Copyright 2026 Axel Waggershauser
// SPDX-License-Identifier: Apache-2.0

package zxingcpp

/*
#include "zxingcgo.h"
*/
import "C"

import (
	"fmt"
	"image"
	"runtime"
	"unsafe"
)

type ReaderOptions struct {
	ptr *C.ZXing_ReaderOptions
}

func NewReaderOptions() *ReaderOptions {
	p := C.ZXing_ReaderOptions_new()
	if p == nil {
		panic("ZXing_ReaderOptions_new failed")
	}
	return &ReaderOptions{ptr: p}
}

func (o *ReaderOptions) Close() error {
	if o == nil || o.ptr == nil {
		return nil
	}
	C.ZXing_ReaderOptions_delete(o.ptr)
	o.ptr = nil
	return nil
}

func (o *ReaderOptions) SetTryHarder(v bool) { C.ZXing_ReaderOptions_setTryHarder(o.ptr, C.bool(v)) }
func (o *ReaderOptions) TryHarder() bool     { return bool(C.ZXing_ReaderOptions_getTryHarder(o.ptr)) }

func (o *ReaderOptions) SetTryRotate(v bool) { C.ZXing_ReaderOptions_setTryRotate(o.ptr, C.bool(v)) }
func (o *ReaderOptions) TryRotate() bool     { return bool(C.ZXing_ReaderOptions_getTryRotate(o.ptr)) }

func (o *ReaderOptions) SetTryInvert(v bool) { C.ZXing_ReaderOptions_setTryInvert(o.ptr, C.bool(v)) }
func (o *ReaderOptions) TryInvert() bool     { return bool(C.ZXing_ReaderOptions_getTryInvert(o.ptr)) }

func (o *ReaderOptions) SetTryDownscale(v bool) {
	C.ZXing_ReaderOptions_setTryDownscale(o.ptr, C.bool(v))
}
func (o *ReaderOptions) TryDownscale() bool {
	return bool(C.ZXing_ReaderOptions_getTryDownscale(o.ptr))
}

func (o *ReaderOptions) SetIsPure(v bool) { C.ZXing_ReaderOptions_setIsPure(o.ptr, C.bool(v)) }
func (o *ReaderOptions) IsPure() bool     { return bool(C.ZXing_ReaderOptions_getIsPure(o.ptr)) }

func (o *ReaderOptions) SetValidateOptionalChecksum(v bool) {
	C.ZXing_ReaderOptions_setValidateOptionalChecksum(o.ptr, C.bool(v))
}
func (o *ReaderOptions) ValidateOptionalChecksum() bool {
	return bool(C.ZXing_ReaderOptions_getValidateOptionalChecksum(o.ptr))
}

func (o *ReaderOptions) SetReturnErrors(v bool) {
	C.ZXing_ReaderOptions_setReturnErrors(o.ptr, C.bool(v))
}
func (o *ReaderOptions) ReturnErrors() bool {
	return bool(C.ZXing_ReaderOptions_getReturnErrors(o.ptr))
}

func (o *ReaderOptions) SetMinLineCount(n int) {
	C.ZXing_ReaderOptions_setMinLineCount(o.ptr, C.int(n))
}
func (o *ReaderOptions) MinLineCount() int { return int(C.ZXing_ReaderOptions_getMinLineCount(o.ptr)) }

func (o *ReaderOptions) SetMaxNumberOfSymbols(n int) {
	C.ZXing_ReaderOptions_setMaxNumberOfSymbols(o.ptr, C.int(n))
}
func (o *ReaderOptions) MaxNumberOfSymbols() int {
	return int(C.ZXing_ReaderOptions_getMaxNumberOfSymbols(o.ptr))
}

func (o *ReaderOptions) SetBinarizer(b Binarizer) {
	C.ZXing_ReaderOptions_setBinarizer(o.ptr, C.ZXing_Binarizer(b))
}
func (o *ReaderOptions) Binarizer() Binarizer {
	return Binarizer(C.ZXing_ReaderOptions_getBinarizer(o.ptr))
}

func (o *ReaderOptions) SetEanAddOnSymbol(v EanAddOnSymbol) {
	C.ZXing_ReaderOptions_setEanAddOnSymbol(o.ptr, C.ZXing_EanAddOnSymbol(v))
}
func (o *ReaderOptions) EanAddOnSymbol() EanAddOnSymbol {
	return EanAddOnSymbol(C.ZXing_ReaderOptions_getEanAddOnSymbol(o.ptr))
}

func (o *ReaderOptions) SetTextMode(v TextMode) {
	C.ZXing_ReaderOptions_setTextMode(o.ptr, C.ZXing_TextMode(v))
}
func (o *ReaderOptions) TextMode() TextMode {
	return TextMode(C.ZXing_ReaderOptions_getTextMode(o.ptr))
}

func (o *ReaderOptions) SetFormats(formats ...BarcodeFormat) {
	ptr := (*C.ZXing_BarcodeFormat)(unsafe.Pointer(unsafe.SliceData(formats)))
	C.ZXing_ReaderOptions_setFormats(o.ptr, ptr, C.int(len(formats)))
	runtime.KeepAlive(formats)
}

func (o *ReaderOptions) Formats() []BarcodeFormat {
	var count C.int
	ptr := C.ZXing_ReaderOptions_getFormats(o.ptr, &count)
	return zxBarcodeFormats2Go(ptr, count)
}

func readBarcodesFromImageView(iv *ImageView, opts *ReaderOptions) ([]*Barcode, error) {
	if iv == nil || iv.ptr == nil {
		return nil, fmt.Errorf("iv is nil")
	}
	var optsPtr *C.ZXing_ReaderOptions
	if opts != nil {
		optsPtr = opts.ptr
	}

	res := C.ZXing_ReadBarcodes(iv.ptr, optsPtr)
	if res == nil {
		return nil, zxLastError()
	}

	size := int(C.ZXing_Barcodes_size(res))
	if size <= 0 {
		return nil, nil
	}

	defer C.ZXing_Barcodes_delete(res)

	out := make([]*Barcode, 0, size)
	for i := range size {
		bc := C.ZXing_Barcodes_move(res, C.int(i))
		if bc == nil {
			return nil, zxLastError("failed to move barcode")
		}
		out = append(out, &Barcode{ptr: bc})
	}
	return out, nil
}

// ReadBarcodes scans for barcodes from either a *ImageView or a standard image.Image.
//
// Supported inputs:
//   - *ImageView
//   - image.Image (implicitly converted via NewImageViewFromImage)
//
// The opts parameter is optional. Call ReadBarcodes(src) for defaults,
// or ReadBarcodes(src, opts) to provide ReaderOptions.
func ReadBarcodes(src any, opts ...*ReaderOptions) ([]*Barcode, error) {
	var ro *ReaderOptions
	if len(opts) > 0 {
		ro = opts[0]
	}

	switch v := src.(type) {
	case *ImageView:
		return readBarcodesFromImageView(v, ro)
	case image.Image:
		iv, err := NewImageViewFromImage(v)
		if err != nil {
			return nil, err
		}
		defer iv.Close()
		return readBarcodesFromImageView(iv, ro)
	default:
		return nil, fmt.Errorf("unsupported source type %T; expected *ImageView or image.Image", src)
	}
}
