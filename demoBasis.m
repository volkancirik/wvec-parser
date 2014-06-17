
clc
clear
close all

models = [];
log = [];

M = {};
L = {};

for i=5:5,
    [w,log] = runParseBasis('1',i);
    fprintf('_______________\n');
    M{i} = w;
    L{i} = log;
end
