#ifndef PDNS_SYNCRES_HH
#define PDNS_SYNCRES_HH
#include <string>
#include "dns.hh"
#include "qtype.hh"
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <iostream>
#include <utility>
#include "misc.hh"
#include "lwres.hh"


/* external functions, opaque to us */

void primeHints(void);

struct NegCacheEntry
{
  string name;
  time_t ttd;
};


template<class Thing> class Throttle
{
public:
  Throttle()
  {
    d_limit=3;
    d_ttl=60;
    d_last_clean=time(0);
  }
  bool shouldThrottle(time_t now, const Thing& t)
  {
    if(now > d_last_clean + 60 ) {
      d_last_clean=now;
      for(typename cont_t::iterator i=d_cont.begin();i!=d_cont.end();) 
	if( i->second.ttd > now) {
	  d_cont.erase(i++);
	}
	else
	  ++i;
    }

    typename cont_t::iterator i=d_cont.find(t);
    if(i==d_cont.end())
      return false;
    if(now > i->second.ttd || i->second.count-- < 0){
      d_cont.erase(i);
      return true;
    }
  }
  void throttle(time_t now, const Thing& t, unsigned int ttl=0, unsigned int tries=0) 
  {
    typename cont_t::iterator i=d_cont.find(t);
    entry e={ now+(ttl ? ttl : d_ttl), tries ? tries : d_limit};

    if(i==d_cont.end()) {
      d_cont[t]=e;
    } 
    else if(i->second.ttd > e.ttd || (i->second.count) < e.count) 
      d_cont[t]=e;

  }
private:
  int d_limit;
  int d_ttl;
  time_t d_last_clean;
  struct entry 
  {
    time_t ttd;
    int count;
  };
  typedef map<Thing,entry> cont_t;
  cont_t d_cont;
};


/** Class that implements a decaying EWMA.
    This class keeps an exponentially weighted moving average which, additionally, decays over time.
    The decaying is only done on get.
*/
class DecayingEwma
{
public:
  DecayingEwma() : d_last(getTime()) , d_lastget(time(0)),  d_val(0.0) {
  }
  void submit(int val) 
  {
    double now=getTime();
    double diff=d_last-now;
    d_last=now;
    double factor=exp(diff)/2.0; // might be '0.5', or 0.0001
    d_val=(1-factor)*val+ factor*d_val; 
  }
  double get()
  {
    double now=getTime();
    double diff=d_lastget-now;
    d_lastget=now;
    double factor=exp(diff/60.0); // is 1.0 or less
    return d_val*=factor;
  }

private:

  double d_last;
  double d_lastget;
  double d_val;
};


class SyncRes
{
public:
  SyncRes() : d_outqueries(0), d_throttledqueries(0), d_timeouts(0), d_cacheonly(false), d_nocache(false), d_now(time(0)) {}
  int beginResolve(const string &qname, const QType &qtype, vector<DNSResourceRecord>&ret);
  void setId(int id)
  {
    d_prefix="["+itoa(id)+"] ";
  }
  static void setLog(bool log)
  {
    s_log=log;
  }
  void setCacheOnly(bool state=true)
  {
    d_cacheonly=state;
  }
  void setNoCache(bool state=true)
  {
    d_nocache=state;
  }
  static unsigned int s_queries;
  static unsigned int s_throttledqueries;
  static unsigned int s_outqueries;
  static unsigned int s_nodelegated;
  unsigned int d_outqueries;
  unsigned int d_throttledqueries;
  unsigned int d_timeouts;
  static map<string,NegCacheEntry> s_negcache;    
  static Throttle<string> s_throttle;
private:
  struct GetBestNSAnswer;
  int doResolveAt(set<string> nameservers, string auth, const string &qname, const QType &qtype, vector<DNSResourceRecord>&ret,
		  int depth, set<GetBestNSAnswer>&beenthere);
  int doResolve(const string &qname, const QType &qtype, vector<DNSResourceRecord>&ret, int depth, set<GetBestNSAnswer>& beenthere);
  bool doCNAMECacheCheck(const string &qname, const QType &qtype, vector<DNSResourceRecord>&ret, int depth, int &res);
  bool doCacheCheck(const string &qname, const QType &qtype, vector<DNSResourceRecord>&ret, int depth, int &res);
  void getBestNSFromCache(const string &qname, set<DNSResourceRecord>&bestns, int depth, set<GetBestNSAnswer>& beenthere);
  void addCruft(const string &qname, vector<DNSResourceRecord>& ret);
  string getBestNSNamesFromCache(const string &qname,set<string>& nsset, int depth, set<GetBestNSAnswer>&beenthere);
  void addAuthorityRecords(const string& qname, vector<DNSResourceRecord>& ret, int depth);

  inline vector<string> shuffle(set<string> &nameservers, const string &prefix);
  bool moreSpecificThan(const string& a, const string &b);
  string getA(const string &qname, int depth, set<GetBestNSAnswer>& beenthere);

  SyncRes(const SyncRes&);
  SyncRes& operator=(const SyncRes&);
private:
  string d_prefix;
  static bool s_log;
  bool d_cacheonly;
  bool d_nocache;
  LWRes d_lwr;
  time_t d_now;

  struct GetBestNSAnswer
  {
    string qname;
    set<DNSResourceRecord> bestns;
    bool operator<(const GetBestNSAnswer &b) const
    {
      if(qname<b.qname)
	return true;
      if(qname==b.qname)
	return bestns<b.bestns;
      return false;
    }
  };

};
#endif
