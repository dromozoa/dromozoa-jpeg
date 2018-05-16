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
#include <vector>

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

    void impl_set_colorspace(lua_State* L) {
      J_COLOR_SPACE colorspace = luaX_check_enum<J_COLOR_SPACE>(L, 2);
      jpeg_set_colorspace(check_compressor(L, 1), colorspace);
      luaX_push_success(L);
    }

    void impl_default_colorspace(lua_State* L) {
      jpeg_default_colorspace(check_compressor(L, 1));
      luaX_push_success(L);
    }

    void impl_set_quality(lua_State* L) {
      int quality = luaX_check_integer(L, 2, 0, 100);
      boolean force_baseline = TRUE;
      if (luaX_is_false(L, 3)) {
        force_baseline = FALSE;
      }
      jpeg_set_quality(check_compressor(L, 1), quality, force_baseline);
      luaX_push_success(L);
    }

    void impl_get_next_scanline(lua_State* L) {
      luaX_push(L, check_compressor(L, 1)->next_scanline + 1);
    }

    void impl_start_compress(lua_State* L) {
      boolean write_all_tables = TRUE;
      if (luaX_is_false(L, 2)) {
        write_all_tables = FALSE;
      }
      jpeg_start_compress(check_compressor_handle(L, 1)->get(true), write_all_tables);
      luaX_push_success(L);
    }

    void impl_write_marker(lua_State* L) {
      int marker = luaX_check_integer<int>(L, 2);
      luaX_string_reference source = luaX_check_string(L, 3);
      jpeg_write_marker(check_compressor_handle(L, 1)->get(true), marker, reinterpret_cast<const JOCTET*>(source.data()), source.size());
      luaX_push_success(L);
    }

    void impl_write_scanlines(lua_State* L) {
      compressor_handle* self = check_compressor_handle(L, 1);
      JDIMENSION num_lines = lua_gettop(L) - 1;
      size_t samples_per_row = self->get()->image_width * self->get()->input_components;
      std::vector<JSAMPLE> storage(samples_per_row * num_lines);
      std::vector<JSAMPROW> scanlines(num_lines);
      for (JDIMENSION i = 0; i < num_lines; ++i) {
        scanlines[i] = &storage[i * samples_per_row];
        if (luaX_string_reference source = luaX_to_string(L, i + 2)) {
          memcpy(scanlines[i], source.data(), std::min(samples_per_row, source.size()));
        }
      }
      luaX_push(L, jpeg_write_scanlines(self->get(true), &scanlines[0], num_lines));
    }

    void impl_finish_compress(lua_State* L) {
      jpeg_finish_compress(check_compressor_handle(L, 1)->get(true));
      luaX_push_success(L);
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
      luaX_set_field(L, -1, "set_colorspace", impl_set_colorspace);
      luaX_set_field(L, -1, "default_colorspace", impl_default_colorspace);
      luaX_set_field(L, -1, "set_quality", impl_set_quality);
      luaX_set_field(L, -1, "get_next_scanline", impl_get_next_scanline);
      luaX_set_field(L, -1, "start_compress", impl_start_compress);
      luaX_set_field(L, -1, "write_marker", impl_write_marker);
      luaX_set_field(L, -1, "write_scanlines", impl_write_scanlines);
      luaX_set_field(L, -1, "finish_compress", impl_finish_compress);
    }
    luaX_set_field(L, -2, "compressor");
  }
}
