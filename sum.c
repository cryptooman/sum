#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <gmp.h>
#include <mpfr.h>

#define DEBUG 0

#define NUM_FORMAT "%.15RNf"    // Fraction tail zeroes will be removed
#define __MPFR_ROUND MPFR_RNDD

const int MAX_LINE_LEN = 64;
const int __MPFR_BASE = 10;
const int __MPFR_PRECISION_BITS = 192;

int __trim_spaces__ = true;
int __scan_line_num__ = 1;

inline void add_value(mpfr_t *sum, mpfr_t *line_val, const char *line);

inline char *trim(char *str);

int main(int argc, char* argv[])
{
    const char usage[] = "sum [-s] < <data>";

    if(argc > 2)
    {
        printf("Incorrect input args. Usage: %s\n", usage);
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i], "-s"))
        {
            printf("Incorrect input args. Usage: %s\n", usage);
            exit(EXIT_FAILURE);
        }
        else
            __trim_spaces__ = false;
    }

    char ch;
    char line[MAX_LINE_LEN + 1];
    int ch_pos = 0;

    mpfr_t line_val;
    mpfr_init2(line_val, __MPFR_PRECISION_BITS);

    mpfr_t sum;
    mpfr_init2(sum, __MPFR_PRECISION_BITS);
    mpfr_set_d(sum, 0, __MPFR_ROUND);      // sum = 0

    while(1)
    {
        ch = getchar();

        if(ch == EOF)
        {
            line[ch_pos] = '\0';
            add_value(&sum, &line_val, line);

            break;
        }
        else if(ch == '\n')
        {
            line[ch_pos] = '\0';
            add_value(&sum, &line_val, line);
            ch_pos = 0;

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
    mpfr_sprintf(sum_formatted, NUM_FORMAT, sum);

    #if DEBUG
        fprintf(stderr, "%d: Sum formatted : %s\n", __LINE__, sum_formatted);
    #endif

    int sum_formatted_len = strlen(sum_formatted);

    char *res = malloc(sum_formatted_len + 1);
    if(res == NULL)
    {
        fprintf(stderr, "%d: error: Failed to allocate memory\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    // Remove fraction tail zeroes
    // 100.01000 -> 100.01
    int fraction_zeroes_len = 0;

    if(strchr(sum_formatted, '.'))
    {
        for(int i = sum_formatted_len - 1; i >= 0; i--)
        {
            if(sum_formatted[i] == '0')
                ++fraction_zeroes_len;
            else
                break;
        }
    }

    strncpy(res, sum_formatted, (size_t) sum_formatted_len - fraction_zeroes_len);
    res[sum_formatted_len - fraction_zeroes_len] = '\0';

    // Remove tail "."
    // 100. -> 100
    int res_len = strlen(res) - 1;
    if(res[res_len] == '.')
        res[res_len] = '\0';

    mpfr_clear(line_val);
    mpfr_clear(sum);

    printf("%s\n", res);

    free(res);

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

    if(mpfr_add(*sum, *sum, *line_val, __MPFR_ROUND) > 0)
    {
        fprintf(stderr, "%d: error: Failed to add value [%s] at line %d\n", __LINE__, line, __scan_line_num__);
        exit(EXIT_FAILURE);
    }
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
