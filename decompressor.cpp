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

    void impl_read_header(lua_State* L) {
      boolean require_image = TRUE;
      if (!lua_isnoneornil(L, 2) && !lua_toboolean(L, 2)) {
        require_image = FALSE;
      }
      jpeg_read_header(check_decompressor(L, 1), require_image);
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
    }
    luaX_set_field(L, -2, "decompressor");
  }
}
