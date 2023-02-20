clc; clear;

freq = 50;
levels = 255;

syms t;

half_T = 1/freq/2

integral = int((sin(100*pi*t)).^2)
full = eval(subs(integral,t,half_T)- subs(integral,t,0));

vec = zeros(levels,1);
f = @(start) eval(subs(integral,t,half_T)- subs(integral,t,start));

for i=1:levels
    
    target = i.*full./levels;
    vec(i) = fsolve(@(x) target - f(x), half_T/2);
    
end

vec = round(vec*1000000/3.2)'
writematrix(vec,'correction.csv') 