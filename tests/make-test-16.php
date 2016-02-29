<?php

ini_set('precision', 17);

for($i = 0; $i < 10000; $i++)
{
    $num = mt_rand(0, 1000000000);
    if(!mt_rand(0, 1))
        $num /= mt_rand(3, 33333333);
    
    if(!mt_rand(0, 4))
        $num *= -1;
    
    $num = round($num, 15);
    
    file_put_contents('test-16.txt', "$num\n", FILE_APPEND);
}

