#!/usr/bin/perl
@timer = ( 1, 9, 5, 10 );
# @timer = ( 2, 0, 0, 0 );
$seconds  = 0;
print( "2" . "0" . ":" . "0" . "0" . "\n");
while( $timer[0] >= 0 ) {
   # sleep( 1 );
    if( $timer[3] == 0 ) {
        $timer[2]--;
        $timer[3] = 10;
        if( $timer[2] < 0  ){
            $timer[1]--;
            $timer[2] = 5;
        }
        if( $timer[1] < 0 ){
            $timer[0]--;
            $timer[1]=9;
        }
    }
    $timer[3]--;
    # print( $timer[0] . $timer[1] . ":" . $timer[2] . $timer[3] . "\n");
    if( $timer[0] >=0 ){
        print( $timer[0] . $timer[1] . ":" . $timer[2] .  $timer[3] . "\n");
    }
}

print( "TIMES UP!\n" );

