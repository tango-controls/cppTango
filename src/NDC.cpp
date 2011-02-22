/*
 * NDC.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifdef LOG4TANGO_HAS_NDC

#include "PortabilityImpl.hh"
#include <log4tango/NDC.hh>
#include <log4tango/threading/Threading.hh>

namespace log4tango {

NDC::DiagnosticContext::DiagnosticContext (const std::string& message) 
  : message(message),
    full_msg(message) 
{
  //no-op
}

NDC::DiagnosticContext::DiagnosticContext (const std::string& message, 
                                           const DiagnosticContext& parent) 
 : message(message),
   full_msg(parent.full_msg + " " + message) 
{
  //no-op
}

namespace {
  threading::ThreadLocalDataHolder<NDC> _nDC;
}

void NDC::clear() 
{
  get_ndc()._clear();
}

NDC::ContextStack* NDC::clone_stack() 
{
  return get_ndc()._clone_stack();
}

const std::string& NDC::get (void) 
{
  return get_ndc()._get();
}

int NDC::get_depth (void) 
{
  return get_ndc()._get_depth();
}

void NDC::inherit (NDC::ContextStack* stack) 
{
  get_ndc()._inherit(stack);
}

std::string NDC::pop (void) 
{
  return get_ndc()._pop();
}

void NDC::push (const std::string& message) 
{
  get_ndc()._push(message);
}

void NDC::set_max_depth(int max_depth) 
{
  get_ndc()._set_max_depth(max_depth);
}

NDC& NDC::get_ndc (void) 
{
  NDC* nDC = _nDC.get();
  if (!nDC) {
    nDC = new NDC();
    _nDC.reset(nDC);
  }
  return *nDC;
}

NDC::NDC () 
{
  //no-op
}

NDC::~NDC () 
{
  //no-op
}

void NDC::_clear (void)
{
  _stack.clear();
}

NDC::ContextStack* NDC::_clone_stack (void) 
{
  return new ContextStack(_stack);
}

const std::string& NDC::_get (void) const 
{
  static std::string empty = "";
  return (_stack.empty() ? empty : _stack.back().full_msg);
}

int NDC::_get_depth (void) const 
{
  return _stack.size();
}

void NDC::_inherit (NDC::ContextStack* stack) 
{
  _stack = *stack;
}

std::string NDC::_pop (void) 
{
  std::string result = _stack.back().message;
  _stack.pop_back();
  return result;
}

void NDC::_push (const std::string& message) 
{
  if (_stack.empty()) {
      _stack.push_back(DiagnosticContext(message));
  } else {            
      _stack.push_back(DiagnosticContext(message, _stack.back()));
  }
}

void NDC::_set_max_depth (int max_depth) {
  // no maximum
}

}  // namespace log4tango

#endif // LOG4TANGO_HAS_NDC
