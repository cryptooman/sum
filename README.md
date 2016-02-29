<pre>
Console tool for numeric column summation with high precision

Max meaningful row line length: 64
Max precision: 0.000000000000001
Max integer: 9.99999e+56
Max float with precision preserve: 9.99999e+56

Install
    $ sudo aptitude install libmpfr-dev
    Or manually from: http://www.mpfr.org

    $ cd <sum-src>
    $ make
    
Usage
    $ <path-to-sum-bin> [-s] < <numeric-data>

    Flags:
        -s      - Strict mode. Disable spaces trim.
                  By default each empty line are converted to zero and line spaces are trimmed

                  Conversion notes
                                        default                 strict
                        "0100"      ->  "100"                   "100"
                        " 100"      ->  "100"                   "100"
                        "100 "      ->  "100"                   exit with error
                        " 100 "     ->  "100"                   exit with error
                        ""          ->  "0"                     exit with error
                        "   "       ->  "0"                     exit with error
                        " 100 0"    ->  exit with error         exit with error
                        "100zzz"    ->  exit with error         exit with error
                        "zzz100"    ->  exit with error         exit with error

    <numeric-data> content format
        See, for example, tests/test-1.txt, tests/test-2.txt files

    Sample:
        $ cd <sum-src>
        $ ./sum < tests/test-1.txt
</pre>