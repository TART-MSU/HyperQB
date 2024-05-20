// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <spot/misc/common.hh>
#include <spot/misc/clz.hh>

#if SPOT_DEBUG && __has_include(<valgrind/memcheck.h>)
#undef USES_MEMCHECK_H
#define USES_MEMCHECK_H 1
#include <valgrind/memcheck.h>
#endif

namespace spot
{
  /// A enum class to define the policy of the fixed_sized_pool.
  /// We propose 2 policies for the pool:
  ///   - Safe: ensure (when used with memcheck) that each allocation
  ///     is deallocated one at a time
  ///   - Unsafe: rely on the fact that deallocating the pool also release
  ///     all elements it contains. This case is useful in a multithreaded
  ///     environnement with multiple fixed_sized_pool allocating the same
  ///     resource. In this case it's hard to detect which pool has allocated
  ///     some resource.
  enum class pool_type { Safe , Unsafe };

  /// A fixed-size memory pool implementation.
  template<pool_type Kind>
  class SPOT_API fixed_size_pool
  {
  public:
    /// Create a pool allocating objects of \a size bytes.
    fixed_size_pool(size_t size)
      : size_(
          [](size_t size)
          {
            // to properly store chunks and freelist, we need size to be at
            // least the size of a block_
            if (size < sizeof(block_))
                size = sizeof(block_);
            // powers of 2 are a good alignment
            if (!(size & (size-1)))
              return size;
            // small numbers are best aligned to the next power of 2
            else if (size < alignof(std::max_align_t))
              return size_t{1} << (CHAR_BIT*sizeof(size_t) - clz(size));
            else
              {
                size_t mask = alignof(std::max_align_t)-1;
                return (size + mask) & ~mask;
              }
          }(size)),
      freelist_(nullptr),
      chunklist_(nullptr)
    {
       new_chunk_();
    }

    /// Free any memory allocated by this pool.
    ~fixed_size_pool()
    {
      while (chunklist_)
        {
          chunk_* prev = chunklist_->prev;
          ::operator delete(chunklist_);
          chunklist_ = prev;
        }
    }

    /// Allocate \a size bytes of memory.
    void*
    allocate()
    {
      block_* f = freelist_;
      // If we have free blocks available, return the first one.
      if (f)
        {
#ifdef USES_MEMCHECK_H
          if (Kind == pool_type::Safe)
            {
              VALGRIND_MALLOCLIKE_BLOCK(f, size_, 0, false);
              // field f->next is initialized: prevents valgrind from
              // complaining about jumps depending on uninitialized memory
              VALGRIND_MAKE_MEM_DEFINED(f, sizeof(block_*));
            }
#endif
          freelist_ = f->next;
          return f;
        }

      // Else, create a block out of the last chunk of allocated
      // memory.

      // If all the last chunk has been used, allocate one more.
      if (free_start_ + size_ > free_end_)
        new_chunk_();

      void* res = free_start_;
      free_start_ += size_;
#ifdef USES_MEMCHECK_H
      if (Kind == pool_type::Safe)
        {
          VALGRIND_MALLOCLIKE_BLOCK(res, size_, 0, false);
        }
#endif
      return res;
    }

    /// \brief Recycle \a size bytes of memory.
    ///
    /// Despite the name, the memory is not really deallocated in the
    /// "delete" sense: it is still owned by the pool and will be
    /// reused by allocate as soon as possible.  The memory is only
    /// freed when the pool is destroyed.
    void
    deallocate(void* ptr)
    {
      SPOT_ASSERT(ptr);
      block_* b = reinterpret_cast<block_*>(ptr);
      b->next = freelist_;
      freelist_ = b;
#ifdef USES_MEMCHECK_H
      if (Kind == pool_type::Safe)
        {
          VALGRIND_FREELIKE_BLOCK(ptr, 0);
        }
#endif
    }

  private:
    void new_chunk_()
    {
     const size_t requested = (size_ > 128 ? size_ : 128) * 8192 - 64;
     chunk_* c = reinterpret_cast<chunk_*>(::operator new(requested));
     c->prev = chunklist_;
     chunklist_ = c;

     free_start_ = c->data_ + size_;
     free_end_ = c->data_ + requested;
    }


    const size_t size_;
    struct block_ { block_* next; }* freelist_;
    char* free_start_;
    char* free_end_;
    // chunk = several agglomerated blocks
    union chunk_ { chunk_* prev; char data_[1]; }* chunklist_;
  };
}
