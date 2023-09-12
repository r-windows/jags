#include <config.h>
#include <compiler/CounterTab.h>

#include <string>
#include <stdexcept>

using std::vector;
using std::pair;
using std::string;
using std::runtime_error;

namespace jags {

    CounterTab::CounterTab()
    {
    }

    CounterTab::~CounterTab()
    {
	unsigned long n = _table.size();
	for (unsigned int i = 0; i < n; i++) {
	    popCounter();
	}
    }

    Counter * CounterTab::pushCounter(string const &name, vector<unsigned long> const &index_range)
    {
	/* Check for duplicated counters */
	for (auto p = _table.begin(); p != _table.end(); ++p) {
	    if (p->first == name) {
		string msg = "Nested for loops with the same counter: " + name;
		throw runtime_error(msg);
	    }
	}
	
	Counter *counter = new Counter(index_range);
	pair<string, Counter*> cpair(name, counter);
	_table.push_back(cpair);
	return counter;
    }

    void CounterTab::popCounter()
    {
	pair<string, Counter *>  &topcpair = _table.back();
	delete topcpair.second;
	_table.pop_back();
    }

    Counter *CounterTab::getCounter(string const &name) const
    {
	for (auto p = _table.begin(); p != _table.end(); ++p) {
	    if (name == p->first)
		return p->second;
	}
	return nullptr;
    }

    vector<unsigned long> CounterTab::counterValues() const
    {
	vector<unsigned long> indices;
	for (auto p = _table.begin(); p != _table.end(); ++p) {
	    indices.push_back(p->second->value());
	}
	return indices;
    }
    
} //namespace jags
