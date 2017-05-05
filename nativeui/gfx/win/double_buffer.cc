// Copyright 2017 Cheng Zhao. All rights reserved.
// Use of this source code is governed by the license that can be found in the
// LICENSE file.

#include "nativeui/gfx/win/double_buffer.h"

namespace nu {

namespace {

HBITMAP CreateBitmap(HDC dc, const Size& size, bool use_dib_bitmap) {
  if (use_dib_bitmap) {
    BITMAPINFOHEADER bih = { 0 };
    bih.biBitCount = 32;
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = size.width();
    bih.biHeight = size.height();
    bih.biPlanes = 1;
    bih.biSizeImage = size.width() * size.height() * 4;
    bih.biCompression = BI_RGB;
    RGBQUAD* bits = nullptr;
    return ::CreateDIBSection(dc, reinterpret_cast<BITMAPINFO*>(&bih), 0,
                              reinterpret_cast<void**>(&bits), NULL, 0);
  } else {
    return ::CreateCompatibleBitmap(dc, size.width(), size.height());
  }
}

}  // namespace

DoubleBuffer::DoubleBuffer(HDC dc, const Size& size, const Rect& src,
                         const Point& dest, bool use_dib_bitmap)
    : dc_(dc), src_(src), dest_(dest),
      mem_dc_(::CreateCompatibleDC(dc)),
      mem_bitmap_(CreateBitmap(dc, size, use_dib_bitmap)),
      select_bitmap_(mem_dc_.Get(), mem_bitmap_.get()) {
}

DoubleBuffer::~DoubleBuffer() {
  if (copy_on_destruction_) {
    // Transfer the off-screen DC to the screen.
    BitBlt(dc_, src_.x(), src_.y(), src_.width(), src_.height(),
           dc(), dest_.x(), dest_.y(), SRCCOPY);
  }
}

}  // namespace nu