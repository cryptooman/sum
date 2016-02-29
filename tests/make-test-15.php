<?php

for($i = 0; $i < 10000; $i++)
{
    $num = mt_rand(0, 10000);
    if(!mt_rand(0, 1))
        $num /= mt_rand(3, 3333);
    
    if(!mt_rand(0, 4))
        $num *= -1;
    
    $num = round($num, 5);
    
    file_put_contents('test-15.txt', "$num\n", FILE_APPEND);
}

