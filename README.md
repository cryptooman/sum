
Summation, average, min/max value of numeric data column with high precision

<pre>
Max meaningful row line length: 64
Max precision: 1.00000e-15
Max integer: 9.99999e+56
Max float with precision preserve: 9.99999e+56

Install
    $ sudo aptitude install libmpfr-dev
    Or manually from: http://www.mpfr.org

    $ cd {sum-src}
    $ make
    
Usage
    $ {path-to-sum-bin} sum [-h|-s|-a|-mi|-ma] [--strict] < {numeric-column-data}

    Flags:
        -h          Help

        -s          Return summation value only

        -a          Return average value only

        -mi         Return minimum value only

        -mi         Return maximum value only

        --strict    Strict mode
                    Disable line spaces trim and empty lines ignore

                    Conversion notes
                                        default                 --strict
                        "0100"      ->  "100"                   "100"
                        " 100"      ->  "100"                   "100"
                        "100 "      ->  "100"                   exit with error
                        " 100 "     ->  "100"                   exit with error
                        ""          ->  skip line               exit with error
                        "\n"        ->  skip line               exit with error
                        "   "       ->  skip line               exit with error
                        " 100 0"    ->  exit with error         exit with error
                        "100zzz"    ->  exit with error         exit with error
                        "zzz100"    ->  exit with error         exit with error

    {numeric-data} content format
        See samples: tests/test-1.txt, tests/test-2.txt

    Sample:
        $ cd {sum-src}

        # 1
        $ ./sum < tests/test-2.txt
        Sum     : 3205.03402
        Avg     : 188.531412941176471
        Min     : -1009.01
        Max     : 1008.01

        # 2
        $ echo -e "101\n102" | ./sum -s
        203

</pre>