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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <stddef.h>
#include <stdio.h>

#include <jpeglib.h>
#include <jerror.h>

#include <dromozoa/bind.hpp>

namespace dromozoa {
  void error_exit(const char* what);
  void error_exit(j_common_ptr cinfo);

  class decompressor_handle_impl;

  class decompressor_handle {
  public:
    static decompressor_handle_impl* create(size_t buffer_size);
    explicit decompressor_handle(decompressor_handle_impl* impl);
    ~decompressor_handle();
    void destroy();
    void set_output_message(lua_State* L, int index);
    void set_fill_input_buffer(lua_State* L, int index);
    j_decompress_ptr get();
    JSAMPARRAY prepare_scanlines(JDIMENSION height, size_t samples_per_row);
  private:
    scoped_ptr<decompressor_handle_impl> impl_;
    decompressor_handle(const decompressor_handle&);
    decompressor_handle& operator=(const decompressor_handle&);
  };

  class compressor_handle_impl;

  class compressor_handle {
  public:
    static compressor_handle_impl* create(size_t buffer_size);
    explicit compressor_handle(compressor_handle_impl* impl);
    ~compressor_handle();
    void destroy();
    void set_output_message(lua_State* L, int index);
    void set_empty_output_buffer(lua_State* L, int index);
    j_compress_ptr get();
    JSAMPARRAY prepare_rows(JDIMENSION height, size_t samples_per_row);
  private:
    scoped_ptr<compressor_handle_impl> impl_;
    compressor_handle(const compressor_handle&);
    compressor_handle& operator=(const compressor_handle&);
  };
}

#endif
