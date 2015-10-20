/*******************************************************************\

Module: Unwinding the Properties

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <cstdlib>

#include <util/namespace.h>
#include <util/cout_message.h>

#include "instantiate.h"
#include "property_netlist.h"

/*******************************************************************\

Function: property

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void property(
  const std::list<exprt> &properties,
  std::list<bvt> &prop_bv,
  message_handlert &message_handler,
  propt &solver,
  const bmc_mapt &map,
  const namespacet &ns)
{
  messaget message(message_handler);

  if(properties.size()==1)
    message.status() << "Adding property" << messaget::eom;
  else
    message.status() << "Adding " << properties.size() 
                     << " properties" << messaget::eom;

  prop_bv.clear();
  bvt all_prop;

  for(std::list<exprt>::const_iterator
      it=properties.begin();
      it!=properties.end();
      it++)
  {
    if(it->is_true())
    {
      prop_bv.push_back(bvt());
      prop_bv.back().resize(map.get_no_timeframes(), const_literal(true));
      continue;
    }
  
    exprt property(*it);

    if(property.id()!=ID_AG ||
       property.operands().size()!=1)
    {
      message.error() << "unsupported property - only AGp implemented" 
                      << messaget::eom;
      exit(1);
    }

    const exprt &p=property.op0();
    
    prop_bv.push_back(bvt());

    for(unsigned c=0; c<map.get_no_timeframes(); c++)
    {
      literalt l=instantiate_convert(solver, map, p, c, c+1, ns, message_handler);
      prop_bv.back().push_back(l);
      all_prop.push_back(solver.lnot(l));
    }
  }

  solver.lcnf(all_prop);
}

