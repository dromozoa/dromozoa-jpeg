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

#include <vector>

#include "common.hpp"

namespace dromozoa {
  class decompressor_handle_impl {
  public:
    decompressor_handle_impl() : cinfo_(), err_(), src_(), default_output_message_() {
      jpeg_std_error(&err_);
      err_.error_exit = error_exit;
      default_output_message_ = err_.output_message;

      src_.init_source = init_source_callback;
      src_.fill_input_buffer = fill_input_buffer_callback;
      src_.skip_input_data = skip_input_data_callback;
      src_.resync_to_restart = jpeg_resync_to_restart;
      src_.term_source = term_source_callback;

      cinfo_.err = &err_;
      cinfo_.client_data = this;
      jpeg_create_decompress(&cinfo_);
    }

    ~decompressor_handle_impl() {
      destroy();
    }

    void destroy() {
      jpeg_destroy_decompress(&cinfo_);
    }

    void set_output_message(lua_State* L, int index) {
      if (lua_isnoneornil(L, index)) {
        luaX_reference<>().swap(output_message_);
        err_.output_message = default_output_message_;
      } else {
        luaX_reference<>(L, index).swap(output_message_);
        err_.output_message = output_message_callback;
      }
    }


    void set_fill_input_buffer(lua_State* L, int index) {
      if (lua_isnoneornil(L, index)) {
        luaX_reference<>().swap(fill_input_buffer_);
        cinfo_.src = 0;
      } else {
        luaX_reference<>(L, index).swap(fill_input_buffer_);
        cinfo_.src = &src_;
      }
    }

    j_decompress_ptr get() {
      return &info_;
    }

  private:
    jpeg_decompress_struct cinfo_;
    jpeg_error_mgr err_;
    jpeg_source_mgr src_;
    void (*default_output_message_)(j_common_ptr);
    luaX_reference<> output_message_;
    luaX_reference<> fill_input_buffer_;
    std::vector<JOCTET> buffer_;

    decompressor_handle_impl(const decompressor_handle_impl&);
    decompressor_handle_impl& operator=(const decompressor_handle_impl&);

    static void output_message_callback(j_common_ptr cinfo) {
      static_cast<decompressor_handle_impl*>(cinfo->client_data)->output_message();
    }

    static void init_source_callback(j_decompress_ptr) {}

    static boolean fill_input_buffer_callback(j_decompress_ptr cinfo) {
      return static_cast<decompressor_handle_impl*>(cinfo->client_data)->fill_input_buffer();
    }

    static void skip_input_data_callback(j_decompress_ptr cinfo, long num_bytes) {
      size_t bytes = num_bytes;
      while (bytes > cinfo->src->bytes_in_buffer) {
        bytes -= cinfo->src->bytes_in_buffer;
        (*cinfo->src->fill_input_buffer)(cinfo);
      }
      cinfo->src->next_input_byte += bytes;
      cinfo->src->bytes_in_buffer -= bytes;
    }

    static void term_source_callback(j_decompress_ptr) {}

    void output_message() {
      lua_State* L = output_message_.state();
      luaX_top_saver save_top(L);
      {
        output_message_.get_field(L);
        char what[JMSG_LENGTH_MAX] = { 0 };
        (*err_.format_message)(reinterpret_cast<j_common_ptr>(&cinfo_), what);
        luaX_push(L, what);
        if (lua_pcall(L, 1, 0, 0) != 0) {
          error_exit(lua_tostring(L, -1));
        }
      }
    }

    boolean fill_input_buffer() {
      lua_State* L = fill_input_buffer_.state();
      luaX_top_saver save_top(L);
      {
        fill_input_buffer_.get_field(L);
        if (lua_pcall(L, 0, 0, 0) == 0) {
          size_t bytes = 0;
          if (const char* ptr = lua_tolstring(L, -1, &bytes)) {
            buffer_.resize(bytes);
            memcpy(&buffer_[0], ptr, bytes);
            src_.next_input_byte = &buffer_[0];
            src_.bytes_in_buffer = bytes;
            return TRUE;
          } else {
            buffer_.resize(2);
            buffer_[0] = 0xFF;
            buffer_[1] = JPEG_EOI;
            src_.next_input_byte = &buffer_[0];
            src_.bytes_in_buffer = 2;
            return TRUE;
          }
        } else {
          error_exit(lua_tostring(L, -1));
          return FALSE;
        }
      }
    }
  };

  decompressor_handle_impl* decompressor_handle::create() {
    return new decompressor_handle_impl();
  }

  decompressor_handle::decompressor_handle(decompressor_handle_impl* impl) : impl_(impl) {}

  decompressor_handle::~decompressor_handle() {}

  void decompressor_handle::destroy() {
    impl_->destroy();
  }

  void decompressor_handle::set_output_message(lua_State* L, int index) {
    impl_->set_output_message(L, index);
  }

  void decompressor_handle::set_fill_input_buffer(lua_State* L, int index) {
    impl_->set_fill_input_buffer(L, index);
  }

  j_decompress_ptr decompressor_handle::get() {
    return impl_->get();
  }
}
