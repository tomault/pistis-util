#ifndef __PISTIS__UTIL__NAMEMAP_HPP__
#define __PISTIS__UTIL__NAMEMAP_HPP__

#include <pistis/exceptions/IllegalValueError.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace pistis {
  namespace util {

    template <typename Value, typename HashValueFn= std::hash<Value>,
	      typename ValueEqualFn= typename std::equal_to<Value> >
    class NameMap {
    public:
      NameMap(const std::string& valueTypeName):
	  valueTypeName_(valueTypeName) {
	// Intentionally left blank
      }

      template <typename Iterator>
      NameMap(const std::string& valueTypeName,
	      Iterator startOfMapping, Iterator endOfMapping):
	  valueTypeName_(valueTypeName) {
	for (auto i= startOfMapping; i != endOfMapping; ++i) {
	  add(i->second, i->first);
	}
      }

      NameMap(
          const std::string& valueTypeName,
	  const std::initializer_list<std::pair<Value, std::string>>& mapping
      ):
	  valueTypeName_(valueTypeName) {
	for (auto i= mapping.begin(); i != mapping.end(); ++i) {
	  add(i->second, i->first);
	}
      }

      const std::string& valueTypeName() const { return valueTypeName_; }
      uint32_t nameCount() const { return nameToValue_.size(); }
      uint32_t valueCount() const { return valueToNames_.size(); }
      
      void add(const std::string& name, const Value& value) {
	addToNameMap_(name, value);

	auto i= valueToNames_.find(value);
	if (i != valueToNames_.end()) {
	  i->second.push_back(name);
	} else {
	  valueToNames_.emplace(value, std::vector<std::string>{ name });
	}
      }
	
      void add(std::string&& name, Value&& value) {
	addToNameMap_(name, value);
	nameToValue_.emplace(name, value);
	  
	auto i= valueToNames_.find(value);
	if (i != valueToNames_.end()) {
	  i->second.push_back(std::move(name));
	} else {
	  valueToNames_.emplace(std::move(value), std::vector<std::string>{ std::move(name) });
	}
      }

      const std::vector<std::string>& allNamesFor(const Value& value) const {
	static const std::vector<std::string> NO_NAMES;
	auto i= valueToNames_.find(value);
	return (i != valueToNames_.end()) ? i->second : NO_NAMES;
      }

      template <typename NoValueFn>
      Value getWithDefaultFn(const std::string& name,
			     NoValueFn noValue) const {
	auto i= nameToValue_.find(name);
	return (i != nameToValue_.end()) ? i->second
	                                 : noValue(const_cast<NameMap*>(this),
						   name);
      }

      Value get(const std::string& name) const {
	auto i= nameToValue_.find(name);
	return (i != nameToValue_.end()) ? i->second : Value();
      }

      Value get(const std::string& name, const Value& defaultValue) const {
	auto i= nameToValue_.find(name);
	return (i != nameToValue_.end()) ? i->second : defaultValue;
      }

      bool get(const std::string& name, Value& v) {
	auto i= nameToValue_.find(name);
	if (i == nameToValue_.end()) {
	  return false;
	}
	v= i->second;
	return true;
      }

      template <typename NoValueFn>
      std::string getNameWithDefaultFn(const Value& v,
				       NoValueFn noValue) const {
	auto i= valueToNames_.find(v);
	return (i != valueToNames_.end()) ? i->second.front()
	                                  : noValue(const_cast<NameMap*>(this),
						    v);
      }

      const std::string& getName(const Value& v,
				 const std::string& defaultName) const {
	auto i= valueToNames_.find(v);
	return (i != valueToNames_.end()) ? i->second.front() : defaultName;
      }

      const std::string& getName(const Value& v) const {
	static const std::string NO_NAME;
	return getName(v, NO_NAME);
      }

      bool getName(const Value& v, std::string& name) const {
	auto i= valueToNames_.find(v);
	if (i == valueToNames_.end()) {
	  return false;
	}
	name= i->second.front();
	return true;
      }

      Value getRequired(const std::string& name) const {
	return getWithDefaultFn(name, [](const NameMap* map,
					 const std::string& name) -> Value {
	  std::ostringstream msg;
	  msg << "Unknown " << map->valueTypeName() << " \"" << name << "\"";
	  throw exceptions::IllegalValueError(msg.str(), PISTIS_EX_HERE);
	});
      }

      const std::string& getRequiredName(const Value& v) const {
	auto i= valueToNames_.find(v);
	if (i == valueToNames_.end()) {
	  throw exceptions::IllegalValueError("Value has no name",
					      PISTIS_EX_HERE);
	}
	return i->second.front();
      }

    protected:
      void addToNameMap_(const std::string& name, Value value) {
	if (nameToValue_.find(name) != nameToValue_.end()) {
	  std::stringstream msg;
	  msg << "Name \"" << name << "\" already has a mapping";
	  throw exceptions::IllegalValueError(msg.str(), PISTIS_EX_HERE);
	}
	nameToValue_.emplace(name, value);
      }

    private:
      std::string valueTypeName_;
      std::unordered_map<std::string, Value> nameToValue_;
      std::unordered_map<
          Value, std::vector<std::string>, HashValueFn, ValueEqualFn
      > valueToNames_;
    };

  }
}
#endif

