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
  class compressor_handle_impl {
  public:
    compressor_handle_impl() : cinfo_(), err_(), dest_(), default_output_message_() {
      jpeg_std_error(&err_);
      err_.error_exit = error_exit;
      default_output_message_ = err_.output_message;

      dest_.init_destination = init_destination_callback;
      dest_.empty_output_buffer = empty_output_buffer_callback;
      dest_.term_destination = term_destination_callback;

      cinfo_.err = &err_;
      cinfo_.client_data = this;
      jpeg_create_compress(&cinfo_);
    }

    ~compressor_handle_impl() {
      destroy();
    }

    void destroy() {
      jpeg_destroy_compress(&cinfo_);
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

    void set_empty_output_buffer(lua_State* L, int index) {
      if (lua_isnoneornil(L, index)) {
        luaX_reference<>().swap(empty_output_buffer_);
        cinfo_.dest = 0;
      } else {
        luaX_reference<>(L, index).swap(empty_output_buffer_);
        cinfo_.dest = &dest_;
      }
    }

    j_compress_ptr get() {
      return &cinfo_;
    }

    JSAMPARRAY prepare_rows(JDIMENSION height, size_t rowbytes) {
      size_t storage_size = height * rowbytes;
      if (row_storage_.size() != storage_size || row_pointers_.size() != height) {
        std::vector<JSAMPLE> row_storage(storage_size);
        std::vector<JSAMPROW> row_pointers(height);
        row_storage.swap(row_storage_);
        row_pointers.swap(row_pointers_);
        if (storage_size > 0) {
          for (size_t y = 0; y < row_pointers_.size(); ++y) {
            row_pointers_[y] = &row_storage_[y * rowbytes];
          }
        }
      }
      if (storage_size > 0) {
        return &row_pointers_[0];
      } else {
        return 0;
      }
    }

  private:
    jpeg_compress_struct cinfo_;
    jpeg_error_mgr err_;
    jpeg_destination_mgr dest_;
    void (*default_output_message_)(j_common_ptr);
    luaX_reference<> output_message_;
    luaX_reference<> empty_output_buffer_;
    std::vector<JOCTET> buffer_;
    std::vector<JSAMPLE> row_storage_;
    std::vector<JSAMPROW> row_pointers_;

    compressor_handle_impl(const compressor_handle_impl&);
    compressor_handle_impl& operator=(const compressor_handle_impl&);

    static void output_message_callback(j_common_ptr cinfo) {
      static_cast<compressor_handle_impl*>(cinfo->client_data)->output_message();
    }

    static void init_destination_callback(j_compress_ptr cinfo) {
      return static_cast<compressor_handle_impl*>(cinfo->client_data)->init_destination();
    }

    static boolean empty_output_buffer_callback(j_compress_ptr cinfo) {
      return static_cast<compressor_handle_impl*>(cinfo->client_data)->empty_output_buffer();
    }

    static void term_destination_callback(j_compress_ptr cinfo) {
      static_cast<compressor_handle_impl*>(cinfo->client_data)->term_destination();
    }

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

    void init_destination() {
      buffer_.resize(4096);
      dest_.next_output_byte = &buffer_[0];
      dest_.free_in_buffer = buffer_.size();
    }

    boolean empty_output_buffer() {
      lua_State* L = empty_output_buffer_.state();
      luaX_top_saver save_top(L);
      {
        empty_output_buffer_.get_field(L);
        lua_pushlstring(L, reinterpret_cast<const char*>(&buffer_[0]), buffer_.size() - dest_.free_in_buffer);
        if (lua_pcall(L, 1, 0, 0) != 0) {
          dest_.next_output_byte = &buffer_[0];
          dest_.free_in_buffer = buffer_.size();
          return TRUE;
        } else {
          error_exit(lua_tostring(L, -1));
          return FALSE;
        }
      }
    }

    void term_destination() {
      if (buffer_.size() > dest_.free_in_buffer) {
        empty_output_buffer();
      }
      dest_.next_output_byte = 0;
      dest_.free_in_buffer = 0;
      buffer_.clear();
    }
  };

  compressor_handle_impl* compressor_handle::create() {
    return new compressor_handle_impl();
  }

  compressor_handle::compressor_handle(compressor_handle_impl* impl) : impl_(impl) {}

  compressor_handle::~compressor_handle() {}

  void compressor_handle::destroy() {
    impl_->destroy();
  }

  void compressor_handle::set_output_message(lua_State* L, int index) {
    impl_->set_output_message(L, index);
  }

  void compressor_handle::set_empty_output_buffer(lua_State* L, int index) {
    impl_->set_empty_output_buffer(L, index);
  }

  j_compress_ptr compressor_handle::get() {
    return impl_->get();
  }

  JSAMPARRAY compressor_handle::prepare_rows(JDIMENSION height, size_t rowbytes) {
    return impl_->prepare_rows(height, rowbytes);
  }
}
