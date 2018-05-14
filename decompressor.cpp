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
  namespace {
    decompressor_handle* check_decompressor_handle(lua_State* L, int arg) {
      return luaX_check_udata<decompressor_handle>(L, arg, "dromozoa.jpeg.decompressor");
    }

    j_decompress_ptr check_decompressor(lua_State* L, int arg) {
      return check_decompressor_handle(L, arg)->get();
    }

    void impl_gc(lua_State* L) {
      check_decompressor_handle(L, 1)->~decompressor_handle();
    }

    void impl_call(lua_State* L) {
      luaX_new<decompressor_handle>(L, decompressor_handle::create());
      luaX_set_metatable(L, "dromozoa.jpeg.decompressor");
    }

    void impl_destroy(lua_State* L) {
      check_decompressor_handle(L, 1)->destroy();
      luaX_push_success(L);
    }

    void impl_set_output_message(lua_State* L) {
      check_decompressor_handle(L, 1)->set_output_message(L, 2);
      luaX_push_success(L);
    }

    void impl_set_fill_input_buffer(lua_State* L) {
      check_decompressor_handle(L, 1)->set_fill_input_buffer(L, 2);
      luaX_push_success(L);
    }

    void impl_read_header(lua_State* L) {
      int result = jpeg_read_header(check_decompressor(L, 1), FALSE);
      luaX_push(L, result);
    }

    void impl_start_decompress(lua_State* L) {
      boolean result = jpeg_start_decompress(check_decompressor(L, 1));
      luaX_push(L, static_cast<bool>(result));
    }

    void impl_read_scanlines(lua_State* L) {
      decompressor_handle* self = check_decompressor_handle(L, 1);
      JDIMENSION height = self->get()->output_height;
      if (JSAMPARRAY scanlines = self->prepare_rows(height, self->get()->output_width * self->get()->output_components)) {
        JDIMENSION result = jpeg_read_scanlines(self->get(), scanlines, height);
        luaX_push(L, result);
      }
    }

    void impl_finish_decompress(lua_State* L) {
      boolean result = jpeg_finish_decompress(check_decompressor(L, 1));
      luaX_push(L, static_cast<bool>(result));
    }

    void impl_get_output_width(lua_State* L) {
      luaX_push(L, check_decompressor(L, 1)->output_width);
    }

    void impl_get_output_height(lua_State* L) {
      luaX_push(L, check_decompressor(L, 1)->output_height);
    }

    void impl_get_out_color_space(lua_State* L) {
      luaX_push<int>(L, check_decompressor(L, 1)->out_color_space);
    }

    void impl_get_out_color_components(lua_State* L) {
      luaX_push(L, check_decompressor(L, 1)->out_color_components);
    }

    void impl_get_output_components(lua_State* L) {
      luaX_push(L, check_decompressor(L, 1)->output_components);
    }

    void impl_get_output_scanline(lua_State* L) {
      luaX_push(L, check_decompressor(L, 1)->output_scanline);
    }
  }

  void initialize_decompressor(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.jpeg.decompressor");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "destroy", impl_destroy);
      luaX_set_field(L, -1, "set_output_message", impl_set_output_message);
      luaX_set_field(L, -1, "set_fill_input_buffer", impl_set_fill_input_buffer);
      luaX_set_field(L, -1, "read_header", impl_read_header);
      luaX_set_field(L, -1, "start_decompress", impl_start_decompress);
      luaX_set_field(L, -1, "read_scanlines", impl_read_scanlines);
      luaX_set_field(L, -1, "finish_decompress", impl_finish_decompress);
      luaX_set_field(L, -1, "get_output_width", impl_get_output_width);
      luaX_set_field(L, -1, "get_output_height", impl_get_output_height);
      luaX_set_field(L, -1, "get_out_color_space", impl_get_out_color_space);
      luaX_set_field(L, -1, "get_out_color_components", impl_get_out_color_components);
      luaX_set_field(L, -1, "get_output_components", impl_get_output_components);
      luaX_set_field(L, -1, "get_output_scanline", impl_get_output_scanline);
    }
    luaX_set_field(L, -2, "decompressor");

    luaX_set_field(L, -1, "JPEG_SUSPENDED", JPEG_SUSPENDED);
    luaX_set_field(L, -1, "JPEG_HEADER_OK", JPEG_HEADER_OK);
    luaX_set_field(L, -1, "JPEG_HEADER_TABLES_ONLY", JPEG_HEADER_TABLES_ONLY);
  }
}
