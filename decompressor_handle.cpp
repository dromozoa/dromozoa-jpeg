// Copyright (C) 2018 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-jpeg.
//
// dromozoa-jpeg is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-jpeg is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-jpeg.  If not, see <http://www.gnu.org/licenses/>.

#include "common.hpp"

namespace dromozoa {
  class decompressor_handle_impl {
  public:
    decompressor_handle_impl() : cinfo_(), err_(), src_() {
      cinfo_.err = jpeg_std_error(&err_);
      cinfo_.client_data = this;
      jpeg_create_decompress(&cinfo_);
    }

    ~decompressor_handle_impl() {
      jpeg_destroy_decompress(&cinfo_);
    }

  private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr err_;
    jpeg_source_mgr src_;

    decompressor_handle_impl(const decompressor_handle_impl&);
    decompressor_handle_impl& operator=(const decompressor_handle_impl&);
  };

  decompressor_handle::decompressor_handle() : impl_(new decompressor_handle_impl()) {}

  decompressor_handle::~decompressor_handle() {}
}
