// file      : odb/database.txx
// copyright : Copyright (c) 2009-2013 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <odb/section.hxx>
#include <odb/exceptions.hxx>
#include <odb/no-op-cache-traits.hxx>
#include <odb/pointer-traits.hxx>

namespace odb
{
  template <typename T>
  result<T> database::
  query (const odb::query<T>& q, bool cache)
  {
    // T is always object_type. We also don't need to check for transaction
    // here; object_traits::query () does this.
    //
    result<T> r (query_<T, id_common>::call (*this, q));

    if (cache)
      r.cache ();

    return r;
  }

  // Implementations (i.e., the *_() functions).
  //
  template <typename T, database_id DB>
  typename object_traits<T>::id_type database::
  persist_ (T& obj)
  {
    // T can be const T while object_type will always be T.
    //
    typedef typename object_traits<T>::object_type object_type;
    typedef object_traits_impl<object_type, DB> object_traits;

    object_traits::persist (*this, obj);

    typename object_traits::reference_cache_traits::position_type p (
      object_traits::reference_cache_traits::insert (
        *this, reference_cache_type<T>::convert (obj)));

    object_traits::reference_cache_traits::persist (p);

    return object_traits::id (obj);
  }

  template <typename T, database_id DB>
  typename object_traits<T>::id_type database::
  persist_ (const typename object_traits<T>::pointer_type& pobj)
  {
    // T can be const T while object_type will always be T.
    //
    typedef typename object_traits<T>::object_type object_type;
    typedef typename object_traits<T>::pointer_type pointer_type;

    typedef object_traits_impl<object_type, DB> object_traits;

    T& obj (pointer_traits<pointer_type>::get_ref (pobj));
    object_traits::persist (*this, obj);

    // Get the canonical object pointer and insert it into object cache.
    //
    typename object_traits::pointer_cache_traits::position_type p (
      object_traits::pointer_cache_traits::insert (
        *this, pointer_cache_type<pointer_type>::convert (pobj)));

    object_traits::pointer_cache_traits::persist (p);

    return object_traits::id (obj);
  }

  template <typename I, typename T, database_id DB>
  void database::
  persist_ (I b, I e, details::meta::no /*ptr*/)
  {
    // T can be const T while object_type will always be T.
    //
    typedef typename object_traits<T>::object_type object_type;
    typedef object_traits_impl<object_type, DB> object_traits;

    multiple_exceptions mex;
    try
    {
      while (b != e)
      {
        std::size_t n (0);
        T* a[object_traits::batch];

        for (; b != e && n < object_traits::batch; ++n)
          a[n] = &(*b++);

        object_traits::persist (*this, a, n, &mex);

        if (mex.fatal ())
          break;

        for (std::size_t i (0); i < n; ++i)
        {
          if (mex[i] != 0) // Don't cache objects that have failed.
            continue;

          mex.current (i); // Set position in case the below code throws.

          typename object_traits::reference_cache_traits::position_type p (
            object_traits::reference_cache_traits::insert (
              *this, reference_cache_type<T>::convert (*a[i])));

          object_traits::reference_cache_traits::persist (p);
        }

        mex.delta (n);
      }
    }
    catch (const odb::exception& ex)
    {
      mex.insert (ex, true);
    }

    if (!mex.empty ())
    {
      mex.prepare ();
      throw mex;
    }
  }

  namespace details
  {
    template <typename P>
    struct pointer_copy
    {
      const P* ref;
      P copy;

      void assign (const P& p) {ref = &p;}
      template <typename P1> void assign (const P1& p1)
      {
        // The passed pointer should be the same or implicit-convertible
        // to the object pointer. This way we make sure the object pointer
        // does not assume ownership of the passed object.
        //
        const P& p (p1);

        copy = p;
        ref = &copy;
      }
    };
  }

  template <typename I, typename T, database_id DB>
  void database::
  persist_ (I b, I e, details::meta::yes /*ptr*/)
  {
    // T can be const T while object_type will always be T.
    //
    typedef typename object_traits<T>::object_type object_type;
    typedef typename object_traits<T>::pointer_type pointer_type;

    typedef object_traits_impl<object_type, DB> object_traits;

    multiple_exceptions mex;
    try
    {
      while (b != e)
      {
        std::size_t n (0);
        T* a[object_traits::batch];
        details::pointer_copy<pointer_type> p[object_traits::batch];

        for (; b != e && n < object_traits::batch; ++n)
        {
          p[n].assign (*b++);
          a[n] = &pointer_traits<pointer_type>::get_ref (*p[n].ref);
        }

        object_traits::persist (*this, a, n, &mex);

        if (mex.fatal ())
          break;

        for (std::size_t i (0); i < n; ++i)
        {
          if (mex[i] != 0) // Don't cache objects that have failed.
            continue;

          mex.current (i); // Set position in case the below code throws.

          // Get the canonical object pointer and insert it into object cache.
          //
          typename object_traits::pointer_cache_traits::position_type pos (
            object_traits::pointer_cache_traits::insert (
              *this, pointer_cache_type<pointer_type>::convert (*p[i].ref)));

          object_traits::pointer_cache_traits::persist (pos);
        }

        mex.delta (n);
      }
    }
    catch (const odb::exception& ex)
    {
      mex.insert (ex, true);
    }

    if (!mex.empty ())
    {
      mex.prepare ();
      throw mex;
    }
  }

  template <typename T, database_id DB>
  typename object_traits<T>::pointer_type database::
  load_ (const typename object_traits<T>::id_type& id)
  {
    // T is always object_type.
    //
    typedef typename object_traits<T>::pointer_type pointer_type;

    pointer_type r (find_<T, DB> (id));

    if (pointer_traits<pointer_type>::null_ptr (r))
      throw object_not_persistent ();

    return r;
  }

  template <typename T, database_id DB>
  void database::
  load_ (const typename object_traits<T>::id_type& id, T& obj)
  {
    if (!find_<T, DB> (id, obj))
      throw object_not_persistent ();
  }

  template <typename T, database_id DB>
  void database::
  load_ (T& obj, section& s)
  {
    connection_type& c (transaction::current ().connection ());

    // T is always object_type.
    //
    if (object_traits_impl<T, DB>::load (c, obj, s))
      s.reset (true, false); // Loaded, unchanged.
    else
      throw section_not_in_object ();
  }

  template <typename T, database_id DB>
  void database::
  reload_ (T& obj)
  {
    // T should be object_type (cannot be const). We also don't need to
    // check for transaction here; object_traits::reload () does this.
    //
    if (!object_traits_impl<T, DB>::reload (*this, obj))
      throw object_not_persistent ();
  }

  template <typename T, database_id DB>
  void database::
  update_ (const T& obj, const section& s)
  {
    if (!s.loaded ())
      throw section_not_loaded ();

    transaction& t (transaction::current ());

    // T is always object_type.
    //
    if (object_traits_impl<T, DB>::update (t.connection (), obj, s))
    {
      if (s.changed ())
        s.reset (true, false, &t); // Clear the change flag.
    }
    else
      throw section_not_in_object ();
  }

  template <typename T, database_id DB>
  struct database::query_<T, DB, class_object>
  {
    template <typename Q>
    static result<T>
    call (database& db, const Q& q)
    {
      return object_traits_impl<T, DB>::query (db, q);
    }
  };

  template <typename T, database_id DB>
  struct database::query_<T, DB, class_view>
  {
    template <typename Q>
    static result<T>
    call (database& db, const Q& q)
    {
      return view_traits_impl<T, DB>::query (db, q);
    }
  };
}
