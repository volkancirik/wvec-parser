function x = outerproduct(a,b)
l1 = size(a,2);
l2 = size(b,2);

x = bsxfun(@times,a',b);
x = reshape(x,l1*l2,1);
x = x';
end    
