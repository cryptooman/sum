/*
    gcc -std=c99 -O3 sum-fast.c -o sum-fast

                    Expect
    -------------------------------
    test-1          5015
    test-2          3205.03402
    test-3-1        5015
    test-3-2        8036.003
    test-4          err
    test-5-1        4.000000001e+18
    test-5-2        err
    test-5-3        err
    test-5-4        err
    test-5-5        err
    test-5-6        err
    test-6          err
    test-7-1        err
    test-7-2        4012
    test-7-3        err
    test-7-4        5022
    test-7-5        5015.05
    test-7-6        4014
    test-7-7        5015
    test-7-8        4012
    test-7-9        err
    test-7-10       0
    test-7-11       0
    test-7-12       0
    test-15         15151199.643
    test-16         1.5370167593e+12

    Estimated time
        # 1
        sum-fast        0m2.649s        time for i in {1..1000}; do cat tests/test-15.txt | ./sum-fast > /dev/null; done
        paste + bc      0m7.125s        time for i in {1..1000}; do cat tests/test-15.txt | paste -sd+ | bc -l > /dev/null; done
        awk             0m2.576s        time for i in {1..1000}; do cat tests/test-15.txt | awk '{sum += $0} END {print sum}' > /dev/null; done

        #2 (Summation of 8544016 numeric rows from log file)
        sum-fast        1m12.870s       time cat <big-log-file> | cut -f2 | ./sum-fast

        paste + bc      1m21.308s       time cat <big-log-file> | cut -f2 | paste -sd+ | bc -l

        awk             1m13.935s       time cat <big-log-file> | cut -f2 | awk '{sum += $1} END {print sum}'
*/

#include <stdio.h>
#include <stdlib.h>

int main()
{
    const int MAX_LINE_LEN = 32;

    char ch;
    char line[MAX_LINE_LEN + 1];
    int ch_pos = 0;

    double sum = 0;

    while(1)
    {
        ch = getchar();

        if(ch == EOF)
        {
            line[ch_pos] = '\0';
            sum += atof(line);

            break;
        }
        else if(ch == '\n')
        {
            line[ch_pos] = '\0';
            sum += atof(line);
            ch_pos = 0;
        }
        else
        {
            line[ch_pos] = ch;
            ++ch_pos;

            if(ch_pos > MAX_LINE_LEN)
            {
                fprintf(stderr, "%d: error: Value is too long [%s ...]\n", __LINE__, line);
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("%.11g\n", sum);

    return 0;
}
