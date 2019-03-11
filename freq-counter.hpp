#ifndef FREQ_COUNTER_H_
#define FREQ_COUNTER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif /* getline() support */
  
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdio>
#include <climits>
#include <cinttypes>
#include <cstring>
#include <getopt.h>
#include <sys/stat.h>

/* longest chromosome in hg38, times four */
#define FREQ_COUNTER_LINE_MAX 995825688

namespace freq_counter
{
  class FreqCounter
  {
  private:
    int _step;
    int _span;
    std::vector<char> _chars;
    std::string _input_fn;
    FILE* _in_stream;

  public:
    void initialize_command_line_options(int argc, char** argv);
    void read_input(void);
    void process_fasta_record(char* header, char* sequence);
    void close_output_streams(void);

    static const std::string client_name;
    static const std::string client_version;
    static const std::string client_authors;

    std::string client_freq_counter_opt_string(void);
    struct option* client_freq_counter_long_options(void);
    std::string client_freq_counter_name(void);
    std::string client_freq_counter_version(void);
    std::string client_freq_counter_authors(void);
    std::string client_freq_counter_usage(void);
    std::string client_freq_counter_description(void);
    std::string client_freq_counter_io_options(void);
    std::string client_freq_counter_general_options(void);
    void print_usage(FILE* wo_stream);
    void print_version(FILE* wo_stream);

    const int& step(void);
    void step(const int& k);

    const int& span(void);
    void span(const int& k);

    const std::vector<char>& chars(void);
    void chars(const std::vector<char>& v);

    const std::string& input_fn(void);
    void input_fn(const std::string& s);

    FILE* in_stream(void);
    void in_stream(FILE** ri_stream_ptr);
    void initialize_in_stream(void);
    void close_in_stream(void);
    
    FreqCounter();
    ~FreqCounter();
  };

  FreqCounter::FreqCounter() {
  }

  FreqCounter::~FreqCounter() {
  }  

  const int& FreqCounter::step(void) { return _step; }
  void FreqCounter::step(const int& step) { _step = step; }
  
  const int& FreqCounter::span(void) { return _span; }
  void FreqCounter::span(const int& span) { _span = span; }
  
  const std::vector<char>& FreqCounter::chars(void) { return _chars; }
  void FreqCounter::chars(const std::vector<char>& v) { _chars = v; }

  FILE* FreqCounter::in_stream(void) { return _in_stream; }
  void FreqCounter::in_stream(FILE** isp) { _in_stream = *isp; }
  void FreqCounter::initialize_in_stream(void) {
    FILE* in_fp = NULL;
    in_fp = this->input_fn().empty() ? stdin : std::fopen(this->input_fn().c_str(), "r");
    if (!in_fp) {
      std::fprintf(stderr, "Error: Input file handle could not be created\n");
      std::exit(ENODATA); /* No message is available on the STREAM head read queue (POSIX.1) */
    }
    this->in_stream(&in_fp);
  }
  void FreqCounter::close_in_stream(void) {
    if (!this->input_fn().empty()) {
      std::fclose(this->in_stream());
    }
  }
  
  const std::string& FreqCounter::input_fn(void) { return _input_fn; }
  void FreqCounter::input_fn(const std::string& s) {
    struct stat s_stat;
    if (stat(s.c_str(), &s_stat) == 0) {
      _input_fn = s;
    }
    else {
      std::fprintf(stderr, "Error: Input file does not exist (%s)\n", s.c_str());
      std::exit(ENODATA); /* No message is available on the STREAM head read queue (POSIX.1) */
    }
  }
  
}

#endif // FREQ_COUNTER_H_

