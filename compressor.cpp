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

#include <string.h>

#include <algorithm>

#include "common.hpp"

namespace dromozoa {
  namespace {
    compressor_handle* check_compressor_handle(lua_State* L, int arg) {
      return luaX_check_udata<compressor_handle>(L, arg, "dromozoa.jpeg.compressor");
    }

    j_compress_ptr check_compressor(lua_State* L, int arg) {
      return check_compressor_handle(L, arg)->get();
    }

    void impl_gc(lua_State* L) {
      check_compressor_handle(L, 1)->~compressor_handle();
    }

    void impl_call(lua_State* L) {
      size_t buffer_size = luaX_opt_integer<size_t>(L, 2, 4096);
      luaX_new<compressor_handle>(L, compressor_handle::create(buffer_size));
      luaX_set_metatable(L, "dromozoa.jpeg.compressor");
    }

    void impl_destroy(lua_State* L) {
      check_compressor_handle(L, 1)->destroy();
      luaX_push_success(L);
    }

    void impl_set_output_message(lua_State* L) {
      check_compressor_handle(L, 1)->set_output_message(L, 2);
      luaX_push_success(L);
    }

    void impl_set_empty_output_buffer(lua_State* L) {
      check_compressor_handle(L, 1)->set_empty_output_buffer(L, 2);
      luaX_push_success(L);
    }

    void impl_set_image_width(lua_State* L) {
      check_compressor(L, 1)->image_width = luaX_check_integer<JDIMENSION>(L, 2);
      luaX_push_success(L);
    }

    void impl_set_image_height(lua_State* L) {
      check_compressor(L, 1)->image_height = luaX_check_integer<JDIMENSION>(L, 2);
      luaX_push_success(L);
    }

    void impl_set_input_components(lua_State* L) {
      check_compressor(L, 1)->input_components = luaX_check_enum<int>(L, 2);
      luaX_push_success(L);
    }

    void impl_set_in_color_space(lua_State* L) {
      check_compressor(L, 1)->in_color_space = luaX_check_enum<J_COLOR_SPACE>(L, 2);
      luaX_push_success(L);
    }

    void impl_set_defaults(lua_State* L) {
      jpeg_set_defaults(check_compressor(L, 1));
      luaX_push_success(L);
    }

    void impl_set_row(lua_State* L) {
      compressor_handle* self = check_compressor_handle(L, 1);
      JDIMENSION height = self->get()->image_height;
      JDIMENSION y = luaX_check_integer<JDIMENSION>(L, 2, 1, height) - 1;
      size_t length = 0;
      const char* ptr = luaL_checklstring(L, 3, &length);
      size_t rowbytes = self->get()->image_width * self->get()->input_components;
      if (JSAMPARRAY scanlines = self->prepare_rows(height, rowbytes)) {
        memcpy(scanlines[y], ptr, std::min(rowbytes, length));
        luaX_push_success(L);
      }
    }

    void impl_start_compress(lua_State* L) {
      jpeg_start_compress(check_compressor(L, 1), TRUE);
      luaX_push_success(L);
    }

    void impl_write_scanlines(lua_State* L) {
      compressor_handle* self = check_compressor_handle(L, 1);
      JDIMENSION height = self->get()->image_height;
      if (JSAMPARRAY scanlines = self->prepare_rows(height, self->get()->image_width * self->get()->input_components)) {
        JDIMENSION result = jpeg_write_scanlines(self->get(), scanlines, height);
        luaX_push(L, result);
      }
    }

    void impl_finish_compress(lua_State* L) {
      jpeg_finish_compress(check_compressor(L, 1));
      luaX_push_success(L);
    }

    void impl_get_next_scanline(lua_State* L) {
      luaX_push(L, check_compressor(L, 1)->next_scanline + 1);
    }
  }

  void initialize_compressor(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.jpeg.compressor");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "destroy", impl_destroy);
      luaX_set_field(L, -1, "set_output_message", impl_set_output_message);
      luaX_set_field(L, -1, "set_empty_output_buffer", impl_set_empty_output_buffer);
      luaX_set_field(L, -1, "set_image_width", impl_set_image_width);
      luaX_set_field(L, -1, "set_image_height", impl_set_image_height);
      luaX_set_field(L, -1, "set_input_components", impl_set_input_components);
      luaX_set_field(L, -1, "set_in_color_space", impl_set_in_color_space);
      luaX_set_field(L, -1, "set_defaults", impl_set_defaults);
      luaX_set_field(L, -1, "set_row", impl_set_row);
      luaX_set_field(L, -1, "start_compress", impl_start_compress);
      luaX_set_field(L, -1, "write_scanlines", impl_write_scanlines);
      luaX_set_field(L, -1, "finish_compress", impl_finish_compress);
      luaX_set_field(L, -1, "get_next_scanline", impl_get_next_scanline);
    }
    luaX_set_field(L, -2, "compressor");
  }
}
