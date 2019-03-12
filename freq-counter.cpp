#include "freq-counter.hpp"

const std::string freq_counter::FreqCounter::client_name = "freq-counter";
const std::string freq_counter::FreqCounter::client_version = "1.0";
const std::string freq_counter::FreqCounter::client_authors = "Alex Reynolds";

int
main(int argc, char** argv)
{
  freq_counter::FreqCounter fc;
  
  fc.initialize_command_line_options(argc, argv);

  /*
  const std::vector<char>& _v = fc.chars();
  for (std::vector<char>::const_iterator i = _v.begin(); i != _v.end(); ++i)
    std::cout << *i << ' ';
  */

  fc.read_input();

  fc.close_output_streams();

  return EXIT_SUCCESS;
}

void
freq_counter::FreqCounter::read_input(void)
{
  char* buf = NULL;
  size_t buf_len = 0;
  ssize_t buf_read = 0;
  char header_str[LINE_MAX] = {0};
  char* sequence_str = NULL;
  ssize_t sequence_read = 0;
  char* sequence_intermediate_buf = NULL;
  ssize_t line_count = 0;

  sequence_str = (char*) malloc(FREQ_COUNTER_LINE_MAX);
  if (!sequence_str) {
    std::fprintf(stderr, "Error: Could not allocate memory for sequence buffer\n");
    std::exit(ENOMEM);
  }
  
  sequence_intermediate_buf = (char*) malloc(FREQ_COUNTER_LINE_MAX);
  if (!sequence_intermediate_buf) {
    std::fprintf(stderr, "Error: Could not allocate memory for sequence intermediate buffer\n");
    std::exit(ENOMEM);
  }

  while ((buf_read = getline(&buf, &buf_len, this->in_stream())) != EOF) {
    if (buf[0] == '>') {
      if ((strlen(header_str) > 0) && (strlen(sequence_str) > 0)) {
        this->process_fasta_record(header_str, sequence_str);
        sequence_read = 0;
      }
      // read in next header
      std::sscanf(buf, ">%[^ \n\t]\n", header_str);
    }
    else {
      std::sscanf(buf, "%s\n", sequence_intermediate_buf);
      std::memcpy(sequence_str + sequence_read, sequence_intermediate_buf, buf_read);
      sequence_read += (buf_read - 1);
      if (sequence_read >= FREQ_COUNTER_LINE_MAX) {
        std::fprintf(stderr, "Error: Input FASTA record is longer than can be stored in the sequence buffer\n");
        std::exit(EOVERFLOW);
      }
    }
    line_count += 1;
  }

  // process final record
  if ((strlen(header_str) > 0) && (strlen(sequence_str) > 0)) {
    this->process_fasta_record(header_str, sequence_str);
    sequence_read = 0;
  }

  // cleanup
  free(buf);
  free(sequence_str);
  free(sequence_intermediate_buf);
}

void
freq_counter::FreqCounter::process_fasta_record(char* header, char* sequence)
{
  std::string seq(sequence);
  std::string n("N");

  std::transform(seq.begin(), seq.end(), seq.begin(), ::toupper);
  std::deque<char> window(seq.begin(), seq.begin() + this->span());
  std::string mer(window.begin(), window.end());

  for (size_t position = this->span(); position <= seq.length(); position += this->step()) {
    mer = std::string(window.begin(), window.end());
    size_t count = 0;
    for (auto iter = this->chars().begin(); iter != this->chars().end(); ++iter) {
      size_t hits = std::count(mer.begin(), mer.end(), *iter);
      count += hits;        
    }
    std::cout << header << '\t' << position << '\t' << position + this->step() << '\t' << (float)count/this->span() << std::endl;
    for (int i = 0; i < this->step(); ++i) {
      window.pop_front();
      window.push_back(seq[position]);
    }
  }
}

void
freq_counter::FreqCounter::close_output_streams(void)
{
  this->close_in_stream();
}

void
freq_counter::FreqCounter::initialize_command_line_options(int argc, char** argv)
{
  int client_long_index;
  int client_opt = getopt_long(argc,
                               argv,
                               this->client_freq_counter_opt_string().c_str(),
                               this->client_freq_counter_long_options(),
                               &client_long_index);
  
  opterr = 0; /* disable error reporting by GNU getopt */

  int _step = -1;
  int _span = -1;
  std::string _s;
  std::vector<char> _v;
  
  while (client_opt != -1) {
    switch (client_opt) {
    case 't':
      std::sscanf(optarg, "%d", &_step);
      this->step(_step);
      break;
    case 'p':
      std::sscanf(optarg, "%d", &_span);
      this->span(_span);
      break;      
    case 'c':
      _s = optarg;
      _v = std::vector<char>(_s.begin(), _s.end());
      this->chars(_v);
      break;
    case 'h':
      this->print_usage(stdout);
      std::exit(EXIT_SUCCESS);
    case 'v':
      this->print_version(stdout);
      std::exit(EXIT_SUCCESS);
    case '?':
      this->print_usage(stdout);
      std::exit(EXIT_SUCCESS);
    default:
      break;
    }
    client_opt = getopt_long(argc,
                             argv,
                             this->client_freq_counter_opt_string().c_str(),
                             this->client_freq_counter_long_options(),
                             &client_long_index);
  }

  if (optind < argc) {
    do {
      if (this->input_fn().empty()) {
        this->input_fn(argv[optind]);
        this->initialize_in_stream();
      }
      else {
        std::fprintf(stderr, "Warning: Ignoring additional input file [%s]\n", argv[optind]);
      }
    }
    while (++optind < argc);
  }

  if ((_step == -1) || (_span == -1)) {
    std::fprintf(stderr, "Error: Specify step and span parameters\n");
    std::exit(EXIT_FAILURE);
  }

}

std::string
freq_counter::FreqCounter::client_freq_counter_name(void)
{
  static std::string _s(freq_counter::FreqCounter::client_name);
  return _s;
}

std::string
freq_counter::FreqCounter::client_freq_counter_version(void)
{
  static std::string _s(freq_counter::FreqCounter::client_version);
  return _s;
}

std::string
freq_counter::FreqCounter::client_freq_counter_authors(void)
{
  static std::string _s(freq_counter::FreqCounter::client_authors);
  return _s;
}

std::string
freq_counter::FreqCounter::client_freq_counter_usage(void)
{
  static std::string _s("\n"                                            \
                        "  Usage:\n"                                    \
                        "\n"                                            \
                        "  $ freq_counter [options] input\n");
  return _s;
}

std::string
freq_counter::FreqCounter::client_freq_counter_description(void)
{
  static std::string _s("  Count freqs of residues in specified FASTA file\n" \
                        "  over span and step parameters, reporting counts\n" \
                        "  at each output window element\n");
  return _s;
}

std::string
freq_counter::FreqCounter::client_freq_counter_io_options(void)
{
  static std::string _s("  General Options:\n\n"                        \
                        "  --step=n          Window step (integer)\n"   \
                        "  --span=n          Window span (integer)\n"   \
                        "  --chars=c         Input characters (string)\n");
  return _s;
}

std::string
freq_counter::FreqCounter::client_freq_counter_general_options(void)
{
  static std::string _s("  Process Flags:\n\n"                          \
                        "  --help                  Show this usage message\n" \
                        "  --version               Show binary version\n");
  return _s;
}

void
freq_counter::FreqCounter::print_usage(FILE* wo_stream)
{
  std::fprintf(wo_stream,
               "%s\n"                                                \
               "  version: %s\n"                                     \
               "  author:  %s\n"                                     \
               "%s\n"                                                \
               "%s\n"                                                \
               "%s\n"                                                \
               "%s\n",
               this->client_freq_counter_name().c_str(),
               this->client_freq_counter_version().c_str(),
               this->client_freq_counter_authors().c_str(),
               this->client_freq_counter_usage().c_str(),
               this->client_freq_counter_description().c_str(),
               this->client_freq_counter_io_options().c_str(),
               this->client_freq_counter_general_options().c_str());
}

void
freq_counter::FreqCounter::print_version(FILE* wo_stream)
{
    std::fprintf(wo_stream,
                 "%s\n"                                              \
                 "  version: %s\n"                                   \
                 "  author:  %s\n",
                 this->client_freq_counter_name().c_str(),
                 this->client_freq_counter_version().c_str(),
                 this->client_freq_counter_authors().c_str());
}

std::string
freq_counter::FreqCounter::client_freq_counter_opt_string(void)
{
    static std::string _s("t:p:c:hv?");
    return _s;
}

struct option*
freq_counter::FreqCounter::client_freq_counter_long_options(void)
{
    static struct option _t = { "step",           required_argument,   NULL,    't' };
    static struct option _p = { "span",           required_argument,   NULL,    'p' };
    static struct option _c = { "chars",          required_argument,   NULL,    'c' };
    static struct option _h = { "help",           no_argument,         NULL,    'h' };
    static struct option _v = { "version",        no_argument,         NULL,    'v' };
    static struct option _0 = { NULL,             no_argument,         NULL,     0  };
    static std::vector<struct option> _s;
    _s.push_back(_t);
    _s.push_back(_p);
    _s.push_back(_c);
    _s.push_back(_h);
    _s.push_back(_v);
    _s.push_back(_0);
    return &_s[0];
}
