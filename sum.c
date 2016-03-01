#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <gmp.h>
#include <mpfr.h>

#define DEBUG 0

#define NUM_FORMAT "%.15RNf"    // Fraction tail zeroes will be removed
#define MAX_VALUE 9.9e+200
#define MIN_VALUE -1.0e-200
#define __MPFR_ROUND MPFR_RNDD

const int MAX_LINE_LEN = 64;
const int __MPFR_BASE = 10;
const int __MPFR_PRECISION_BITS = 192;

bool __trim_spaces__ = true;
int __scan_line_num__ = 1;
unsigned long long int __total_lines__ = 0;

#define MODE_DEFAULT 0
#define MODE_SUM 1
#define MODE_AVG 2
#define MODE_MIN 3
#define MODE_MAX 4
int __mode__ = MODE_DEFAULT;

inline void add_value(mpfr_t *sum, mpfr_t *line_val, const char *line);

inline char *trim(char *str);

char *cut_fraction_zeroes(char *str);

int main(int argc, char* argv[])
{
    const char usage[] = "sum [-h|-s|-a|-mi|-ma] [--strict] < <numeric-column-data>";

    if(argc > 4)
    {
        printf("Incorrect input args. Usage: %s\n", usage);
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i < argc; i++)
    {
        if(!strcmp(argv[i], "-h"))
        {
            printf(
                "Summation tool v0.002\n"
                "Usage: %s\n"
                "-h\t\tHelp\n"
                "-s\t\tReturn summation value only\n"
                "-a\t\tReturn average value only\n"
                "-mi\t\tReturn minimum value only\n"
                "-mi\t\tReturn maximum value only\n"
                "--strict\tDisable empty lines ignore\n",
                usage
            );

            exit(0);
        }
        else if(!strcmp(argv[i], "-s"))
            __mode__ = MODE_SUM;
        else if(!strcmp(argv[i], "-a"))
            __mode__ = MODE_AVG;
        else if(!strcmp(argv[i], "-mi"))
            __mode__ = MODE_MIN;
        else if(!strcmp(argv[i], "-ma"))
            __mode__ = MODE_MAX;
        else if(!strcmp(argv[i], "--strict"))
            __trim_spaces__ = false;
        else
        {
            printf("Incorrect input args. Usage: %s\n", usage);
            exit(EXIT_FAILURE);
        }
    }

    #if DEBUG
        fprintf(stderr, "%d: Mode : %d\n", __LINE__, __mode__);
    #endif

    mpfr_t sum_value;
    mpfr_init2(sum_value, __MPFR_PRECISION_BITS);
    mpfr_set_d(sum_value, 0, __MPFR_ROUND);      // sum_value = 0

    mpfr_t line_val;
    mpfr_init2(line_val, __MPFR_PRECISION_BITS);

    mpfr_t avg_value;
    mpfr_t min_value;
    mpfr_t max_value;

    mpfr_init2(avg_value, __MPFR_PRECISION_BITS);
    mpfr_init2(min_value, __MPFR_PRECISION_BITS);
    mpfr_init2(max_value, __MPFR_PRECISION_BITS);

    mpfr_set_d(avg_value, 0, __MPFR_ROUND);
    mpfr_set_d(min_value, MAX_VALUE, __MPFR_ROUND);
    mpfr_set_d(max_value, MIN_VALUE, __MPFR_ROUND);

    char ch;
    char line[MAX_LINE_LEN + 1];
    int ch_pos = 0;

    while(1)
    {
        ch = getchar();

        if(ch == EOF)
        {
            line[ch_pos] = '\0';
            add_value(&sum_value, &line_val, line);

            if(mpfr_cmp(min_value, line_val) > 0)
                mpfr_set(min_value, line_val, __MPFR_ROUND);

            if(mpfr_cmp(max_value, line_val) < 0)
                mpfr_set(max_value, line_val, __MPFR_ROUND);

            break;
        }
        else if(ch == '\n')
        {
            line[ch_pos] = '\0';
            add_value(&sum_value, &line_val, line);
            ch_pos = 0;

            if(mpfr_cmp(min_value, line_val) > 0)
                mpfr_set(min_value, line_val, __MPFR_ROUND);

            if(mpfr_cmp(max_value, line_val) < 0)
                mpfr_set(max_value, line_val, __MPFR_ROUND);

            ++__scan_line_num__;
        }
        else
        {
            line[ch_pos] = ch;
            ++ch_pos;

            if(ch_pos > MAX_LINE_LEN)
            {
                fprintf(stderr, "%d: error: Value is too long [%s ...] at line %d\n", __LINE__, line, __scan_line_num__);
                exit(EXIT_FAILURE);
            }
        }
    }

    char sum_formatted[MAX_LINE_LEN + 1];
    char avg_formatted[MAX_LINE_LEN + 1];
    char min_formatted[MAX_LINE_LEN + 1];
    char max_formatted[MAX_LINE_LEN + 1];

    // Sum
    mpfr_sprintf(sum_formatted, NUM_FORMAT, sum_value);

    #if DEBUG
        fprintf(stderr, "%d: Sum formatted : %s\n", __LINE__, sum_formatted);
    #endif

    // Avg
    if(__total_lines__)
    {
        {
            mpfr_t mpfr_total_lines;
            mpfr_init2(mpfr_total_lines, __MPFR_PRECISION_BITS);
            mpfr_set_d(mpfr_total_lines, __total_lines__, __MPFR_ROUND);

            mpfr_div(avg_value, sum_value, mpfr_total_lines, __MPFR_ROUND);

            mpfr_clear(mpfr_total_lines);
        }
    }
    else
        mpfr_set_d(avg_value, 0, __MPFR_ROUND);

    mpfr_sprintf(avg_formatted, NUM_FORMAT, avg_value);

    #if DEBUG
        fprintf(stderr, "%d: Avg formatted : %s\n", __LINE__, avg_formatted);
    #endif

    // Min
    mpfr_sprintf(min_formatted, NUM_FORMAT, min_value);

    #if DEBUG
        fprintf(stderr, "%d: Min formatted : %s\n", __LINE__, min_formatted);
    #endif

    // Max
    mpfr_sprintf(max_formatted, NUM_FORMAT, max_value);

    #if DEBUG
        fprintf(stderr, "%d: Min formatted : %s\n", __LINE__, max_formatted);
    #endif

    #if DEBUG
        fprintf(stderr, "%d: Meaningful rows : %llu\n", __LINE__, __total_lines__);
    #endif

    if(__mode__ == MODE_DEFAULT)
    {
        printf("Sum\t: %s\n", cut_fraction_zeroes(sum_formatted));
        printf("Avg\t: %s\n", cut_fraction_zeroes(avg_formatted));
        printf("Min\t: %s\n", cut_fraction_zeroes(min_formatted));
        printf("Max\t: %s\n", cut_fraction_zeroes(max_formatted));
    }
    else if(__mode__ == MODE_SUM)
        printf("%s\n", cut_fraction_zeroes(sum_formatted));
    else if(__mode__ == MODE_AVG)
        printf("%s\n", cut_fraction_zeroes(avg_formatted));
    else if(__mode__ == MODE_MIN)
        printf("%s\n", cut_fraction_zeroes(min_formatted));
    else if(__mode__ == MODE_MAX)
        printf("%s\n", cut_fraction_zeroes(max_formatted));

    mpfr_clear(line_val);
    mpfr_clear(sum_value);
    mpfr_clear(avg_value);
    mpfr_clear(min_value);
    mpfr_clear(max_value);

    return 0;
}

inline void add_value(mpfr_t *sum, mpfr_t *line_val, const char *line)
{
    #if DEBUG
        fprintf(stderr, "%d: Value: %s at line %d\n", __LINE__, line, __scan_line_num__);
    #endif

    if(mpfr_set_str(*line_val, line, __MPFR_BASE, __MPFR_ROUND) == -1)
    {
        if(__trim_spaces__)
        {
            char line_fixed[MAX_LINE_LEN + 1];
            strcpy(line_fixed, line);
            trim(line_fixed);

            if(line_fixed[0] == '\0')
            {
                line_fixed[0] = '0';
                line_fixed[1] = '\0';
            }
            else
                ++__total_lines__;

            if(mpfr_set_str(*line_val, line_fixed, __MPFR_BASE, __MPFR_ROUND) == -1)
            {
                fprintf(stderr, "%d: error: Illegal value [%s] at line %d\n", __LINE__, line, __scan_line_num__);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fprintf(stderr, "%d: error: Illegal value [%s] at line %d\n", __LINE__, line, __scan_line_num__);
            exit(EXIT_FAILURE);
        }
    }
    else
        ++__total_lines__;

    mpfr_add(*sum, *sum, *line_val, __MPFR_ROUND);
}

inline char *trim(char *str)
{
    int s_len = strlen(str);

    char *s_trimmed = malloc(sizeof(char) * s_len + 1);
    if(s_trimmed == NULL)
    {
        fprintf(stderr, "%d: error: Failed to allocate memory\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    int offset = 0;
    int non_space_found = false;
    for(int i = 0; i < s_len; i++)
    {
        if(!isspace(str[i]) )
        {
            offset = i;
            non_space_found = true;
            break;
        }
    }

    if(!non_space_found)
    {
        str[0] = '\0';
        return str;
    }

    int s_trimmed_len = s_len - offset;
    for(int i = s_len - 1; i >= 0; i--)
    {
        if(!isspace(str[i]) )
        {
            s_trimmed_len = (i + 1) - offset;
            break;
        }
    }

    int st_i = 0;
    int trim_end = s_trimmed_len + offset;
    for(int i = offset; i < trim_end; i++)
        s_trimmed[st_i++] = str[i];
    s_trimmed[st_i] = '\0';

    strncpy(str, s_trimmed, strlen(s_trimmed) + 1);

    free(s_trimmed);

    return str;
}

// 100.00100    -> 100.001
// 100.00000    -> 100
// 100.         -> 100
char *cut_fraction_zeroes(char *str)
{
    if(!strchr(str, '.'))
        return str;

    for(int i = strlen(str) - 1; i >= 0; i--)
    {
        if(str[i] == '0')
            *(str + i) = '\0';
        else if(str[i] == '.')
        {
            *(str + i) = '\0';
            break;
        }
        else
            break;
    }

    return str;
}
