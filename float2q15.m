function [ res ] = float2q15( in )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    a = in; % your float point number
    n = 0;         % number bits for integer part of your number      
    m = 15;         % number bits for fraction part of your number
    % binary number
    d2b = fix(rem(abs(a)*pow2(-(n-1):m),2)); 
    if a < 0
        d2b = [1 d2b];
    else
        d2b = [0 d2b];
    end
    % convert to 16 bit integer notation
    res = 0;
    for i = 0:(m+n-1)
        res = res + d2b(m+n+1-i)*(2^i);
    end
    if d2b(1) == 1
       res = res * -1; 
    end
end

