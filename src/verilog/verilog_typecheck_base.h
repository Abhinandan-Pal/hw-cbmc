/*******************************************************************\

Module: Verilog Type Checker Base

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#ifndef CPROVER_VERILOG_TYPECHEK_BASE_H
#define CPROVER_VERILOG_TYPECHEK_BASE_H

#include <util/namespace.h>
#include <util/typecheck.h>
#include <util/mp_arith.h>

irep_idt verilog_module_symbol(const irep_idt &base_name);
irep_idt verilog_module_name(const irep_idt &identifier);
irep_idt strip_verilog_prefix(const irep_idt &identifier);

class array_typet;

class verilog_typecheck_baset:public typecheckt
{
public:
  verilog_typecheck_baset(
    const namespacet &_ns,
    message_handlert &_message_handler):
    typecheckt(_message_handler),
    ns(_ns),
    mode(ID_Verilog)
  { }

  // overloaded to use verilog syntax
  
  std::string to_string(const typet &type);
  std::string to_string(const exprt &expr);

protected:
  const namespacet &ns;
  const irep_idt mode;
  
  std::size_t get_width(const exprt &expr) { return get_width(expr.type()); }
  std::size_t get_width(const typet &type);
  mp_integer array_size(const array_typet &);
  mp_integer array_offset(const array_typet &);
  typet index_type(const array_typet &);

public:
  class errort final
  {
  public:
    std::string what() const
    {
      return message.str();
    }

    std::ostringstream &message_ostream()
    {
      return message;
    }

    errort with_location(source_locationt _location) &&
    {
      __location = std::move(_location);
      return std::move(*this);
    }

    const source_locationt &source_location() const
    {
      return __location;
    }

  protected:
    std::ostringstream message;
    source_locationt __location = source_locationt::nil();

    template <typename T>
    friend errort operator<<(errort &&e, const T &);
  };
};

template <typename T>
verilog_typecheck_baset::errort
operator<<(verilog_typecheck_baset::errort &&e, const T &message)
{
  e.message_ostream() << message;
  return std::move(e);
}

#endif
