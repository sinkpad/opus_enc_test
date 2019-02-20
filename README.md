# opus_enc_test
opus 1.3 lib encode test

download the opus 1.3 lib
% cd opus-1.3
% ./configure --enable-fixed-point --disable-float-api
% make

% cd ../test
% make clean; make
% ./enc test_file.raw
