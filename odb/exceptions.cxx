// file      : odb/exceptions.cxx
// copyright : Copyright (c) 2009-2013 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#include <cstring> // std::strlen
#include <sstream>

#include <odb/exceptions.hxx>

using namespace std;

namespace odb
{
  const char* null_pointer::
  what () const throw ()
  {
    return "NULL pointer";
  }

  null_pointer* null_pointer::
  clone () const
  {
    return new null_pointer (*this);
  }

  const char* already_in_transaction::
  what () const throw ()
  {
    return "transaction already in progress in this thread";
  }

  already_in_transaction* already_in_transaction::
  clone () const
  {
    return new already_in_transaction (*this);
  }

  const char* not_in_transaction::
  what () const throw ()
  {
    return "operation can only be performed in transaction";
  }

  not_in_transaction* not_in_transaction::
  clone () const
  {
    return new not_in_transaction (*this);
  }

  const char* transaction_already_finalized::
  what () const throw ()
  {
    return "transaction already committed or rolled back";
  }

  transaction_already_finalized* transaction_already_finalized::
  clone () const
  {
    return new transaction_already_finalized (*this);
  }

  const char* already_in_session::
  what () const throw ()
  {
    return "session already in effect in this thread";
  }

  already_in_session* already_in_session::
  clone () const
  {
    return new already_in_session (*this);
  }

  const char* not_in_session::
  what () const throw ()
  {
    return "session not in effect in this thread";
  }

  not_in_session* not_in_session::
  clone () const
  {
    return new not_in_session (*this);
  }

  const char* session_required::
  what () const throw ()
  {
    return "session required to load this object relationship";
  }

  session_required* session_required::
  clone () const
  {
    return new session_required (*this);
  }

  const char* deadlock::
  what () const throw ()
  {
    return "transaction aborted due to deadlock";
  }

  deadlock* deadlock::
  clone () const
  {
    return new deadlock (*this);
  }

  const char* connection_lost::
  what () const throw ()
  {
    return "connection to database lost";
  }

  connection_lost* connection_lost::
  clone () const
  {
    return new connection_lost (*this);
  }

  const char* timeout::
  what () const throw ()
  {
    return "database operation timeout";
  }

  timeout* timeout::
  clone () const
  {
    return new timeout (*this);
  }

  const char* object_not_persistent::
  what () const throw ()
  {
    return "object not persistent";
  }

  object_not_persistent* object_not_persistent::
  clone () const
  {
    return new object_not_persistent (*this);
  }

  const char* object_already_persistent::
  what () const throw ()
  {
    return "object already persistent";
  }

  object_already_persistent* object_already_persistent::
  clone () const
  {
    return new object_already_persistent (*this);
  }

  const char* object_changed::
  what () const throw ()
  {
    return "object changed concurrently";
  }

  object_changed* object_changed::
  clone () const
  {
    return new object_changed (*this);
  }

  const char* result_not_cached::
  what () const throw ()
  {
    return "query result is not cached";
  }

  result_not_cached* result_not_cached::
  clone () const
  {
    return new result_not_cached (*this);
  }

  const char* abstract_class::
  what () const throw ()
  {
    return "database operation on instance of abstract class";
  }

  abstract_class* abstract_class::
  clone () const
  {
    return new abstract_class (*this);
  }

  const char* no_type_info::
  what () const throw ()
  {
    return "no type information";
  }

  no_type_info* no_type_info::
  clone () const
  {
    return new no_type_info (*this);
  }

  prepared_already_cached::
  prepared_already_cached (const char* name)
      : name_ (name)
  {
    what_ = "prepared query '";
    what_ += name;
    what_ += "' is already cached";
  }

  prepared_already_cached::
  ~prepared_already_cached () throw ()
  {
  }

  const char* prepared_already_cached::
  what () const throw ()
  {
    return what_.c_str ();
  }

  prepared_already_cached* prepared_already_cached::
  clone () const
  {
    return new prepared_already_cached (*this);
  }

  prepared_type_mismatch::
  prepared_type_mismatch (const char* name)
      : name_ (name)
  {
    what_ = "type mismatch while looking up prepared query '";
    what_ += name;
    what_ += "'";
  }

  prepared_type_mismatch::
  ~prepared_type_mismatch () throw ()
  {
  }

  const char* prepared_type_mismatch::
  what () const throw ()
  {
    return what_.c_str ();
  }

  prepared_type_mismatch* prepared_type_mismatch::
  clone () const
  {
    return new prepared_type_mismatch (*this);
  }

  unknown_schema::
  unknown_schema (const std::string& name)
      : name_ (name)
  {
    what_ = "unknown database schema '";
    what_ += name;
    what_ += "'";
  }

  unknown_schema::
  ~unknown_schema () throw ()
  {
  }

  const char* unknown_schema::
  what () const throw ()
  {
    return what_.c_str ();
  }

  unknown_schema* unknown_schema::
  clone () const
  {
    return new unknown_schema (*this);
  }

  unknown_schema_version::
  unknown_schema_version (schema_version v)
      : version_ (v)
  {
    ostringstream os;
    os << v;
    what_ = "unknown database schema version ";
    what_ += os.str ();
  }

  unknown_schema_version::
  ~unknown_schema_version () throw ()
  {
  }

  const char* unknown_schema_version::
  what () const throw ()
  {
    return what_.c_str ();
  }

  unknown_schema_version* unknown_schema_version::
  clone () const
  {
    return new unknown_schema_version (*this);
  }

  const char* section_not_loaded::
  what () const throw ()
  {
    return "section is not loaded";
  }

  section_not_loaded* section_not_loaded::
  clone () const
  {
    return new section_not_loaded (*this);
  }

  const char* section_not_in_object::
  what () const throw ()
  {
    return "section instance is not part of an object (section was copied?)";
  }

  section_not_in_object* section_not_in_object::
  clone () const
  {
    return new section_not_in_object (*this);
  }

  // multiple_exceptions
  //
  multiple_exceptions::
  ~multiple_exceptions () throw () {}

  void multiple_exceptions::
  insert (size_t p, const odb::exception& e, bool fatal)
  {
    if (const multiple_exceptions* me =
        dynamic_cast<const multiple_exceptions*> (&e))
    {
      // Multipe exceptions in the batch. Splice them in.
      //
      for (iterator i (me->begin ()); i != me->end (); ++i)
        set_.insert (
          value_type (delta_ + i->position (), i->exception_ptr ()));

      fatal_ = fatal_ || me->fatal () || fatal;
    }
    else
    {
      set_.insert (value_type (delta_ + p, e));
      fatal_ = fatal_ || fatal;
    }
  }

  const odb::exception* multiple_exceptions::
  lookup (size_t p) const
  {
    iterator i (set_.find (value_type (p + delta_)));
    return i == set_.end () ? 0 : &i->exception ();
  }

  void multiple_exceptions::
  prepare ()
  {
    current_ = 0;
    delta_ = 0;

    ostringstream os;
    os << "multiple exceptions, "
       << attempted_ << " element" << (attempted_ != 1 ? "s" : "") <<
      " attempted, "
       << size () << " failed"
       << (fatal_ ? ", fatal" : "") << ":";

    bool nl (true);
    for (iterator i (begin ()); i != end (); ++i)
    {
      const char* w (i->exception ().what ());

      os << (nl ? "\n" : "")
         << '[' << i->position () << "] " << w;

      nl = (w[strlen (w) - 1] != '\n');
    }

    what_ = os.str ();
  }

  const char* multiple_exceptions::
  what () const throw ()
  {
    return what_.c_str ();
  }

  multiple_exceptions* multiple_exceptions::
  clone () const
  {
    return new multiple_exceptions (*this);
  }
}
