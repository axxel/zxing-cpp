// Copyright 2026 Axel Waggershauser
// SPDX-License-Identifier: Apache-2.0

package zxingcpp

/*
#include <stdlib.h>
#include "zxingcgo.h"
*/
import "C"

import (
	"fmt"
	"image"
	"runtime"
	"strings"
	"unsafe"
)

func CreateBarcode(src any, format BarcodeFormat, options ...string) (*Barcode, error) {
	opts := C.ZXing_CreatorOptions_new(C.ZXing_BarcodeFormat(format))
	if opts == nil {
		panic("ZXing_CreatorOptions_new failed") // only out of memory should cause this, so panic is reasonable
	}
	defer C.ZXing_CreatorOptions_delete(opts)

	if len(options) > 0 {
		cs := C.CString(strings.Join(options, ","))
		defer C.free(unsafe.Pointer(cs))
		C.ZXing_CreatorOptions_setOptions(opts, cs)
	}

	var bc *C.ZXing_Barcode
	switch data := src.(type) {
	case string:
		bc = C.ZXing_CreateBarcodeFromText((*C.char)(unsafe.Pointer(unsafe.StringData(data))), C.int(len(data)), opts)
		runtime.KeepAlive(data)
	case []byte:
		bc = C.ZXing_CreateBarcodeFromBytes(unsafe.Pointer(unsafe.SliceData(data)), C.int(len(data)), opts)
		runtime.KeepAlive(data)
	default:
		return nil, fmt.Errorf("unsupported source type %T; expected string or []byte", src)
	}

	if bc == nil {
		return nil, zxLastError()
	}
	return &Barcode{ptr: bc}, nil
}

// WriterOption is a functional option for configuring writer options.
type WriterOption func(*C.ZXing_WriterOptions)

// WithScale sets the scale factor for the output image.
func WithScale(v int) WriterOption {
	return func(o *C.ZXing_WriterOptions) { C.ZXing_WriterOptions_setScale(o, C.int(v)) }
}

// WithRotation sets the rotation angle (0, 90, 180, or 270 degrees).
func WithRotation(v int) WriterOption {
	return func(o *C.ZXing_WriterOptions) { C.ZXing_WriterOptions_setRotate(o, C.int(v)) }
}

// WithHRT sets whether to add Human Readable Text.
func WithHRT(v bool) WriterOption {
	return func(o *C.ZXing_WriterOptions) { C.ZXing_WriterOptions_setAddHRT(o, C.bool(v)) }
}

// WithQuietZones sets whether to add quiet zones around the barcode.
func WithQuietZones(v bool) WriterOption {
	return func(o *C.ZXing_WriterOptions) { C.ZXing_WriterOptions_setAddQuietZones(o, C.bool(v)) }
}

func createAndConfigureWriterOptions(opts ...WriterOption) *C.ZXing_WriterOptions {
	wo := C.ZXing_WriterOptions_new()
	if wo == nil {
		panic("failed to create WriterOptions") // only out of memory should cause this, so panic is reasonable
	}
	for _, opt := range opts {
		opt(wo)
	}
	return wo
}

func (b *Barcode) ToSVG(opts ...WriterOption) (string, error) {
	wo := createAndConfigureWriterOptions(opts...)
	defer C.ZXing_WriterOptions_delete(wo)

	s := C.ZXing_WriteBarcodeToSVG(b.ptr, wo)
	if s == nil {
		return "", zxLastError()
	}
	return zxString2Go(s), nil
}

func (b *Barcode) ToImage(opts ...WriterOption) (image.Image, error) {
	wo := createAndConfigureWriterOptions(opts...)
	defer C.ZXing_WriterOptions_delete(wo)

	return zxImage2Go(C.ZXing_WriteBarcodeToImage(b.ptr, wo))
}
