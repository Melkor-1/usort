#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE   700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>
#include <errno.h>
#include <ctype.h>

#include <unistd.h>
#include <getopt.h>

#define OP_LIST "nrfho:"        /* Short options */

struct flags {
    bool nflag;                 /* Numeric flag */
    bool rflag;                 /* Reverse flag */
    bool fflag;                 /* Case flag */
    FILE *output;               /* Output to FILE */
};

static void help(void)
{
    puts("\nUSAGE\n\tusort <options> <filename>\n\n"
         "NAME\n\tusort - sort lines of text.\n\n"
         "DESCRIPTION\n\tWrite sorted concatenation of FILE to standard output.\n"
         "	In the absence of options, lexicographical order is taken as default.\n\n"
         "ORDERING OPTIONS:\n\n"
         "		-n, --numeric            	sorts by numeric order.\n"
         "		-r, --reverse 		        sorts in reverse order.\n"
         "        -f, --ignore-case           folds upper case to lower case characters.\n"
         "		-h, --help                  displays this message and exits.\n"
         "		-o, --output=FILE           writes result to FILE instead of standard output.\n");
    exit(EXIT_SUCCESS);
}

static void err_msg(void)
{
    fprintf(stderr, "The syntax of the command is incorrect.\n"
            "Try usort -h for more information.\n");
    exit(EXIT_FAILURE);
}

static void parse_options(const struct option *restrict long_options,
                          struct flags *restrict opt_ptr, int argc, char *const argv[])
{
    int c;

    while ((c = getopt_long(argc, argv, OP_LIST, long_options, NULL)) != -1) {
        switch (c) {
            case 'n':
                opt_ptr->nflag = true;
                break;
            case 'r':
                opt_ptr->rflag = true;
                break;
            case 'f':
                opt_ptr->fflag = true;
                break;
            case 'h':
                help();
                break;
            case 'o':
                opt_ptr->output = fopen(optarg, "a");

                if (!opt_ptr->output) {
                    perror(optarg);
                    exit(EXIT_FAILURE);
                }
                break;

            /* case '?' */
            default:
                err_msg();
                break;
        }
    }
}

/*	 The function returns true if we are writing to stdout; if not then it only returns true if we 
*    successfully closed the stream. Or viewed another way, it returns false only if we needed to 
* 	 close the stream but failed when doing so. 
*/
static bool write_line(char *restrict lines[], size_t num_lines,
                       const struct flags *restrict opt_ptr)
{
    FILE *const f = opt_ptr->output;
    const bool reverse = opt_ptr->rflag;

    if (f != stdout && ftruncate(fileno(f), 0)) {
        perror("seek");
        return false;
    }
    for (size_t i = 0; i < num_lines; i++) {
        const size_t n = reverse ? num_lines - 1 - i : i;

        if (fprintf(f, "%s\n", lines[n]) < 0) {
            perror("write");
            return false;
        }
    }
    return f == stdout || !fclose(f);
}

/* Compares s1 and s2 in a case-insensitive, lexicographical order */
static int case_cmp(const void *restrict s1, const void *restrict s2)
{
    const char *const *v1 = s1;
    const char *const *v2 = s2;

    return strcasecmp(*v1, *v2);
}

/* Compares s1 and s2 lexicographically */
static int s_cmp(const void *restrict s1, const void *restrict s2)
{
    const char *const *v1 = s1;
    const char *const *v2 = s2;

    return strcmp(*v1, *v2);
}

/* Compares s1 and s2 numerically */
static int num_cmp(const void *restrict s1, const void *restrict s2)
{
    const char *const *t1 = s1;
    const char *const *t2 = s2;
    char *p1;
    char *p2;

    intmax_t v1 = strtoimax(*t1, &p1, 10);
    intmax_t v2 = strtoimax(*t2, &p2, 10);

    return v1 > v2 ? 1 : v1 < v2 ? -1 : s_cmp(&p1, &p2);        /* In case of trailing junk,
                                                                 * sort rest of line as a string */
}

static char *read_file(FILE *fp)
{
    const size_t page_size = 4096;
    char *content = 0;
    size_t len = 0;

    for (size_t rcount = 1; rcount; len += rcount) {
        void *const tmp = realloc(content, len + page_size);

        if (!tmp) {
            free(content);
            content = NULL;
            errno = ENOMEM;
            return 0;
        }
        content = tmp;
        rcount = fread(content + len, 1, page_size - 1, fp);

        if (ferror(fp)) {
            free(content);
            content = NULL;
            errno = ENOMEM;
            return 0;
        }
    }
    content[len] = '\0';
    return content;
}

static size_t split_lines(char *restrict s, char ***restrict lines)
{
    const size_t chunk_size = 1024;
    size_t capacity = 0;
    size_t line_count = 0;

    while (s && *s) {
        if (line_count >= capacity) {
            char **new = realloc(*lines,
                                 sizeof **lines * (capacity += chunk_size));

            if (!new) {
                free(*lines);
                *lines = NULL;
                errno = ENOMEM;
                return 0;
            }
            *lines = new;
        }
        (*lines)[line_count++] = s;
        s = strchr(s, '\n');

        if (s) {
            *s++ = '\0';
        }
    }
    return line_count;
}

int main(int argc, char *argv[])
{

    /* Sanity check. POSIX requires the invoking process to pass a non-NULL 
     * argv[0]. 
     */
    if (!argv) {
        fprintf(stderr,
                "A NULL argv[0] was passed through an exec system call.\n");
        return EXIT_FAILURE;
    }

    static const struct option long_options[] = {
        { "numeric", no_argument, NULL, 'n' },
        { "reverse", no_argument, NULL, 'r' },
        { "output", required_argument, NULL, 'o' },
        { "help", no_argument, NULL, 'h' },
        { "ignore-case", no_argument, NULL, 'f' },
        { NULL, 0, NULL, 0 }
    };

    FILE *in_file = stdin;
    struct flags options = { false, false, false, stdout };

    parse_options(long_options, &options, argc, argv);

    if ((optind + 1) == argc) {
        in_file = fopen(argv[optind], "r");
        if (!in_file) {
            perror(argv[optind]);
            return EXIT_FAILURE;
        }
    }

    else if (optind > argc) {
        err_msg();
    }
   
    char *const content = read_file(in_file);
    char **lines = NULL;
    const size_t num_lines = split_lines(content, &lines);

    if (!lines) {
        free(content);
        perror("read ");
        return EXIT_FAILURE;
    }
    qsort(lines, num_lines, sizeof *lines,
          options.nflag ? num_cmp : options.fflag ? case_cmp : s_cmp);

    if (!write_line(lines, num_lines, &options)) {
        free(content);
        free(lines);
        return EXIT_FAILURE;
    }
    free(content);
    free(lines);
    return EXIT_SUCCESS;
}
