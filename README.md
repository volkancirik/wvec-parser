wvec-parser
===========

### Word Vector Dependency Parser ###

A first-order projective dependency parser with vectors. Implemented in matlab.

Setup your mex for cpp compilaton.

    mex -setup cpp
    
Compile the decoder

    mex decoder.cpp
    
    
Run the demo in matlab

    >> demoBasis

This will run dependency parser on a subsample of 100 sentence training, 10 sentence dev and test
file.

- Subsample data turkish conll06
- Word vectors are SCODE [Cirik and Yuret, 2014](https://github.com/ai-ku/wvec)
- features : wvectors second degree polynomial (kronocker product of arc vectors)

#####TODO:

- Write a better README.
