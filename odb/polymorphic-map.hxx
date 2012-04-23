// file      : odb/polymorphic-map.hxx
// copyright : Copyright (c) 2005-2012 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_POLYMORPHIC_MAP_HXX
#define ODB_POLYMORPHIC_MAP_HXX

#include <odb/pre.hxx>

#include <map>
#include <utility>  // std::move
#include <cstddef>  // std::size_t
#include <cassert>
#include <typeinfo>

#include <odb/callback.hxx>

#include <odb/details/config.hxx>    // ODB_CXX11
#include <odb/details/type-info.hxx>

#include <odb/polymorphic-info.hxx>

namespace odb
{
  template <typename R>
  struct polymorphic_map
  {
    typedef R root_type;
    typedef polymorphic_concrete_info<root_type> info_type;
    typedef typename info_type::discriminator_type discriminator_type;

    polymorphic_map (): ref_count_ (1) {}

    const info_type&
    find (const std::type_info& t) const;

    const info_type&
    find (const discriminator_type& d) const;

  public:
    typedef
    std::map<const std::type_info*,
             const info_type*,
             details::type_info_comparator>
    type_map;

    struct discriminator_comparator
    {
      bool
      operator() (const discriminator_type* x,
                  const discriminator_type* y) const
      {
        return *x < *y;
      }
    };

    typedef
    std::map<const discriminator_type*,
             const info_type*,
             discriminator_comparator>
    discriminator_map;

  public:
    std::size_t ref_count_;
    type_map type_map_;
    discriminator_map discriminator_map_;
  };

  template <typename R>
  struct polymorphic_entry_impl
  {
    typedef R root_type;
    typedef object_traits<root_type> root_traits;
    typedef polymorphic_concrete_info<root_type> info_type;

    static void
    insert (const info_type&);

    static void
    erase (const info_type&);
  };

  template <typename T>
  typename object_traits<typename object_traits<T>::root_type>::pointer_type
  create_impl ()
  {
    typedef object_traits<T> derived_traits;
    typedef object_traits<typename derived_traits::root_type> root_traits;

    typedef typename derived_traits::pointer_type derived_pointer_type;
    typedef typename root_traits::pointer_type root_pointer_type;

    derived_pointer_type p (
      access::object_factory<T, derived_pointer_type>::create ());

    // Implicit downcast.
    //
#ifdef ODB_CXX11
    root_pointer_type r (std::move (p));
#else
    root_pointer_type r (p);
#endif
    return r;
  }

  template <typename T, typename R>
  struct dispatch_load
  {
    static void
    call (database& db, T& obj, std::size_t d)
    {
      object_traits<T>::load_ (db, obj, d);
    }
  };

  template <typename R>
  struct dispatch_load<R, R>
  {
    static void
    call (database&, R&, std::size_t)
    {
      assert (false);
    }
  };

  template <typename T, bool auto_id>
  struct dispatch_persist
  {
    static void
    call (database& db, const T& obj)
    {
      // Top-level call, no dynamic type checking.
      //
      object_traits<T>::persist (db, obj, true, false);
    }
  };

  template <typename T>
  struct dispatch_persist<T, true>
  {
    static void
    call (database& db, const T& obj)
    {
      // Top-level call, no dynamic type checking.
      //
      object_traits<T>::persist (db, const_cast<T&> (obj), true, false);
    }
  };

  template <typename T>
  bool dispatch_impl (
    typename polymorphic_concrete_info<
      typename object_traits<T>::root_type>::call_type c,
    database& db,
    const typename object_traits<T>::root_type* pobj,
    const void* arg)
  {
    typedef object_traits<T> derived_traits;
    typedef typename derived_traits::root_type root_type;
    typedef object_traits<root_type> root_traits;
    typedef typename root_traits::id_type id_type;
    typedef polymorphic_concrete_info<root_type> info_type;

    bool r (false);

    switch (c)
    {
    case info_type::call_callback:
      {
        derived_traits::callback (
          db,
          *const_cast<T*> (static_cast<const T*> (pobj)),
          *static_cast<const callback_event*> (arg));
        break;
      }
    case info_type::call_persist:
      {
        dispatch_persist<T, root_traits::auto_id>::call (
          db,
          *static_cast<const T*> (pobj));
        break;
      }
    case info_type::call_update:
      {
        derived_traits::update (
          db,
          *static_cast<const T*> (pobj),
          true,   // Top-level call.
          false); // No dynamic type checking.
        break;
      }
    case info_type::call_find:
      {
        r = derived_traits::find (
          db,
          *static_cast<const id_type*> (arg),
          *const_cast<T*> (static_cast<const T*> (pobj)),
          false); // No dynamic type checking.
        break;
      }
    case info_type::call_reload:
      {
        r = derived_traits::reload (
          db,
          *const_cast<T*> (static_cast<const T*> (pobj)),
          false); // No dynamic type checking.
        break;
      }
    case info_type::call_load:
      {
        dispatch_load<T, root_type>::call (
          db,
          *const_cast<T*> (static_cast<const T*> (pobj)),
          *static_cast<const std::size_t*> (arg));
        break;
      }
    case info_type::call_erase:
      {
        if (pobj != 0)
          derived_traits::erase (
            db,
            *static_cast<const T*> (pobj),
            true,   // Top-level call.
            false); // No dynamic type checking.
        else
          derived_traits::erase (
            db,
            *static_cast<const id_type*> (arg),
            true,   // Top-level call.
            false); // No dynamic type checking.
        break;
      }
    }

    return r;
  }
}

#include <odb/polymorphic-map.ixx>
#include <odb/polymorphic-map.txx>

#include <odb/post.hxx>

#endif // ODB_POLYMORPHIC_MAP_HXX
