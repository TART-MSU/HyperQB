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

#include <cstdlib>
#include <stdexcept>
#include <cassert>

#pragma once

#ifdef __GNUC__
#define SPOT_LIKELY(expr)   __builtin_expect(!!(expr), 1)
#define SPOT_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#define SPOT_LIKELY(expr) (expr)
#define SPOT_UNLIKELY(expr) (expr)
#endif

#ifdef SWIG
  #define SPOT_DEPRECATED(msg)
  #define SPOT_FALLTHROUGH
#else
  #if __cplusplus < 201703L
    #error C++17 compiler required
  #endif
  #define SPOT_DEPRECATED(msg) [[deprecated(msg)]]
  #define SPOT_FALLTHROUGH [[fallthrough]]
#endif

#if defined _WIN32 || defined __CYGWIN__
  #define SPOT_HELPER_DLL_IMPORT __declspec(dllimport)
  #define SPOT_HELPER_DLL_EXPORT __declspec(dllexport)
  #define SPOT_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define SPOT_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define SPOT_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define SPOT_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define SPOT_HELPER_DLL_IMPORT
    #define SPOT_HELPER_DLL_EXPORT
    #define SPOT_HELPER_DLL_LOCAL
  #endif
#endif

#ifdef SPOT_BUILD
  #define SPOT_DLL
#endif


// We should not call assert() in headers.  For the rare cases where
// we do really want to call assert(), use spot_assert__ instead.
// Else use SPOT_ASSERT so the assert() are removed from user's
// builds.
#define spot_assert__ assert
#if defined(SPOT_BUILD) || defined(SPOT_DEBUG)
  #define SPOT_ASSERT(x) spot_assert__(x)
#else
   // Do not replace by SPOT_ASSUME(x), as x can have some costly
   // side-effects we do not want to pay outside of debug mode.
  #define SPOT_ASSERT(x) while (0)
#endif

// SPOT_API is used for the public API symbols. It either DLL imports
// or DLL exports (or does nothing for static build) SPOT_LOCAL is
// used for non-api symbols that may occur in header files.
#ifdef SPOT_DLL
  #ifdef SPOT_BUILD
    #define SPOT_API SPOT_HELPER_DLL_EXPORT
  #else
    #define SPOT_API SPOT_HELPER_DLL_IMPORT
  #endif
  #define SPOT_LOCAL SPOT_HELPER_DLL_LOCAL
#else
  #define SPOT_API
  #define SPOT_LOCAL
#endif
#define SPOT_API_VAR extern SPOT_API


// Swig 3.0.2 does not understand 'final' when used
// at class definition.
#ifdef SWIG
  #define final
#endif


// Do not use those in code, prefer SPOT_UNREACHABLE() instead.
#if defined __clang__ || defined __GNUC__
#  define SPOT_UNREACHABLE_BUILTIN() __builtin_unreachable()
# elif defined _MSC_VER
#  define SPOT_UNREACHABLE_BUILTIN() __assume(0)
# else
#  define SPOT_UNREACHABLE_BUILTIN() abort()
#endif

// The extra parentheses in assert() is so that this
// pattern is not caught by the style checker.
#define SPOT_UNREACHABLE() do {                         \
     SPOT_ASSERT(!("unreachable code reached"));      \
     SPOT_UNREACHABLE_BUILTIN();                        \
   } while (0)

#define SPOT_UNIMPLEMENTED() throw std::runtime_error("unimplemented");


#if SPOT_DEBUG && !defined(NDEBUG)
#define SPOT_ASSUME(cond) assert(cond)
#else
#define SPOT_ASSUME(cond)                       \
  do                                            \
    {                                           \
      if (!(cond))                              \
        SPOT_UNREACHABLE_BUILTIN();             \
    }                                           \
  while (0)
#endif


// Useful when forwarding methods such as:
//   auto func(int param) SPOT_RETURN(implem_.func(param));
#define SPOT_RETURN(code) -> decltype(code) { return code; }

namespace spot
{
  struct SPOT_API parse_error: public std::runtime_error
  {
    parse_error(const std::string& s)
      : std::runtime_error(s)
      {
      }
  };

  /// \brief This class is used to tell parallel algorithms what
  /// resources they may use.
  ///
  /// Currently, this simply stores an integer indicating the number
  /// of threads that the algorithm may create, but in the future it
  /// will probably do more.
  class SPOT_API parallel_policy
  {
    unsigned nthreads_;
  public:
    parallel_policy(unsigned nthreads = 1) : nthreads_(nthreads)
    {
    }

    unsigned nthreads() const
    {
      return nthreads_;
    }
  };

}

// This is a workaround for the issue described in GNU GCC bug 89303.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89303
//
// In brief: with some version of gcc distributed by Debian unstable
// and that correspond to something a bit newer than 8.2.1 (Debian is
// tracking the gcc-8-branch instead of following releases), mixing
// make_shared with enable_shared_from_this produces memory leaks or
// bad_weak_ptr exceptions.
//
// Our workaround is simply to avoid calling make_shared in those
// cases.
//
// The use of "enabled" in the macro name is just here to remember
// that we only need this macro for classes that inherit from
// enable_shared_from_this.
#if __GNUC__ == 8 && __GNUC_MINOR__ == 2
#  define SPOT_make_shared_enabled__(TYPE, ...) \
   std::shared_ptr<TYPE>(new TYPE(__VA_ARGS__))
#else
#  define SPOT_make_shared_enabled__(TYPE, ...)  \
   std::make_shared<TYPE>(__VA_ARGS__)
#endif
