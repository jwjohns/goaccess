/**
 * options.c -- functions related to parsing program options
 *    ______      ___
 *   / ____/___  /   | _____________  __________
 *  / / __/ __ \/ /| |/ ___/ ___/ _ \/ ___/ ___/
 * / /_/ / /_/ / ___ / /__/ /__/  __(__  |__  )
 * \____/\____/_/  |_\___/\___/\___/____/____/
 *
 * The MIT License (MIT)
 * Copyright (c) 2009-2016 Gerardo Orellana <hello @ goaccess.io>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#ifdef HAVE_LIBTOKYOCABINET
#include "tcabdb.h"
#include "tcbtdb.h"
#endif

#include "options.h"

#include "error.h"
#include "util.h"

static char short_options[] = "f:e:p:o:l:H:M:"
#ifdef HAVE_LIBGEOIP
  "g"
#endif
  "acirmMhHqdsV";

/* *INDENT-OFF* */
struct option long_opts[] = {
  {"agent-list"           , no_argument       , 0 , 'a' } ,
  {"config-dialog"        , no_argument       , 0 , 'c' } ,
  {"config-file"          , required_argument , 0 , 'p' } ,
  {"debug-file"           , required_argument , 0 , 'l' } ,
  {"exclude-ip"           , required_argument , 0 , 'e' } ,
#ifdef HAVE_LIBGEOIP
  {"std-geoip"            , no_argument       , 0 , 'g' } ,
#endif
  {"help"                 , no_argument       , 0 , 'h' } ,
  {"hl-header"            , no_argument       , 0 , 'i' } ,
  {"http-method"          , required_argument , 0 , 'M' } ,
  {"http-protocol"        , required_argument , 0 , 'H' } ,
  {"log-file"             , required_argument , 0 , 'f' } ,
  {"no-query-string"      , no_argument       , 0 , 'q' } ,
  {"no-term-resolver"     , no_argument       , 0 , 'r' } ,
  {"output-format"        , required_argument , 0 , 'o' } ,
  {"storage"              , no_argument       , 0 , 's' } ,
  {"version"              , no_argument       , 0 , 'V' } ,
  {"with-mouse"           , no_argument       , 0 , 'm' } ,
  {"with-output-resolver" , no_argument       , 0 , 'd' } ,
  {"444-as-404"           , no_argument       , 0 ,  0  } ,
  {"4xx-to-unique-count"  , no_argument       , 0 ,  0  } ,
  {"addr"                 , required_argument , 0 ,  0  } ,
  {"all-static-files"     , no_argument       , 0 ,  0  } ,
  {"color"                , required_argument , 0 ,  0  } ,
  {"color-scheme"         , required_argument , 0 ,  0  } ,
  {"date-format"          , required_argument , 0 ,  0  } ,
  {"date-spec"            , required_argument , 0 ,  0  } ,
  {"dcf"                  , no_argument       , 0 ,  0  } ,
  {"double-decode"        , no_argument       , 0 ,  0  } ,
  {"enable-panel"         , required_argument , 0 ,  0  } ,
  {"hour-spec"            , required_argument , 0 ,  0  } ,
  {"html-custom-css"      , required_argument , 0 ,  0  } ,
  {"html-custom-js"       , required_argument , 0 ,  0  } ,
  {"html-report-title"    , required_argument , 0 ,  0  } ,
  {"ignore-crawlers"      , no_argument       , 0 ,  0  } ,
  {"ignore-panel"         , required_argument , 0 ,  0  } ,
  {"ignore-referer"       , required_argument , 0 ,  0  } ,
  {"ignore-status"        , required_argument , 0 ,  0  } ,
  {"invalid-requests"     , required_argument , 0 ,  0  } ,
  {"json-pretty-print"    , no_argument       , 0 ,  0  } ,
  {"log-format"           , required_argument , 0 ,  0  } ,
  {"max-items"            , required_argument , 0 ,  0  } ,
  {"no-color"             , no_argument       , 0 ,  0  } ,
  {"no-column-names"      , no_argument       , 0 ,  0  } ,
  {"no-csv-summary"       , no_argument       , 0 ,  0  } ,
  {"no-global-config"     , no_argument       , 0 ,  0  } ,
  {"no-progress"          , no_argument       , 0 ,  0  } ,
  {"no-tab-scroll"        , no_argument       , 0 ,  0  } ,
  {"origin"               , required_argument , 0 ,  0  } ,
  {"output"               , required_argument , 0 ,  0  } ,
  {"port"                 , required_argument , 0 ,  0  } ,
  {"real-os"              , no_argument       , 0 ,  0  } ,
  {"real-time-html"       , no_argument       , 0 ,  0  } ,
  {"sort-panel"           , required_argument , 0 ,  0  } ,
  {"static-file"          , required_argument , 0 ,  0  } ,
  {"time-format"          , required_argument , 0 ,  0  } ,
  {"ws-url"               , required_argument , 0 ,  0  } ,
#ifdef HAVE_LIBGEOIP
  {"geoip-database"       , required_argument , 0 ,  0  } ,
  {"geoip-city-data"      , required_argument , 0 ,  0  } ,
#endif
#ifdef TCB_BTREE
  {"cache-lcnum"          , required_argument , 0 ,  0  } ,
  {"cache-ncnum"          , required_argument , 0 ,  0  } ,
  {"compression"          , required_argument , 0 ,  0  } ,
  {"db-path"              , required_argument , 0 ,  0  } ,
  {"keep-db-files"        , no_argument       , 0 ,  0  } ,
  {"load-from-disk"       , no_argument       , 0 ,  0  } ,
  {"tune-bnum"            , required_argument , 0 ,  0  } ,
  {"tune-lmemb"           , required_argument , 0 ,  0  } ,
  {"tune-nmemb"           , required_argument , 0 ,  0  } ,
  {"xmmap"                , required_argument , 0 ,  0  } ,
#endif
  {0, 0, 0, 0}
};

/* Command line help. */
void
cmd_help (void)
{
  printf ("\nGoAccess - %s\n\n", GO_VERSION);
  printf (
  "Usage: "
  "goaccess [ options ... ] -f log_file [-c][-M][-H][-q][-d][...]\n"
  "The following options can also be supplied to the command:\n\n"

  /* Log & Date Format Options */
  "Log & Date Format Options\n\n"
  "  --date-format=<dateformat>      - Specify log date format. e.g., %%d/%%b/%%Y\n"
  "  --log-format=<logformat>        - Specify log format. Inner quotes need to be\n"
  "                                    escaped, or use single quotes.\n"
  "  --time-format=<timeformat>      - Specify log time format. e.g., %%H:%%M:%%S\n\n"

  /* User Interface Options */
  "User Interface Options\n\n"
  "  -c --config-dialog              - Prompt log/date/time configuration window.\n"
  "  -i --hl-header                  - Color highlight active panel.\n"
  "  -m --with-mouse                 - Enable mouse support on main dashboard.\n"
  "  --color=<fg:bg[attrs, PANEL]>   - Specify custom colors. See manpage for more\n"
  "                                    details and options.\n"
  "  --color-scheme=<1|2|3>          - Schemes: 1 => Grey, 2 => Green, 3 => Monokai.\n"
  "  --html-custom-css=<path.css>    - Specify a custom CSS file in the HTML report.\n"
  "  --html-custom-js=<path.js>      - Specify a custom JS file in the HTML report.\n"
  "  --html-report-title=<title>     - Set HTML report page title and header.\n"
  "  --json-pretty-print             - Format JSON output w/ tabs & newlines.\n"
  "  --max-items                     - Maximum number of items to show per panel.\n"
  "                                    See man page for limits.\n"
  "  --no-color                      - Disable colored output.\n"
  "  --no-column-names               - Don't write column names in term output.\n"
  "  --no-csv-summary                - Disable summary metrics on the CSV output.\n"
  "  --no-progress                   - Disable progress metrics.\n"
  "  --no-tab-scroll                 - Disable scrolling through panels on TAB.\n"
  "\n"

  /* Server Options */
  "Server Options\n\n"
  "  --addr=<addr>                   - Specify IP address to bind server to.\n"
  "  --origin=<addr>                 - Ensure clients send the specified origin header\n"
  "                                    upon the WebSocket handshake.\n"
  "  --port=<port>                   - Specify the port to use.\n"
  "  --real-time-html                - Enable real-time HTML output.\n"
  "  --ws-url=<url>                  - URL to which the WebSocket server responds.\n"
  "\n"

  /* File Options */
  "File Options\n\n"
  "  -f --log-file=<filename>        - Path to input log file.\n"
  "  -l --debug-file=<filename>      - Send all debug messages to the specified\n"
  "                                    file.\n"
  "  -p --config-file=<filename>     - Custom configuration file.\n"
  "  --invalid-requests=<filename>   - Log invalid requests to the specified file.\n"
  "  --no-global-config              - Don't load global configuration file.\n"
  "\n"

  /* Parse Options */
  "Parse Options\n\n"
  "  -a --agent-list                 - Enable a list of user-agents by host.\n"
  "  -d --with-output-resolver       - Enable IP resolver on HTML|JSON output.\n"
  "  -e --exclude-ip=<IP>            - Exclude one or multiple IPv4/6. Allows IP\n"
  "                                    ranges e.g. 192.168.0.1-192.168.0.10\n"
  "  -H --http-protocol=<yes|no>     - Set/unset HTTP request protocol if found.\n"
  "  -M --http-method=<yes|no>       - Set/unser HTTP request method if found.\n"
  "  -o --output=file.html|json|csv  - Output either an HTML, JSON or a CSV file.\n"
  "  -q --no-query-string            - Ignore request's query string. Removing the\n"
  "                                    query string can greatly decrease memory\n"
  "                                    consumption.\n"
  "  -r --no-term-resolver           - Disable IP resolver on terminal output.\n"
  "  --444-as-404                    - Treat non-standard status code 444 as 404.\n"
  "  --4xx-to-unique-count           - Add 4xx client errors to the unique visitors\n"
  "                                    count.\n"
  "  --all-static-files              - Include static files with a query string.\n"
  "  --date-spec=<date|hr>           - Date specificity. Possible values: `date`\n"
  "                                    (default), or `hr`.\n"
  "  --double-decode                 - Decode double-encoded values.\n"
  "  --enable-panel=<PANEL>          - Enable parsing/displaying the given panel.\n"
  "  --hour-spec=<hr|min>            - Hour specificity. Possible values: `hr`\n"
  "                                    (default), or `min` (tenth of a min).\n"
  "  --ignore-crawlers               - Ignore crawlers.\n"
  "  --ignore-panel=<PANEL>          - Ignore parsing/displaying the given panel.\n"
  "  --ignore-referer=<NEEDLE>       - Ignore a referer from being counted. Wild cards\n"
  "                                    are allowed. i.e., *.bing.com\n"
  "  --ignore-status=<CODE>          - Ignore parsing the given status code.\n"
  "  --real-os                       - Display real OS names. e.g, Windows XP, Snow\n"
  "                                    Leopard.\n"
  "  --sort-panel=PANEL,METRIC,ORDER - Sort panel on initial load. For example:\n"
  "                                    --sort-panel=VISITORS,BY_HITS,ASC. See\n"
  "                                    manpage for a list of panels/fields.\n"
  "  --static-file=<extension>       - Add static file extension. e.g.: .mp3.\n"
  "                                    Extensions are case sensitive.\n"
  "\n"

/* GeoIP Options */
#ifdef HAVE_LIBGEOIP
  "GeoIP Options\n\n"
  "  -g --std-geoip                  - Standard GeoIP database for less memory\n"
  "                                    consumption.\n"
  "  --geoip-database=<path>         - Specify path to GeoIP database file. i.e.,\n"
  "                                    GeoLiteCity.dat, GeoIPv6.dat ...\n"
  "\n"
#endif

/* On-Disk Database Options */
#ifdef TCB_BTREE
  "On-Disk Database Options\n\n"
  "  --keep-db-files                 - Persist parsed data into disk.\n"
  "  --load-from-disk                - Load previously stored data from disk.\n"
  "  --db-path=<path>                - Path of the database file. Default [%s]\n"
  "  --cache-lcnum=<number>          - Max number of leaf nodes to be cached. Default\n"
  "                                    [%d]\n"
  "  --cache-ncnum=<number>          - Max number of non-leaf nodes to be cached.\n"
  "                                    Default [%d]\n"
  "  --tune-bnum=<number>            - Number of elements of the bucket array. Default\n"
  "                                    [%d]\n"
  "  --tune-lmemb=<number>           - Number of members in each leaf page. Default\n"
  "                                    [%d]\n"
  "  --tune-nmemb=<number>           - Number of members in each non-leaf page.\n"
  "                                    Default [%d]\n"
  "  --xmmap=<number>                - Set the size in bytes of the extra mapped\n"
  "                                    memory. Default [%d]\n"
#if defined(HAVE_ZLIB) || defined(HAVE_BZ2)
  "  --compression=<zlib|bz2>        - Specifies that each page is compressed with\n"
  "                                    ZLIB|BZ2 encoding.\n"
  "\n"
#endif
#endif

/* Other Options */
  "Other Options\n\n"
  "  -h --help                       - This help.\n"
  "  -V --version                    - Display version information and exit.\n"
  "  -s --storage                    - Display current storage method. e.g., B+\n"
  "                                    Tree, Hash.\n"
  "  --dcf                           - Display the path of the default config\n"
  "                                    file when `-p` is not used.\n"
  "\n"

  "Examples can be found by running `man goaccess`.\n\n"
  "For more details visit: http://goaccess.io\n"
  "GoAccess Copyright (C) 2009-2016 by Gerardo Orellana"
  "\n\n"
#ifdef TCB_BTREE
  , TC_DBPATH, TC_MMAP, TC_LCNUM, TC_NCNUM, TC_LMEMB, TC_NMEMB, TC_BNUM
#endif
  );
  exit (EXIT_FAILURE);
}
/* *INDENT-ON* */

/* Determine if the '--no-global-config' command line option needs to be
 * enabled or not. */
void
verify_global_config (int argc, char **argv)
{
  int o, idx = 0;

  conf.load_global_config = 1;
  while ((o = getopt_long (argc, argv, short_options, long_opts, &idx)) >= 0) {
    if (-1 == o || EOF == o)
      break;

    switch (o) {
    case 'p':
      conf.iconfigfile = optarg;
      break;
    case 0:
      if (!strcmp ("no-global-config", long_opts[idx].name))
        conf.load_global_config = 0;
      break;
    case '?':
      exit (EXIT_FAILURE);
    }
  }
  for (idx = optind; idx < argc; idx++)
    cmd_help ();

  /* reset it to 1 */
  optind = 1;
}

/* Parse command line long options. */
static void
parse_long_opt (const char *name, const char *oarg)
{
  if (!strcmp ("no-global-config", name))
    return;

  /* colors */
  if (!strcmp ("color", name) && conf.color_idx < MAX_CUSTOM_COLORS)
    conf.colors[conf.color_idx++] = oarg;

  /* color scheme */
  if (!strcmp ("color-scheme", name))
    conf.color_scheme = atoi (oarg);

  /* log format */
  if (!strcmp ("log-format", name) && !conf.log_format)
    set_log_format_str (oarg);

  /* time format */
  if (!strcmp ("time-format", name))
    set_time_format_str (oarg);

  /* date format */
  if (!strcmp ("date-format", name))
    set_date_format_str (oarg);

  /* invalid requests */
  if (!strcmp ("invalid-requests", name)) {
    conf.invalid_requests_log = oarg;
    invalid_log_open (conf.invalid_requests_log);
  }

  /* static file */
  if (!strcmp ("static-file", name) && conf.static_file_idx < MAX_EXTENSIONS) {
    if (conf.static_file_max_len < strlen (oarg))
      conf.static_file_max_len = strlen (oarg);
    conf.static_files[conf.static_file_idx++] = oarg;
  }
  /* 4xx to unique count */
  if (!strcmp ("4xx-to-unique-count", name))
    conf.client_err_to_unique_count = 1;

  /* html report title */
  if (!strcmp ("html-report-title", name))
    conf.html_report_title = oarg;

  /* html custom CSS */
  if (!strcmp ("html-custom-css", name))
    conf.html_custom_css = oarg;

  /* html custom JS */
  if (!strcmp ("html-custom-js", name))
    conf.html_custom_js = oarg;

  /* 444 as 404 */
  if (!strcmp ("444-as-404", name))
    conf.code444_as_404 = 1;

  /* all static files */
  if (!strcmp ("all-static-files", name))
    conf.all_static_files = 1;

  /* enable panel */
  if (!strcmp ("enable-panel", name) && conf.enable_panel_idx < TOTAL_MODULES) {
    if (!str_inarray (oarg, conf.enable_panels, conf.enable_panel_idx))
      conf.enable_panels[conf.enable_panel_idx++] = oarg;
  }

  /* ignore crawlers */
  if (!strcmp ("ignore-crawlers", name))
    conf.ignore_crawlers = 1;

  /* ignore status code */
  if (!strcmp ("ignore-status", name) &&
      conf.ignore_status_idx < MAX_IGNORE_STATUS) {
    if (!str_inarray (oarg, conf.ignore_status, conf.ignore_status_idx))
      conf.ignore_status[conf.ignore_status_idx++] = oarg;
  }

  /* ignore panel */
  if (!strcmp ("ignore-panel", name) && conf.ignore_panel_idx < TOTAL_MODULES) {
    if (!str_inarray (oarg, conf.ignore_panels, conf.ignore_panel_idx))
      conf.ignore_panels[conf.ignore_panel_idx++] = oarg;
  }

  /* ignore referer */
  if (!strcmp ("ignore-referer", name) &&
      conf.ignore_referer_idx < MAX_IGNORE_REF)
    conf.ignore_referers[conf.ignore_referer_idx++] = oarg;

  /* sort view */
  if (!strcmp ("sort-panel", name) && conf.sort_panel_idx < TOTAL_MODULES)
    conf.sort_panels[conf.sort_panel_idx++] = oarg;

  /* output file */
  if (!strcmp ("output", name) && conf.output_format_idx < MAX_OUTFORMATS)
    conf.output_formats[conf.output_format_idx++] = optarg;

  /* real os */
  if (!strcmp ("real-os", name))
    conf.real_os = 1;

  /* real time HTML */
  if (!strcmp ("real-time-html", name))
    conf.real_time_html = 1;

  /* address to bind to */
  if (!strcmp ("ws-url", name))
    conf.ws_url = oarg;

  /* address to bind to */
  if (!strcmp ("addr", name))
    conf.addr = oarg;

  /* WebSocket origin */
  if (!strcmp ("origin", name))
    conf.origin = oarg;

  /* port to use */
  if (!strcmp ("port", name)) {
    int port = strtol (oarg, NULL, 10);
    if (port < 0 || port > 65535)
      LOG_DEBUG (("Invalid port."));
    else
      conf.port = oarg;
  }

  /* double decode */
  if (!strcmp ("double-decode", name))
    conf.double_decode = 1;

  /* hour specificity */
  if (!strcmp ("hour-spec", name) && !strcmp (oarg, "min"))
    conf.hour_spec_min = 1;

  /* date specificity */
  if (!strcmp ("date-spec", name) && !strcmp (oarg, "hr"))
    conf.date_spec_hr = 1;

  /* max items */
  if (!strcmp ("max-items", name)) {
    char *sEnd;
    int max = strtol (oarg, &sEnd, 10);
    if (oarg == sEnd || *sEnd != '\0' || errno == ERANGE)
      conf.max_items = 1;
    else
      conf.max_items = max;
  }

  /* no color */
  if (!strcmp ("no-color", name))
    conf.no_color = 1;

  /* no columns */
  if (!strcmp ("no-column-names", name))
    conf.no_column_names = 1;

  /* no csv summary */
  if (!strcmp ("no-csv-summary", name))
    conf.no_csv_summary = 1;

  /* json pretty print */
  if (!strcmp ("json-pretty-print", name))
    conf.json_pretty_print = 1;

  /* no progress */
  if (!strcmp ("no-progress", name))
    conf.no_progress = 1;

  /* no tab scroll */
  if (!strcmp ("no-tab-scroll", name))
    conf.no_tab_scroll = 1;

  /* specifies the path of the GeoIP City database file */
  if (!strcmp ("geoip-city-data", name) || !strcmp ("geoip-database", name))
    conf.geoip_database = oarg;

  /* load data from disk */
  if (!strcmp ("load-from-disk", name))
    conf.load_from_disk = 1;

  /* keep database files */
  if (!strcmp ("keep-db-files", name))
    conf.keep_db_files = 1;

  /* specifies the path of the database file */
  if (!strcmp ("db-path", name))
    conf.db_path = oarg;

  /* set the size in bytes of the extra mapped memory */
  if (!strcmp ("xmmap", name))
    conf.xmmap = atoi (oarg);

  /* specifies the maximum number of leaf nodes to be cached */
  if (!strcmp ("cache-lcnum", name))
    conf.cache_lcnum = atoi (oarg);

  /* specifies the maximum number of non-leaf nodes to be cached */
  if (!strcmp ("cache-ncnum", name))
    conf.cache_ncnum = atoi (oarg);

  /* number of members in each leaf page */
  if (!strcmp ("tune-lmemb", name))
    conf.tune_lmemb = atoi (oarg);

  /* number of members in each non-leaf page */
  if (!strcmp ("tune-nmemb", name))
    conf.tune_nmemb = atoi (oarg);

  /* number of elements of the bucket array */
  if (!strcmp ("tune-bnum", name))
    conf.tune_bnum = atoi (oarg);

  /* specifies that each page is compressed with X encoding */
  if (!strcmp ("compression", name)) {
#ifdef HAVE_ZLIB
    if (!strcmp ("zlib", oarg))
      conf.compression = TC_ZLIB;
#endif
#ifdef HAVE_BZ2
    if (!strcmp ("bz2", oarg))
      conf.compression = TC_BZ2;
#endif
  }

  /* default config file --dwf */
  if (!strcmp ("dcf", name)) {
    display_default_config_file ();
    exit (EXIT_SUCCESS);
  }
}

/* Read the user's supplied command line options. */
void
read_option_args (int argc, char **argv)
{
  int o, idx = 0;

#ifdef HAVE_LIBGEOIP
  conf.geo_db = GEOIP_MEMORY_CACHE;
#endif

  while ((o = getopt_long (argc, argv, short_options, long_opts, &idx)) >= 0) {
    if (-1 == o || EOF == o)
      break;
    switch (o) {
    case 'f':
      conf.ifile = optarg;
      break;
    case 'p':
      /* ignore it */
      break;
#ifdef HAVE_LIBGEOIP
    case 'g':
      conf.geo_db = GEOIP_STANDARD;
      break;
#endif
    case 'e':
      if (conf.ignore_ip_idx < MAX_IGNORE_IPS)
        conf.ignore_ips[conf.ignore_ip_idx++] = optarg;
      break;
    case 'a':
      conf.list_agents = 1;
      break;
    case 'c':
      conf.load_conf_dlg = 1;
      break;
    case 'i':
      conf.hl_header = 1;
      break;
    case 'q':
      conf.ignore_qstr = 1;
      break;
    case 'o':
      if (conf.output_format_idx < MAX_OUTFORMATS)
        conf.output_formats[conf.output_format_idx++] = optarg;
      break;
    case 'l':
      conf.debug_log = optarg;
      dbg_log_open (conf.debug_log);
      break;
    case 'r':
      conf.skip_term_resolver = 1;
      break;
    case 'd':
      conf.enable_html_resolver = 1;
      break;
    case 'm':
      conf.mouse_support = 1;
      break;
    case 'M':
      if (strcmp ("no", optarg) == 0)
        conf.append_method = 0;
      else
        conf.append_method = 1;
      break;
    case 'h':
      cmd_help ();
      break;
    case 'H':
      if (strcmp ("no", optarg) == 0)
        conf.append_protocol = 0;
      else
        conf.append_protocol = 1;
      break;
    case 'V':
      display_version ();
      exit (EXIT_SUCCESS);
      break;
    case 0:
      parse_long_opt (long_opts[idx].name, optarg);
      break;
    case 's':
      display_storage ();
      exit (EXIT_SUCCESS);
    case '?':
      exit (EXIT_FAILURE);
    default:
      exit (EXIT_FAILURE);
    }
  }

  for (idx = optind; idx < argc; idx++)
    cmd_help ();
}
