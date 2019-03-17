function val=perman_mat(A)
% PERMAN_MAT    Permament of matrix.
%   PERMAN_MAT(A) is the permament of square matrix, A.
%
%   The permanent is defined like the determininant in the standard 
%   summation manner taught in school, except all of the sign terms are 
%   removed. Permanents play a role in combinatorics.
%
% INPUT: A   A square matrix. Entries may be real or complex.
%
% OUTPUT:    The matrix permanent of A.
%
%   The algorithm is PERMAN from Chapter 23 of A. Nijenhuis and H. S. Wilf, 
% "Combinatorial Algorithms for Computers and Calculators," 2nd ed. 
% New York: Academic press, 1978 and self-published as of 2006.  
% It is used as it is more efficient and appears to be less susceptible 
% to finite precision errors than the standard Ryser algorithms.
[m, n]=size(A);
if (m ~= n),
    error('Must be a square matrix. perman()  %d %d',m,n)
end
%Empty matrices have a permanent of 1 by definition.
if isempty(A),
    val=1;
    return;
end
x = A(:,n) - sum(A,2)/2;
p = prod(x);
% the next 5 lines are only a function of n
tn11=2^(n-1)-1;
y=bitxor(1:tn11,bitshift(1:tn11,-1));  % gray-coded count  
z=log2(double(bitxor([0 y(1:tn11-1)],y(1:tn11))))+1; 
s=(-1).^(1+bitget(y, z));
prodsign=(-1).^(1:tn11);
% The following loop consumes 91.7% of profile time (n=20)
disp(p);
for i=1:tn11,
    if i<10
      fprintf("%f,%f, %f \n",double(p),double(prod(x)),prodsign(i));
      end
    x = x + s(i)*A(:,z(i));          % 15.4% of profile time (n=20)
    p = p + prodsign(i) * prod(x);
  
end                                  % 22.0% of profile time (n=20)
val=(4*mod(n,2)-2)*p;
end
